#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DataAggregationSimulation");

void AggregationCallback(Ptr<const Packet> packet, std::map<std::string, double>& aggregatedValues, std::map<std::string, int>& packetCounts, const std::string& nodeName)
{
    aggregatedValues[nodeName] += 1.0;  // Simplified, assume each packet carries a value of 1.0
    packetCounts[nodeName]++;

    if (packetCounts[nodeName] >= 10) // Aggregation happens after receiving 10 packets (adjust as needed)
    {
        double averageValue = aggregatedValues[nodeName] / packetCounts[nodeName];
        NS_LOG_INFO("Aggregated value at " << nodeName << " is: " << averageValue);
        aggregatedValues[nodeName] = 0.0;
        packetCounts[nodeName] = 0;
    }
}

int main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse (argc, argv);

    LogComponentEnable("DataAggregationSimulation", LOG_LEVEL_ALL);
    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);

    // Create nodes
    NodeContainer nodes;
    std::map<std::string, Ptr<Node>> nodeMap;

    // Read link.txt to create nodes and connections
    NS_LOG_INFO("Reading link.txt to create nodes and connections");
    std::ifstream infile("link.txt");
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string node1, node2;
        std::string dataRate, delay;
        iss >> node1 >> node2 >> dataRate >> delay;

        if (nodeMap.find(node1) == nodeMap.end())
        {
            NS_LOG_INFO("Creating node: " << node1);
            Ptr<Node> newNode = CreateObject<Node>();
            nodeMap[node1] = newNode;
            nodes.Add(newNode);
        }
        if (nodeMap.find(node2) == nodeMap.end())
        {
            NS_LOG_INFO("Creating node: " << node2);
            Ptr<Node> newNode = CreateObject<Node>();
            nodeMap[node2] = newNode;
            nodes.Add(newNode);
        }
    }

    // Install Internet stack
    NS_LOG_INFO("Installing Internet stack");
    InternetStackHelper internet;
    internet.Install (nodes);

    // Re-read link.txt to set up connections
    infile.clear();
    infile.seekg(0, std::ios::beg);
    PointToPointHelper p2p;
    std::vector<NetDeviceContainer> netDevices;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string node1, node2;
        std::string dataRate, delay;
        iss >> node1 >> node2 >> dataRate >> delay;

        NS_LOG_INFO("Creating link between " << node1 << " and " << node2 << " with data rate " << dataRate << " and delay " << delay);
        p2p.SetDeviceAttribute("DataRate", StringValue(dataRate));
        p2p.SetChannelAttribute("Delay", StringValue(delay));

        NetDeviceContainer devices = p2p.Install(nodeMap[node1], nodeMap[node2]);
        netDevices.push_back(devices);
    }

    // Assign IP addresses
    NS_LOG_INFO("Assigning IP addresses");
    Ipv4AddressHelper address;
    int subnet = 1;
    std::vector<Ipv4InterfaceContainer> interfaces;
    for (auto &devices : netDevices)
    {
        std::string base = "10." + std::to_string(subnet) + ".0.0";
        NS_LOG_INFO("Assigning IP address base: " << base);
        address.SetBase(base.c_str(), "255.255.255.0");
        Ipv4InterfaceContainer iface = address.Assign(devices);
        interfaces.push_back(iface);
        subnet++;
    }

    // Install applications: pro nodes send data to agg nodes using TCP
    uint16_t port = 8080;
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps;

    NS_LOG_INFO("Installing sink applications at agg and com nodes");
    for (auto &nodePair : nodeMap)
    {
        if (nodePair.first.find("agg") != std::string::npos || nodePair.first.find("com") != std::string::npos)
        {
            NS_LOG_INFO("Installing sink on node: " << nodePair.first);
            sinkApps.Add (sinkHelper.Install (nodePair.second));
        }
    }
    sinkApps.Start (Seconds (1.0));
    sinkApps.Stop (Seconds (100.0));

    OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
    clientHelper.SetAttribute ("DataRate", StringValue ("1Gbps"));
    clientHelper.SetAttribute ("PacketSize", UintegerValue (2000));
    clientHelper.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    clientHelper.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer clientApps;
    NS_LOG_INFO("Installing client applications at pro nodes");
    for (auto &nodePair : nodeMap)
    {
        if (nodePair.first.find("pro") != std::string::npos)
        {
            for (auto &aggPair : nodeMap)
            {
                if (aggPair.first.find("agg") != std::string::npos)
                {
                    NS_LOG_INFO("Installing client on node: " << nodePair.first << " targeting agg node: " << aggPair.first);
                    AddressValue remoteAddress (InetSocketAddress (aggPair.second->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port));
                    clientHelper.SetAttribute ("Remote", remoteAddress);
                    clientApps.Add (clientHelper.Install (nodePair.second));
                }
            }
        }
    }
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (100.0));

    // Set up the data aggregation logic at agg and com nodes asynchronously
    std::map<std::string, double> aggregatedValues;
    std::map<std::string, int> packetCounts;

    for (auto &nodePair : nodeMap)
    {
        if (nodePair.first.find("agg") != std::string::npos || nodePair.first.find("com") != std::string::npos)
        {
            Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(nodePair.second->GetId()));
            if (sink)
            {
                // Ensure the callback function matches the expected signature
                sink->TraceConnectWithoutContext("Rx", MakeCallback([&](Ptr<const Packet> packet) {
                    AggregationCallback(packet, aggregatedValues, packetCounts, nodePair.first);
                }));
            }
        }
    }

    // Enable routing
    NS_LOG_INFO("Populating routing tables");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    NS_LOG_INFO("Running the simulation");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO("Simulation finished");
    return 0;
}

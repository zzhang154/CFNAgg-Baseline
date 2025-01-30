#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/output-stream-wrapper.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <unistd.h>
#include <limits.h>

// Zhuoxu: no need to use the absolute path
#include "ns3/consumer.h"
#include "ns3/producer.h"
#include "ns3/aggregator.h"
#include "ns3/parameter.h"
#include "ns3/utils.h"

using namespace ns3;

// Variable declarations
extern const std::string currentDir;
extern const std::string Number;

extern std::string routerFilePath;
extern std::string linkFilePath;
extern std::string aggGroupFilePath;

extern const std::string conName;
extern const std::string proName;
extern const std::string fowName;
extern const std::string aggName;

extern uint32_t consumerNum;
extern uint32_t producerNum;
extern uint32_t forwarderNum;
extern uint32_t aggregatorNum;
extern std::string cc;
extern uint16_t basetime;
extern uint16_t starttime;
extern uint16_t stoptime;

// Function to get the current working directory
std::string GetCurrentWorkingDir() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

void PrintRoutingTables()
{
    Ptr<Node> node;
    Ptr<Ipv4> ipv4;
    Ptr<Ipv4RoutingProtocol> routing;

    for (uint32_t i = 0; i < NodeList::GetNNodes(); ++i)
    {
        node = NodeList::GetNode(i);
        ipv4 = node->GetObject<Ipv4>();
        routing = ipv4->GetRoutingProtocol();

        Ptr<Ipv4StaticRouting> staticRouting = DynamicCast<Ipv4StaticRouting>(routing);
        if (staticRouting)
        {
            Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(&std::cout);
            std::cout << "Routing table for node " << node->GetId() << ":\n";
            staticRouting->PrintRoutingTable(stream);
            std::cout << "\n";
        }
        else
        {
            Ptr<Ipv4ListRouting> listRouting = DynamicCast<Ipv4ListRouting>(routing);
            if (listRouting)
            {
                int16_t priority;
                Ptr<Ipv4RoutingProtocol> temp;
                for (uint32_t j = 0; j < listRouting->GetNRoutingProtocols(); ++j)
                {
                    temp = listRouting->GetRoutingProtocol(j, priority);
                    staticRouting = DynamicCast<Ipv4StaticRouting>(temp);
                    if (staticRouting)
                    {
                        Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(&std::cout);
                        std::cout << "Routing table for node " << node->GetId() << ":\n";
                        staticRouting->PrintRoutingTable(stream);
                        std::cout << "\n";
                    }
                }
            }
        }
    }
}

// name nodes
void NameNodes(NodeContainer &nodes, std::string baseName) {
    //Name cons
    for (uint8_t i = 0; i < nodes.GetN(); ++i)
    {
        std::string nodeName = baseName + std::to_string(i);
        if (!Names::Find<Node> (nodeName)) {
            Names::Add(nodeName, nodes.Get(i));
            //std::cout << "Name added successfully: " << nodeName << std::endl;

        } else {
            std::cout << "Error: Name already exists: " << nodeName << std::endl;
        }
    }
}

void CountRouterNodes(const std::string& filename) {

    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Read the file line by line
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("con") == 0) {
            ++consumerNum;
        } else if (line.find("pro") == 0) {
            ++producerNum;
        } else if (line.find("for") == 0) {
            ++forwarderNum;
        } else if (line.find("agg") == 0) {
            ++aggregatorNum;
        }
    }

    // Close the file
    file.close();

    // Output the results
    std::cout << "uint32_t consumerNum = " << consumerNum << ";" << std::endl;
    std::cout << "uint32_t producerNum = " << producerNum << ";" << std::endl;
    std::cout << "uint32_t forwarderNum = " << forwarderNum << ";" << std::endl;
    std::cout << "uint32_t aggregatorNum = " << aggregatorNum << ";" << std::endl;
}

// read link.txt to create links
void BuildTopo(std::string &linkFile,  NodeContainer &consumer, NodeContainer &producer, 
            NodeContainer &forwarder, NodeContainer &aggregator) {
    
    consumer.Create(consumerNum);
    producer.Create(producerNum);
    forwarder.Create(forwarderNum);
    aggregator.Create(aggregatorNum);

    // name nodes
    NameNodes (consumer, conName);
    NameNodes (producer, proName);
    NameNodes (forwarder, fowName);
    NameNodes (aggregator, aggName);
    //std::cout << "name end" << std::endl;

    // install internet stack
    InternetStackHelper stack;
    stack.Install (consumer);
    stack.Install (producer);
    stack.Install (forwarder);
    stack.Install (aggregator);
    //std::cout << "install internet end" << std::endl;

    std::ifstream infile (linkFile);
    std::string line;
    // create p2p links
    PointToPointHelper p2p;
    Ipv4AddressHelper address;
    uint32_t ia = 1, ic = 1;
    
    while (std::getline (infile, line)) {
        std::istringstream iss (line);
        std::string node1, node2, dataRate, delay, queueSize;
        double lossRate;
        if (!(iss >> node1 >> node2 >> dataRate >> lossRate >> delay >> queueSize)) {
            //std::cout << "iss >> node1 >> node2 >> dataRate >> lossRate >> delay >> queueSize" << std::endl;
            //std::cout<<node1<<"------------------"<<node2<<"------------------"<<dataRate<<"------------------"<<lossRate<<"------------------"<<delay<<"------------------"<<queueSize<<std::endl;
            continue; // jump error-format lines
        }
        //std::cout<<"Installing nodes------------------"<<std::endl;
        //std::cout<<node1<<"------------------"<<node2<<"------------------"<<dataRate<<"------------------"<<lossRate<<"------------------"<<delay<<"------------------"<<queueSize<<std::endl;
        p2p.SetDeviceAttribute("DataRate", StringValue (dataRate));
        p2p.SetChannelAttribute("Delay", StringValue (delay));
        p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue (queueSize + "p"));//maxsize=30packets
        //std::cout << "set p2p linkAttribute end" << std::endl;

        if (Names::Find<Node> (node1) == nullptr || Names::Find<Node> (node2) == nullptr) {
            std::cout << "Name does not exist, node1 : " << node1 << " node2 : " << node2 << std::endl;
        }


        NodeContainer n1n2 = NodeContainer (Names::Find<Node> (node1), Names::Find<Node> (node2));
        // install p2p
        NetDeviceContainer d1d2 = p2p.Install (n1n2);
        
        // assign ip address
        std::string ipBaseAddr;
        if (node1.find ("agg") != std::string::npos || node2.find ("agg") != std::string::npos) {
            ipBaseAddr = "10.2." + std::to_string (ia) + ".0";
            ++ia;

        } else {
            ipBaseAddr = "10.1." + std::to_string (ic) + ".0";
            ++ic;
        }

        address.SetBase(Ipv4Address(ipBaseAddr.c_str()), "255.255.255.0");
        address.Assign(d1d2);
        Ipv4Address node1Addr = n1n2.Get(0)->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal();
        Ipv4Address node2Addr = n1n2.Get(1)->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal();
        std::cout<<"---node1  ip---"<<node1<<"---"<<node1Addr<<"---node2  ip---"<<node2<<"---"<<node2Addr<<std::endl;
        ipToNodeName[Ipv4AddressToString(node1Addr)] = node1; // Store in map
        ipToNodeName[Ipv4AddressToString(node2Addr)] = node2; // Store in map
    }
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    p2p.EnablePcapAll("/home/dd/wiresharkRecordFile/");
    infile.close ();
}

void Createpro (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, std::vector<Address> &sGroup, 
                std::vector<Address> &cGroup, Ptr<Node> node) {
    Ptr<Producer> producer = CreateObject<Producer> ();
    //cGroup.resize(0);
    cGroup.clear();
    node->AddApplication (producer);
    producer->SetupProducer (port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    producer->SetStartTime (Seconds(starttime));
    producer->SetStopTime (Seconds(stoptime));
}

void Createagg (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, Ptr<Node> node) {

    // Zhuoxu: add for debug and test
    std::cout << "in Createagg ..." << std::endl;

    // Iterate over the network devices to find the IP address
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    if (ipv4) {
        for (uint32_t i = 0; i < ipv4->GetNInterfaces(); ++i) {
            for (uint32_t j = 0; j < ipv4->GetNAddresses(i); ++j) {
                Ipv4Address address = ipv4->GetAddress(i, j).GetLocal();
                if (address != Ipv4Address::GetLoopback()) {
                    std::cout << "Node IP Address: " << address << std::endl;
                }
            }
        }
    }

    Ptr<Aggregator> aggragator = CreateObject<Aggregator> ();
    node->AddApplication (aggragator);
    aggragator->SetupAggregator (port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    aggragator->SetStartTime (Seconds(starttime));
    aggragator->SetStopTime (Seconds(stoptime));
}

void Createcon (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize,
                    std::vector<Address> &sGroup, std::vector<Address> &cGroup, 
                    Ptr<Node> node) {
    Ptr<Consumer> consumer = CreateObject<Consumer> ();
    node->AddApplication (consumer);
    consumer->SetupConsumer (port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    consumer->SetStartTime (Seconds(starttime));
    consumer->SetStopTime (Seconds(stoptime));
}

void CreateAggGroup (std::string aggGroupFile, 
                        std::unordered_map<std::string, std::vector<std::string>> &aggGroups) {
    std::ifstream infile (aggGroupFile);
    std::string line;
    while (std::getline (infile, line)) {
        std::istringstream iss (line);
        std::vector<std::string> nodes;
        std::string nodeName, childName;
        iss >> nodeName;
        nodeName. pop_back (); // delete ':' character
    
        while (iss >> childName) {
            nodes. push_back (childName);
        }
        aggGroups[nodeName] = nodes;
    }
}

void CreateAggTree (std::string &nodeName, std::vector<Address> pa,
                std::unordered_map<std::string, std::vector<std::string>> &aggGroups,
                uint8_t rank, uint16_t itr, uint32_t vsize, uint16_t server_port) {
    std::vector<Address> sGroup;
    Ptr<Ipv4> ipv4 = Names::Find<Node> (nodeName)->GetObject<Ipv4>(); // find ipv4 address of the root node 'con0' for the first time
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
    Ipv4Address addr = iaddr.GetLocal ();
    sGroup. push_back (addr);

    std::vector<Address> cGroup;  // store the address of the child nodes
    for (std::string &str : aggGroups[nodeName]) {
        rank += 1;
        CreateAggTree (str, sGroup, aggGroups, rank, itr, vsize, server_port);
        // Zhuoxu: the following function can help us to get the IpAddress of a node class.
        ipv4 = Names::Find<Node> (str)->GetObject<Ipv4>();
        iaddr = ipv4->GetAddress (1, 0);
        addr = iaddr.GetLocal ();
        cGroup. push_back (addr);
    }
    // std::cout << "current node " << nodeName << std::endl;

    if (nodeName. find ("con") != std::string::npos)
        // for each object, we have to specify its connection. So here we pass the address of the parent node and children nodes for construction.
        Createcon(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node> (nodeName));
    if (nodeName. find ("agg") != std::string::npos)
    {
        Createagg(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node> (nodeName));
        // std::cout << "Createcon--agg node " << nodeName << std::endl;
    }
    if (nodeName. find ("pro") != std::string::npos)
        Createpro(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node> (nodeName));
}

void CreateAggTreeTopo (uint16_t itr, uint32_t vsize, uint16_t server_port) {
    // get Aggragator Group
    std::unordered_map<std::string, std::vector<std::string>> aggGroups;

    // std::cout << "aggGroupFilePath: " << aggGroupFilePath << std::endl;
    CreateAggGroup (aggGroupFilePath, aggGroups);
    // create agg tree

    std::string root = "con0";
    CreateAggTree (root, std::vector<Address>(), aggGroups, 0, itr, vsize, server_port);
}


void CreateDirectTopo (NodeContainer &cons, NodeContainer &pros, uint16_t itr, 
                       uint32_t vsize, uint16_t server_port) {
    std::vector<Address> consumers;
    Ptr<Ipv4> ipv4 = cons. Get (0)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
    Ipv4Address addr = iaddr.GetLocal ();
    consumers. push_back (addr);

    std::vector<Address> producers;
    for (uint8_t i = 0; i < pros. GetN (); ++i) { //
        ipv4 = pros. Get (i)->GetObject<Ipv4>();
        iaddr = ipv4->GetAddress (1, 0);
        addr = iaddr.GetLocal ();
        producers. push_back (addr);
    }

    uint8_t rank = 0;
    std::vector<Address> nullV;
    for (uint8_t i = 0; i < pros. GetN (); ++i) {
        Ptr<Producer> producer = CreateObject<Producer> ();
        pros. Get (i)->AddApplication (producer);
        producer->SetupProducer (server_port, 0, rank, vsize, consumers, nullV, basetime * 2, cc);
        rank++;
        producer->SetStartTime (Seconds(starttime));
        producer->SetStopTime (Seconds(stoptime));
    }
    

    Ptr<Consumer> consumer = CreateObject<Consumer> ();
    cons. Get (0)->AddApplication (consumer);
    consumer->SetupConsumer (server_port, itr, rank, vsize, nullV, producers, basetime, cc);
    consumer->SetStartTime (Seconds(starttime));
    consumer->SetStopTime (Seconds(stoptime));
    //std::cout << consumer <<std::endl;
}

void EnableLoggingComponents() {

    LogComponentEnable("TCPclient", LOG_LEVEL_ALL);
    LogComponentEnable("TCPserver", LOG_LEVEL_ALL);
    LogComponentEnable("TcpSocketBase", LOG_LEVEL_ALL);
    LogComponentEnable("TcpRxBuffer", LOG_LEVEL_ALL);
    LogComponentEnable("TcpTxBuffer", LOG_LEVEL_ALL);
    LogComponentEnable("Packet", LOG_LEVEL_DEBUG);
}

void DisableLoggingComponents() {
    LogComponentDisable("TCPclient", LOG_LEVEL_ALL);
    LogComponentDisable("TCPserver", LOG_LEVEL_ALL);
    LogComponentDisable("TcpSocketBase", LOG_LEVEL_ALL);
    LogComponentDisable("TcpRxBuffer", LOG_LEVEL_ALL);
    LogComponentDisable("TcpTxBuffer", LOG_LEVEL_ALL);
    LogComponentDisable("Packet", LOG_LEVEL_DEBUG);
}
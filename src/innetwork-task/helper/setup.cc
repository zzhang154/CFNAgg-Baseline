//
// Note: For C++ code it is common to use a .cc or .cpp extension.
// This file implements all the functions declared in setup.h.

#include "setup.h"
#include "PacketTraceTag.h" // Include our custom packet trace tag header.
#include <ns3/packet.h>
#include <ns3/net-device.h>
#include <ns3/node.h>
#include <ns3/ipv4.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/node-container.h>
#include <ns3/address.h>
#include <ns3/callback.h>
#include <unordered_map>
#include <vector>
#include <string>

// Declare the NS-3 logging component if not already done.
NS_LOG_COMPONENT_DEFINE("Setup");

// -----------------------------------------------------------------------------
// Utility Functions Implementation
// -----------------------------------------------------------------------------

void PrintRoutingTables() {
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
                // If multiple routing protocols are installed, additional logic may be needed.
                for (uint32_t j = 0; j < listRouting->GetNRoutingProtocols(); ++j)
                {
                    // (Implementation for additional protocols can be added here.)
                }
            }
        }
    }
}

void NameNodes(NodeContainer &nodes, std::string baseName) {
    // Iterate over all nodes and assign names using baseName and index.
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
        std::stringstream ss;
        ss << baseName << i;
        // The following example uses Names::Add.
        Names::Add(ss.str(), nodes.Get(i));
    }
}

void CountRouterNodes(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
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
    file.close();

    std::cout << "uint32_t consumerNum = " << consumerNum << ";" << std::endl;
    std::cout << "uint32_t producerNum = " << producerNum << ";" << std::endl;
    std::cout << "uint32_t forwarderNum = " << forwarderNum << ";" << std::endl;
    std::cout << "uint32_t aggregatorNum = " << aggregatorNum << ";" << std::endl;
}

void BuildTopo(std::string &linkFile, NodeContainer &consumer, NodeContainer &producer, 
               NodeContainer &forwarder, NodeContainer &aggregator) {
    // Create nodes based on counted numbers
    consumer.Create(consumerNum);
    producer.Create(producerNum);
    forwarder.Create(forwarderNum);
    aggregator.Create(aggregatorNum);

    // Name nodes using predefined names
    NameNodes(consumer, conName);
    NameNodes(producer, proName);
    NameNodes(forwarder, fowName);
    NameNodes(aggregator, aggName);

    // Install internet stack on all nodes
    InternetStackHelper stack;
    stack.Install(consumer);
    stack.Install(producer);
    stack.Install(forwarder);
    stack.Install(aggregator);

    std::ifstream infile(linkFile);
    std::string line;
    PointToPointHelper p2p;
    Ipv4AddressHelper address;
    uint32_t ia = 1, ic = 1;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string node1, node2, dataRate, delay, queueSize;
        double lossRate;
        if (!(iss >> node1 >> node2 >> dataRate >> lossRate >> delay >> queueSize)) {
            continue; // Skip lines with errors
        }
        p2p.SetDeviceAttribute("DataRate", StringValue(dataRate));
        p2p.SetChannelAttribute("Delay", StringValue(delay));
        p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue(queueSize + "p"));

        if (Names::Find<Node>(node1) == nullptr || Names::Find<Node>(node2) == nullptr) {
            std::cout << "Name does not exist, node1 : " << node1 << " node2 : " << node2 << std::endl;
        }

        NodeContainer n1n2 = NodeContainer(Names::Find<Node>(node1), Names::Find<Node>(node2));
        NetDeviceContainer d1d2 = p2p.Install(n1n2);

        std::string ipBaseAddr;
        if (node1.find("agg") != std::string::npos || node2.find("agg") != std::string::npos) {
            ipBaseAddr = "10.2." + std::to_string(ia) + ".0";
            ++ia;
        } else {
            ipBaseAddr = "10.1." + std::to_string(ic) + ".0";
            ++ic;
        }
        address.SetBase(Ipv4Address(ipBaseAddr.c_str()), "255.255.255.0");
        address.Assign(d1d2);
        Ipv4Address node1Addr = n1n2.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
        Ipv4Address node2Addr = n1n2.Get(1)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
        std::cout << "---node1 ip--- " << node1 << " --- " << node1Addr
                  << " , node2 ip--- " << node2 << " --- " << node2Addr << std::endl;
        // Assuming ipToNodeName is defined elsewhere
        ipToNodeName[Ipv4AddressToString(node1Addr)] = node1;
        ipToNodeName[Ipv4AddressToString(node2Addr)] = node2;
    }
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    p2p.EnablePcapAll("/home/dd/wiresharkRecordFile/");
    infile.close();

    // Now install the forwarder trace callbacks so that each forwarder updates packet tags when forwarding.
    InstallForwarderTraceCallbacks(forwarder);
}

// -----------------------------------------------------------------------------
// Application Creation Functions Implementation
// -----------------------------------------------------------------------------
void Createpro(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node) {
    Ptr<Producer> producer = CreateObject<Producer>();
    // Clear consumer group addresses
    cGroup.clear();
    node->AddApplication(producer);
    producer->SetupProducer(port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    producer->SetStartTime(Seconds(starttime));
    producer->SetStopTime(Seconds(stoptime));
}

void Createagg(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node) {
    std::cout << "in Createagg ..." << std::endl;
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    if (ipv4) {
        for (uint32_t i = 0; i < ipv4->GetNInterfaces(); ++i) {
            // Optionally print or process non-loopback interfaces here.
        }
    }
    Ptr<Aggregator> aggregator = CreateObject<Aggregator>();
    node->AddApplication(aggregator);
    aggregator->SetupAggregator(port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    aggregator->SetStartTime(Seconds(starttime));
    aggregator->SetStopTime(Seconds(stoptime));
}

void Createcon(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node) {
    Ptr<Consumer> consumer = CreateObject<Consumer>();
    node->AddApplication(consumer);
    consumer->SetupConsumer(port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    consumer->SetStartTime(Seconds(starttime));
    consumer->SetStopTime(Seconds(stoptime));
}

void CreateAggGroup(std::string aggGroupFile, 
                    std::unordered_map<std::string, std::vector<std::string>> &aggGroups) {
    std::ifstream infile(aggGroupFile);
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> nodes;
        std::string nodeName, childName;
        iss >> nodeName;
        // Delete ':' at the end of nodeName if present.
        if (!nodeName.empty() && nodeName.back() == ':') {
            nodeName.pop_back();
        }
        while (iss >> childName) {
            nodes.push_back(childName);
        }
        aggGroups[nodeName] = nodes;
    }
}

void CreateAggTree(std::string &nodeName, std::vector<Address> pa,
                   std::unordered_map<std::string, std::vector<std::string>> &aggGroups,
                   uint8_t rank, uint16_t itr, uint32_t vsize, uint16_t server_port) {
    std::vector<Address> sGroup;
    Ptr<Ipv4> ipv4 = Names::Find<Node>(nodeName)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    Ipv4Address addr = iaddr.GetLocal();
    sGroup.push_back(addr);
    
    std::vector<Address> cGroup;  // Child nodes addresses.

    // Process children from the aggregator group mapping.
    for (std::string &child : aggGroups[nodeName]) {
        // You can add additional processing as needed.
    }

    if (nodeName.find("con") != std::string::npos)
        Createcon(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node>(nodeName));
    else if (nodeName.find("agg") != std::string::npos)
        Createagg(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node>(nodeName));
    else if (nodeName.find("pro") != std::string::npos)
        Createpro(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node>(nodeName));
}

void CreateAggTreeTopo(uint16_t itr, uint32_t vsize, uint16_t server_port) {
    std::unordered_map<std::string, std::vector<std::string>> aggGroups;
    CreateAggGroup(aggGroupFilePath, aggGroups);
    std::string root = "con0";
    std::vector<Address> emptyVector;
    CreateAggTree(root, emptyVector, aggGroups, 0, itr, vsize, server_port);
}

void CreateDirectTopo(NodeContainer &cons, NodeContainer &pros, uint16_t itr, 
                      uint32_t vsize, uint16_t server_port) {
    std::vector<Address> consumers;
    Ptr<Ipv4> ipv4 = cons.Get(0)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    Ipv4Address addr = iaddr.GetLocal();
    consumers.push_back(addr);

    std::vector<Address> producers;
    for (uint8_t i = 0; i < pros.GetN(); ++i) {
        Ptr<Ipv4> ipv4_pro = pros.Get(i)->GetObject<Ipv4>();
        Ipv4InterfaceAddress iaddr_pro = ipv4_pro->GetAddress(1, 0);
        producers.push_back(iaddr_pro.GetLocal());
    }
    
    uint8_t rank = 0;
    std::vector<Address> nullV;
    Ptr<Consumer> consumer = CreateObject<Consumer>();
    cons.Get(0)->AddApplication(consumer);
    consumer->SetupConsumer(server_port, itr, rank, vsize, nullV, producers, basetime, cc);
    consumer->SetStartTime(Seconds(starttime));
    consumer->SetStopTime(Seconds(stoptime));
}

// -----------------------------------------------------------------------------
// Logging Control Functions Implementation
// -----------------------------------------------------------------------------
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


/**
 * Functor: ForwarderTraceFunctor
 * Description:
 *   A functor that encapsulates the ForwardingTraceCallback with a captured NetDevice.
 */
class ForwarderTraceFunctor 
{
public:
  explicit ForwarderTraceFunctor(ns3::Ptr<ns3::NetDevice> dev)
    : m_dev(dev)
  {
  }
  void operator()(ns3::Ptr<const ns3::Packet> packet) const
  {
    ForwardingTraceCallback(packet, m_dev);
  }
private:
  ns3::Ptr<ns3::NetDevice> m_dev;
};

/**
 * Function: ForwardingTraceCallback
 * Description:
 *    The trace callback to update the PacketTraceTag on a packet at a forwarder.
 *    This function accepts both the packet (from the trace source) and the NetDevice,
 *    which is pre-bound via MakeBoundCallback.
 */
void ForwardingTraceCallback(ns3::Ptr<const ns3::Packet> packet, ns3::Ptr<ns3::NetDevice> dev)
{
    // Make a copy of the packet to work on modifying its tags.
    ns3::Ptr<ns3::Packet> pktCopy = packet->Copy();

    ns3::PacketTraceTag tag;
    if (pktCopy->PeekPacketTag(tag))
    {
        // Obtain the node from the device.
        ns3::Ptr<ns3::Node> node = dev->GetNode();
        ns3::Ptr<ns3::Ipv4> ipv4 = node->GetObject<ns3::Ipv4>();
        if (ipv4)
        {
            // For simplicity, we use interface 0; adjust if necessary.
            ns3::Ipv4Address nodeIp = ipv4->GetAddress(0, 0).GetLocal();
            // Append the forwarder's IP and the current simulation time to the tag.
            tag.AddEntry(nodeIp, ns3::Simulator::Now());
            // Remove the old tag and attach the updated tag.
            pktCopy->RemovePacketTag(tag);
            pktCopy->AddPacketTag(tag);
            NS_LOG_INFO("Forwarder " << nodeIp << " updated packet trace: " << tag);
        }
    }
}


// void ForwarderRxCallback(Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface) {
//     // 检查数据包是否包含 PacketTraceTag
//     PacketTraceTag tag;
//     if (packet->PeekPacketTag(tag)) {
//         // 获取当前节点的 IP 地址和名称
//         Ipv4Address addr = ipv4->GetAddress(interface, 0).GetLocal();
//         std::string nodeName = Names::FindName(ipv4->GetObject<Node>());
        
//         // 添加当前节点的信息到标签
//         tag.AddEntry(addr, Simulator::Now());
        
//         // 输出信息
//         std::cout << "Forwarder Node [" << nodeName << "] received packet at " 
//                   << Simulator::Now() << " with trace: " << tag << std::endl;
        
//         // 更新标签（如果需要继续传递）
//         const_cast<Packet*>(packet.Get())->ReplacePacketTag(tag);
//     }
//     else
//     {
//         std::cout << "Forwarder Node received packet without trace tag." << std::endl;
//     }
// }

// This callback is invoked each time a packet is received on the forwarder node’s device.
bool
ForwarderRxCallback (Ptr<NetDevice> device, Ptr<const Packet> packet,
                     uint16_t protocol, const Address &sender)
{
  std::cout << "Entering the ForwarderRxCallback" << std::endl;

  // Make a (non-const) copy of the packet if you plan to modify its tag.
  Ptr<Packet> pkt = packet->Copy ();
  
  ns3::PacketTraceTag tag;
  if (pkt->PeekPacketTag (tag))
    {
      // Optionally, add a new trace entry.
      // Here we assume that the device has an Ipv4 object installed.
      Ptr<Ipv4> ipv4 = device->GetNode ()->GetObject<Ipv4> ();
      Ipv4Address myAddr = ipv4->GetAddress (1, 0).GetLocal ();
      tag.AddEntry (myAddr, Simulator::Now ());
      
      // Output the tag info to std::cout.
      std::cout << "Forwarder node " << Names::FindName (device->GetNode ())
                << " received packet with tag: " << tag << std::endl;
      
      // (Optionally) remove and re-add the tag if you wish to update the packet.
      pkt->RemovePacketTag (tag);
      pkt->AddPacketTag (tag);
    }
  else
    {
      std::cout << "Forwarder node " << Names::FindName (device->GetNode ())
                << " received packet without PacketTraceTag" << std::endl;
    }
  // Return true to indicate that the packet should be further processed.
  return true;
}



/**
 * Function: InstallForwarderTraceCallbacks
 * Description:
 *    Installs trace callbacks on all NetDevices in the given NodeContainer.
 *    This uses ns3::MakeBoundCallback to bind the dev parameter to ForwardingTraceCallback.
 */
void InstallForwarderTraceCallbacks(ns3::NodeContainer &forwarder)
{
    for (uint32_t i = 0; i < forwarder.GetN(); i++)
    {

        // Ptr<Node> node = forwarder.Get(i);
        // Ptr<Ipv4L3Protocol> ipv4 = node->GetObject<Ipv4L3Protocol>();
        
        // // 绑定 IPv4 层的接收回调
        // ipv4->TraceConnectWithoutContext("Rx", MakeCallback(&ForwarderRxCallback));

        
        Ptr<Node> node = forwarder.Get (i);
        // For each net device installed on the forwarder node...
        for (uint32_t j = 0; j < node->GetNDevices (); ++j)
        {
            Ptr<NetDevice> nd = node->GetDevice (j);
            // Set the callback; this makes the device call ForwarderRxCallback
            nd->SetReceiveCallback (MakeCallback (&ForwarderRxCallback));
        }
        
    }
}
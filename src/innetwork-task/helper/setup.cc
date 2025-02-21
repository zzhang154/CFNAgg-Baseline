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

#include "ns3/global-data.h"

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

/**
 * @brief Assigns names to nodes in a NodeContainer
 * @param nodes Container of nodes to name
 * @param baseName Base name for node naming convention
 */
void NameNodes(NodeContainer &nodes, std::string baseName) {
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
        std::stringstream ss;
        ss << baseName << i;
        Names::Add(ss.str(), nodes.Get(i));
    }
}

/**
 * @brief Counts different node types from topology file
 * @param filename Input file containing node configurations
 */
void CountRouterNodes(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

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
    file.close();

    std::cout << "uint32_t consumerNum = " << consumerNum << ";\n"
              << "uint32_t producerNum = " << producerNum << ";\n"
              << "uint32_t forwarderNum = " << forwarderNum << ";\n"
              << "uint32_t aggregatorNum = " << aggregatorNum << ";\n" << std::flush;
}

/**
 * @brief Builds network topology from link configuration file
 * @param linkFile File containing link definitions
 * @param consumer Container for consumer nodes
 * @param producer Container for producer nodes
 * @param forwarder Container for forwarder nodes
 * @param aggregator Container for aggregator nodes
 */
void BuildTopo(std::string &linkFile, NodeContainer &consumer, NodeContainer &producer, 
               NodeContainer &forwarder, NodeContainer &aggregator) {
    // Node creation and naming
    consumer.Create(consumerNum);
    producer.Create(producerNum);
    forwarder.Create(forwarderNum);
    aggregator.Create(aggregatorNum);

    NameNodes(consumer, conName);
    NameNodes(producer, proName);
    NameNodes(forwarder, fowName);
    NameNodes(aggregator, aggName);

    // Network stack installation
    InternetStackHelper stack;
    stack.Install(consumer);
    stack.Install(producer);
    stack.Install(forwarder);
    stack.Install(aggregator);

    // Link configuration
    std::ifstream infile(linkFile);
    std::string line;
    PointToPointHelper p2p;
    Ipv4AddressHelper address;
    
    // Initialize counters for each node type
    uint32_t proSecondOctet = 0, proThirdOctet = 1;      // Producers: 10.0-7.x.0 (2048 subnets), starting at 10.0.1.0 (con0 uses 10.0.0.0)
    uint32_t aggSecondOctet = 8, aggThirdOctet = 0;      // Aggregators: 10.8-15.x.0
    uint32_t forwarderSecondOctet = 16, forwarderThirdOctet = 0; // Forwarders: 10.16-23.x.0
    bool con0Assigned = false; // Flag to ensure con0's subnet is assigned only once

    while (std::getline(infile, line)) {
        // std::cout << "Processing line: " << line << std::endl;
        std::istringstream iss(line);
        std::string node1, node2, dataRate, delay, queueSize;
        double linkCost;
        if (!(iss >> node1 >> node2 >> dataRate >> linkCost >> delay >> queueSize)) {
            continue; // Skip malformed lines
        }

        // Configure P2P link parameters
        p2p.SetDeviceAttribute("DataRate", StringValue(dataRate));
        // p2p.SetChannelAttribute("LossRate", StringValue(0.01));
        p2p.SetChannelAttribute("Delay", StringValue(delay));
        p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue(queueSize + "p"));

        // Node validation
        if (Names::Find<Node>(node1) == nullptr || Names::Find<Node>(node2) == nullptr) {
            std::cout << "Invalid node names: " << node1 << " - " << node2 << std::endl;
            continue;
        }

        // Device installation and IP assignment
        NodeContainer n1n2 = NodeContainer(Names::Find<Node>(node1), Names::Find<Node>(node2));
        NetDeviceContainer d1d2 = p2p.Install(n1n2);

        // Instead of using only 10.1.x.0, cycle through ranges like 10.1.x.0, 10.3.x.0, etc., by incrementing coreSubnet when ic exceeds 255.
        // Check if this link involves the special consumer "con0"
        std::string ipBaseAddr;
        bool isCon0 = (node1 == "con0" || node2 == "con0");
        bool isPro = (node1.find("pro") != std::string::npos) || 
                     (node2.find("pro") != std::string::npos);
        bool isAgg = (node1.find("agg") != std::string::npos) || 
                     (node2.find("agg") != std::string::npos);
        bool isForwarder = !isPro && !isAgg; // True only if BOTH nodes are forwarders

        if (isCon0) {
            if (con0Assigned) {
                NS_FATAL_ERROR("con0 subnet already assigned.");
            }
            ipBaseAddr = "10.0.0.0"; // Dedicated producer subnet for con0
            con0Assigned = true;
        } 
        else if (isPro) {
            // Producer subnets: 10.0-7.x.0
            ipBaseAddr = "10." + std::to_string(proSecondOctet) + "." + std::to_string(proThirdOctet) + ".0";
            proThirdOctet++;
            if (proThirdOctet > 255) {
                proThirdOctet = 0;
                proSecondOctet++;
                if (proSecondOctet > 7) NS_FATAL_ERROR("Producer subnets exhausted.");
            }
        } 
        else if (isAgg) {
            // Aggregator subnets: 10.8-15.x.0 (includes aggregator-forwarder links)
            ipBaseAddr = "10." + std::to_string(aggSecondOctet) + "." + std::to_string(aggThirdOctet) + ".0";
            aggThirdOctet++;
            if (aggThirdOctet > 255) {
                aggThirdOctet = 0;
                aggSecondOctet++;
                if (aggSecondOctet > 15) NS_FATAL_ERROR("Aggregator subnets exhausted.");
            }
        } 
        else if (isForwarder) {
            // Forwarder-forwarder links: 10.16-23.x.0
            ipBaseAddr = "10." + std::to_string(forwarderSecondOctet) + "." + std::to_string(forwarderThirdOctet) + ".0";
            forwarderThirdOctet++;
            if (forwarderThirdOctet > 255) {
                forwarderThirdOctet = 0;
                forwarderSecondOctet++;
                if (forwarderSecondOctet > 23) {
                    NS_FATAL_ERROR("Forwarder subnets exhausted.");
                }
            }
        }

        address.SetBase(Ipv4Address(ipBaseAddr.c_str()), "255.255.255.0");
        address.Assign(d1d2);

        // Store IP-to-Node mappings
        Ipv4Address node1Addr = n1n2.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
        Ipv4Address node2Addr = n1n2.Get(1)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
        // std::cout << "---node1 ip--- " << node1 << " --- " << node1Addr << ", node2 ip--- " << node2 << " --- " << node2Addr << std::endl;
        ipToNodeName[Ipv4AddressToString(node1Addr)] = node1;
        ipToNodeName[Ipv4AddressToString(node2Addr)] = node2;

        // Inside the link configuration loop:
        if (node1.find("pro") != std::string::npos || node2.find("pro") != std::string::npos) {
            // Determine which NetDevice belongs to the non-pro0 node
            int targetDeviceIndex = (node1.find("pro") != std::string::npos) ? 1 : 0; // pro0 is sender → apply loss to receiver

            Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
            em->SetAttribute("ErrorRate", DoubleValue(MyConfig::GetLossRate()));
            em->SetAttribute("ErrorUnit", EnumValue(RateErrorModel::ERROR_UNIT_PACKET));

            // Apply error model to the receiver's device (non-pro0 side)
            d1d2.Get(targetDeviceIndex)->SetAttribute("ReceiveErrorModel", PointerValue(em));
        }
    }

    // Finalize network setup
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    p2p.EnablePcapAll("/home/dd/wiresharkRecordFile/");
    infile.close();
    InstallForwarderPromiscCallbacks(forwarder);
    InstallForwarderPromiscCallbacks(aggregator);
    InstallForwarderPromiscCallbacks(consumer);

    // Comment both of them would be choose for Qbic
    // Set TCP congestion control for ALL nodes
    // SetTcpCongestionControl(TcpBbr::GetTypeId(), consumer, producer, forwarder, aggregator); // Use BBR
    // OR
    // SetTcpCongestionControl(TcpNewReno::GetTypeId(), consumer, producer, forwarder, aggregator); // Use NewReno (AIMD)
    // SetTcpCongestionControl(TcpAIMD::GetTypeId(), consumer, producer, forwarder, aggregator); 

    TypeId tcpTypeId = TypeId::LookupByName(MyConfig::GetCongestionControl());
    SetTcpCongestionControl(tcpTypeId, consumer, producer, forwarder, aggregator);
}

// *****************************************************************************
// Application Creation Functions Implementation
// *****************************************************************************
/**
 * @brief Creates and sets up a Producer application.
 * @param port The port number.
 * @param itr Number of iterations.
 * @param rank Rank of the node.
 * @param vsize Vector size parameter.
 * @param sGroup Source group addresses.
 * @param cGroup Consumer group addresses.
 * @param node The node to install the application on.
 */
void Createpro(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node) {
    NS_LOG_INFO("Createpro: Setting up Producer application on node " << Names::FindName(node));
    Ptr<Producer> producer = CreateObject<Producer>();
    cGroup.clear();
    node->AddApplication(producer);
    producer->SetupProducer(port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    producer->SetStartTime(Seconds(starttime));
    producer->SetStopTime(Seconds(stoptime));
}

/**
 * @brief Creates and sets up an Aggregator application.
 * @param port The port number.
 * @param itr Number of iterations.
 * @param rank Rank of the node.
 * @param vsize Vector size parameter.
 * @param sGroup Source group addresses.
 * @param cGroup Consumer group addresses.
 * @param node The node to install the application on.
 */
void Createagg(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node) {
    NS_LOG_INFO("Createagg: Setting up Aggregator application on node " << Names::FindName(node));
    Ptr<Aggregator> aggregator = CreateObject<Aggregator>();
    node->AddApplication(aggregator);
    aggregator->SetupAggregator(port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    aggregator->SetStartTime(Seconds(starttime));
    aggregator->SetStopTime(Seconds(stoptime));
}

/**
 * @brief Creates and sets up a Consumer application.
 * @param port The port number.
 * @param itr Number of iterations.
 * @param rank Rank of the node.
 * @param vsize Vector size parameter.
 * @param sGroup Source group addresses.
 * @param cGroup Consumer group addresses.
 * @param node The node to install the application on.
 */
void Createcon(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node) {
    NS_LOG_INFO("Createcon: Setting up Consumer application on node " << Names::FindName(node));
    Ptr<Consumer> consumer = CreateObject<Consumer>();
    node->AddApplication(consumer);
    consumer->SetupConsumer(port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    consumer->SetStartTime(Seconds(starttime));
    consumer->SetStopTime(Seconds(stoptime));
}

/**
 * @brief Reads an aggregator group file and creates aggregator groups.
 * @param aggGroupFile The file containing aggregator groups.
 * @param aggGroups A mapping from aggregator node names to child node names.
 */
void CreateAggGroup(std::string aggGroupFile, 
                    std::unordered_map<std::string, std::vector<std::string>> &aggGroups) {
    NS_LOG_INFO("CreateAggGroup: Reading aggregator group file " << aggGroupFile);
    std::ifstream infile(aggGroupFile);
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> nodes;
        std::string nodeName, childName;
        iss >> nodeName;
        if (!nodeName.empty() && nodeName.back() == ':') {
            nodeName.pop_back();
        }
        while (iss >> childName) {
            nodes.push_back(childName);
        }
        aggGroups[nodeName] = nodes;
    }
}

/**
 * @brief Recursively builds the aggregator tree topology.
 * @param nodeName The name of the current node.
 * @param pa Parent node addresses.
 * @param aggGroups Mapping of aggregator groups.
 * @param rank Rank of the node.
 * @param itr Number of iterations.
 * @param vsize Vector size parameter.
 * @param server_port The server port.
 */
void CreateAggTree(std::string &nodeName, std::vector<Address> pa,
                   std::unordered_map<std::string, std::vector<std::string>> &aggGroups,
                   uint8_t rank, uint16_t itr, uint32_t vsize, uint16_t server_port) {
    NS_LOG_INFO("CreateAggTree: Processing node " << nodeName);
    std::vector<Address> sGroup;
    Ptr<Node> node = Names::Find<Node>(nodeName);
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
    Ipv4Address addr = iaddr.GetLocal();
    sGroup.push_back(addr);

    std::vector<Address> cGroup;
    auto it = aggGroups.find(nodeName);
    if (it != aggGroups.end()) {
        for (std::string &child : it->second) {
            Ptr<Node> childNode = Names::Find<Node>(child);
            if (childNode != nullptr) {
                Ptr<Ipv4> childIpv4 = childNode->GetObject<Ipv4>();
                if (childIpv4) {
                    Ipv4InterfaceAddress childAddr = childIpv4->GetAddress(1, 0);
                    cGroup.push_back(childAddr.GetLocal());
                }
                CreateAggTree(child, sGroup, aggGroups, rank, itr, vsize, server_port);
            }
        }
    }

    if (nodeName.find("con") != std::string::npos)
        Createcon(server_port, itr, rank, vsize, pa, cGroup, node);
    else if (nodeName.find("agg") != std::string::npos)
        Createagg(server_port, itr, rank, vsize, pa, cGroup, node);
    else if (nodeName.find("pro") != std::string::npos)
        Createpro(server_port, itr, rank, vsize, pa, cGroup, node);
    else if (nodeName.find("for") != std::string::npos){
        node->SetAttribute("IpForward", BooleanValue(true));
        std::cout << "node->SetAttribute(IpForward, BooleanValue(true));";
    }
}

/**
 * @brief Creates the full aggregator tree topology.
 * @param itr Number of iterations.
 * @param vsize Vector size parameter.
 * @param server_port The server port.
 */
void CreateAggTreeTopo(uint16_t itr, uint32_t vsize, uint16_t server_port) {
    std::unordered_map<std::string, std::vector<std::string>> aggGroups;
    CreateAggGroup(aggGroupFilePath, aggGroups);
    std::string root = "con0";
    std::vector<Address> emptyVector;
    CreateAggTree(root, emptyVector, aggGroups, 0, itr, vsize, server_port);
}

/**
 * @brief Creates a direct topology connecting consumer and producer nodes.
 * @param cons Container for consumer nodes.
 * @param pros Container for producer nodes.
 * @param itr Number of iterations.
 * @param vsize Vector size parameter.
 * @param server_port The server port.
 */
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

// *****************************************************************************
// Logging Control Functions Implementation
// *********************************************************************************
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


// *****************************************************************************
// Forwarder and Trace Callback Functions Implementation
// *********************************************************************************

/**
 * @brief Callback for generic packet forwarding events.
 * @param packet The packet being forwarded.
 * @param dev The NetDevice on which the packet is forwarded.
 */
void ForwardingTraceCallback(Ptr<const Packet> packet, Ptr<NetDevice> dev) {
    NS_LOG_INFO("ForwardingTraceCallback: Device " << dev->GetInstanceTypeId().GetName()
                << " forwarded a packet at time " << Simulator::Now());
}

/**
 * @brief IP-layer Rx callback for forwarder nodes.
 * @param packet The received packet.
 * @param ipv4 The IPv4 object of the node.
 * @param interface The interface index.
 */
void ForwarderIpRxCallback(Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface) {
    NS_LOG_INFO("ForwarderIpRxCallback: Node " << Names::FindName(ipv4->GetObject<Node>())
                << " receives packet on interface " << interface
                << " at time " << Simulator::Now());
}

/**
 * @brief Installs the IP-layer Rx trace callback on all forwarder nodes.
 * @param forwarder Container of forwarder nodes.
 */
void InstallForwarderIpTraceCallbacks(NodeContainer &forwarder) {
    NS_LOG_INFO("Installing IP Rx trace callbacks on forwarder nodes...");
    for (uint32_t i = 0; i < forwarder.GetN(); ++i) {
        Ptr<Node> node = forwarder.Get(i);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (ipv4) {
            ipv4->TraceConnectWithoutContext("Rx", MakeCallback(&ForwarderIpRxCallback));
        }
    }
}

/**
 * @brief IP-layer Tx callback for forwarder nodes.
 * @param packet The packet being transmitted.
 * @param ipv4 The IPv4 object of the node.
 * @param interface The interface index.
 */
void ForwarderIpTxCallback(Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface) {
    Ptr<Packet> pkt = packet->Copy();
    Ipv4Header ipHeader;
    pkt->PeekHeader(ipHeader);
    Ipv4Address receiver = ipHeader.GetDestination();

    Ptr<Node> node = ipv4->GetObject<Node>();
    std::string nodeName = Names::FindName(node);
    NS_LOG_INFO("ForwarderIpTxCallback: Node " << nodeName 
                << " transmits packet to " << receiver 
                << " at time " << Simulator::Now());
}

/**
 * @brief Installs the IP-layer Tx trace callback on all forwarder nodes.
 * @param forwarder Container of forwarder nodes.
 */
void InstallForwarderIpTxTraceCallbacks(NodeContainer &forwarder) {
    NS_LOG_INFO("Installing IP Tx trace callbacks on forwarder nodes...");
    for (uint32_t i = 0; i < forwarder.GetN(); ++i) {
        Ptr<Node> node = forwarder.Get(i);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (ipv4) {
            ipv4->TraceConnectWithoutContext("Tx", MakeCallback(&ForwarderIpTxCallback));
        }
    }
}

/**
 * @brief Promiscuous-mode callback for forwarder NetDevices.
 * @param device The NetDevice that received the packet.
 * @param packet The received packet.
 * @param protocol The protocol number.
 * @param sender The sender's address.
 * @param receiver The intended recipient's address.
 * @param packetType The packet type.
 * @return true Always returns true to indicate the packet has been handled.
 */

bool ForwarderPromiscRxCallback(Ptr<NetDevice> device, Ptr<const Packet> packet,
                                uint16_t protocol, const Address &sender,
                                const Address &receiver, NetDevice::PacketType packetType)
{
    // 1. Create a mutable copy of the original packet
    Ptr<Packet> pktCopy = packet->Copy();
    
    // 2. Remove existing tag from the COPY
    PacketTraceTag tag;
    if (pktCopy->RemovePacketTag(tag)) {
        // 3. Modify the tag contents
        Ptr<Node> node = device->GetNode();
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (ipv4) {
            Ipv4Address nodeIp = ipv4->GetAddress(1, 0).GetLocal();
            tag.AddEntry(nodeIp, Simulator::Now());
            
            // --- Custom Log Start ---
            // Create a copy to peek into the IP header
            Ipv4Header ipHeader;
            if (pktCopy->PeekHeader(ipHeader)) {
                Ipv4Address dest = ipHeader.GetDestination();
                std::string nodeName = Names::FindName(node);
                NS_LOG_INFO("ForwarderPromiscRxCallback: Node " << nodeName 
                            << " updates tag and transmits packet to " << dest 
                            << " at time " << Simulator::Now());
            }
            // Log the current tag info
            NS_LOG_INFO("ForwarderPromiscRxCallback: Updated PacketTraceTag: " << tag);

            // --- Custom Log End ---

            // Store the log in the global data structure
            AddToTraceRecord(tag);
            
            /* Zhuoxu: No nned to do this at now
            // 4. Add modified tag back to the COPY
            modifiedPacket->AddPacketTag(tag);
            
            // 5. Transmit the modified packet instead of original
            device->Send(modifiedPacket, receiver, protocol);
            
            // 6. Return false to suppress original packet processing
            return false;
            */
        }
    }
    // 7. Return true to allow original packet processing if no modifications
    return true;
}

/**
 * @brief Installs promiscuous-mode callbacks on all forwarder nodes.
 * @param forwarder Container of forwarder nodes.
 */
void InstallForwarderPromiscCallbacks(NodeContainer &forwarder) {
    NS_LOG_INFO("Installing promiscuous callbacks on forwarder nodes...");
    for (uint32_t i = 0; i < forwarder.GetN(); ++i) {
        Ptr<Node> node = forwarder.Get(i);
        for (uint32_t j = 0; j < node->GetNDevices(); ++j) {
            Ptr<NetDevice> nd = node->GetDevice(j);
            NS_LOG_INFO("Setting promiscuous callback on device " 
                        << nd->GetInstanceTypeId().GetName()
                        << ", name id: " << Names::FindName(nd->GetNode()));
            nd->SetPromiscReceiveCallback(
                MakeCallback<bool, Ptr<NetDevice>, Ptr<const Packet>,
                             uint16_t, const Address&, const Address&,
                             NetDevice::PacketType>(&ForwarderPromiscRxCallback)
            );
        }
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (ipv4) {
            ipv4->SetAttribute("IpForward", BooleanValue(true));  // Enable forwarding
            NS_LOG_INFO("Enabled IP forwarding on node: " << node->GetId());
        } else {
            NS_LOG_ERROR("Node " << node->GetId() << " has no IPv4 stack installed!");
        }
    }
}

    // Function to set TCP congestion control for all nodes in given containers
void SetTcpCongestionControl(const TypeId& typeId, 
                             const NodeContainer& consumer, 
                             const NodeContainer& producer,
                             const NodeContainer& forwarder,
                             const NodeContainer& aggregator) {
    // Set CC for all consumer nodes
    for (uint32_t i = 0; i < consumer.GetN(); ++i) {
        Ptr<Node> node = consumer.Get(i);
        node->GetObject<TcpL4Protocol>()->SetAttribute("SocketType", TypeIdValue(typeId));
    }

    // Set CC for all producer nodes
    for (uint32_t i = 0; i < producer.GetN(); ++i) {
        Ptr<Node> node = producer.Get(i);
        node->GetObject<TcpL4Protocol>()->SetAttribute("SocketType", TypeIdValue(typeId));
    }

    // Set CC for all forwarder nodes
    for (uint32_t i = 0; i < forwarder.GetN(); ++i) {
        Ptr<Node> node = forwarder.Get(i);
        node->GetObject<TcpL4Protocol>()->SetAttribute("SocketType", TypeIdValue(typeId));
    }

    // Set CC for all aggregator nodes
    for (uint32_t i = 0; i < aggregator.GetN(); ++i) {
        Ptr<Node> node = aggregator.Get(i);
        node->GetObject<TcpL4Protocol>()->SetAttribute("SocketType", TypeIdValue(typeId));
    }
}
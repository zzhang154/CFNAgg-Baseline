#ifndef SETUP_H
#define SETUP_H

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

// User application headers
#include "ns3/consumer.h"
#include "ns3/producer.h"
#include "ns3/aggregator.h"
#include "ns3/parameter.h"
#include "ns3/utils.h"
#include "ns3/PacketTraceTag.h"
#include "ns3/parameter.h"

using namespace ns3;

// *****************************************************************************
// External Variables
// *****************************************************************************
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

// *****************************************************************************
// Utility Functions
// *****************************************************************************

/**
 * @brief Returns the current working directory.
 * @return A string containing the current working directory.
 */
inline std::string GetCurrentWorkingDir() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

/**
 * @brief Prints the routing tables for all nodes.
 */
void PrintRoutingTables();

/**
 * @brief Names nodes in a NodeContainer using a base name.
 * @param nodes The container of nodes.
 * @param baseName The base name used for naming.
 */
void NameNodes(NodeContainer &nodes, std::string baseName);

/**
 * @brief Counts different types of router nodes from a file.
 * @param filename The file containing node information.
 */
void CountRouterNodes(const std::string& filename);

/**
 * @brief Builds the network topology from the provided link file.
 * @param linkFile The file describing links.
 * @param consumer NodeContainer for consumer nodes.
 * @param producer NodeContainer for producer nodes.
 * @param forwarder NodeContainer for forwarder nodes.
 * @param aggregator NodeContainer for aggregator nodes.
 */
void BuildTopo(std::string &linkFile, NodeContainer &consumer, NodeContainer &producer, 
               NodeContainer &forwarder, NodeContainer &aggregator);

// *****************************************************************************
// Application Creation Functions
// *****************************************************************************
/**
 * @brief Creates and configures a Producer application.
 */
void Createpro(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node);

/**
 * @brief Creates and configures an Aggregator application.
 */
void Createagg(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node);

/**
 * @brief Creates and configures a Consumer application.
 */
void Createcon(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node);

/**
 * @brief Creates aggregator groups from a file.
 */
void CreateAggGroup(std::string aggGroupFile, 
                    std::unordered_map<std::string, std::vector<std::string>> &aggGroups);

/**
 * @brief Recursively builds the aggregator tree topology.
 */
void CreateAggTree(std::string &nodeName, std::vector<Address> pa,
                   std::unordered_map<std::string, std::vector<std::string>> &aggGroups,
                   uint8_t rank, uint16_t itr, uint32_t vsize, uint16_t server_port);

/**
 * @brief Creates an Aggregator Tree topology.
 */
void CreateAggTreeTopo(uint16_t itr, uint32_t vsize, uint16_t server_port);

/**
 * @brief Creates a Direct Topology connecting consumer and producer nodes.
 */
void CreateDirectTopo(NodeContainer &cons, NodeContainer &pros, uint16_t itr, 
                      uint32_t vsize, uint16_t server_port);

// *****************************************************************************
// Logging Control Functions
// *****************************************************************************
/**
 * @brief Enables logging for key NS-3 components.
 */
void EnableLoggingComponents();

/**
 * @brief Disables logging for key NS-3 components.
 */
void DisableLoggingComponents();

// *****************************************************************************
// Forwarder and Trace Callback Functions
// *****************************************************************************

/**
 * @brief Installs trace callbacks on forwarder nodes for packet forwarding events.
 */
void InstallForwarderTraceCallbacks(NodeContainer &forwarder);

/**
 * @brief Callback function used for forwarding trace events.
 * @param packet The packet being forwarded.
 * @param dev The device that forwarded the packet.
 */
void ForwardingTraceCallback(Ptr<const Packet> packet, Ptr<NetDevice> dev);

/**
 * @brief Callback for IP-layer packet reception on forwarder nodes.
 *        Signature expected by Ipv4L3Protocol "Rx" trace source.
 */
void ForwarderIpRxCallback (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);

/**
 * @brief Installs the IP-layer trace callback on forwarder nodes.
 */
void InstallForwarderIpTraceCallbacks (NodeContainer &forwarder);

/**
 * @brief Callback for IP-layer packet transmission events.
 */
void ForwarderIpTxCallback (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);

/**
 * @brief Installs the IP-layer transmission trace callback on forwarder nodes.
 */
void InstallForwarderIpTxTraceCallbacks (NodeContainer &forwarder);

/**
 * @brief Promiscuous mode callback for NetDevices on forwarder nodes.
 * @return true if the packet was processed.
 */
bool ForwarderPromiscRxCallback (Ptr<NetDevice> device, Ptr<const Packet> packet,
                                 uint16_t protocol, const Address &sender,
                                 const Address &receiver, NetDevice::PacketType packetType);

/**
 * @brief Installs the promiscuous callback on all forwarder nodes.
 */
void InstallForwarderPromiscCallbacks (NodeContainer &forwarder);

#endif // SETUP_H
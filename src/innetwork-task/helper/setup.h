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

// Zhuoxu: no need to use the absolute path
#include "ns3/consumer.h"
#include "ns3/producer.h"
#include "ns3/aggregator.h"
#include "ns3/parameter.h"
#include "ns3/utils.h"
#include "ns3/PacketTraceTag.h"

using namespace ns3;

// -----------------------------------------------------------------------------
// External Variables
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Utility Functions
// -----------------------------------------------------------------------------
/**
 * @brief Gets the current working directory.
 *
 * @return A string representing the current working directory.
 */
inline std::string GetCurrentWorkingDir() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
    } 

/**
 * @brief Outputs routing tables for all nodes in the simulation.
 */
void PrintRoutingTables();

/**
 * @brief Assigns names to nodes using a base name.
 *
 * @param nodes A container of nodes.
 * @param baseName The base name used for naming nodes.
 */
void NameNodes(NodeContainer &nodes, std::string baseName);

/**
 * @brief Counts different types of router nodes from a file.
 *
 * The file is read line by line and counts are updated based on line prefixes.
 *
 * @param filename Path to the file containing node information.
 */
void CountRouterNodes(const std::string& filename);

/**
 * @brief Builds the network topology from a link file.
 *
 * This function creates NodeContainers for consumer, producer, forwarder, 
 * and aggregator nodes, assigns names, installs the internet stack, and sets up 
 * point-to-point links with IP addressing.
 *
 * @param linkFile The file that describes links between nodes.
 * @param consumer NodeContainer for consumer nodes.
 * @param producer NodeContainer for producer nodes.
 * @param forwarder NodeContainer for forwarder nodes.
 * @param aggregator NodeContainer for aggregator nodes.
 */
void BuildTopo(std::string &linkFile, NodeContainer &consumer, NodeContainer &producer, 
               NodeContainer &forwarder, NodeContainer &aggregator);

// -----------------------------------------------------------------------------
// Application Creation Functions
// -----------------------------------------------------------------------------
/**
 * @brief Creates and configures a Producer application.
 *
 * @param port The port number.
 * @param itr Iteration count.
 * @param rank Application rank.
 * @param vsize Virtual size parameter.
 * @param sGroup Sender group addresses.
 * @param cGroup Consumer group addresses.
 * @param node The node on which the application is installed.
 */
void Createpro(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node);

/**
 * @brief Creates and configures an Aggregator application.
 *
 * @param port The port number.
 * @param itr Iteration count.
 * @param rank Application rank.
 * @param vsize Virtual size parameter.
 * @param sGroup Sender group addresses.
 * @param cGroup Consumer group addresses.
 * @param node The node on which the application is installed.
 */
void Createagg(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node);

/**
 * @brief Creates and configures a Consumer application.
 *
 * @param port The port number.
 * @param itr Iteration count.
 * @param rank Application rank.
 * @param vsize Virtual size parameter.
 * @param sGroup Sender group addresses.
 * @param cGroup Consumer group addresses.
 * @param node The node on which the application is installed.
 */
void Createcon(uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, 
               std::vector<Address> &sGroup, std::vector<Address> &cGroup, Ptr<Node> node);

/**
 * @brief Creates Aggregator groups from an aggregator group file.
 *
 * @param aggGroupFile File containing aggregator group definitions.
 * @param aggGroups Mapping of group names to a vector of node names.
 */
void CreateAggGroup(std::string aggGroupFile, 
                    std::unordered_map<std::string, std::vector<std::string>> &aggGroups);

/**
 * @brief Recursively creates an Aggregator Tree topology.
 *
 * Based on the given aggregator groups, this function creates the tree topology 
 * by creating applications on the nodes.
 *
 * @param nodeName The root node name.
 * @param pa Parent address vector.
 * @param aggGroups Mapping of aggregator groups.
 * @param rank Rank of the application.
 * @param itr Iteration count.
 * @param vsize Virtual size parameter.
 * @param server_port Port number.
 */
void CreateAggTree(std::string &nodeName, std::vector<Address> pa,
                   std::unordered_map<std::string, std::vector<std::string>> &aggGroups,
                   uint8_t rank, uint16_t itr, uint32_t vsize, uint16_t server_port);

/**
 * @brief Creates an Aggregator Tree Topology.
 *
 * This function wraps CreateAggGroup and CreateAggTree to build the full tree topology.
 *
 * @param itr Iteration count.
 * @param vsize Virtual size parameter.
 * @param server_port Port number.
 */
void CreateAggTreeTopo(uint16_t itr, uint32_t vsize, uint16_t server_port);

/**
 * @brief Creates a Direct Topology connecting consumer and producer nodes.
 *
 * @param cons NodeContainer for consumer nodes.
 * @param pros NodeContainer for producer nodes.
 * @param itr Iteration count.
 * @param vsize Virtual size parameter.
 * @param server_port Port number.
 */
void CreateDirectTopo(NodeContainer &cons, NodeContainer &pros, uint16_t itr, 
                      uint32_t vsize, uint16_t server_port);

// -----------------------------------------------------------------------------
// Logging Control Functions
// -----------------------------------------------------------------------------
/**
 * @brief Enables logging for key components.
 */
void EnableLoggingComponents();

/**
 * @brief Disables logging for key components.
 */
void DisableLoggingComponents();

// New function to install forwarding trace callbacks on the forwarder nodes.
void InstallForwarderTraceCallbacks(NodeContainer &forwarder);

// Declaration of the trace callback used by forwarder nodes.
void ForwardingTraceCallback(Ptr<const Packet> packet, Ptr<NetDevice> dev);

#endif // SETUP_H
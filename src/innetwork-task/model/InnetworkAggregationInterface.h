#ifndef INNETWORK_AGG_INTERFACE_H
#define INNETWORK_AGG_INTERFACE_H

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <fstream>

// ns3 headers
#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/ipv4-address.h"
#include "ns3/applications-module.h"
#include "ns3/parameter.h"
#include "ns3/utils.h"
#include "ns3/vectorop.h"
#include "ns3/TCPclient.h"
#include "ns3/TCPserver.h"
#include "ns3/PacketTraceTag.h"

namespace ns3 {

// Forward declarations
class DataChunk;
class TCPclient;
class TCPserver;

typedef std::map<std::string, std::vector<uint64_t>> ChildChunkMap;

class InnetworkAggregationInterface : public Object {
public:
    // Core infrastructure
    static TypeId GetTypeId(void);
    InnetworkAggregationInterface();
    virtual ~InnetworkAggregationInterface();

    // Configuration interface
    void SetupInnetworkAggregationInterface(uint16_t port, uint16_t itr,
                                          uint8_t aggTreeLevel,
                                          std::vector<Address>& sGroup,
                                          std::vector<Address>& cGroup,
                                          Ptr<Node> node, bool isEnd);
    void SetVSize(uint32_t size);

    // Network management
    void CreateSocketPool(std::string cc_name);
    void ReceiveDataFromAll();
    void TriggerHandleRead();

    // Data operations
    int SendResponseVToP(std::vector<uint64_t>& vec, uint16_t iterationNum,
                       std::string fromStr = "");
    void ProduceVToP(uint16_t iterationNum);
    int SendResponseVTo(std::string toStr, std::vector<uint64_t>& vec,
                      uint16_t iterationNum, std::string fromStr = "");
    int SendPacket(std::string toStr, uint16_t iterationNum,
                 std::vector<uint8_t>& serializeVec, std::string fromStr = "");
    void HandleSendFailure(std::string toStr, uint16_t iterNum, std::vector<uint8_t>& buf,
                        std::string fromStr);

    // Data processing
    void ReceiveDataFrom(std::string fromStr);
    void UpdateQueue(std::string fromStr);

    // Monitoring & debugging
    bool PrintCompInfo(uint16_t iterationNum);
    void PrintAllInfo();
    void TraceSingleNodeInfo();
    bool CheckQueueOrder(std::queue<uint16_t> q, uint16_t maxIteration);
    void PrintRxBuffer(std::string chStr);
    
    // Utility functions
    uint16_t HashIpToPort(const std::string& ip);

private:
    // Configuration parameters
    uint8_t aggTreeLevel;
    uint16_t m_peerPort;
    uint32_t vsize;
    Ptr<Node> node;
    uint16_t iterationNum;
    uint16_t maxIteration;
    bool isEnd;

    // Network state
    std::vector<Address> cGroup;  // Child nodes (clients)
    std::vector<Address> sGroup;  // Parent nodes (servers)
    std::map<std::string, Ptr<Application>> socketPool;

    // Data processing state
    uint16_t currentIteration;
    std::queue<uint16_t> compQueue;
    std::map<uint16_t, DataChunk> iterChunk;
    std::queue<uint16_t> successIter;
    
    // Transmission tracking
    std::set<uint16_t> sendSuccToPSet;
    std::map<uint16_t, std::string> sendSchedSet;

    // Output handling
    std::ofstream outFile;
    std::string thisAddress;
    int printCount;
    Time startTime;
};

} // namespace ns3

#endif /* INNETWORK_AGG_INTERFACE_H */

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "ns3/application.h"
#include "ns3/internet-module.h"
#include "ns3/packet-loss-counter.h"
#include "ns3/parameter.h"
#include "ns3/utils.h"
#include "ns3/vectorop.h"
#include <queue>
#include <map>
#include <list>

namespace ns3 {

// Forward declarations
class DataChunk;
class ReceivedChunk;

enum PacketState {
    FIRST_PACKET,
    SECOND_PACKET
};

class TCPserver : public Application
{
public:
    // Lifecycle Management
    static TypeId GetTypeId(void);
    TCPserver();
    TCPserver(Ptr<Node> node);
    virtual ~TCPserver();

    // Application lifecycle
    virtual void StartApplication(void) override;
    virtual void StopApplication(void) override;
    virtual void DoInitialize(void) override;
    virtual void DoDispose(void) override;

    // Configuration Interface
    void Bind(uint16_t port);
    void SetParams(Ptr<Node> node, uint16_t size, 
                  std::map<uint16_t, DataChunk>* iterChunk,
                  std::string peerIpAddrStr, 
                  std::string localIpAddrStr,
                  std::queue<uint16_t>* compQueuePtr);
    void SetCongestionControlAlgorithm(std::string cc_name);
    void SetPacketWindowSize(uint16_t size);

    // Status Reporting
    uint32_t GetLost(void) const;
    uint64_t GetReceived(void) const;
    uint16_t GetPacketWindowSize() const;
    Ptr<Socket> GetSocket();
    Address GetBindAddress();
    InetSocketAddress GetLocalAddress() const;

    // Data Processing
    void CheckChComp(uint16_t iterationNum);
    int CheckMemory();
    void DoChComp(uint16_t iterationNum);
    void ProcessPerPkt();

    // Utility Functions called by the main program
    void CallSendEmptyPacket();
    void CallHandleRead();
    void PrintAppTable();

private:
    // Internal Processing
    void HandleRead(Ptr<Socket> socket);
    bool CheckHeader(uint8_t* packetContent, int len);
    
    // Helper Functions
    void InitializeSocket();
    void LogSocketAddress(Ptr<Socket> socket);

    // Member Variables
    Ptr<Socket> m_socket;
    Ptr<Node> m_node;
    PacketLossCounter m_lossCounter;
    uint64_t m_received;
    uint16_t m_port;
    Address m_bindIp;
    Address m_peerAddress;
    
    // Data processing structures
    std::queue<uint16_t> compQueue;
    std::map<uint16_t, ReceivedChunk> iterChunkMap;
    std::map<uint16_t, DataChunk>* iterChunkPtr;
    std::queue<uint16_t>* compQueuePtr;
    
    // Buffer management
    std::map<std::string, uint16_t> m_iterationMap;
    std::map<std::string, uint8_t*> m_bufferMap;
    std::map<std::string, uint32_t> m_bufferPtrMap;
    uint8_t* m_buffer;
    uint32_t m_bufferPtr;
    uint32_t m_pktPtr;
    uint16_t m_iteration;

    // Configuration parameters
    std::string LocalAddressStr;
    std::string ipAddressStr;
    uint16_t cGroupSize;
    PacketState m_packetState;
    std::string congestionControlAlgorithm;
};

} // namespace ns3

#endif /* TCP_SERVER_H */

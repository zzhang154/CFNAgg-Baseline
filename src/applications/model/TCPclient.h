#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/ipv4-address.h"
#include "ns3/event-id.h"
#include "ns3/PacketTraceTag.h"

#include "ns3/tcp-socket-base.h"            // For TcpSocketBase
#include "ns3/ipv4-routing-protocol.h"        // For Ipv4RoutingProtocol
#include "ns3/ipv4-header.h"                  // For Ipv4Header

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup tcpclientserver
 * \brief TCP client application for establishing connections and exchanging data with servers
 *
 * Handles socket creation, connection management, data transmission, and reception.
 */
class TCPclient : public Application
{
public:
    static TypeId GetTypeId(void);
    TCPclient();
    virtual ~TCPclient();

    // Configuration methods
    void Bind(uint16_t port);
    void SetRemote(Address ip, uint16_t port);
    void SetRemote(Address addr);
    void SetNode(Ptr<Node> node);
    void SetCongestionControlAlgorithm(std::string cc_name);
    void SetSendTag(PacketTraceTag tag);

    // Socket operations
    int Send(const uint8_t* buffer, size_t len, bool isProducer);
    void CreateSocket(Ptr<Node> node, uint16_t port);
    
    // Informational methods
    InetSocketAddress GetLocalAddress() const;
    Ptr<Socket> GetSocket() { return m_socket; }
    uint16_t GetBindPort();
    Address GetBindAddress();
    Ipv4Address GetIpv4LocalAddress();

    // Debugging utilities
    void PrintSocketInfo(Ptr<Socket> socket);
    void CheckSocketState();
    void LogSocketInfo();

protected:
    virtual void DoDispose(void) override;

private:
    virtual void StartApplication(void) override;
    virtual void StopApplication(void) override;

    // Packet handling
    void HandleRead(Ptr<Socket> socket);
    void RecvPacket(Ptr<Socket> socket);
    void SetIpTable();

    // Connection management
    void connectToserver();
    void ReceiveData();

    // Member variables
    Ptr<Socket> m_socket = nullptr;
    Ptr<Node> m_node = nullptr;
    Address m_bindIp;
    uint16_t m_bindPort;
    uint16_t m_peerPort = 0;
    Address m_peerIp;
    Address m_peerAddress;
    PacketTraceTag m_sendTag;
    std::string congestionControlAlgorithm = "reno";

    // Traffic generation parameters
    uint32_t m_count;
    uint32_t m_rcount;
    Time m_interval;
    uint32_t m_size;
    uint32_t m_sent;
    EventId m_sendEvent;
    uint32_t m_numStreams = 1;
    uint32_t m_lastUsedStream = 1;
    bool m_running = false;
};

} // namespace ns3

#endif /* TCP_CLIENT_H */

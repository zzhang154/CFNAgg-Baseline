#include "TCPclient.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/inet-socket-address.h"
#include "ns3/seq-ts-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("TCPclient");
NS_OBJECT_ENSURE_REGISTERED(TCPclient);

TypeId TCPclient::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::TCPclient")
        .SetParent<Application>()
        .SetGroupName("Applications")
        .AddConstructor<TCPclient>();
    return tid;
}

TCPclient::TCPclient() : m_count(0), m_rcount(0), m_sent(0) {}

TCPclient::~TCPclient() = default;

// Configuration methods implementation
void TCPclient::SetRemote(Address ip, uint16_t port)
{
    NS_LOG_FUNCTION(this << ip << port);
    m_peerIp = ip;
    m_peerPort = port;
    m_peerAddress = InetSocketAddress(Ipv4Address::ConvertFrom(m_peerIp), m_peerPort);
}

void TCPclient::SetRemote(Address addr)
{
    NS_LOG_FUNCTION(this << addr);
    m_peerAddress = addr;
}

void TCPclient::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this << node);
    m_node = node;
}

void TCPclient::SetCongestionControlAlgorithm(std::string cc_name)
{
    NS_LOG_FUNCTION(this << cc_name);
    congestionControlAlgorithm = cc_name;
}

void TCPclient::SetSendTag(PacketTraceTag tag)
{
    NS_LOG_FUNCTION(this);
    m_sendTag = tag;
}

// Socket management implementation
void TCPclient::Bind(uint16_t port)
{
    NS_LOG_FUNCTION(this << port);
    m_running = true;
    m_bindPort = port;

    if(!m_socket) {
        m_socket = Socket::CreateSocket(m_node, TcpSocketFactory::GetTypeId());
        InetSocketAddress local = GetLocalAddress();
        
        if(m_socket->Bind(local) == -1) {
            NS_FATAL_ERROR("Failed to bind socket");
        }
        
        m_socket->Connect(InetSocketAddress::ConvertFrom(m_peerAddress));
        m_socket->SetRecvCallback(MakeCallback(&TCPclient::HandleRead, this));
        m_socket->SetAllowBroadcast(true);
    }

    CheckSocketState();
    LogSocketInfo();
    SetIpTable();
}

// Data transmission implementation
int TCPclient::Send(const uint8_t* buffer, size_t len, bool isProducer)
{
    NS_LOG_FUNCTION(this << len);
    NS_ASSERT(m_sendEvent.IsExpired());

    Ptr<Packet> p = Create<Packet>(buffer, len);
    m_sendTag.AddEntry(GetIpv4LocalAddress(), Simulator::Now());
    p->AddPacketTag(m_sendTag);

    int sentSize = m_socket->Send(p);
    if(sentSize >= 0) {
        NS_LOG_INFO("Sent " << sentSize << " bytes to " << m_peerAddress);
        ++m_sent;
    } else {
        NS_LOG_WARN("Failed to send " << len << " bytes to " << m_peerAddress);
    }

    m_lastUsedStream = (m_lastUsedStream % m_numStreams) + 1;
    return sentSize;
}

void TCPclient::CreateSocket(Ptr<Node> node, uint16_t port)
{
    NS_LOG_FUNCTION(this << node << port);
    m_node = node;
    m_bindPort = port;
    Bind(port);
}

// Packet reception implementation
void TCPclient::HandleRead(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    if(!m_running) return;

    Address from;
    Ptr<Packet> packet;
    uint32_t totalReceive = 0;

    while((packet = socket->RecvFrom(from))) {
        uint32_t packetSize = packet->GetSize();
        packet->RemoveAllPacketTags();
        packet->RemoveAllByteTags();
        
        uint8_t* packetContent = new uint8_t[packetSize];
        packet->CopyData(packetContent, packetSize);
        delete[] packetContent;

        totalReceive += packetSize;
        m_rcount++;
    }

    NS_LOG_INFO("Received " << totalReceive << " bytes from " << 
                InetSocketAddress::ConvertFrom(from).GetIpv4());
}

void TCPclient::RecvPacket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    // Actual handling done in HandleRead()
}

InetSocketAddress
TCPclient::GetLocalAddress() const {
    Ptr<Node> node=m_node;
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
    Ipv4Address local_ip = ipv4->GetAddress (1, 0).GetLocal();
    return InetSocketAddress{local_ip,m_bindPort}; 
}

Ipv4Address TCPclient::GetIpv4LocalAddress()
{
    Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4>();
    return ipv4->GetAddress(1, 0).GetLocal();
}

Address TCPclient::GetBindAddress()
{
    return InetSocketAddress(GetIpv4LocalAddress(), m_bindPort);
}

// Debugging utilities implementation
void TCPclient::PrintSocketInfo(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    Address localAddress;
    socket->GetSockName(localAddress);
    InetSocketAddress inetLocal = InetSocketAddress::ConvertFrom(localAddress);
    NS_LOG_INFO("Socket IP: " << inetLocal.GetIpv4() << " Port: " << inetLocal.GetPort());
}

void TCPclient::SetIpTable() {
  // Get node's IP and socket's bound IP
  Ipv4Address local_ip = m_node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
  Address socketAddress;
  m_socket->GetObject<TcpSocketBase>()->GetSockName(socketAddress);
  Ipv4Address tcp_ip = InetSocketAddress::ConvertFrom(socketAddress).GetIpv4();

  NS_LOG_INFO("IPs - Local: " << local_ip << " Socket: " << tcp_ip);

  if (tcp_ip != local_ip) {
      // Convert IPs to strings using stringstream
      std::stringstream ss;
      ss << tcp_ip;  // Convert socket IP
      std::string new_ip = ss.str();
      ss.str("");    // Clear stream
      ss << local_ip;// Convert local IP
      NewToOldIpMap[new_ip] = ss.str();
      
      NS_LOG_DEBUG("IP mismatch detected");
      // PrintIpToNodeMap();
      // PrintNewToOldIpMap();
  }
}

void TCPclient::LogSocketInfo()
{
    if(Ptr<TcpSocketBase> tcpSocket = m_socket->GetObject<TcpSocketBase>()) {
        Address local, peer;
        m_socket->GetSockName(local);
        m_socket->GetPeerName(peer);
        
        NS_LOG_INFO("Local: " << InetSocketAddress::ConvertFrom(local).GetIpv4() << ":" <<
                    InetSocketAddress::ConvertFrom(local).GetPort());
        NS_LOG_INFO("Peer: " << InetSocketAddress::ConvertFrom(peer).GetIpv4() << ":" <<
                    InetSocketAddress::ConvertFrom(peer).GetPort());
    }
}

void TCPclient::CheckSocketState()
{
    if(m_socket && m_socket->GetObject<TcpSocketBase>()) {
        TcpSocket::TcpStates_t state = m_socket->GetObject<TcpSocketBase>()->GetState();
        NS_LOG_INFO("Socket state: " << TcpSocket::TcpStateName[state]);
    }
}

// Application lifecycle management
void TCPclient::StartApplication()
{
    NS_LOG_FUNCTION(this);
    m_running = true;
}

void TCPclient::StopApplication()
{
    NS_LOG_FUNCTION(this);
    m_running = false;
    Simulator::Cancel(m_sendEvent);
    
    if(m_socket) {
        m_socket->Close();
        m_socket = nullptr;
    }
}

void TCPclient::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_socket = nullptr;
    m_node = nullptr;
    Application::DoDispose();
}

} // namespace ns3

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007,2008,2009 INRIA, UDCAST
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Amine Ismail <amine.ismail@sophia.inria.fr>
 *                      <amine.ismail@udcast.com>
 */
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "TCPclient.h"
#include "seq-ts-header.h"
#include <cstdlib>
#include <cstdio>

#include <ns3/tcp-socket-factory.h>
#include <ns3/callback.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TCPclient");

NS_OBJECT_ENSURE_REGISTERED (TCPclient);

TypeId
TCPclient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TCPclient")
    .SetParent<Application> ()
    .SetGroupName ("Applications")
    .AddConstructor<TCPclient> ()
  ;
  return tid;
}

TCPclient::TCPclient () {
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_rcount = 0;
  m_socket = nullptr ;
  m_lastUsedStream = 1;
  m_numStreams = 1;
  m_sendEvent = EventId ();
}

TCPclient::~TCPclient () {
  NS_LOG_FUNCTION (this);
  m_socket = nullptr;
  m_node = nullptr;
}

void
TCPclient::SetRemote (Address ip, uint16_t port) {
  NS_LOG_FUNCTION (this << Ipv4Address::ConvertFrom(ip) << port);
  m_peerIp = ip;
  m_peerPort = port;
  m_peerAddress = InetSocketAddress(Ipv4Address::ConvertFrom(m_peerIp),m_peerPort);
}

void
TCPclient::SetRemote (Address addr) {
  NS_LOG_FUNCTION (this << addr);
  m_peerAddress = addr;
}

void
TCPclient::SetNode (Ptr<Node> node) {
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

void 
TCPclient::SetCongestionControlAlgorithm(std::string cc_name) {
  congestionControlAlgorithm = cc_name;
}


InetSocketAddress 
TCPclient::GetLocalAddress() const {
    Ptr<Node> node=m_node;
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
    Ipv4Address local_ip = ipv4->GetAddress (1, 0).GetLocal();
    return InetSocketAddress{local_ip,m_bindPort}; 
}


Ptr<Socket> 
TCPclient::GetSocket() {
  return m_socket;
}

uint16_t 
TCPclient::GetBindPort() {
  return m_bindPort;
}

Address 
TCPclient::GetBindAddress() {
  m_bindIp=GetLocalAddress().GetIpv4();
  return m_bindIp;
}


void
TCPclient::DoDispose (void) {
  NS_LOG_FUNCTION (this);

  m_socket = nullptr;
  m_node = nullptr;
  //p = nullptr;
  Application::DoDispose ();
}

void 
TCPclient::StartApplication() {
    m_running=true;
    NS_LOG_INFO ("client start application success");
}

void 
TCPclient::StopApplication() {
  //p = nullptr;
  m_running=false;
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
  if (m_socket != nullptr )
    {
      m_socket->Close ();
      m_socket = nullptr ;
    }
}

void 
TCPclient::Bind (uint16_t port) {
  NS_LOG_FUNCTION (this<<port);
  m_running=true;
  m_bindPort = port;

  if(m_node == nullptr)
      NS_LOG_DEBUG(this << " m_node is nullptr");
  else
      NS_LOG_DEBUG(this << " m_node is not nullptr");
      
  if (m_socket == nullptr ) {
    m_socket = Socket::CreateSocket(m_node, TcpSocketFactory::GetTypeId());

    if (!m_socket) {
        NS_FATAL_ERROR("m_socket not found!");
        return;
    }

    //bind port to socket
    InetSocketAddress local = GetLocalAddress();
    if (m_socket->GetObject<TcpSocketBase>()->Bind (local) == -1) {
      NS_FATAL_ERROR ("Failed to bind socket");
    }
    m_socket->GetObject<TcpSocketBase>()->Connect(InetSocketAddress::ConvertFrom(m_peerAddress));

    NS_LOG_DEBUG("Create Socket, m_node: " << m_node << " m_bindPort: " << static_cast<int>(m_bindPort));
    NS_LOG_DEBUG( "m_socket->Connect(InetSocketAddress::ConvertFrom(m_peerAddress)); "
          << InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4() << " "
          << InetSocketAddress::ConvertFrom(m_peerAddress).GetPort() );
  }
  m_socket->SetRecvCallback (MakeCallback(&TCPclient::HandleRead,this));
  m_socket->SetAllowBroadcast (true);
  // Check the state of the socket
  CheckSocketState();
  // make sure that the "LogSocketInfo()" can only be called after the socket is binded.
  LogSocketInfo();
}


void
TCPclient::PrintSocketInfo(Ptr<Socket> socket)
{
    Address localAddress;
    socket->GetSockName(localAddress);
    InetSocketAddress inetLocalAddress = InetSocketAddress::ConvertFrom(localAddress);
    Ipv4Address localIp = inetLocalAddress.GetIpv4();
    uint16_t localPort = inetLocalAddress.GetPort();

    NS_LOG_INFO("Socket created with IP address: " << localIp << " and port: " << localPort);
}

Ipv4Address 
TCPclient::GetIpv4LocalAddress(){
  // Ptr<TcpSocketBase> tcpSocket = m_socket->GetObject<TcpSocketBase>();
  // // Get local address and port
  // Address localAddress;
  // tcpSocket->GetSockName(localAddress);
  // InetSocketAddress inetLocalAddress = InetSocketAddress::ConvertFrom(localAddress);
  // Ipv4Address localIp = inetLocalAddress.GetIpv4();
  // return localIp;

  Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4> ();
  Ipv4Address localIp = ipv4->GetAddress(1,0).GetLocal();
  return localIp;
}

void
TCPclient::connectToserver(){}

void
TCPclient::ReceiveData(){}


int
TCPclient::Send(const uint8_t* buffer, size_t len, bool isProducer) {
  NS_LOG_FUNCTION (this<<len);

  NS_LOG_INFO ("Before send, Check the socket state");
  CheckSocketState();
  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Packet> p = Create<ns3::Packet>(buffer,len);

  // agg the tag here.
  m_sendTag.AddEntry(GetIpv4LocalAddress(),Simulator::Now());
  // std::cout << "In TCPclient::Send: " << m_sendTag << std::endl;
  p->AddPacketTag(m_sendTag);

  int sentSize = m_socket->Send (p,0);
  if (sentSize >= 0) {
      ++m_sent;
      NS_LOG_INFO ("TraceDelay TX " << sentSize << " bytes to " << m_peerAddress<< " Uid: " << p->GetUid () << " Time: " << (Simulator::Now ()).GetSeconds ());
  }
  else {
      LogSocketInfo();
      NS_LOG_INFO ("Error while sending " << sentSize << " bytes to " << m_peerAddress );
  }

  m_lastUsedStream++;
  if (m_lastUsedStream > m_numStreams) {
      m_lastUsedStream = 1;
  }
  /*std::cout<<"socket state after TCPclient::Send-------"<<"TCPclient----"<<GetLocalAddress().GetIpv4()
  <<"----state:"<<m_socket->GetObject<QuicSocketBase>()->GetSocketState()<<"---with conID---"
  <<m_socket->GetObject<QuicSocketBase>()->GetConnectionId()
  <<"--at--time--"<<Simulator::Now()<<std::endl;*/
  return sentSize;
}

void 
TCPclient::HandleRead (Ptr<Socket> socket) {
  NS_LOG_FUNCTION (this);
  if (!m_running) {return ;}
  Address from;
  bool isReading=true;
  uint32_t totalReceive = 0;
  
  //uint8_t zero=0;
  while (isReading) {  
    Ptr<Packet> packet = socket->RecvFrom (from);
    if(!packet){    
      isReading =false;
      break;
    }
    uint32_t packetSize=packet->GetSize () ;
    packet->RemoveAllPacketTags ();
    packet->RemoveAllByteTags ();
    m_peerAddress = from;

    totalReceive = totalReceive +packetSize;
    m_rcount ++;
    NS_LOG_FUNCTION (this << socket <<packetSize);

    uint8_t* packetContent = new uint8_t[packetSize];
    uint32_t copyedSize =  packet->CopyData(packetContent,packetSize);
    delete [] packetContent;
    packet = nullptr;
  }
    std::cout<<"TCPclient---"<<GetLocalAddress().GetIpv4()<<"-received---response---from--"<<InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4()<<std::endl;
    std::cout<<"total received---packet--num---"<<m_rcount<<" Total Receive: "<<totalReceive<<std::endl;
    //m_circularBuffer->print();
    this->m_socket = socket;
        
}

void TCPclient::RecvPacket(Ptr<Socket> socket){
    if (!m_running) {return ;}
}


void
TCPclient::LogSocketInfo()
{
    Ptr<TcpSocketBase> tcpSocket = m_socket->GetObject<TcpSocketBase>();
    if (tcpSocket != nullptr)
    {
        // Get local address and port
        Address localAddress;
        tcpSocket->GetSockName(localAddress);
        InetSocketAddress inetLocalAddress = InetSocketAddress::ConvertFrom(localAddress);
        Ipv4Address localIp = inetLocalAddress.GetIpv4();
        uint16_t localPort = inetLocalAddress.GetPort();

        // Get peer address and port
        Address peerAddress;
        tcpSocket->GetPeerName(peerAddress);
        InetSocketAddress inetPeerAddress = InetSocketAddress::ConvertFrom(peerAddress);
        Ipv4Address peerIp = inetPeerAddress.GetIpv4();
        uint16_t peerPort = inetPeerAddress.GetPort();

        NS_LOG_INFO("Local IP: " << localIp << ", Local Port: " << localPort);
        NS_LOG_INFO("Peer IP: " << peerIp << ", Peer Port: " << peerPort);
    }
    else
    {
        NS_LOG_INFO("TcpSocketBase is null");
    }
}

void
TCPclient::CheckSocketState()
{
    // NS_LOG_INFO (this);
    if (m_socket != nullptr)
    {
        Ptr<TcpSocketBase> tcpSocket = m_socket->GetObject<TcpSocketBase>();
        if (tcpSocket != nullptr)
        {
            TcpSocket::TcpStates_t state = tcpSocket->GetState();
            // NS_LOG_INFO("Socket state: " << TcpSocket::TcpStateName[state]);

            switch (state)
            {
            case TcpSocket::CLOSED:
                NS_LOG_INFO("Socket state is CLOSED");
                break;
            case TcpSocket::LISTEN:
                NS_LOG_INFO("Socket state is LISTEN");
                break;
            case TcpSocket::SYN_SENT:
                NS_LOG_INFO("Socket state is SYN_SENT");
                break;
            case TcpSocket::SYN_RCVD:
                NS_LOG_INFO("Socket state is SYN_RCVD");
                break;
            case TcpSocket::ESTABLISHED:
                NS_LOG_INFO("Socket state is ESTABLISHED");
                break;
            case TcpSocket::CLOSE_WAIT:
                NS_LOG_INFO("Socket state is CLOSE_WAIT");
                break;
            case TcpSocket::LAST_ACK:
                NS_LOG_INFO("Socket state is LAST_ACK");
                break;
            case TcpSocket::FIN_WAIT_1:
                NS_LOG_INFO("Socket state is FIN_WAIT_1");
                break;
            case TcpSocket::FIN_WAIT_2:
                NS_LOG_INFO("Socket state is FIN_WAIT_2");
                break;
            case TcpSocket::CLOSING:
                NS_LOG_INFO("Socket state is CLOSING");
                break;
            case TcpSocket::TIME_WAIT:
                NS_LOG_INFO("Socket state is TIME_WAIT");
                break;
            default:
                NS_LOG_INFO("Socket state is UNKNOWN");
                break;
            }
        }
        else
        {
            NS_LOG_INFO("Socket is not a TcpSocketBase or m_tcb is null");
        }
    }
    else
    {
        NS_LOG_INFO("Socket is not created");
    }
}

void
TCPclient::SetSendTag(PacketTraceTag tag)
{
  NS_LOG_FUNCTION(this);
  // Simply copy the tag into the member variable
  this->m_sendTag = tag;
}

} // Namespace ns3

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
#include "quic-myclient.h"
#include "seq-ts-header.h"
#include <cstdlib>
#include <cstdio>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("QuicMyClient");

NS_OBJECT_ENSURE_REGISTERED (QuicMyClient);

TypeId
QuicMyClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicMyClient")
    .SetParent<Application> ()
    .SetGroupName ("Applications")
    .AddConstructor<QuicMyClient> ()
    .AddAttribute ("MaxPackets",
                   "The maximum number of packets the application will send",
                   UintegerValue (100),
                   MakeUintegerAccessor (&QuicMyClient::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between packets", TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&QuicMyClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&QuicMyClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&QuicMyClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("LocalPort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&QuicMyClient::m_bindPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. The minimum packet size is 12 bytes which is the size of the header carrying the sequence number and the time stamp.",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&QuicMyClient::m_size),
                   MakeUintegerChecker<uint32_t> (12,1500))
    .AddAttribute ("NumStreams",
                   "Number of streams to be used in the underlying QUIC socket",
                   UintegerValue (1),
                   MakeUintegerAccessor (&QuicMyClient::m_numStreams),
                   MakeUintegerChecker<uint32_t> (1,20)) // TODO check the max value  
  ;
  return tid;
}

QuicMyClient::QuicMyClient () {
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_rcount = 0;
  m_socket = nullptr ;
  m_lastUsedStream = 1;
  m_numStreams = 1;
  m_sendEvent = EventId ();
  quicSocketFactory=nullptr;
  m_circularBuffer = Create<CircularBuffer>(); 
}

QuicMyClient::~QuicMyClient () {
  NS_LOG_FUNCTION (this);
  m_socket = nullptr;
  m_node = nullptr;
  m_quicL4 = nullptr;
  quicSocketFactory=nullptr;
  m_circularBuffer = nullptr;
}

void
QuicMyClient::SetRemote (Address ip, uint16_t port) {
  NS_LOG_FUNCTION (this << Ipv4Address::ConvertFrom(ip) << port);
  m_peerIp = ip;
  m_peerPort = port;
  m_peerAddress = InetSocketAddress(Ipv4Address::ConvertFrom(m_peerIp),m_peerPort);
}

void
QuicMyClient::SetRemote (Address addr) {
  NS_LOG_FUNCTION (this << addr);
  m_peerAddress = addr;
}

void
QuicMyClient::SetNode (Ptr<Node> node) {
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

void 
QuicMyClient::SetCongestionControlAlgorithm(std::string cc_name) {
  congestionControlAlgorithm = cc_name;
}


InetSocketAddress 
QuicMyClient::GetLocalAddress() const {
    Ptr<Node> node=m_node;
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
    Ipv4Address local_ip = ipv4->GetAddress (1, 0).GetLocal();
    return InetSocketAddress{local_ip,m_bindPort}; 
}


Ptr<Socket> 
QuicMyClient::GetSocket() {
  return m_socket;
}

uint16_t 
QuicMyClient::GetBindPort() {
  return m_bindPort;
}

Address 
QuicMyClient::GetBindAddress() {
  m_bindIp=GetLocalAddress().GetIpv4();
  return m_bindIp;
}

Ptr<CircularBuffer> 
QuicMyClient::GetBuffer() {
  return m_circularBuffer;
}

void
QuicMyClient::DoDispose (void) {
  NS_LOG_FUNCTION (this);

  m_socket = nullptr;
  m_node = nullptr;
  m_quicL4 = nullptr;
  quicSocketFactory=nullptr;
  //p = nullptr;
  //m_quicL4 = nullptr;
  Application::DoDispose ();
}

void 
QuicMyClient::StartApplication() {

    m_running=true;
    //Simulator::Schedule(MilliSeconds(10), &QuicClientApp::SendHello, this);
    NS_LOG_INFO ("client start application success");
    

}

void 
QuicMyClient::StopApplication() {

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
QuicMyClient::Bind (uint16_t port) {
  NS_LOG_FUNCTION (this<<port);
  m_running=true;
  m_bindPort = port;
  if (m_socket == nullptr ) {
    CreateSocket(m_node,m_bindPort);
    m_socket->Connect(InetSocketAddress::ConvertFrom(m_peerAddress));
    std::cout << "m_socket->Connect(InetSocketAddress::ConvertFrom(m_peerAddress)); "
          << InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4() << " "
          << InetSocketAddress::ConvertFrom(m_peerAddress).GetPort() << std::endl;
  }
  m_socket->SetRecvCallback (MakeCallback(&QuicMyClient::HandleRead,this));
  m_socket->SetAllowBroadcast (true);
}


void 
QuicMyClient::CreateSocket(Ptr<Node> node,uint16_t port) {
  if (m_socket == nullptr ) {
    // QuicL4Protocol
    m_quicL4 = CreateObject<QuicL4Protocol>();
    m_quicL4->SetNode(node);
    if (!m_quicL4) {
      NS_FATAL_ERROR("QuicL4Protocol instance not found!");
      return;
    }

    // QuicSocketFactory 
    quicSocketFactory=CreateObject<QuicSocketFactory>();
    quicSocketFactory->SetQuicL4(m_quicL4);
    m_socket = quicSocketFactory->CreateSocket();
    //m_socket->GetObject<QuicSocketBase>()->SetAttribute("RTO",TimeValue (Seconds (30.0)));
    //m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kMinRTOTimeout",TimeValue (MilliSeconds (50)));

    m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kUsingTimeLossDetection",BooleanValue (true));
    m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kTimeReorderingFraction",DoubleValue (29 / 8));
    //m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->m_kMaxPacketsReceivedBeforeAckSend = 10;
    m_socket->GetObject<QuicSocketBase>()->SetAttribute("LegacyCongestionControl",BooleanValue (true));
    m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kReorderingThreshold",UintegerValue (5));
    //m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kMaxPacketsReceivedBeforeAckSend",UintegerValue (10));
    
    
    if (!m_socket) {
        NS_FATAL_ERROR("m_socket not found!");
        return;
    }

    //bind port to socket
    InetSocketAddress local = GetLocalAddress();
    if (m_socket->Bind (local) == -1) {
        NS_FATAL_ERROR ("Failed to bind socket");
      }

    
    
    // congestion strategy  
    /*Ptr<QuicSocketState> m_tcb = m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb();
    uint32_t maxPacketsBeforeAck = 50;
    m_tcb->SetAttribute("kMaxPacketsReceivedBeforeAckSend", UintegerValue(maxPacketsBeforeAck));
    double TimeReorderingFraction = 1.2;
    m_tcb->SetAttribute("kTimeReorderingFraction", DoubleValue(TimeReorderingFraction));
    uint32_t ReorderingThreshold = 5;
    m_tcb->SetAttribute("kReorderingThreshold", UintegerValue(ReorderingThreshold));
    uint32_t maxTLPs = 3;
    m_tcb->SetAttribute("kMaxTLPs", UintegerValue(maxTLPs));*/
    if(congestionControlAlgorithm == "bbr") {
      Ptr<QuicCongestionOps> quicbbr = CreateObject<QuicBbr>();
      double highGain = 2.5;//3.0
      quicbbr->SetAttribute("HighGain", ns3::DoubleValue(highGain));
      quicbbr->SetAttribute("BwWindowLength", UintegerValue(20));
      m_socket->GetObject<QuicSocketBase>()->SetCongestionControlAlgorithm(quicbbr);
    }

  }
}


int
QuicMyClient::Send(const uint8_t* buffer, size_t len) {
  NS_LOG_FUNCTION (this<<len);
  NS_ASSERT (m_sendEvent.IsExpired ());
  Ptr<Packet> p = Create<ns3::Packet>(buffer,len);
  int sentSize = m_socket->Send (p,0);
  if (sentSize >= 0) {
      ++m_sent;
      NS_LOG_INFO ("TraceDelay TX " << sentSize << " bytes to " << m_peerAddress<< " Uid: " << p->GetUid () << " Time: " << (Simulator::Now ()).GetSeconds ());
  }
  else {
      NS_LOG_INFO ("Error while sending " << sentSize << " bytes to " << m_peerAddress );
  }

  m_lastUsedStream++;
  if (m_lastUsedStream > m_numStreams) {
      m_lastUsedStream = 1;
  }
  /*std::cout<<"socket state after QuicMyClient::Send-------"<<"QuicMyClient----"<<GetLocalAddress().GetIpv4()
  <<"----state:"<<m_socket->GetObject<QuicSocketBase>()->GetSocketState()<<"---with conID---"
  <<m_socket->GetObject<QuicSocketBase>()->GetConnectionId()
  <<"--at--time--"<<Simulator::Now()<<std::endl;*/
  return sentSize;
}



void 
QuicMyClient::HandleRead (Ptr<Socket> socket) {
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
      m_circularBuffer->write(packetContent,copyedSize);
      delete [] packetContent;
      packet = nullptr;
  }
    std::cout<<"QuicMyClient---"<<GetLocalAddress().GetIpv4()<<"-received---response---from--"<<InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4()<<std::endl;
    std::cout<<"total received---packet--num---"<<m_rcount<<" Total Receive: "<<totalReceive<<std::endl;
    //m_circularBuffer->print();
    this->m_socket = socket;
        
}



void QuicMyClient::RecvPacket(Ptr<Socket> socket){
    if (!m_running) {return ;}
    
}


} // Namespace ns3

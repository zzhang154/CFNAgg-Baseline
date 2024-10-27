/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008,2009 INRIA, UDCAST
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
#include "ns3/string.h"
#include "packet-loss-counter.h"

#include "seq-ts-header.h"
#include "quic-myserver.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("QuicMyServer");

NS_OBJECT_ENSURE_REGISTERED (QuicMyServer);


TypeId
QuicMyServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::QuicMyServer")
    .SetParent<Application> ()
    .SetGroupName ("Applications")
    .AddConstructor<QuicMyServer> ()
    .AddAttribute ("Port",
                   "Port on which we listen for incoming packets.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&QuicMyServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketWindowSize",
                   "The size of the window used to compute the packet loss. This value should be a multiple of 8.",
                   UintegerValue (128),
                   MakeUintegerAccessor (&QuicMyServer::GetPacketWindowSize,
                                         &QuicMyServer::SetPacketWindowSize),
                   MakeUintegerChecker<uint16_t> (8,256))
    .AddAttribute ("OutputFilename",
                   "A string with the name of the file in which rx packets will be logged",
                   StringValue ("QuicMyServerRx.txt"),
                   MakeStringAccessor (&QuicMyServer::m_outFilename),
                   MakeStringChecker ())
  ;
  return tid;


}

QuicMyServer::QuicMyServer ()
  : m_lossCounter (0)
{
  NS_LOG_FUNCTION (this);
  m_received = 0;
  m_node = node;
  quicSocketFactory=nullptr;
  m_socket = nullptr ;
  m_circularBuffer = Create<CircularBuffer>(); 
}

QuicMyServer::~QuicMyServer () {
  NS_LOG_FUNCTION (this);

  m_socket = nullptr;
  m_node = nullptr;
  m_quicL4 = nullptr;
  //p=nullptr;
  quicSocketFactory=nullptr;
}

void
QuicMyServer::SetNode (Ptr<Node> node) {
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

void 
QuicMyServer::SetCongestionControlAlgorithm(std::string cc_name) {
  congestionControlAlgorithm = cc_name;
}

uint16_t
QuicMyServer::GetPacketWindowSize () const {
  NS_LOG_FUNCTION (this);
  return m_lossCounter.GetBitMapSize ();
}

void
QuicMyServer::SetPacketWindowSize (uint16_t size) {
  NS_LOG_FUNCTION (this << size);
  m_lossCounter.SetBitMapSize (size);
}

uint32_t
QuicMyServer::GetLost (void) const {
  NS_LOG_FUNCTION (this);
  return m_lossCounter.GetLost ();
}

uint64_t
QuicMyServer::GetReceived (void) const {
  NS_LOG_FUNCTION (this);
  return m_received;
}


Ptr<Socket> 
QuicMyServer::GetSocket() {
  return m_socket;
}

Address 
QuicMyServer::GetBindAddress() {
  m_bindIp=GetLocalAddress().GetIpv4();
  return m_bindIp;
}

InetSocketAddress 
QuicMyServer::GetLocalAddress() const {
  Ptr<Node> node=m_node;
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
  Ipv4Address local_ip = ipv4->GetAddress (1, 0).GetLocal();
  return InetSocketAddress{local_ip,m_port}; 
}

Ptr<CircularBuffer> 
QuicMyServer::GetBuffer() {
  return m_circularBuffer;
}

void
QuicMyServer::DoDispose (void) {
  NS_LOG_FUNCTION (this);
  m_socket = nullptr;
  m_node = nullptr;
  m_quicL4 = nullptr;
  quicSocketFactory=nullptr;
  Application::DoDispose();
}



void QuicMyServer::CreateSocket(Ptr<Node> node,uint16_t port) {
  if (m_socket == nullptr )
    {
      // QuicL4Protocol
      m_quicL4 = CreateObject<QuicL4Protocol>();
      m_quicL4->SetNode(m_node);
      if (!m_quicL4) {
        NS_FATAL_ERROR("QuicL4Protocol instance not found!");
        return;
      }

      // QuicSocketFactory 
      quicSocketFactory=CreateObject<QuicSocketFactory>();
      quicSocketFactory->SetQuicL4(m_quicL4); 
      m_socket = quicSocketFactory->CreateSocket();

      m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kUsingTimeLossDetection",BooleanValue (true));
      m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kTimeReorderingFraction",DoubleValue (29 / 8));
      //m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->m_kMaxPacketsReceivedBeforeAckSend = 3;
      m_socket->GetObject<QuicSocketBase>()->SetAttribute("LegacyCongestionControl",BooleanValue (true));

      //m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kMaxPacketsReceivedBeforeAckSend",UintegerValue (1));
      if (!m_socket) {
          NS_FATAL_ERROR("m_socket not found!");
          return;
      }

      // bind port to socket
      InetSocketAddress local = GetLocalAddress();
      if (m_socket->Bind (local) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }

      // congestion strategy
      //if(congestionControlAlgorithm == "bbr") {
      //Ptr<QuicCongestionOps> quicbbr = CreateObject<QuicBbr>();
      //double highGain = 2.5;//3.0
      //quicbbr->SetAttribute("HighGain", ns3::DoubleValue(highGain));
      //m_socket->GetObject<QuicSocketBase>()->SetCongestionControlAlgorithm(quicbbr);
    //}
     
    }
}



void
QuicMyServer::HandleRead (Ptr<Socket> socket) {
  NS_LOG_FUNCTION (this );
  Ptr<Packet> packet;
  Address from;
  while (packet = socket->RecvFrom (from) ) {  
      m_peerAddress = from;
      uint32_t packetSize=packet->GetSize () ;
      NS_LOG_FUNCTION (this << socket <<packetSize);
      if (packetSize > 0) {
          m_received++;
          uint8_t* packetContent = new uint8_t[packetSize];
          uint32_t copyedSize =  packet->CopyData(packetContent,packetSize);
          m_circularBuffer->write(packetContent,copyedSize);
      }
    }
    //std::cout<<"QuicMyServer----"<<GetLocalAddress().GetIpv4()<<"-received---request---from--"<<InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4()<<std::endl;
    //m_circularBuffer->print();
    this->m_socket = socket;
}

void 
QuicMyServer::Bind (uint16_t port) {
  NS_LOG_FUNCTION (this);
  m_port = port;
}



void 
QuicMyServer::Send(const uint8_t* buffer, uint32_t len) {
    NS_LOG_FUNCTION (this);

    Ptr<Packet> p = Create<ns3::Packet>(buffer,len);
    int sentSize = m_socket->Send(p,0);
    if (sentSize >= 0) {
      NS_LOG_INFO ("TraceDelay TX " << sentSize << " bytes to " << m_peerAddress<< " Uid: " << p->GetUid () << " Time: " << (Simulator::Now ()).GetSeconds ());
    }
    else {
        NS_LOG_INFO ("Error while sending " << sentSize << " bytes to " << m_peerAddress );
    }
}


void
QuicMyServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  CreateSocket(m_node,m_port);

  m_socket->Listen ();
  m_socket->SetRecvCallback (MakeCallback (&QuicMyServer::HandleRead, this));

}

void
QuicMyServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != nullptr )
    {
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = nullptr ;
    }
}

void 
QuicMyServer::RecvPacket(Ptr<Socket> socket) {
  NS_LOG_FUNCTION (this);

}


} // Namespace ns3

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
  m_node = node;
  quicSocketFactory=nullptr;
  m_socket = nullptr ;
  // Zhuoxu: we don't have to initialize the iterVecMap here.
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

uint16_t 
QuicMyServer::GetCompIterNum(){
  if (!compQueue.empty())
  {
    uint16_t iterNumFromQueue = compQueue.front();
    NS_LOG_DEBUG(this << " success receive server socket with iteration: " << iterNumFromQueue);
    compQueue.pop();
    return iterNumFromQueue;
  }
  else{
    NS_LOG_DEBUG(this << " compQueue.size(): "<<compQueue.size());
    return UINT16_MAX;
  }
}

// Zhuoxu: need to return the reference of the map. Otherwise, it will cause the problem of copying the large.
ReceivedChunk
QuicMyServer::GetResult(uint16_t iterNum){
  return iterChunkMap[iterNum];
}

void 
QuicMyServer::ReleaseMap(uint16_t iterNum){
  iterChunkMap.erase(iterNum);
  return;
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

/*
Ptr<CircularBuffer> 
QuicMyServer::GetBuffer() {
  return m_circularBuffer;
}
*/

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

bool
QuicMyServer::CheckHeader(uint8_t* packetContent, int len) {
  for (int i = 0; i < len; i++) {
    if (packetContent[i] != headerChr) {
      NS_LOG_WARN("Invalid header for first packet: "
                     << static_cast<int>(packetContent[0]) << " "
                     << static_cast<int>(packetContent[1]) << " "
                     << static_cast<int>(packetContent[2]) << " "
                     << static_cast<int>(packetContent[3]) << " "
                     << static_cast<int>(packetContent[4]) << " "
                     << static_cast<int>(packetContent[5]) << " "
                     << static_cast<int>(packetContent[6]) << " "
                     << static_cast<int>(packetContent[7]));
    }
  }
  NS_LOG_DEBUG("Header check passed");
  return true;
}

void 
QuicMyServer::SetcGroupSize(uint16_t size){
  cGroupSize = size;
}

void 
QuicMyServer::CheckChComp(uint16_t iterNum){
  if(iterChunkMap[iterNum].chAddr.size() == cGroupSize){
    NS_LOG_DEBUG( this << " All children have collected the data for iteration " << iterNum - uint16_t(0) << " in the m_bindIp: " << InetSocketAddress::ConvertFrom(GetLocalAddress()).GetIpv4());

    for (uint16_t i = 0; i < iterChunkMap[iterNum].vec.size(); ++i) {
      iterChunkMap[iterNum].vec[i] = static_cast<uint16_t>(iterChunkMap[iterNum].vec[i] / uint16_t(cGroupSize));
    }

    // Zhuoxu: we need to add the code to trigger the interface function.
    compQueue.push(iterNum);
    NS_LOG_DEBUG(this << " Printing all members of compQueue with size: " << compQueue.size());
    std::queue<uint16_t> tempQueue = compQueue; // Create a copy of the queue to iterate through
    while (!tempQueue.empty()) {
        uint16_t value = tempQueue.front();
        std::cout << value << " ";
        tempQueue.pop();
    }
    std::cout << std::endl;
  }
}

 void 
 QuicMyServer::PrintBuffInfo_8(uint8_t* buffer, uint32_t packetSize){
  NS_LOG_DEBUG(this);
  std::cout << "Buffer info:" << std::endl;
  std::cout << "m_iterationMap[ipAddressStr]: " << m_iterationMap[ipAddressStr] << std::endl;

  std::cout << "Printing all data bytes by bytes" << std::endl;
  int byteCount = 0;
  for (uint32_t i = 0; i < packetSize; ++i) {
    std::cout << buffer[i] - uint8_t(0) << " ";
    byteCount++;
    if (byteCount % 8 == 0) {
      std::cout << " the " << byteCount / 8 << "th byte" << std::endl;
    }
  }
}

void 
QuicMyServer::PrintState(){
    // loop over the keys in the m_bufferMap and print its length of uint8_t*, which is defined as m_bufferPtrMap.
    for(auto const& x : m_bufferMap){
      NS_LOG_INFO( "From Address " << x.first << " m_bufferPtrMap: " << m_bufferPtrMap[x.first] );
    }
    NS_LOG_INFO("print m_iterationMap[]");
    for(auto const& x : m_iterationMap){
      NS_LOG_INFO("m_iterationMap[" << x.first << "]: " << x.second);
    }
    NS_LOG_INFO("print iterChunkMap[] keys");
    for(auto const& x : iterChunkMap){
      NS_LOG_INFO("iterChunkMap[" << x.first << "]: " );
    }
}

void
QuicMyServer::ProcessPerPkt(){
  // check header
  CheckHeader(m_bufferMap[ipAddressStr], 8);
  NS_LOG_DEBUG(this);
  // extract the iteration number
  m_iterationMap[ipAddressStr] = (static_cast<uint16_t>(m_bufferMap[ipAddressStr][8]) << 8) | static_cast<uint16_t>(m_bufferMap[ipAddressStr][9]);

  std::vector<uint64_t> vecTmp (chunkSize, 0);
  DeserializeVector(vecTmp.data(), m_bufferMap[ipAddressStr] + 10, pktlen - 10);
  NS_LOG_INFO("Cover vector: vecTmp[0]:" << vecTmp[0] << " vecTmp[1]:" << vecTmp[1] << " vecTmp[MAX-1]:" << vecTmp[vecTmp.size() - 2] << " vector[MAX]:" << vecTmp.back());

  // Ensure the vectors are properly sized
  if (iterChunkMap[m_iterationMap[ipAddressStr]].vec.size() != vecTmp.size()) {
      NS_LOG_ERROR("Vector sizes do not match.");
      return;
  }

  // Zhuoxu: sum up the vector
  for (uint32_t i = 0; i < chunkSize; ++i) {
    iterChunkMap[m_iterationMap[ipAddressStr]].vec[i] += vecTmp[i];
  }

  // Zhuoxu: add the child count
  iterChunkMap[m_iterationMap[ipAddressStr]].chAddr.insert(ipAddressStr);


  NS_LOG_INFO("iterChunkMap[" << m_iterationMap[ipAddressStr] <<"].size(): " << iterChunkMap[m_iterationMap[ipAddressStr]].chAddr.size()<<" iterChunkMap.size(): "<<iterChunkMap.size());

  NS_LOG_INFO("compQueue.size(): "<<compQueue.size());

  // check if all the children have collected
  CheckChComp(m_iterationMap[ipAddressStr]);
}

bool 
QuicMyServer::CheckReTransmit(uint8_t* packetContent){
  // Zhuoxu: it seems that there is no Retransmission currently, we don't need to do this now.
  uint32_t nextHeadPtr = pktlen - m_bufferPtrMap[ipAddressStr];
  for (int i = 0; i < 8; i++) {
    if (packetContent[nextHeadPtr + i] != headerChr) 
      return true;
  return false;
  }
}


void
QuicMyServer::HandleRead (Ptr<Socket> socket) {
  // std::cout << "***********************START***************************" << std::endl;
  NS_LOG_DEBUG (this << " Entering the handleRead Function ......" );

  // NS_LOG_DEBUG (this << " m_bindIp: " << this->m_bindIp.GetIpv4());

  Ptr<Packet> packet;
  Address from;
  uint32_t packetSize = 0;
  
  // Zhuoxu: ensure 24010 bytes (pktlen + 10) is read.
  if(!(packet = socket->RecvFrom (from)))
    NS_FATAL_ERROR("Invalid packet received in QuicMyServer");
  
  m_peerAddress = from;

  // Convert ns3::Ipv4Address to string and save it
  ns3::Ipv4Address str = InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4();
  std::ostringstream oss;
  str.Print(oss);
  ipAddressStr = oss.str();
  NS_LOG_DEBUG (this << " m_bindIp: " << InetSocketAddress::ConvertFrom(GetLocalAddress()).GetIpv4() << " m_peerAddress: " << ipAddressStr);

  // initialize the m_bufferMap
  if(m_bufferMap.find(ipAddressStr) == m_bufferMap.end()){
    m_bufferMap[ipAddressStr] = nullptr;
    m_bufferPtrMap[ipAddressStr] = 0;
    m_iterationMap[ipAddressStr] = 0;
  }
  
  packetSize=packet->GetSize () ;
  NS_LOG_DEBUG (this << " Socket: "<< socket << " packetSize: " << packetSize);

  if (packetSize <= 0)
    NS_FATAL_ERROR("Packet size <= 0 in QuicMyServer");

  m_received++;

  // Zhuoxu: new code here to handle continue packets
  uint8_t* packetContent = new uint8_t[packetSize];
  m_pktPtr = 0;
  std::vector<uint64_t> vecTmp (chunkSize, 0);
  uint32_t copyedSize =  packet->CopyData(packetContent,packetSize);
  NS_LOG_DEBUG( this << " Copy data size: " << copyedSize);
  // general case: pktContent[i-1], pktContent[i], pktContent[i+1]; So we should consider the general case. In worse case, we need to consider the storage of 3 packets.

  while(m_pktPtr < packetSize){
    // std::cout << "------------------------------------" << std::endl;
    // std::cout << "before process, m_pktPtr is: " << static_cast<int>(m_pktPtr) << std::endl;
    // std::cout << "before process, m_bufferPtr is: " << static_cast<int>(m_bufferPtrMap[ipAddressStr]) << std::endl;
    
    if(m_bufferMap[ipAddressStr] == nullptr){
      m_bufferMap[ipAddressStr] = new uint8_t[pktlen];
    }

    while(m_bufferPtrMap[ipAddressStr] <  pktlen && m_pktPtr < packetSize){
      m_bufferMap[ipAddressStr][m_bufferPtrMap[ipAddressStr]] = packetContent[m_pktPtr];
      m_bufferPtrMap[ipAddressStr]++;
      m_pktPtr++;
    }
    if (m_bufferPtrMap[ipAddressStr] == pktlen){
      // pocess the bufferPtr
      // std::cout << "Begin process the packet, now the m_bufferPtrMap[ipAddressStr] is: " << m_bufferPtrMap[ipAddressStr] - uint16_t(0) << std::endl;
      ProcessPerPkt();
      m_bufferPtrMap[ipAddressStr] = 0;
      delete[] m_bufferMap[ipAddressStr];
      m_bufferMap[ipAddressStr] = nullptr;
    }
    // std::cout << "after process, m_pktPtr is: " << static_cast<int>(m_pktPtr) << std::endl;
    // std::cout << "after process, m_bufferPtr is: " << static_cast<int>(m_bufferPtrMap[ipAddressStr]) << std::endl;
    // std::cout << "------------------------------------" << std::endl;
  }
  // Zhuoxu: only print the packet of 10.1.1.1
  // if(ipAddressStr == "10.1.1.1")
  //   PrintBuffInfo_8(packetContent, packetSize);
  delete[] packetContent;
  //std::cout<<"QuicMyServer----"<<GetLocalAddress().GetIpv4()<<"-received---request---from--"<<InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4()<<std::endl;
  this->m_socket = socket;
  // PrintState();
}

void 
QuicMyServer::Bind (uint16_t port) {
  NS_LOG_FUNCTION (this);
  m_port = port;
}
int 
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
    return sentSize;
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

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

void 
QuicMyServer::SetIpAddrStr(std::string ipAddrStr){
  this->ipAddressStr = ipAddrStr;
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
std::queue<uint16_t>
QuicMyServer::GetCompIterQueue(){
  return compQueue;
}

void QuicMyServer::ClearCompQueue() {
    NS_LOG_FUNCTION(this);
    while (!compQueue.empty()) {
        compQueue.pop();
    }
    NS_LOG_DEBUG("compQueue has been cleared.");
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
      m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kTimeReorderingFraction",DoubleValue (19 / 8));
      m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->m_kMaxPacketsReceivedBeforeAckSend = 15;
      m_socket->GetObject<QuicSocketBase>()->SetAttribute("LegacyCongestionControl",BooleanValue (true));

      // m_socket->GetObject<QuicSocketBase>()->GetSocketStatetcb()->SetAttribute("kMaxPacketsReceivedBeforeAckSend",UintegerValue (100));
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
      // Ptr<QuicCongestionOps> quicbbr = CreateObject<QuicBbr>();
      // double highGain = 2.5;//3.0
      // quicbbr->SetAttribute("HighGain", ns3::DoubleValue(highGain));
      // m_socket->GetObject<QuicSocketBase>()->SetCongestionControlAlgorithm(quicbbr);
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
      Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 0.01));
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
QuicMyServer::SetIterChunkPtr(std::unordered_map<uint16_t, DataChunk>* iterChunk){
  iterChunkPtr = iterChunk;
}

int
QuicMyServer::CheckMemory(){
  // Addr2Str(m_bindIp, addrStr); cannot run this command.
  int chunkAvailable = 0;

  // NS_LOG_DEBUG(this << " Checking memory for Node: " << ipAddressStr);
  // std::cout << "Existing table size: " << (*iterChunkPtr).size() << std::endl;
  // std::cout << "Iteration wait list: ";
  for (const auto& pair : *iterChunkPtr) {
        const DataChunk& chunk = pair.second;
        if (chunk.chAddr.find(ipAddressStr) == chunk.chAddr.end()) {
            chunkAvailable++;
            // std::cout << pair.first << " ";
        }
  }

  // NS_LOG_DEBUG( "Extra iteration can be processed: " << static_cast<int>(ITERTHRESH-uint16_t(0)- (*iterChunkPtr).size()) );
  // Limit the memory
  chunkAvailable += static_cast<int>(ITERTHRESH-uint16_t(0)- (*iterChunkPtr).size());
  // NS_LOG_DEBUG(this << " Available memory: " << chunkAvailable);
  return chunkAvailable;
}

void 
QuicMyServer::CheckChComp(uint16_t iterNum){
  if((*iterChunkPtr)[iterNum].chAddr.size() == cGroupSize){
    NS_LOG_DEBUG( this << " All children have collected the data for iteration " << iterNum - uint16_t(0) << " in the m_bindIp: " << ipAddressStr);

    for (uint16_t i = 0; i < iterChunkMap[iterNum].vec.size(); ++i) {
      (*iterChunkPtr)[iterNum].vec[i] = static_cast<uint16_t>((*iterChunkPtr)[iterNum].vec[i] / uint16_t(cGroupSize));
    }

    // Zhuoxu: we need to add the code to trigger the interface function.
    compQueue.push(iterNum);

    // Zhuoxu: Some problem here, the size of the compQueue should not be larger than the threshold.
    NS_LOG_DEBUG(this << " Printing all members of compQueue with size: " << compQueue.size());
    // if(compQueue.size() == 7)
    // {
    //   NS_LOG_DEBUG(this << " Printing all members of compQueue with size: " << compQueue.size());

    // }
    std::queue<uint16_t> tempQueue = compQueue; // Create a copy of the queue to iterate through
    while (!tempQueue.empty()) {
        uint16_t value = tempQueue.front();
        std::cout << value - uint16_t(0) << "-";
        tempQueue.pop();
    }
    std::cout << std::endl;
  }
}

void 
QuicMyServer::PrintBuffInfo_8(uint8_t* buffer, uint32_t packetSize){
  NS_LOG_DEBUG(this);
  NS_LOG_DEBUG("Buffer info:");
  NS_LOG_DEBUG("m_iterationMap[ipAddressStr]: " << m_iterationMap[ipAddressStr]);

  NS_LOG_DEBUG("Printing all data bytes by bytes");
  int byteCount = 0;
  for (uint32_t i = 0; i < packetSize; ++i) {
    NS_LOG_DEBUG(static_cast<int>(buffer[i]) - uint8_t(0) << " ");
    byteCount++;
    if (byteCount % 8 == 0) {
      NS_LOG_DEBUG(" the " << byteCount / 8 << "th byte");
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
  CheckHeader(m_buffer, 8);
  // extract the iteration number
  m_iteration = (static_cast<uint16_t>(m_buffer[8]) << 8) | static_cast<uint16_t>(m_buffer[9]);

  std::vector<uint64_t> vecTmp (chunkSize, 0);
  DeserializeVector(vecTmp.data(), m_buffer + 10, pktlen - 10);
  NS_LOG_DEBUG("Recover vector from " << ipAddressStr << " in iteration---" << m_iteration - uint16_t(0) << "\nvecTmp[0]:" << vecTmp[0] << ", vecTmp[1]:" << vecTmp[1] << ", vecTmp[" << vecTmp.size() - 2 << "]:" << vecTmp[vecTmp.size() - 2] << ", vector[" << vecTmp.size() - 1 << "]:" << vecTmp.back());

  // Check whether iteration exist in the map
  if(iterChunkPtr->find(m_iteration) == iterChunkPtr->end()){
    DataChunk chunkTmp;
    (*iterChunkPtr)[m_iteration] = chunkTmp;
  }

  // Check whether this iteration has been received before.
  if((*iterChunkPtr)[m_iteration].chAddr.find(ipAddressStr) != (*iterChunkPtr)[m_iteration].chAddr.end()){
    NS_LOG_DEBUG("iteration-" << m_iteration - uint16_t(0) << "has been received before.");
    return;
  }
  else{
    // Zhuoxu: add the child count
    NS_LOG_DEBUG("(*iterChunkPtr)[" << static_cast<int>(m_iteration) << "].chAddr.insert(" << ipAddressStr << ");");
    (*iterChunkPtr)[m_iteration].chAddr.insert(ipAddressStr);
  }

  // Ensure the vectors are properly sized
  if ((*iterChunkPtr)[m_iteration].vec.size() != vecTmp.size()) {
      NS_LOG_ERROR("Vector sizes do not match.");
      return;
  }

  // Zhuoxu: sum up the vector
  for (uint32_t i = 0; i < chunkSize; ++i) {
    (*iterChunkPtr)[m_iteration].vec[i] += vecTmp[i];
  }

  CheckChComp(m_iteration);
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

// Zhuoxu: the interface should pass the pointer of the buffer to the server.
// Zhuoxu: table[iteration] = DataChunk;
void
QuicMyServer::HandleRead (Ptr<Socket> socket) {
  NS_LOG_DEBUG( this << " Entering the HandleRead function ..., with Ipv4 address " << ipAddressStr);
  NS_LOG_DEBUG(this << " Printing all members of compQueue with size: " << compQueue.size());

  std::queue<uint16_t> tempQueue = compQueue; // Create a copy of the queue to iterate through
  while (!tempQueue.empty()) {
      uint16_t value = tempQueue.front();
      std::cout << value - uint16_t(0) << "-";
      tempQueue.pop();
  }
  std::cout << std::endl;

  int memState = CheckMemory();
  if(memState <= 0){
    NS_LOG_DEBUG( this << " No memory available for new chunk");
  }
  else{
    socket->GetObject<QuicSocketBase>()->m_rxBuffer->Print(std::cout);
    // read from RecvBuffer
    Ptr<Packet> packet;
    Address from;
    // NS_LOG_DEBUG( this << " RxBuffer Available(): " << socket->GetObject<QuicSocketBase>()->GetRxBuffer()->Available() - uint32_t(0));
    // NS_LOG_DEBUG( this << " RxBuffer Size(): " << socket->GetObject<QuicSocketBase>()->GetRxBuffer()->Size() - uint32_t(0));
    // Zhuoxu: also output the value of RxstreamBuffer.
    while ((memState > 0) && (packet = socket->GetObject<QuicSocketBase>()->RecvFrom (memState * pktlen, 0, from)))
    {
      NS_LOG_DEBUG("***********START***********");
      // socket->GetObject<QuicSocketBase>()->m_rxBuffer->Print(std::cout);
      NS_LOG_DEBUG("memState: " << memState << ", memState * pktlen = " << memState * pktlen);
      m_peerAddress = from;
      uint32_t packetSize=packet->GetSize();
      m_received++; 
      m_pktPtr = 0;

      // Zhuoxu: if we can let the quic to give us exactly the size of chunkSize data in each quic frame, then we can simplify the code here.
      // Todo: Check the quic frame size setting.
      uint8_t* packetContent = new uint8_t[packetSize];
      m_pktPtr = 0;
      std::vector<uint64_t> vecTmp (chunkSize, 0);
      uint32_t copyedSize =  packet->CopyData(packetContent,packetSize);
      NS_LOG_DEBUG( this << " Copy data size: " << copyedSize);
      if (ipAddressStr == "10.1.1.1"){
        NS_LOG_DEBUG("Print data from " << ipAddressStr << " for iteration- " << m_iteration <<" group");
        for(int i =0;i<copyedSize;i++){
          std::cout << static_cast<int>(packetContent[i]) << '|';
          if(i==copyedSize-1)
            std::cout << "Ending..." << std::endl;
        }
      std::cout;
      }
      // general case: pktContent[i-1], pktContent[i], pktContent[i+1]; So we should consider the general case. In worse case, we need to consider the storage of 3 packets.

      while(m_pktPtr < packetSize){
        NS_LOG_DEBUG("------------------------------------");
        NS_LOG_DEBUG("before process, m_pktPtr is: " << static_cast<int>(m_pktPtr));
        NS_LOG_DEBUG("before process, m_bufferPtr is: " << static_cast<int>(m_bufferPtr));
        
        if(m_buffer == nullptr){
          m_buffer = new uint8_t[pktlen];
        }

        // Copy context from packetContent to buffer
        while(m_bufferPtr <  pktlen && m_pktPtr < packetSize){
          m_buffer[m_bufferPtr] = packetContent[m_pktPtr];
          m_bufferPtr++;
          m_pktPtr++;
        }
        if (m_bufferPtr == pktlen){
          // pocess the bufferPtr
          // NS_LOG_DEBUG("Begin process the packet, now the m_bufferPtrMap[ipAddressStr] is: " << m_bufferPtrMap[ipAddressStr] - uint16_t(0));
          ProcessPerPkt();
          memState--;
          m_bufferPtr = 0;
          delete[] m_buffer;
          m_buffer = nullptr;
        }
        NS_LOG_DEBUG("after process, m_pktPtr is: " << static_cast<int>(m_pktPtr));
        NS_LOG_DEBUG("after process, m_bufferPtr is: " << static_cast<int>(m_bufferPtrMap[ipAddressStr]));
        NS_LOG_DEBUG("------------------------------------");
      }
      // Zhuoxu: only print the packet of 10.1.1.1
      // if(ipAddressStr == "10.1.1.1")
      //   PrintBuffInfo_8(packetContent, packetSize);
      delete[] packetContent;
      // NS_LOG_DEBUG("delete packetContent");
      // NS_LOG_DEBUG("QuicMyServer----"<<GetLocalAddress().GetIpv4()<<"-received---request---from--"<<+
      ;
      // PrintState();
      NS_LOG_DEBUG("#########END###########");
    }
    NS_ASSERT(memState >= 0);
  }
  //std::cout<<"QuicMyServer----"<<GetLocalAddress().GetIpv4()<<"-received---request---from--"<<InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4()<<std::endl;
  //m_circularBuffer->print();
  this->m_socket = socket;
  NS_LOG_DEBUG("quit the function...");
  // ns3::Simulator::Schedule(ns3::MilliSeconds(10), &QuicMyServer::HandleRead, this, socket);
}

void 
QuicMyServer::CallHandleRead(){
  if(m_socket != nullptr){
    HandleRead(m_socket);
  }
  else{
    NS_LOG_DEBUG("m_socket is nullptr");
  }
  return;
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

void 
QuicMyServer::Addr2Str (Address addr, std::string &str) {
    std::stringstream ss;
    ss << Ipv4Address::ConvertFrom (addr);
    str = ss.str();
}

} // Namespace ns3

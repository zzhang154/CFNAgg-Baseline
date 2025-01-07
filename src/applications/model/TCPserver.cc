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
#include "TCPserver.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TCPserver");

NS_OBJECT_ENSURE_REGISTERED (TCPserver);


TypeId
TCPserver::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TCPserver")
    .SetParent<Application> ()
    .SetGroupName ("Applications")
    .AddConstructor<TCPserver> ()
  ;
  return tid;
}

TCPserver::TCPserver()
    : m_lossCounter(0)
{
    NS_LOG_FUNCTION(this);
    // Default constructor does not initialize m_node or m_socket
}

TCPserver::TCPserver(Ptr<Node> node)
    : m_node(node), m_lossCounter(0)
{
    NS_LOG_FUNCTION(this);
}

TCPserver::~TCPserver () {
  NS_LOG_FUNCTION (this);

  m_socket = nullptr;
  m_node = nullptr;
  //p=nullptr;
}

void
TCPserver::DoInitialize(void)
{
    NS_LOG_FUNCTION(this);
    Application::DoInitialize();
}

void
TCPserver::PrintSocketInfo(Ptr<Socket> socket)
{
    Address localAddress;
    socket->GetSockName(localAddress);
    InetSocketAddress inetLocalAddress = InetSocketAddress::ConvertFrom(localAddress);
    Ipv4Address localIp = inetLocalAddress.GetIpv4();
    uint16_t localPort = inetLocalAddress.GetPort();

    NS_LOG_INFO("Socket info. IP address: " << localIp << " and port: " << localPort);
}

void
TCPserver::SetNode (Ptr<Node> node) {
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

void 
TCPserver::SetCongestionControlAlgorithm(std::string cc_name) {
  congestionControlAlgorithm = cc_name;
}

uint16_t
TCPserver::GetPacketWindowSize () const {
  NS_LOG_FUNCTION (this);
  return m_lossCounter.GetBitMapSize ();
}

void
TCPserver::SetPacketWindowSize (uint16_t size) {
  NS_LOG_FUNCTION (this << size);
  m_lossCounter.SetBitMapSize (size);
}

uint32_t
TCPserver::GetLost (void) const {
  NS_LOG_FUNCTION (this);
  return m_lossCounter.GetLost ();
}

void 
TCPserver::SetPeerAddrStr(std::string ipAddrStr){
  this->ipAddressStr = ipAddrStr;
}

uint64_t
TCPserver::GetReceived (void) const {
  NS_LOG_FUNCTION (this);
  return m_received;
}


// Zhuoxu: need to return the reference of the unordered_map. Otherwise, it will cause the problem of copying the large.
std::queue<uint16_t>*
TCPserver::GetCompIterQueue() {
  return &compQueue;
}

void 
TCPserver::ReleaseMap(uint16_t iterNum){
  iterChunkMap.erase(iterNum);
  return;
}

Ptr<Socket> 
TCPserver::GetSocket() {
  return m_socket;
}

Address 
TCPserver::GetBindAddress() {
  m_bindIp=GetLocalAddress().GetIpv4();
  return m_bindIp;
}

InetSocketAddress 
TCPserver::GetLocalAddress() const {
  Ptr<Node> node=m_node;
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
  Ipv4Address local_ip = ipv4->GetAddress (1, 0).GetLocal();
  // the m_port info has been changed previously
  return InetSocketAddress{local_ip,m_port}; 
}
 

void
TCPserver::DoDispose (void) {
  NS_LOG_FUNCTION (this);
  m_socket = nullptr;
  m_node = nullptr;
  Application::DoDispose();
}


bool
TCPserver::CheckHeader(uint8_t* packetContent, int len) {
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
TCPserver::SetcGroupSize(uint16_t size){
  cGroupSize = size;
}

void 
TCPserver::SetIterChunkPtr(std::map<uint16_t, DataChunk>* iterChunk){
  iterChunkPtr = iterChunk;
}

// Todo: print the info of the table
void 
TCPserver::PrintTable(){
  std::stringstream ss;
  ss << "\n";
  for (const auto& pair : *iterChunkPtr) {
    ss << "iteration: " << pair.first << "  collected child: " << std::endl; 
    const std::unordered_set<std::string>& chAddr = pair.second.chAddr;
    for (const auto& addr : chAddr) {
        ss << addr << "----";
    }
    ss << std::endl;
  }
  NS_LOG_DEBUG(ss.str());
}

int
TCPserver::CheckMemory(){
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
TCPserver::CheckChComp(uint16_t iterNum){
  if((*iterChunkPtr)[iterNum].chAddr.size() == cGroupSize){
    NS_LOG_DEBUG( this << " All children have collected the data for iteration " << iterNum - uint16_t(0) << " in the m_bindIp: " << ipAddressStr);

    // Zhuoxu: we block this at the moment.
    // std::cout << "All children have collected the data for iteration " << iterNum - uint16_t(0) << " in server IP: " << this->LocalAddressStr << std::endl;

    for (uint16_t i = 0; i < iterChunkMap[iterNum].vec.size(); ++i) {
      (*iterChunkPtr)[iterNum].vec[i] = static_cast<uint16_t>((*iterChunkPtr)[iterNum].vec[i] / uint16_t(cGroupSize));
    }

    // Zhuoxu: we need to add the code to trigger the interface function.
    compQueuePtr->push(iterNum);

    // Zhuoxu: Some problem here, the size of the compQueue should not be larger than the threshold.
    NS_LOG_DEBUG(this << " Printing all members of compQueue with size: " << compQueuePtr->size());

    std::queue<uint16_t> tempQueue = *compQueuePtr; // Create a copy of the queue to iterate through
    std::stringstream ss;
    while (!tempQueue.empty()) {
        uint16_t value = tempQueue.front();
        ss << value - uint16_t(0) << "-";
        tempQueue.pop();
    }
    ss << std::endl;
    NS_LOG_DEBUG("compQueue: " << ss.str());
    if(LocalAddressStr == TraceIPAddress)
    {
        std::cout << "compQueue for TraceIPAddress: " << TraceIPAddress << ", with socket fromStr: " << ipAddressStr << "  | " << ss.str() << std::endl;
    }
  }
}

void 
TCPserver::PrintBuffInfo_8(uint8_t* buffer, uint32_t packetSize){
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
TCPserver::PrintState(){
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
TCPserver::ProcessPerPkt(){
  // check header
  CheckHeader(m_buffer, 8);
  // extract the iteration number
  m_iteration = (static_cast<uint16_t>(m_buffer[8]) << 8) | static_cast<uint16_t>(m_buffer[9]);

  std::vector<uint64_t> vecTmp (chunkSize, 0);
  DeserializeVector(vecTmp.data(), m_buffer + 10, pktlen - 10);
  NS_LOG_DEBUG("Recover vector from " << ipAddressStr << " in iteration---" << m_iteration - uint16_t(0) << "\nvecTmp[0]:" << vecTmp[0] << ", vecTmp[1]:" << vecTmp[1] << ", vecTmp[" << vecTmp.size() - 2 << "]:" << vecTmp[vecTmp.size() - 2] << ", vector[" << vecTmp.size() - 1 << "]:" << vecTmp.back());

  // Check whether iteration exist in the unordered_map
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
TCPserver::CheckReTransmit(uint8_t* packetContent){
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
TCPserver::HandleRead (Ptr<Socket> socket) {
  NS_LOG_DEBUG("Ipv4 address " << ipAddressStr << ", print compQueue with size: " << compQueuePtr->size());
  PrintSocketInfo(socket);

  Address localAddress;
  socket->GetObject<TcpSocketBase>()->GetSockName(localAddress);
  if (InetSocketAddress::IsMatchingType(localAddress))
  {
      InetSocketAddress inetLocalAddress = InetSocketAddress::ConvertFrom(localAddress);
      Ipv4Address localIp = inetLocalAddress.GetIpv4();
      uint16_t localPort = inetLocalAddress.GetPort();
      NS_LOG_DEBUG("Local IP: " << localIp << ", Local Port: " << localPort);
  }
  else
  {
      NS_LOG_WARN("Unknown local address type");
  }

  std::queue<uint16_t> tempQueue = *compQueuePtr; // Create a copy of the queue to iterate through
  std::stringstream ss;
  while (!tempQueue.empty()) {
      uint16_t value = tempQueue.front();
      ss << value - uint16_t(0) << "-";
      tempQueue.pop();
  }
  // ss << std::endl;
  NS_LOG_DEBUG("Complete iteration: " << ss.str());

  int memState = CheckMemory();
  if(memState <= 0){
    NS_LOG_DEBUG( this << " No memory available for new chunk");
    PrintTable();
  }
  else{
    // read from RecvBuffer
    Ptr<Packet> packet;
    Address from;
    // Zhuoxu: also output the value of RxstreamBuffer.
    while ((memState > 0) && (packet = socket->GetObject<TcpSocketBase>()->RecvFrom (memState * pktlen, 0, from)))
    {
      NS_LOG_DEBUG("***********START***********");
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

      // Zhuoxu: now, we needn't show this anymore. We can cancel the comment once we want to debug received data in the application layer.

      // if (ipAddressStr == "10.1.1.1"){
      //   NS_LOG_DEBUG("Print data from " << ipAddressStr << " for iteration- " << m_iteration <<" group");
      //   for(int i =0;i<copyedSize;i++){
      //     std::cout << static_cast<int>(packetContent[i]) << '|';
      //     if(i==copyedSize-1)
      //       std::cout << "Ending..." << std::endl;
      //   }
      // }
      // general case: pktContent[i-1], pktContent[i], pktContent[i+1]; So we should consider the general case. In worse case, we need to consider the storage of 3 packets.

      while(m_pktPtr < packetSize){
        NS_LOG_DEBUG("----------------BEGIN PROCESS--------------------");
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
        NS_LOG_DEBUG("----------------END PROCESS--------------------");
      }
      // Zhuoxu: only print the packet of 10.1.1.1
      // if(ipAddressStr == "10.1.1.1")
      //   PrintBuffInfo_8(packetContent, packetSize);
      delete[] packetContent;
      // NS_LOG_DEBUG("delete packetContent");
      // NS_LOG_DEBUG("TCPserver----"<<GetLocalAddress().GetIpv4()<<"-received---request---from--"<<+
      ;
      // PrintState();
      NS_LOG_DEBUG("#########END###########");
    }
    NS_ASSERT(memState >= 0);
  }
  //std::cout<<"TCPserver----"<<GetLocalAddress().GetIpv4()<<"-received---request---from--"<<InetSocketAddress::ConvertFrom(m_peerAddress).GetIpv4()<<std::endl;
  //m_circularBuffer->print();
  this->m_socket = socket;
  NS_LOG_DEBUG("quit the function...\n\n");
  // ns3::Simulator::Schedule(ns3::MilliSeconds(10), &TCPserver::HandleRead, this, socket);
}

void 
TCPserver::CallHandleRead(){
  if(m_socket != nullptr){
    HandleRead(m_socket);
  }
  else{
    NS_LOG_DEBUG("m_socket is nullptr");
  }
  return;
}

void 
TCPserver::Bind (uint16_t port) {
  NS_LOG_FUNCTION (this);
  m_port = port;
  if(m_socket == nullptr)
  {
    NS_LOG_DEBUG( this << " m_socket is nullptr");
    m_socket = Socket::CreateSocket(m_node, TcpSocketFactory::GetTypeId());
  }

  
  if (m_socket->GetObject<TcpSocketBase>()->Bind (GetLocalAddress()) == -1) {
      NS_FATAL_ERROR ("Failed to bind socket");
    }
  m_socket->Listen();
  m_socket->SetRecvCallback (MakeCallback (&TCPserver::HandleRead, this));

  NS_LOG_DEBUG("after the binding operation ...");
  PrintSocketInfo(m_socket); // Print socket information after binding
  CheckSocketState();
  LogSocketInfo();
}

void
TCPserver::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
}

void 
TCPserver::CallSendEmptyPacket()
{
  NS_LOG_FUNCTION (this << " CallSendEmptyPacket");
  std::cout << std::endl;
  m_socket->GetObject<TcpSocketBase>()->CallSendEmptyPacketACK();
}

void
TCPserver::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != nullptr )
    {
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = nullptr ;
    }
}

void 
TCPserver::RecvPacket(Ptr<Socket> socket) {
  NS_LOG_FUNCTION (this);
}

void 
TCPserver::Addr2Str (Address addr, std::string &str) {
    std::stringstream ss;
    ss << Ipv4Address::ConvertFrom (addr);
    str = ss.str();
}

void
TCPserver::LogSocketInfo()
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
TCPserver::CheckSocketState()
{
    NS_LOG_INFO (this);
    if (m_socket != nullptr)
    {
        Ptr<TcpSocketBase> tcpSocket = m_socket->GetObject<TcpSocketBase>();
        if (tcpSocket != nullptr)
        {
            TcpSocket::TcpStates_t state = tcpSocket->GetState();
            NS_LOG_INFO("Socket state: " << TcpSocket::TcpStateName[state]);

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
TCPserver::SetLocalAddressStr(std::string str){
  LocalAddressStr = str;
}

void 
TCPserver::SetParams(Ptr<Node> node, uint16_t size, std::map<uint16_t, DataChunk>* iterChunk, std::string peerIpAddrStr, std::string localIpAddrStr, std::queue<uint16_t>* compQueuePtr){
  m_node = node;
  cGroupSize = size;
  iterChunkPtr = iterChunk;
  this->ipAddressStr = peerIpAddrStr;
  LocalAddressStr = localIpAddrStr;
  this->compQueuePtr = compQueuePtr;
}

} // Namespace ns3

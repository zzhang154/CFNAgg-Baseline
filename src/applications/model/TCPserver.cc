#include "TCPserver.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/ipv4.h"
#include "ns3/seq-ts-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("TCPserver");
NS_OBJECT_ENSURE_REGISTERED(TCPserver);

// TypeId Implementation
TypeId TCPserver::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::TCPserver")
        .SetParent<Application>()
        .SetGroupName("Applications")
        .AddConstructor<TCPserver>();
    return tid;
}

// Default constructor
TCPserver::TCPserver() : 
    m_lossCounter(8), // Initialize with default window size
    m_socket(nullptr), 
    m_node(nullptr), 
    m_received(0),
    m_buffer(nullptr), 
    m_bufferPtr(0), 
    m_pktPtr(0),
    m_iteration(0), 
    cGroupSize(0),
    m_packetState(FIRST_PACKET) {}

// Parameterized constructor
TCPserver::TCPserver(Ptr<Node> node) : 
    m_lossCounter(8), // Initialize with default window size
    m_socket(nullptr), 
    m_node(node),
    m_received(0), 
    m_buffer(nullptr),
    m_bufferPtr(0), 
    m_pktPtr(0),
    m_iteration(0), 
    cGroupSize(0),
    m_packetState(FIRST_PACKET) {}

TCPserver::~TCPserver() {
    delete[] m_buffer;
    for (auto& pair : m_bufferMap) {
        delete[] pair.second;
    }
}

// Lifecycle Management
void TCPserver::DoInitialize() {
    Application::DoInitialize();
    InitializeSocket();
}

void TCPserver::DoDispose() {
    m_socket = nullptr;
    m_node = nullptr;
    Application::DoDispose();
}

// Socket Management
void TCPserver::InitializeSocket() {
    if (!m_socket && m_node) {
        m_socket = Socket::CreateSocket(m_node, TcpSocketFactory::GetTypeId());
        InetSocketAddress bindAddress(m_node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), m_port);

        // Check if Bind() succeeds
        int bindResult = m_socket->Bind(bindAddress);
        if (bindResult == -1) {
            NS_LOG_ERROR("TCPserver: Failed to bind to "
                         << bindAddress.GetIpv4() << " on port " << bindAddress.GetPort());
            NS_FATAL_ERROR("Bind failed");
        }

        // Check if Listen() succeeds
        int listenResult = m_socket->Listen();
        if (listenResult == -1) {
            // Retrieve the actual bound address/port for logging
            Address localAddress;
            m_socket->GetSockName(localAddress);
            InetSocketAddress sockAddr = InetSocketAddress::ConvertFrom(localAddress);
            NS_LOG_ERROR("TCPserver: Failed to listen on "
                         << sockAddr.GetIpv4() << ":" << sockAddr.GetPort());
            NS_FATAL_ERROR("Listen failed");
        }

        m_socket->SetRecvCallback(MakeCallback(&TCPserver::HandleRead, this));
    }
}

void TCPserver::Bind(uint16_t port) {
    m_port = port;
    if (m_socket) {
        m_socket->Close();
    }
    InitializeSocket();
}

// Data Processing Core
void TCPserver::HandleRead(Ptr<Socket> socket)
{
  NS_LOG_DEBUG("Ipv4 " << ipAddressStr << ", compQueue size: " << compQueuePtr->size());
  
  // Print debug info.
  this->LogSocketAddress(socket);
  NS_LOG_DEBUG("Complete iteration: " << QueueToString(*compQueuePtr));
  
  // Check memory availability.
  int memState = CheckMemory();
  if (memState <= 0)
  {
      NS_LOG_DEBUG(this << " No memory available for new chunk");
      PrintAppTable();
      return;
  }

  Ptr<TcpSocketBase> tcpSocket = socket->GetObject<TcpSocketBase>();
  Ptr<Packet> packet;
  Address from;
  
  // Process incoming packets as long as memory is available.
  while ((memState > 0) && (packet = tcpSocket->RecvFrom(memState * pktlen, 0, from)))
  {
      NS_LOG_DEBUG("*********** START ***********");
      NS_LOG_DEBUG("Memory state: " << memState << ", expecting bytes: " << memState * pktlen);
      m_peerAddress = from;
      uint32_t packetSize = packet->GetSize();
      m_received++;
      m_pktPtr = 0;
      
      // Copy packet into temporary buffer.
      uint8_t* packetContent = new uint8_t[packetSize];
      packet->CopyData(packetContent, packetSize);
      NS_LOG_DEBUG(this << " Copied data size: " << packetSize);
      
      // Process data in segments of size 'pktlen' using memcpy.
      while (m_pktPtr < packetSize)
      {
          // Allocate m_buffer if not already allocated.
          if (m_buffer == nullptr)
              m_buffer = new uint8_t[pktlen];
          
          uint32_t remainBuffer = pktlen - m_bufferPtr;
          uint32_t remainPacket = packetSize - m_pktPtr;
          uint32_t copyLen = std::min(remainBuffer, remainPacket);
          memcpy(m_buffer + m_bufferPtr, packetContent + m_pktPtr, copyLen);
          m_bufferPtr += copyLen;
          m_pktPtr += copyLen;
          
          // When full, process the buffer.
          if (m_bufferPtr == pktlen)
          {
              ProcessPerPkt();
              memState--;
              m_bufferPtr = 0;
              delete [] m_buffer;
              m_buffer = nullptr;
          }
      }
      delete [] packetContent;
      NS_LOG_DEBUG("######### END ###########");
  }
  NS_ASSERT(memState >= 0);
  // Save the socket reference.
  m_socket = socket;
  NS_LOG_DEBUG("Exiting HandleRead");
}

bool TCPserver::CheckHeader(uint8_t* packetContent, int len) {
  bool valid = std::all_of(packetContent, packetContent + len, [this](uint8_t b){ return b == headerChr; });
  if (!valid) {
      std::ostringstream ss;
      for (int i = 0, n = std::min(len, 8); i < n; i++) 
        ss << static_cast<int>(packetContent[i]) << " ";
      NS_LOG_WARN("Invalid header: " << ss.str());
      NS_LOG_ERROR("Invalid application header");
  } else NS_LOG_DEBUG("Header check passed");
  return valid;
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
    // Add the child count
    NS_LOG_DEBUG("(*iterChunkPtr)[" << static_cast<int>(m_iteration) << "].chAddr.insert(" << ipAddressStr << ");");
    (*iterChunkPtr)[m_iteration].chAddr.insert(ipAddressStr);
  }

  // Ensure the vectors are properly sized
  if ((*iterChunkPtr)[m_iteration].vec.size() != vecTmp.size()) {
      NS_LOG_ERROR("Vector sizes do not match.");
      return;
  }

  // Sum up the vector
  for (uint32_t i = 0; i < chunkSize; ++i) {
    (*iterChunkPtr)[m_iteration].vec[i] += vecTmp[i];
  }

  CheckChComp(m_iteration);
}


// Memory Management
int TCPserver::CheckMemory() {
    NS_LOG_FUNCTION(this);
    
    int chunkAvailable = 0;
    for (const auto& pair : *iterChunkPtr) {
        if (pair.second.chAddr.find(ipAddressStr) == pair.second.chAddr.end()) {
            chunkAvailable++;
        }
    }
    chunkAvailable += static_cast<int>(ITERTHRESH-uint16_t(0)- (*iterChunkPtr).size());
    return chunkAvailable;
}

// Chunk Completion Handling
void TCPserver::CheckChComp(uint16_t iterNum) {
    NS_LOG_FUNCTION(this << iterNum);
    
    if (iterChunkPtr->at(iterNum).chAddr.size() == cGroupSize) {
        Simulator::Schedule(MilliSeconds(2), &TCPserver::DoChComp, this, iterNum);
    }
}

void TCPserver::DoChComp(uint16_t iterNum) {
    NS_LOG_FUNCTION(this << iterNum);
    
    // Process completed iteration
    for (auto& element : iterChunkPtr->at(iterNum).vec) {
        element /= cGroupSize;
    }
    
    compQueuePtr->push(iterNum);
    NS_LOG_DEBUG("Completed iteration " << iterNum << " with " << cGroupSize << " chunks");
}

// Utility Functions
void TCPserver::LogSocketAddress(Ptr<Socket> socket) {
  Address localAddress;
  socket->GetSockName(localAddress);
  if (InetSocketAddress::IsMatchingType(localAddress)) {
      auto inetAddr = InetSocketAddress::ConvertFrom(localAddress);
      NS_LOG_DEBUG("Local: " << inetAddr.GetIpv4() 
                   << ":" << inetAddr.GetPort());
  }
}

// Utility Functions called by the main program

void  
TCPserver::CallSendEmptyPacket() // Important function to let the system run normally! Don't delte it!
{
  NS_LOG_FUNCTION (this << " CallSendEmptyPacket");
  // Zhuoxu: extra std::endl comes from this.
  m_socket->GetObject<TcpSocketBase>()->CallSendEmptyPacketACK();
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

void TCPserver::PrintAppTable() {
  NS_LOG_FUNCTION(this);
  
  std::stringstream ss;
  for (const auto& pair : *iterChunkPtr) {
      ss << "Iteration " << pair.first << " children: ";
      for (const auto& addr : pair.second.chAddr) {
          ss << addr << " ";
      }
      ss << "\n";
  }
  NS_LOG_INFO(ss.str());
}

// Configuration
void TCPserver::SetParams(Ptr<Node> node, uint16_t size, 
                         std::map<uint16_t, DataChunk>* iterChunk,
                         std::string peerIpAddrStr, 
                         std::string localIpAddrStr,
                         std::queue<uint16_t>* compQueuePtr) {
    m_node = node;
    cGroupSize = size;
    iterChunkPtr = iterChunk;
    ipAddressStr = peerIpAddrStr;
    LocalAddressStr = localIpAddrStr;
    this->compQueuePtr = compQueuePtr;
    InitializeSocket();
}

// Application Lifecycle
void TCPserver::StartApplication() {
    NS_LOG_FUNCTION(this);
    InitializeSocket();
}

void TCPserver::StopApplication() {
    NS_LOG_FUNCTION(this);
    
    if (m_socket) {
        m_socket->Close();
        m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    }
}

} // namespace ns3

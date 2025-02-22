#include "InnetworkAggregationInterface.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <queue>
#include <iostream>
#include <fstream>
#include <functional>

namespace ns3 {

// Debugging helper for queues
template <typename T>
std::ostream& operator<<(std::ostream& os, std::queue<T> q) {
    os << "Queue[ ";
    while (!q.empty()) {
        os << q.front() << " ";
        q.pop();
    }
    return os << "]";
}

NS_LOG_COMPONENT_DEFINE("InnetworkAggregationInterface");
NS_OBJECT_ENSURE_REGISTERED(InnetworkAggregationInterface);

// Core Infrastructure ========================================================
TypeId InnetworkAggregationInterface::GetTypeId(void) {
    NS_LOG_DEBUG("GetTypeId() called.");
    static TypeId tid = TypeId("ns3::InnetworkAggregationInterface")
        .SetParent<Object>()
        .SetGroupName("innetwork-train")
        .AddConstructor<InnetworkAggregationInterface>();
    NS_LOG_DEBUG("GetTypeId() returning tid.");
    return tid;
}

InnetworkAggregationInterface::InnetworkAggregationInterface()
    : printCount(0), compQueue(std::queue<uint16_t>())
{
    NS_LOG_DEBUG("Constructor called. this = " << this);
}

InnetworkAggregationInterface::~InnetworkAggregationInterface() {
    NS_LOG_DEBUG("Destructor called at " << Simulator::Now().GetSeconds() 
                 << "s. Cleaning up socketPool, this = " << this);
    for (auto& pair : socketPool) {
        pair.second = nullptr;
    }
    socketPool.clear();
}

// Configuration & Setup ======================================================
void InnetworkAggregationInterface::SetupInnetworkAggregationInterface(
    uint16_t port, uint16_t itr, uint8_t aggTreeLevel,
    std::vector<Address>& sGroup, std::vector<Address>& cGroup,
    Ptr<Node> node, bool isEnd)
{
    NS_LOG_DEBUG("SetupInnetworkAggregationInterface called with port " << port 
                 << ", itr " << itr << ", aggTreeLevel " << static_cast<int>(aggTreeLevel));
    this->isEnd = isEnd;
    m_peerPort = port;
    this->node = node;
    maxIteration = itr;
    currentIteration = 0;
    this->aggTreeLevel = aggTreeLevel;
    iterationNum = 0;

    DeepVectorCopy(this->sGroup, sGroup);
    DeepVectorCopy(this->cGroup, cGroup);

    if (!node) {
        NS_LOG_LOGIC("Initialized without node binding on port: " << port);
    } else {
        NS_LOG_DEBUG("Node successfully bound in SetupInnetworkAggregationInterface.");
    }
}

void InnetworkAggregationInterface::SetVSize(uint32_t size) {
    NS_LOG_DEBUG("SetVSize called with size " << size);
    vsize = size;
}

// Network Management =========================================================
void InnetworkAggregationInterface::CreateSocketPool(std::string cc_name) {
    NS_LOG_DEBUG("CreateSocketPool called with cc_name: " << cc_name);
    
    // Server setup: Get local node's IP address.
    Address serverAddr = GetIpAddrFromNode(node);
    Addr2Str(serverAddr, thisAddress);
    NS_LOG_DEBUG("Local node address set to: " << thisAddress);

    // Client connections (sGroup): setup TCPclient for each.
    for (size_t i = 0; i < sGroup.size(); ++i) {
        std::string addrStr;
        Addr2Str(sGroup[i], addrStr);
        NS_LOG_DEBUG("Setting up TCPclient for sGroup[" << i << "]: " << addrStr);

        Ptr<TCPclient> client = CreateObject<TCPclient>();
        client->SetNode(node);
        
        uint16_t serverPort = HashIpToPort(thisAddress);
        NS_LOG_DEBUG("Computed serverPort for local node (" << thisAddress << "): " << serverPort);
        client->SetRemote(Ipv4Address::ConvertFrom(sGroup[i]), serverPort);
        NS_LOG_DEBUG("TCPclient remote set for " << addrStr);

        client->Bind(m_peerPort);
        NS_LOG_DEBUG("TCPclient bound to m_peerPort: " << m_peerPort);

        socketPool[addrStr] = client;
        NS_LOG_DEBUG("Created client to " << addrStr << ":" << serverPort);
    }

    // Server endpoints (cGroup): setup TCPserver for each.
    for (size_t i = 0; i < cGroup.size(); ++i) {
        std::string addrStr;
        Addr2Str(cGroup[i], addrStr);
        NS_LOG_DEBUG("Setting up TCPserver for cGroup[" << i << "]: " << addrStr);

        uint16_t serverPort = HashIpToPort(addrStr);
        NS_LOG_DEBUG("Computed serverPort for cGroup[" << i << "]: " << serverPort);

        Ptr<TCPserver> server = CreateObject<TCPserver>(node);
        server->Bind(serverPort);
        NS_LOG_DEBUG("TCPserver bound to port: " << serverPort);

        server->SetParams(node, cGroup.size(), &iterChunk, addrStr, thisAddress, &compQueue);
        NS_LOG_DEBUG("TCPserver parameters set for addr: " << addrStr);

        socketPool[addrStr] = server;
        NS_LOG_DEBUG("Created server for " << addrStr << ":" << serverPort);
    }
    NS_LOG_DEBUG("CreateSocketPool finished. Current time (ms): " << Simulator::Now().GetMilliSeconds());
    NS_LOG_DEBUG("Local address: " << thisAddress << ", sGroup.size(): " << sGroup.size() 
                 << ", cGroup.size(): " << cGroup.size());
}

int InnetworkAggregationInterface::SendResponseVToP(std::vector<uint64_t>& vec,
                                                   uint16_t iterationNum,
                                                   std::string fromStr)
{
    NS_LOG_FUNCTION(this << " iteration: " << iterationNum);
    int status = -1;
    for (size_t i = 0; i < sGroup.size(); ++i) {
        std::string toStr;
        Addr2Str(sGroup[i], toStr);
        NS_LOG_DEBUG("Sending response V to P for sGroup[" << i << "] to " << toStr);
        status = SendResponseVTo(toStr, vec, iterationNum, fromStr);
    }
    NS_LOG_DEBUG("SendResponseVToP completed for iteration " << iterationNum 
                 << " with status " << status);
    return status;
}

int InnetworkAggregationInterface::SendResponseVTo(std::string toStr,
                                                  std::vector<uint64_t>& vec,
                                                  uint16_t iterationNum,
                                                  std::string fromStr)
{
    NS_LOG_FUNCTION(this << " toStr: " << toStr << " iteration: " << iterationNum);
    if (vsize <= 0) {
        NS_LOG_ERROR("Invalid vector size: " << vsize);
        return -1;
    }

    std::vector<uint8_t> serialized(vsize * sizeof(uint64_t), 0);
    SerializeVector(vec, serialized.data());
    NS_LOG_DEBUG("Vector serialized for iteration " << iterationNum);

    std::vector<uint8_t> packet(pktlen, 0);
    // Header configuration for packet.
    for (int i = 0; i < 8; ++i)
        packet[i] = 3;  // Type identifier
    packet[8] = static_cast<uint8_t>((iterationNum >> 8) & 0xFF);
    packet[9] = static_cast<uint8_t>(iterationNum & 0xFF);
    std::copy(serialized.begin(), serialized.end(), packet.begin() + 10);
    NS_LOG_DEBUG("Packet constructed for iteration " << iterationNum);

    return SendPacket(toStr, iterationNum, packet, fromStr);
}

int InnetworkAggregationInterface::SendPacket(std::string toStr,
                                             uint16_t iterationNum,
                                             std::vector<uint8_t>& buffer,
                                             std::string fromStr)
{
    NS_LOG_FUNCTION(this << " toStr: " << toStr << " iteration: " << iterationNum);
    Ptr<TCPclient> client = socketPool[toStr]->GetObject<TCPclient>();
    if (!client) {
        NS_LOG_ERROR("No TCPclient found for " << toStr);
        return -1;
    }

    // Please put these into a separate function.
    PacketTraceTag tag;
    tag.SetIteration(iterationNum);
    client->SetSendTag(tag);
    NS_LOG_DEBUG("SendPacket: Tag set for iteration " << iterationNum);

    int sent = client->Send(buffer.data(), pktlen, true);
    NS_LOG_DEBUG("SendPacket: Attempted send, sent bytes: " << sent);

    if (sent > 0) {
        NS_LOG_DEBUG("Sent " << sent << " bytes for iteration " << iterationNum);
        if (cGroup.empty()) {
            NS_LOG_DEBUG("No children group; calling ProduceVToP for iteration " << iterationNum + 1);
            ProduceVToP(iterationNum + 1);
        } else {
            NS_LOG_DEBUG("Children group exists; calling UpdateQueue from " << fromStr);
            UpdateQueue(fromStr);
        }
    } else {
        NS_LOG_DEBUG("SendPacket: Sending failed for iteration " << iterationNum 
            << " from source " << thisAddress << ". Handling failure.");
        HandleSendFailure(toStr, iterationNum, buffer, fromStr);
    }
    
    return sent;
}

void InnetworkAggregationInterface::ProduceVToP(uint16_t iterationNum) {
    NS_LOG_DEBUG("ProduceVToP called for iteration " << iterationNum);
    if (iterationNum >= maxIteration) {
        NS_LOG_DEBUG("Maximum iteration reached, no more production.");
        return;
    }
    std::vector<uint64_t> initData(chunkSize, iterationNum);
    for (size_t i = 0; i < sGroup.size(); ++i) {
        std::string toStr;
        Addr2Str(sGroup[i], toStr);
        NS_LOG_DEBUG("Producing VToP for sGroup[" << i << "] to " << toStr);
        SendResponseVTo(toStr, initData, iterationNum, "");
    }
}

// Data Reception & Processing ================================================
void InnetworkAggregationInterface::ReceiveDataFromAll() {
    NS_LOG_DEBUG("ReceiveDataFromAll called at: " << Simulator::Now().GetMilliSeconds() << "ms");
    for (auto& entry : socketPool) {
        if (entry.second->GetObject<TCPserver>()) {
            NS_LOG_DEBUG("ReceiveDataFrom: found TCPserver for " << entry.first);
            ReceiveDataFrom(entry.first);
        }
    }
}

void InnetworkAggregationInterface::ReceiveDataFrom(std::string fromStr) {
    // NS_LOG_DEBUG("ReceiveDataFrom called for " << fromStr);
    if (compQueue.empty()) {
        // NS_LOG_DEBUG("compQueue is empty.");
        if (!isEnd) {
            // NS_LOG_DEBUG("Scheduling ReceiveDataFrom for " << fromStr << " in 300 ns");
            Simulator::Schedule(NanoSeconds(300), &InnetworkAggregationInterface::ReceiveDataFrom,
                                this, fromStr);
        }
        return;
    }

    uint16_t iterNum = compQueue.front();
    NS_LOG_DEBUG("ReceiveDataFrom: Processing iteration number: " << iterNum);
    if (!sGroup.empty()) {
        NS_LOG_DEBUG("sGroup is not empty. Sending response VToP.");
        SendResponseVToP(iterChunk[iterNum].vec, iterNum, fromStr);
    } else {
        NS_LOG_DEBUG("sGroup is empty. Updating queue.");
        UpdateQueue(fromStr);
    }
}

void InnetworkAggregationInterface::TriggerHandleRead() {
    NS_LOG_DEBUG("TriggerHandleRead called.");
    for (auto& entry : socketPool) {
        if (Ptr<TCPserver> server = entry.second->GetObject<TCPserver>()) {
            NS_LOG_DEBUG("Triggering handle-read for server at " << entry.first);
            server->CallSendEmptyPacket();
            server->CallHandleRead();
        }
    }
}

// Queue Management ===========================================================
void InnetworkAggregationInterface::UpdateQueue(std::string fromStr) {
    NS_LOG_DEBUG("UpdateQueue called for fromStr: " << fromStr);
    if (compQueue.empty()) {
        NS_LOG_DEBUG("compQueue is empty, nothing to update.");
        return;
    }

    uint16_t iterNum = compQueue.front();
    NS_LOG_DEBUG("UpdateQueue: Processing iteration " << iterNum);
    successIter.push(iterNum);
    compQueue.pop();
    iterChunk.erase(iterNum);
    NS_LOG_DEBUG("UpdateQueue: Removed iteration " << iterNum << " from compQueue and iterChunk.");

    if ((isEnd = PrintCompInfo(iterNum))) {
        NS_LOG_DEBUG("UpdateQueue: PrintCompInfo returned true for iteration " << iterNum);
        return;
    }

    NS_LOG_DEBUG("Processed iteration " << iterNum << " from " << fromStr);
    TriggerHandleRead();
    ReceiveDataFrom(fromStr);
}

bool InnetworkAggregationInterface::CheckQueueOrder(std::queue<uint16_t> q,
                                                   uint16_t maxIter)
{
    NS_LOG_DEBUG("CheckQueueOrder called with maxIter " << maxIter);
    for (uint16_t i = 0; i < maxIter; ++i) {
        if (q.empty() || q.front() != i) {
            NS_LOG_ERROR("Queue order violation at iteration " << i);
            return false;
        }
        q.pop();
    }
    NS_LOG_DEBUG("Queue order is valid.");
    return true;
}

// Utility Functions ==========================================================

bool InnetworkAggregationInterface::PrintCompInfo(uint16_t iterationNum) {
    NS_LOG_DEBUG("PrintCompInfo called for iteration " << iterationNum);
    Time now = Simulator::Now();
    
    if (successIter.size() >= maxIteration - padIter) {
        if (sGroup.empty()) {
            NS_LOG_UNCOND("Consumer completed all iterations-" << successIter.size() << " in " 
                       << (now.GetMilliSeconds() - 2000) << "ms: " << currentFileName);
            Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 0.001));
        } else {
            NS_LOG_INFO("Aggregator " << thisAddress 
                       << " completed processing");
        }
        return true;
    }

    // Zhuoxu: Print every iteration.
    if ((iterationNum + 1) % 500 == 0) {
        // PrintRxBuffer("10.0.1.1");
        std::string nodeType = sGroup.empty() ? "Consumer" : "Aggregator";
        NS_LOG_INFO(nodeType << " " << thisAddress 
                   << " completed iteration " << iterationNum+1
                   << " at " << now.GetMilliSeconds() << "ms");
    }
    return false;
}

uint16_t InnetworkAggregationInterface::HashIpToPort(const std::string& ip) {
    NS_LOG_DEBUG("HashIpToPort called for ip: " << ip);
    std::hash<std::string> hasher;
    uint16_t port = static_cast<uint16_t>(hasher(ip) % 65536);
    NS_LOG_DEBUG("HashIpToPort: Computed port " << port << " for ip: " << ip);
    return port;
}

// Debugging Tools ============================================================
void InnetworkAggregationInterface::PrintAllInfo() {
    NS_LOG_DEBUG("PrintAllInfo called for node " << thisAddress);
    
    // Client status
    for (auto& entry : socketPool) {
        if (Ptr<TCPclient> client = entry.second->GetObject<TCPclient>()) {
            NS_LOG_DEBUG("Client to " << entry.first 
                       << " CWND: " 
                       << client->GetSocket()->GetObject<TcpSocketBase>()
                          ->GetTcpSocketState()->m_cWnd);
        }
    }

    // Server status
    for (auto& entry : socketPool) {
        if (Ptr<TCPserver> server = entry.second->GetObject<TCPserver>()) {
            NS_LOG_DEBUG("Printing table for server " << entry.first);
            server->PrintAppTable();
            break;
        }
    }
}

void InnetworkAggregationInterface::TraceSingleNodeInfo() {
    NS_LOG_DEBUG("TraceSingleNodeInfo called for node " << thisAddress);
    if (thisAddress == TraceIPAddress) {
        NS_LOG_UNCOND("Tracing node: " << thisAddress);
        PrintAllInfo();
    }
}

void InnetworkAggregationInterface::PrintRxBuffer(std::string chStr) {
    if (Ptr<TCPserver> server = socketPool[chStr]->GetObject<TCPserver>()) {
        NS_LOG_DEBUG("RxBuffer for " << chStr << ":");
        LogComponentEnable("TcpRxBuffer", LOG_LEVEL_DEBUG);
        // server->GetSocket()->GetObject<TcpSocketBase>()->GetRxBuffer()->PrintRxBuffer();
        LogComponentDisable("TcpRxBuffer", LOG_LEVEL_DEBUG);
    }
}

// Private Helpers ============================================================
void InnetworkAggregationInterface::HandleSendFailure(std::string toStr,
                                                     uint16_t iterNum,
                                                     std::vector<uint8_t>& buf,
                                                     std::string fromStr)
{
    NS_LOG_DEBUG("Send failed for iteration " << iterNum << ", retrying...");
    
    if (cGroup.empty()) {
        Simulator::Schedule(MilliSeconds(1), &InnetworkAggregationInterface::SendPacket,
                          this, toStr, iterNum, buf, fromStr);
    } else if (sendSchedSet.find(iterNum) == sendSchedSet.end()) {
        Simulator::Schedule(MilliSeconds(1), &InnetworkAggregationInterface::SendPacket,
                          this, toStr, iterNum, buf, fromStr);
        sendSchedSet[iterNum] = fromStr;
    } else if (sendSchedSet[iterNum] == fromStr) {
        Simulator::Schedule(MilliSeconds(1), &InnetworkAggregationInterface::SendPacket,
                          this, toStr, iterNum, buf, fromStr);
    }
}

} // namespace ns3

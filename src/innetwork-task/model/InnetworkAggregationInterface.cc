#include "InnetworkAggregationInterface.h"
#include <cstring> 
#include <cstdlib>   // 包含rand()和srand()
#include <ctime>     // 包含time()


namespace ns3 {

    // Overload the << operator for std::queue
    template <typename T>
    std::ostream& operator<<(std::ostream& os, std::queue<T> q) {
        os << "Queue elements: ";
        while (!q.empty()) {
            os << q.front() << " ";
            q.pop();
        }
        return os;
    }

    NS_LOG_COMPONENT_DEFINE ("InnetworkAggregationInterface");
    NS_OBJECT_ENSURE_REGISTERED (InnetworkAggregationInterface);

    TypeId 
    InnetworkAggregationInterface::GetTypeId (void) {
        static TypeId tid = TypeId ("ns3::InnetworkAggregationInterface")
            .SetParent<Object> ()
            .SetGroupName("innetwork-train")
            .AddConstructor<InnetworkAggregationInterface> ()
        ;
        return tid;
    };

    InnetworkAggregationInterface::InnetworkAggregationInterface () {
        //NS_LOG_FUNCTION (this);
        this->printCount = 0;
        compQueue = std::queue<uint16_t>();
        // sendSuccToPSet = std::set<uint16_t>();
        // sendSchedSet = std::set<uint16_t>();
    }

    InnetworkAggregationInterface::~InnetworkAggregationInterface () {
        //NS_LOG_FUNCTION (this);
        for (auto& pair : socketPool) {
        // 确保删除所有指针以释放内存
            pair.second = nullptr;
        }
        socketPool.clear();
    }

    void 
    InnetworkAggregationInterface::SetupInnetworkAggregationInterface (uint16_t port, uint16_t itr, uint8_t aggTreeLevel, std::vector<Address> &sGroup, 
                        std::vector<Address> &cGroup, Ptr<Node> node, bool isEnd) {
        //NS_LOG_FUNCTION (this);
        this->isEnd = isEnd;
        this->m_peerPort = port;
        this->node = node;
        this->maxIteration = itr;
        this->currentIteration = 0;
        //this->pktlen = BASESIZE+10;
        //this->rqtlen = 20;
        //this->chunkSize = static_cast<uint16_t>(BASESIZE/sizeof(uint64_t));
        this->aggTreeLevel = aggTreeLevel;

        this->iterationNum = 0;
        DeepVectorCopy (this->sGroup, sGroup);
        DeepVectorCopy (this->cGroup, cGroup);

        if(node == nullptr)
            NS_LOG_DEBUG("port: " << port);
    }

    void
    InnetworkAggregationInterface::SetVSize (uint32_t size) {
        //NS_LOG_FUNCTION (this);
        this->vsize = size;
    }

    ns3::Ipv4Address
    InnetworkAggregationInterface::GetIpAddrFromNode (Ptr<Node> node){
        NS_LOG_FUNCTION (this);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
        Ipv4Address addr = iaddr.GetLocal ();
        return addr;
    }


    // socketPool:
    // <addrString(IPv4),Ptr<Application> 
    void 
    InnetworkAggregationInterface::CreateSocketPool (std::string cc_name) {
        NS_LOG_DEBUG("Connection Setting Time: "<< Simulator::Now().GetMilliSeconds() << "ms");
        //NS_LOG_FUNCTION (this);
        std::string addrStr;
        auto serverAddr = GetIpAddrFromNode(this->node);
        std::string serverAddrStr;
        Addr2Str(serverAddr, serverAddrStr);
        this->thisAddress = serverAddrStr;

        // Zhuoxu: Becareful that client and server should be binded to different ports. Otherwise there will be error.
        for (uint8_t i = 0; i < this->sGroup.size(); ++i) {
            NS_LOG_DEBUG("******sGroup[" << static_cast<int>(i) << "]*********");
            Addr2Str(sGroup[i], addrStr);
            Ptr<TCPclient> myclient = CreateObject<TCPclient>();
            myclient->SetNode(node);

            // Hash(IpAddress(thisNode))
            uint16_t serverPort = HashIpToPort(serverAddrStr);
            NS_LOG_DEBUG("serverPort: "<<serverPort);
            myclient->SetRemote(Ipv4Address::ConvertFrom(sGroup[i]), serverPort);
            myclient->Bind(m_peerPort);
            socketPool[addrStr] = myclient;
            NS_LOG_DEBUG( "client ptr:" << myclient );
            NS_LOG_DEBUG(this << " CreateSocketPool: socketPool["<< addrStr << "] -myclient, port: " << m_peerPort);
            
            NS_LOG_DEBUG( " CreateSocketPool: socketPool["<< addrStr << "] -myclient, m_peerPort: " << m_peerPort );
            
            NS_LOG_DEBUG("local ip: " << serverAddrStr);
            NS_LOG_DEBUG("---------End-----------\n");
        }

        // Setup itself as servers, if it has children
        for (uint8_t i = 0; i < this->cGroup.size(); ++i) {
            NS_LOG_DEBUG("******cGroup[" << static_cast<int>(i) << "]*********");
            Addr2Str(cGroup[i], addrStr);

            // Hash(IpAddress(cGroup[i]))
            uint16_t serverPort = HashIpToPort(addrStr);
            NS_LOG_DEBUG("serverPort: "<<serverPort);
            
            // Zhuoxu: We only pass the node parameter in this function.
            Ptr<TCPserver> myserver = CreateObject<TCPserver>(node);
            // Zhuoxu: Here, we should defind another set function that pass the address of the IterationChunk to all the servers binded in this node.
            // myserver->SetNode(node);
            // myserver->SetcGroupSize(cGroup.size());
            // myserver->SetIterChunkPtr(&this->iterChunk);
            myserver->Bind(serverPort);
            // set server IP address
            // myserver->SetPeerAddrStr(addrStr);
            // myserver->SetLocalAddressStr(this->thisAddress);
            
            // NS_LOG_DEBUG("Output id for the server: socketPool[" << addrStr << "] " << );
            myserver->GetSocket()->GetObject<TcpSocketBase>()->GetTxAvailable();

            myserver->SetParams(node, cGroup.size(), &this->iterChunk, addrStr, this->thisAddress, &this->compQueue);

            // Zhuoxu: socket[] is owned by each node, therefore there won't be any conflict on the same port.
            socketPool[addrStr] = myserver;
            
            std::cout<<"myserver ptr:"<<myserver<<std::endl;
            NS_LOG_DEBUG(this << " CreateSocketPool: socketPool["<< addrStr << "] -myserver, serverPort: " << serverPort);

            std::cout << " CreateSocketPool: socketPool[" << addrStr << "] -myserver, serverPort: " << serverPort << ", cGroup[" << i - uint8_t(0) << "]: " << addrStr << std::endl;

            NS_LOG_DEBUG("---------End-----------\n");
        }

        NS_LOG_DEBUG("Connection Setting Finishing in: "<< Simulator::Now().GetMilliSeconds() << "ms\n");
        NS_LOG_DEBUG( this << "this->address: " << this->thisAddress << " sGroup.size(): " << this->sGroup.size() << " cGroup.size(): " << this->cGroup.size());

        NS_LOG_DEBUG( "-------Node " << this->thisAddress << " Connection Setting Finishing in: " << Simulator::Now().GetMilliSeconds() << "ms-----------\n");
    }

    void 
    InnetworkAggregationInterface::ReceiveDataFromAll () {
        NS_LOG_DEBUG("ReceiveDataFromAll: "<< Simulator::Now().GetMilliSeconds() << "ms");
        for (auto item : socketPool) {
            // Zhuoxu: only receive data from the server
            if ( socketPool[item.first]->GetObject<TCPserver>() )
                ReceiveDataFrom (item.first);
        }
    }

    // Zhuoxu: This function is really important! We should be careful in the quic!
    void 
    InnetworkAggregationInterface::TriggerHandleRead () {
        // Zhuoxu: Since we share the memory table, here we should trigger the handle read function of all the servers.
        for (auto item : socketPool) {
            // Zhuoxu: only receive data from the server
            if ( socketPool[item.first]->GetObject<TCPserver>() ){
                Ptr<TCPserver> server = socketPool[item.first]->GetObject<TCPserver>();
                // We first let the receiver to send the space message to the sender.
                server->CallSendEmptyPacket();
                server->CallHandleRead();
            }
        }
        return;
    }

    bool
    InnetworkAggregationInterface::PrintCompInfo(uint16_t iterationNum){

        // LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_INFO);

        Time currentTime = Simulator::Now();
        //check if all the iteration has been collected
        if (successIter.size() == maxIteration - padIter) {
            if (this->sGroup.size() <= 0) {
                NS_LOG_INFO("Consumer All iteration-"<< maxIteration << " completed in: " << currentTime.GetMilliSeconds() - 2000<< "ms");
                std::cout << "Consumer All iteration-"<< maxIteration << " completed in: " << currentTime.GetMilliSeconds() - 2000<< "ms" << std::endl;
                if(CheckQueueOrder(this->successIter, maxIteration)){
                    NS_LOG_INFO("Consumer Order is correct.");
                }
                else{
                    NS_LOG_INFO("Consumer Order is incorrect.");
                }
                Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 0.001));
            }
            else{
                NS_LOG_INFO("Receive the ending packet. Finish Process.");
                if(CheckQueueOrder(this->successIter, maxIteration)){
                    NS_LOG_INFO( this << " ----Aggregator Order is correct.");
                }
                else{
                    NS_LOG_INFO( this << " ----Aggregator Order is incorrect.");
                }
            }
            return true;
        }
        if (this->sGroup.size() <= 0 && ((iterationNum + 1) % 1000 == 0))
            NS_LOG_INFO("IterationNum-"<<iterationNum+1-uint16_t(0)<< ", Consumer completed in: " << currentTime.GetMilliSeconds() << "ms");
        else if((iterationNum + 1) % 1000 == 0)
            NS_LOG_INFO("IterationNum-"<<iterationNum+1-uint16_t(0)<< ", Aggregator " << this->thisAddress << " completed in: " << currentTime.GetMilliSeconds() << "ms");
        
        // Zhuoxu: We should be really careful here, otherwise, 
        // LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);

        return false;
    }

    void 
    InnetworkAggregationInterface::ReceiveDataFrom (std::string fromStr) {
        if(this->thisAddress == TraceIPAddress){
            NS_LOG_DEBUG(this << " entering the ReceiveDataFrom of TraceIPAddress: " << this->thisAddress);
        }
        Ptr<TCPserver> server;
        server = socketPool[fromStr]->GetObject<TCPserver>();

        // Solution 1: We first try not to pass the address of the queue of the server, to see if it works. If this not works, then we should pass the address of the queue, which needs to modify the function "TCPserver::GetCompIterQueue()". Here, we just pass the copy of the queue, and clear at once.
        if(!this->compQueue.empty()){
            // 
            uint16_t iterNum = this->compQueue.front();
            // if it is aggregator
            if(this->sGroup.size() > 0)
                SendResponseVToP (iterChunk[iterNum].vec, iterNum, fromStr);
            // else if it is consumer
            else{
                UpdateQueue(fromStr);
            }
        }
        else{
            
            if(!isEnd){
                
                // test if the interval of the millisecond will affect the process? Here we try 1ms to see what's happen.
                ns3::Simulator::Schedule(ns3::NanoSeconds(300), &InnetworkAggregationInterface::ReceiveDataFrom, this, fromStr);
            }
        }
    // Zhuoxu: we don't need this currently.
    // void 
    // InnetworkAggregationInterface::ScheduleSendResponseVToP (std::vector<uint64_t> &vec , uint16_t iterationNum){
    //     if(SendResponseVToP (iterChunk[iterNum].vec, iterNum) > 0)
    //         return;
    //     else
    //         ns3::Simulator::Schedule(ns3::MilliSeconds(4), &InnetworkAggregationInterface::ScheduleSendResponseVToP, this, vec, iterationNum);
    // }
    }

    int 
    InnetworkAggregationInterface::SendResponseVToP (std::vector<uint64_t> &avg , uint16_t iterationNum, std::string fromStr) {
        NS_LOG_FUNCTION (this << " iteration: " << iterationNum);
        int flag = -1;
        for (uint8_t i = 0; i < this->sGroup.size (); ++i) {
            std::string toStr;
            Addr2Str (this->sGroup[i], toStr);
            flag = SendResponseVTo (toStr, avg, iterationNum, fromStr);
            NS_LOG_FUNCTION (this << "SendResponseVTo (toStr, avg, iterationNum, fromStr);");
            NS_LOG_FUNCTION ("SendResponseVTo(" << toStr << ", avg, " << static_cast<int>(iterationNum) << ", " << fromStr << ") " << static_cast<int>(i));
        }
        return flag;
    }

    // serialize and slice vec into chunks of k elements, that is k*8 uint8_t/chunk.
    // send data prepare.
    int
    InnetworkAggregationInterface::SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec, uint16_t iterationNum, std::string fromStr) {
        NS_LOG_FUNCTION (this);
        int flag = -1;
        // Zhuoxu-bugfix: for aggregator, we should check if the children has already called the schedule function.
        // if (this->sGroup.size() > 0 && sendSuccToPSet.find(iterationNum) != sendSuccToPSet.end())
        //     return flag;
        //uint16_t iterationNum = 0;
        //Ptr<TCPserver> server = socketPool[toStr]->GetObject<TCPserver>();
        uint32_t pos = 0;
        uint32_t end = vsize*sizeof(uint64_t); //transalte from uint64_t to uint8_t
        uint8_t type = 1;
        //uint8_t *serializeVec = new uint8_t[end]();
        std::vector<uint8_t> serializeVec = std::vector<uint8_t>(end,0);
        //uint8_t *chunkBuffer = new uint8_t[pktlen]();note that: uint16_t pktlen = BASESIZE + 10; So the first 10 position are for chunkNum and type.
        SerializeVector(vec,serializeVec.data());
        std::vector<uint8_t> chunkBuffer = std::vector<uint8_t>(pktlen,0); //Zhuoxu: 
        NS_LOG_DEBUG( this << " pktlen " << pktlen);
        uint8_t* buffer = serializeVec.data(); 
        // mark 33... as the type of the send data packet
        for (uint16_t i = 0; i < 8; ++i) {
            chunkBuffer[i] = 3;
        }

        chunkBuffer[8] = static_cast<uint8_t>((iterationNum >> 8) & 0xFF); // High byte
        chunkBuffer[9] = static_cast<uint8_t>(iterationNum & 0xFF);
        std::copy(buffer, buffer + BASESIZE, chunkBuffer.data() + 10);
        flag = SendPacket(toStr, iterationNum, chunkBuffer, fromStr);
        return flag;
    }

    // SendPacket function should be able to distinguish leaf node and non-leaf node. Their behavior is different.
    int
    InnetworkAggregationInterface::SendPacket (std::string toStr, uint16_t iterationNum, std::vector<uint8_t> &chunkBuffer, std::string fromStr){
        //NS_LOG_INFO("iteration-"<<iterationNum-uint16_t(0));

        // Zhuoxu: send ending packet for padding. Otherwise, packet for the last iteration cannot be process because of size(MTU) != size(per quic frame)
        if (iterationNum == maxIteration ){
            std::vector<uint8_t> chunkBuffer = std::vector<uint8_t>(pktlen,0);

            int sentSize = -1;
            for (uint16_t i = 0; i < 10; ++i) {
                chunkBuffer[i] = 7;
            }

            for(int i=10;i<pktlen;i++)
                chunkBuffer[i] = 9;

            return SendEndPacket (toStr, chunkBuffer, fromStr);
        }

        // Zhuoxu: change the producer to client here
        Ptr<TCPclient> client = socketPool[toStr]->GetObject<TCPclient>();
        NS_LOG_DEBUG( this << " Sends data to socketPool["<<toStr<<"] ");
        // NS_LOG_DEBUG( client->m_socket->GetObject<QuicSocketBase>()->m_txBuffer->PrintToStr());

        // record the time RTT
        // 在这里获取是否
        int sentSize = client->Send(chunkBuffer.data(),pktlen);
        // determine if the sent operation success
        if(sentSize > 0){
            NS_LOG_DEBUG(this << " client->Send()--sentSize success: " << sentSize << " at iteration "<<iterationNum-uint16_t(0));
            // Zhuoxu: block this currently. Since this will output unneccssary info.
            // PrintBufferSummary(chunkBuffer);
            
            if (this->cGroup.size() == 0)
                ProduceVToP (iterationNum + 1);
            // else block for non-leaf node: we should trigger receive data function to check if there is available data to process.
            else if (this->cGroup.size() > 0){
                // for aggregator, we should avoid sending multiple packets from different children.
                // sendSuccToPSet.insert(iterationNum);
                UpdateQueue(fromStr);
            }
        }
        else{
            NS_LOG_DEBUG(this << " client->Send() failed at iteration " << iterationNum - uint16_t(0) << " schedule next send");
            // Zhuoxu: we first try 100ns, to see what happen...
            // Zhuoxu-bugfix: for aggregator, check if the children has already called the schedule function. Just Becareful of the condition that we should care. Besides, we should also distinguish the behavior of producer.
            // Zhuoxu-bugfix: Producer behavior
            if (this->cGroup.size() == 0)
                ns3::Simulator::Schedule(ns3::MilliSeconds(1), &InnetworkAggregationInterface::SendPacket, this, toStr, iterationNum, chunkBuffer, fromStr);
            // Zhuoxu-bugfix: Aggregator first time send, but at the same time block the send request for the other children.
            else if (this->cGroup.size() > 0 && this->sGroup.size() > 0 && sendSchedSet.find(iterationNum) == sendSchedSet.end())
            {
                ns3::Simulator::Schedule(ns3::MilliSeconds(1), &InnetworkAggregationInterface::SendPacket, this, toStr, iterationNum, chunkBuffer, fromStr);
                sendSchedSet[iterationNum] = fromStr;
                if(thisAddress == TraceIPAddress){
                    NS_LOG_DEBUG( TraceIPAddress << " sendSchedSet.find(iterationNum) == sendSchedSet.end()");
                }
            }
            // Zhuoxu-bugfix: Aggregator non-first time send from the same child
            else if (this->cGroup.size() > 0 && this->sGroup.size() > 0 && sendSchedSet.find(iterationNum) != sendSchedSet.end() && sendSchedSet[iterationNum] == fromStr)
            {
                ns3::Simulator::Schedule(ns3::MilliSeconds(1), &InnetworkAggregationInterface::SendPacket, this, toStr, iterationNum, chunkBuffer, fromStr);
                if(thisAddress == TraceIPAddress){
                    NS_LOG_DEBUG( TraceIPAddress << " sendSchedSet[iterationNum] == fromStr");
                }
            }
        }
        return sentSize;
        // if it is a producer, then begin the next send call of next iteration.
    }

    int
    InnetworkAggregationInterface::SendEndPacket (std::string toStr, std::vector<uint8_t> &chunkBuffer, std::string fromStr){
        // Zhuoxu: change the producer to client here
        Ptr<TCPclient> client = socketPool[toStr]->GetObject<TCPclient>();
        NS_LOG_DEBUG( this << " Sends End Packet to socketPool["<<toStr<<"] ");

        // Zhuoxu: for debug
        std::cout << "\n Sends End Packet from " << thisAddress << " to " << toStr << std::endl;

        int sentSize = client->Send(chunkBuffer.data(),pktlen);
        // determine if the sent operation success

        // Zhuoxu: only check buffer for TraceIPAddress
        return sentSize;
    }

    void 
    InnetworkAggregationInterface::ProduceVToP (uint16_t iterationNum){
        if(iterationNum == this->maxIteration)
            return;
        for (uint8_t i = 0; i < this->sGroup.size (); ++i) {
            std::string toStr;
            Addr2Str (this->sGroup[i], toStr);
            std::vector<uint64_t> initData(chunkSize, static_cast<int>(iterationNum));
            SendResponseVTo (toStr, initData, iterationNum);
            }
    }

    void 
    InnetworkAggregationInterface::SaveResult (std::vector<uint64_t> &avg ) {
        //NS_LOG_FUNCTION (this);
        //save result
        for (uint16_t i = 0; i < uint16_t(avg. size ()); ++i) {
            outFile << avg[i];
            // after saving, clear it
            avg[i] = 0;
            // Add space except for the last element
            if (i != avg. size () - 1) {
                outFile << " ";
            }

        }
        outFile << std::endl; // End the line
    }

    void 
    InnetworkAggregationInterface::SetOutFile (const std::string fileName) {
        //NS_LOG_FUNCTION (this);
        outFile.open(fileName, std::ios::app);
        if (!outFile.is_open()) {
            NS_LOG_INFO ("Failed to open file" << fileName);
        }
    }

    void 
    InnetworkAggregationInterface::Addr2Str (Address addr, std::string &str) {
    std::stringstream ss;
    ss << Ipv4Address::ConvertFrom (addr);
    str = ss.str();
    }

    uint16_t 
    InnetworkAggregationInterface::HashIpToPort(const std::string& ip) {
    // Use std::hash to hash the IP address string
    std::hash<std::string> hash_fn;
    size_t hash = hash_fn(ip);

    // Use modulo operation to fit the hash into the uint16_t range
    uint16_t port = static_cast<uint16_t>(hash % 65536);

    return port;
    }

    // Zhuoxu: DIY function to print all the buffer and table information.
    void 
    InnetworkAggregationInterface::PrintAllInfo(){
        EnableLoggingComponents();
        // Print IP address
        NS_LOG_DEBUG (this << " In PrintAllInfo. Local ip: " << this->thisAddress);

        // Print TxBuffer
        for (auto& item : socketPool) {
            if(socketPool[item.first]->GetObject<TCPclient>()){
                NS_LOG_DEBUG("Print TxBuffer for Peer ip: " << item.first);   
                Ptr<TCPclient> client = socketPool[item.first]->GetObject<TCPclient>();
                NS_LOG_INFO("Print cwnd: " << client->GetSocket()->GetObject<TcpSocketBase>()->GetTcpSocketState()->m_cWnd);
                // client->GetSocket()->GetObject<TcpSocketBase>()->GetTxBuffer()->PrintTxBuffer();
            }
        }

        // Print RxBuffer

        // for (auto& item : socketPool) {
        //     if(socketPool[item.first]->GetObject<TCPserver>()){
        //         NS_LOG_DEBUG("Print RxBuffer for Peer ip: " << item.first);
        //         Ptr<TCPserver> server = socketPool[item.first]->GetObject<TCPserver>();
        //         NS_LOG_DEBUG(server->GetSocket()->GetObject<TcpSocketBase>()->GetRxBuffer()->PrintRxBuffer());
        //     }
        // }

        // Print Table
        
        for (auto& item : socketPool) {
            if(socketPool[item.first]->GetObject<TCPserver>()){
                NS_LOG_DEBUG("Print Application Table information ");
                Ptr<TCPserver> server = socketPool[item.first]->GetObject<TCPserver>();
                server->PrintTable();
                break;
            }
        }

        DisableLoggingComponents();
    }

    void 
    InnetworkAggregationInterface::EnableLoggingComponents() {
    LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
    LogComponentEnable("TCPclient", LOG_LEVEL_ALL);
    LogComponentEnable("TCPserver", LOG_LEVEL_ALL);
    LogComponentEnable("TcpSocketBase", LOG_LEVEL_ALL);
    LogComponentEnable("TcpRxBuffer", LOG_LEVEL_ALL);
    LogComponentEnable("TcpTxBuffer", LOG_LEVEL_ALL);
    LogComponentEnable("Packet", LOG_LEVEL_DEBUG);
    }

    void 
    InnetworkAggregationInterface::DisableLoggingComponents() {
    LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
    LogComponentDisable("TCPclient", LOG_LEVEL_ALL);
    LogComponentDisable("TCPserver", LOG_LEVEL_ALL);
    LogComponentDisable("TcpSocketBase", LOG_LEVEL_ALL);
    LogComponentDisable("TcpRxBuffer", LOG_LEVEL_ALL);
    LogComponentDisable("TcpTxBuffer", LOG_LEVEL_ALL);
    LogComponentDisable("Packet", LOG_LEVEL_DEBUG);
    }

    void 
    InnetworkAggregationInterface::TraceSingleNodeInfo(){
        if(this->thisAddress == TraceIPAddress)
        {
            std::cout << "Trace the node information for " << this->thisAddress << std::endl;
            PrintAllInfo();
            // Zhuoxu: we don't need the recursive loop now.
            // ns3::Simulator::Schedule(ns3::MilliSeconds(50), &InnetworkAggregationInterface::TraceSingleNodeInfo, this);
            return;
        }        
    }

    bool 
    InnetworkAggregationInterface::CheckQueueOrder(std::queue<uint16_t> q, uint16_t maxIteration) {
    std::ostringstream logStream;
    logStream << "In the CheckQueueOrder function: " << std::endl;

    for (uint16_t i = 0; i < maxIteration; ++i) {
        if (q.empty() || q.front() != i) {
            std::ostringstream errorStream;
            errorStream << "\nExpected: " << i << ", but queue is ";
            if (q.empty()) {
                errorStream << "empty.";
            } else {
                errorStream << "not empty and front element is " << q.front() << ".";
            }
            errorStream << std::endl;
            NS_LOG_ERROR(errorStream.str());
            return false;
        }
        logStream << q.front() << " ";
        q.pop();
    }
    logStream << std::endl;
    NS_LOG_DEBUG(logStream.str());
    return q.empty();
}

    void
    InnetworkAggregationInterface::UpdateQueue(std::string fromStr){
        // get queue pointer
        Ptr<TCPserver> server;
        server = socketPool[fromStr]->GetObject<TCPserver>();
        //  = server->GetCompIterQueue();

        uint16_t iterNum = this->compQueue.front();
        successIter.push(iterNum);
        // ToDo: We need to complete a schedule function to make sure that the SendResponseVToP must success, and then iterQueuePtr->pop() can be executed. This order could not be twisted.
        // We should put the pointer of the server socketpool into a queue, to prevent different children node process.
        NS_LOG_DEBUG( this << " iterQueuePtr->pop();");
        this->compQueue.pop();
        // clear the iterChunk
        auto it = iterChunk.find(iterNum);
        iterChunk.erase(it);
        isEnd = PrintCompInfo(iterNum);
        NS_LOG_DEBUG("fromStr: " << fromStr << " current successIter.size(): " << successIter.size());
        NS_LOG_DEBUG("isEnd " << isEnd);

        /*
        if(this->thisAddress == TraceIPAddress){
            std::cout << "Print successIter of " <<  TraceIPAddress << std::endl;
            // for (const auto& element : successIter) {
            //     std::cout << element << "-";
            // }
            // std::cout << std::endl;
            std::cout << successIter << std::endl;
        }
        */
        NS_LOG_DEBUG("fromStr: " << fromStr << ", current " << successIter.size());
        // Call handleRead function after the memory has been released. Now we call handleread when each data is processed.
        TriggerHandleRead ();
        this->ReceiveDataFrom (fromStr);
    }

    void 
    InnetworkAggregationInterface::PrintRxBuffer(std::string chStr){
        Ptr<TCPserver> server = socketPool[chStr]->GetObject<TCPserver>();
        NS_LOG_DEBUG("checking the RxBuffer of child node: " << chStr << ", this node: " << this->thisAddress << " content for debug...");
        LogComponentEnable("TcpRxBuffer", LOG_LEVEL_ALL);
        // NS_LOG_DEBUG(server->GetSocket()->GetObject<TcpSocketBase>()->GetRxBuffer()->PrintRxBuffer());
        LogComponentDisable("TcpRxBuffer", LOG_LEVEL_ALL);
    }

}; /*namespace ns3*/
#include "InnetworkAggregationInterface.h"
#include <cstring> 
namespace ns3 {
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
            std::cout<<"client ptr:"<<myclient<<std::endl;
            NS_LOG_DEBUG(this << " CreateSocketPool: socketPool["<< addrStr << "] -myclient, port: " << m_peerPort);
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
            myserver->SetNode(node);
            myserver->SetcGroupSize(cGroup.size());
            myserver->SetIterChunkPtr(&this->iterChunk);
            // Todo: initialize the chunkMap here
            // Todo: myserver->SetIterationChunk(&chunkMap[i]);
            myserver->Bind(serverPort);
            // set server IP address
            myserver->SetIpAddrStr(addrStr);
            
            // NS_LOG_DEBUG("Output id for the server: socketPool[" << addrStr << "] " << );
            myserver->GetSocket()->GetObject<TcpSocketBase>()->GetTxAvailable();

            // Zhuoxu: socket[] is owned by each node, therefore there won't be any conflict on the same port.
            socketPool[addrStr] = myserver;
            
            std::cout<<"myserver ptr:"<<myserver<<std::endl;
            NS_LOG_DEBUG(this << " CreateSocketPool: socketPool["<< addrStr << "] -myserver, port: " << serverPort);
            NS_LOG_DEBUG("---------End-----------\n");
        }

        NS_LOG_DEBUG("Connection Setting Finishing in: "<< Simulator::Now().GetMilliSeconds() << "ms\n");
    }

    void 
    InnetworkAggregationInterface::ReceiveDataFromAll () {
        for (auto item : socketPool) {
            // Zhuoxu: only receive data from the server
            if ( socketPool[item.first]->GetObject<TCPserver>() )
                ReceiveDataFrom (item.first);
        }
    }


    void 
    InnetworkAggregationInterface::TriggerHandleRead () {
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
        Time currentTime = Simulator::Now();
        //check if all the iteration has been collected
        if (successIter.size() == maxIteration - padIter) {
            if (this->sGroup.size() <= 0) {
                NS_LOG_INFO("Consumer All iteration-"<< maxIteration-1 << " completed in: " << currentTime.GetMilliSeconds() - 2000<< "ms");
                Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 0.001));
            }
            else{
                NS_LOG_INFO("Receive the ending packet. Finish Process.");
            }
            return true;
        }
        if (this->sGroup.size() <= 0)
            NS_LOG_INFO("IterationNum-"<<iterationNum-uint16_t(0)<< ", Consumer completed in: " << currentTime.GetMilliSeconds() << "ms");
        else
            NS_LOG_INFO("IterationNum-"<<iterationNum-uint16_t(0)<< ", Aggregator completed in: " << currentTime.GetMilliSeconds() << "ms");
        return false;
    }

    void 
    InnetworkAggregationInterface::ReceiveDataFrom (std::string fromStr) {
        // first handle read
        Ptr<TCPserver> server;
        server = socketPool[fromStr]->GetObject<TCPserver>();

        // Zhuoxu: comment this to see what happen.
        // NS_LOG_DEBUG("Runing the call handle read function in " << fromStr);
        // server->CallHandleRead();

        // check if there are any complete iteration.
        // here, we should pass a local vector to record all the finished iteration.
        std::queue<uint16_t> iterQueue = server->GetCompIterQueue();
        if(!iterQueue.empty()){
            NS_LOG_DEBUG( "fromStr: " << fromStr << ", Size of iterQueue: " << iterQueue.size());
            if(fromStr == "10.1.1.1"){
                NS_LOG_DEBUG("checking the RxBuffer content for debug...");
                NS_LOG_DEBUG(server->GetSocket()->GetObject<TcpSocketBase>()->GetRxBuffer()->PrintRxBuffer());
            }
        }
        else{
            if(fromStr == "10.1.1.1"){
                NS_LOG_DEBUG("iterQueue is empty, checking the RxBuffer content ...");
                NS_LOG_DEBUG(server->GetSocket()->GetObject<TcpSocketBase>()->GetRxBuffer()->PrintRxBuffer());
            }
        }

        if(!iterQueue.empty()){
            server->ClearCompQueue();
            // Output the log info

            // Send result to parent
            while (!iterQueue.empty()) {
                // NS_LOG_DEBUG("test....");
                uint16_t iterNum = iterQueue.front();
                successIter.insert(iterNum);
                iterQueue.pop();
                SendResponseVToP (iterChunk[iterNum].vec, iterNum);
                // clear the iterChunk
                auto it = iterChunk.find(iterNum);
                iterChunk.erase(it);
                isEnd = PrintCompInfo(iterNum);
                NS_LOG_DEBUG("fromStr: " << fromStr << " current successIter.size(): " << successIter.size());
                NS_LOG_DEBUG("isEnd " << isEnd);
            }
            // Call handleRead function after the memory has been released.
            TriggerHandleRead ();
        }
        NS_LOG_DEBUG("fromStr: " << fromStr << ", maxIteration " << maxIteration);
        if(!isEnd)
            ns3::Simulator::Schedule(ns3::MilliSeconds(10), &InnetworkAggregationInterface::ReceiveDataFrom, this, fromStr);
    }

    void 
    InnetworkAggregationInterface::SendResponseVToP (std::vector<uint64_t> &avg , uint16_t iterationNum) {
        NS_LOG_FUNCTION (this << " iteration: " << iterationNum);
        for (uint8_t i = 0; i < this->sGroup.size (); ++i) {
            std::string toStr;
            Addr2Str (this->sGroup[i], toStr);
            SendResponseVTo (toStr, avg, iterationNum);
        }
    }

    // serialize and slice vec into chunks of k elements, that is k*8 uint8_t/chunk.
    // send data prepare.
    void 
    InnetworkAggregationInterface::SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec, uint16_t iterationNum) {
        NS_LOG_FUNCTION (this);
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
        SendPacket(toStr, iterationNum, chunkBuffer);
    }

    void 
    InnetworkAggregationInterface::SendPacket (std::string toStr, uint16_t iterationNum, std::vector<uint8_t> &chunkBuffer){
        //NS_LOG_INFO("iteration-"<<iterationNum-uint16_t(0));

        // Zhuoxu: change the producer to client here
        Ptr<TCPclient> client = socketPool[toStr]->GetObject<TCPclient>();
        NS_LOG_DEBUG( this << " Sends data to socketPool["<<toStr<<"] ");
        NS_LOG_DEBUG( "print the send buffer of this client ...");
        // NS_LOG_DEBUG( client->m_socket->GetObject<QuicSocketBase>()->m_txBuffer->PrintToStr());

        // record the time RTT
        // 在这里获取是否
        int sentSize = client->Send(chunkBuffer.data(),pktlen);
        // determine if the sent operation success
        if(sentSize > 0){
            NS_LOG_DEBUG(this << " client->Send()--sentSize success: " << sentSize << " at iteration "<<iterationNum-uint16_t(0));
            if(thisAddress == "10.1.1.1")
            {
                NS_LOG_DEBUG("Print the TxBuffer of 10.1.1.1 when send success");
                std::cout << *(client->GetSocket()->GetObject<TcpSocketBase>()->GetTxBuffer()) << std::endl;
            }
            PrintBufferSummary(chunkBuffer);
            if (this->cGroup.size() == 0)
                ProduceVToP (iterationNum + 1);

        }
        else{
            NS_LOG_DEBUG(this << " client->Send() failed at iteration " << iterationNum - uint16_t(0) << " schedule next send");
            if(thisAddress == "10.1.1.1")
            {
                NS_LOG_DEBUG("Print the TxBuffer of 10.1.1.1 when send failed");
                client->GetSocket()->GetObject<TcpSocketBase>()->GetTxBuffer()->PrintTxBuffer();
                std::cout << *(client->GetSocket()->GetObject<TcpSocketBase>()->GetTxBuffer()) << std::endl;
            }
            // Zhuoxu: we first try 100ns, to see what happen...
            ns3::Simulator::Schedule(ns3::MilliSeconds(1), 
                         &InnetworkAggregationInterface::SendPacket,
                         this, toStr, iterationNum, chunkBuffer);
        }

        // Zhuoxu: send ending packet for padding. Otherwise, packet for the last iteration cannot be process because of size(MTU) != size(per quic frame)
        if (iterationNum == maxIteration - 1){
            std::vector<uint8_t> chunkBuffer = std::vector<uint8_t>(pktlen,0); 

            int sentSize = -1;
            for (uint16_t i = 0; i < 10; ++i) {
                chunkBuffer[i] = 7;
            }

            for(int i=10;i<pktlen;i++)
                chunkBuffer[i] = 9;

            SendEndPacket (toStr, chunkBuffer);
        }
        
        // if it is a producer, then begin the next send call of next iteration.
    }

    void 
    InnetworkAggregationInterface::SendEndPacket (std::string toStr, std::vector<uint8_t> &chunkBuffer){
        // Zhuoxu: change the producer to client here
        Ptr<TCPclient> client = socketPool[toStr]->GetObject<TCPclient>();
        NS_LOG_DEBUG( this << " Sends End Packet to socketPool["<<toStr<<"] ");

        int sentSize = client->Send(chunkBuffer.data(),pktlen);
        // determine if the sent operation success
        if(sentSize > 0){
            NS_LOG_DEBUG(this<<" client->Send() ending packet success --sentSize: " << sentSize <<" at iteration "<<iterationNum-uint16_t(0));
            PrintBufferSummary(chunkBuffer);
        }
        else{
            NS_LOG_DEBUG(this<<" client->Send() ending packet failed at iteration "<<iterationNum-uint16_t(0));
            ns3::Simulator::Schedule(ns3::MilliSeconds(10), 
                         &InnetworkAggregationInterface::SendEndPacket,
                         this, toStr, chunkBuffer);
        }
    }
    
    void 
    InnetworkAggregationInterface::PrintBufferSummary(std::vector<uint8_t>& chunkBuffer){
        // Zhuoxu: Todo: print the send content of aggregator
        std::stringstream ss;
        if(cGroup.size()>0 && sGroup.size()>0){
            NS_LOG_DEBUG("Agg send to consumer, the content is as follows: ");
            ss << "------------------------------------" << std::endl;
            for(int i = 0; i < 24; i++){
                ss << static_cast<int>(chunkBuffer[i]) << " ";
                if((i+1) % 8 == 0){
                    ss << " the " << (i+1) / 8 << "th byte" << std::endl;
                }
            } 
            for(int i = pktlen - 24; i < pktlen; i++) {
                ss << static_cast<int>(chunkBuffer[i]) << " ";
                if((i + 1) % 8 == 0) {
                    ss << " the " << (i + 1) / 8 << "th byte" << std::endl;
              }
            }
        }
        else if(cGroup.size()==0){
            NS_LOG_DEBUG("producer send to aggregator, the content is as follows: ");
            ss << "------------------------------------" << std::endl;
            for(int i = 0; i < 24; i++){
                ss << static_cast<int>(chunkBuffer[i]) << " ";
                if((i+1) % 8 == 0){
                    ss << " the " << (i+1) / 8 << "th byte" << std::endl;
                }
            }
            for(int i = pktlen - 24, j = 0; i < pktlen; i++, j++) {
                std::cout << static_cast<int>(chunkBuffer[i]) << " ";
                if((j + 1) % 8 == 0) {
                    ss << " the " << (i + 1) / 8 << "th byte" << std::endl;
                }
            }
        }
        NS_LOG_DEBUG("PrintBufferSummary:\n" << ss.str());
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
                // std::cout<<"iterationNum:"<<static_cast<int>(j)<<std::endl;
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

    void InnetworkAggregationInterface::Addr2Str (Address addr, std::string &str) {
    std::stringstream ss;
    ss << Ipv4Address::ConvertFrom (addr);
    str = ss.str();
    }

    uint16_t InnetworkAggregationInterface::HashIpToPort(const std::string& ip) {
    // Use std::hash to hash the IP address string
    std::hash<std::string> hash_fn;
    size_t hash = hash_fn(ip);

    // Use modulo operation to fit the hash into the uint16_t range
    uint16_t port = static_cast<uint16_t>(hash % 65536);

    return port;
    }

}; /*namespace ns3*/
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
    }

    void
    InnetworkAggregationInterface::SetVSize (uint32_t size) {
        //NS_LOG_FUNCTION (this);
        this->vsize = size;
    }

    // socketPool:
    // <addrString(IPv4),Ptr<Application> 
    void 
    InnetworkAggregationInterface::CreateSocketPool (std::string cc_name) {
        NS_LOG_DEBUG("Connection Setting Time: "<< Simulator::Now().GetMilliSeconds() << "ms");
        //NS_LOG_FUNCTION (this);
        std::string addrStr;


        // Zhuoxu: Becareful that client and server should be binded to different ports. Otherwise there will be error.
        for (uint8_t i = 0; i < this->sGroup.size(); ++i) {
            Addr2Str(sGroup[i], addrStr);
            Ptr<QuicMyClient> myclient = Create<QuicMyClient>();
            myclient->SetNode(node);
            myclient->SetRemote(Ipv4Address::ConvertFrom(sGroup[i]), m_peerPort+1);
            myclient->Bind(m_peerPort);
            socketPool[addrStr] = myclient;
            std::cout<<"client ptr:"<<myclient<<std::endl;
            NS_LOG_DEBUG(this << " CreateSocketPool: "<<addrStr<<" -myclient, port: " << m_peerPort);
        }

        // Setup itself as servers, if it has children
        for (uint8_t i = 0; i < this->cGroup.size(); ++i) {
            Addr2Str(cGroup[i], addrStr);
            QuicServerHelper myserverhelper(m_peerPort+1);
            myserverhelper.Install(node); 
            Ptr<QuicMyServer> myserver = myserverhelper.GetServer();
            // Zhuoxu: Here, we should defind another set function that pass the address of the IterationChunk to all the servers binded in this node.
            myserver->SetNode(node);
            myserver->SetcGroupSize(cGroup.size());
            // Todo: initialize the chunkMap here
            // Todo: myserver->SetIterationChunk(&chunkMap[i]);
            myserver->Bind(m_peerPort+1);
            // Zhuoxu: socket[] is owned by each node, therefore there won't be any conflict on the same port.
            socketPool[addrStr] = myserver;
            
            std::cout<<"myserver ptr:"<<myserver<<std::endl;
            NS_LOG_DEBUG(this << " CreateSocketPool: "<<addrStr<<" -myserver, port: " << m_peerPort+1);
        }

        NS_LOG_DEBUG("Connection Setting Finishing in: "<< Simulator::Now().GetMilliSeconds() << "ms");
    }

    void 
    InnetworkAggregationInterface::ReceiveDataFromAll () {
        NS_LOG_DEBUG (this);
        Ptr<QuicMyServer> server;
        uint16_t iterationNum;
        // Zhuoxu: get received server socket;
        for (auto item : socketPool) {
            // std::cout<<"ReceiveDataFromAll () item.first:"<<item.first<<std::endl;
            if ( socketPool[item.first]->GetObject<QuicMyServer>() )
            {
                server = socketPool[item.first]->GetObject<QuicMyServer>();
                iterationNum = server->GetCompIterNum();
                if (iterationNum != UINT16_MAX) {
                    NS_LOG_DEBUG(this << " sucess server ptr: "<<socketPool[item.first]->GetObject<QuicMyServer>() << " socketPool[" << item.first << "]:");
                    // Zhuoxu: must break here, otherwise the iterationNum will be overwrited by the last server.
                    break;
                }
                // Zhuoxu: bugs to be fixed, iteration received has problem ... check.
            }
        }

        bool hasData = true;
        ReceivedChunk result(0);

        while(iterationNum != UINT16_MAX){
            // Zhuoxu: get the result from the server
            result = server->GetResult(iterationNum);
            // Zhuoxu: release the map
            server->ReleaseMap(iterationNum);

            // Zhuoxu: add the iteration number to the success set
            successIter.insert(iterationNum);

            Time currentTime = Simulator::Now();
            NS_LOG_INFO("IterationNum-"<<iterationNum-uint16_t(0)<< " Innetwork aggregation completed in: " << currentTime.GetMilliSeconds() << "ms");

            // Send packet to the parent
            SendResponseVToP (result.vec , iterationNum);
            
            //check if all the iteration has been collected
            if (successIter.size() == maxIteration) {
                if (this->sGroup.size() <= 0) {
                    NS_LOG_INFO("All iteration-"<<maxIteration<< " completed in: " << currentTime.GetMilliSeconds() - 2000<< "ms");
                    Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 1));
                }
                return;
            }

            // Zhuoxu: Check if the next iteration is ready
            iterationNum = server->GetCompIterNum();
        }
        
        ns3::Simulator::Schedule(ns3::MilliSeconds(3), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
        return;
        // The minimum time interval is 1ms in the test.
        // ns3::Simulator::Schedule(ns3::MilliSeconds(4), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
    }


    // serialize and slice vec into chunks of k elements, that is k*8 uint8_t/chunk.
    void 
    InnetworkAggregationInterface::SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec, uint16_t iterationNum) {
        NS_LOG_FUNCTION (this);
        //uint16_t iterationNum = 0;
        //Ptr<QuicMyServer> server = socketPool[toStr]->GetObject<QuicMyServer>();
        uint32_t pos = 0;
        uint32_t end = vsize*sizeof(uint64_t); //transalte from uint64_t to uint8_t
        uint8_t type = 1;
        //uint8_t *serializeVec = new uint8_t[end]();
        std::vector<uint8_t> serializeVec = std::vector<uint8_t>(end,0);
        //uint8_t *chunkBuffer = new uint8_t[pktlen]();note that: uint16_t pktlen = BASESIZE + 10; So the first 10 position are for chunkNum and type.
        SerializeVector(vec,serializeVec.data());
        SendPacket(toStr, iterationNum, serializeVec);
    }

    void
    InnetworkAggregationInterface::SendPacket (std::string toStr, uint16_t iterationNum, std::vector<uint8_t> &serializeVec){
        
        //NS_LOG_INFO("iteration-"<<iterationNum-uint16_t(0));

        // Zhuoxu: change the producer to client here
        Ptr<QuicMyClient> client = socketPool[toStr]->GetObject<QuicMyClient>();
        NS_LOG_DEBUG( this << " Sends data to socketPool["<<toStr<<"] ");
        std::vector<uint8_t> chunkBuffer = std::vector<uint8_t>(pktlen,0); //Zhuoxu: 
        NS_LOG_DEBUG( this << " pktlen " << pktlen);
        uint8_t* buffer = serializeVec.data(); 
        // mark 33... as the type of the send data packet
        for (uint16_t i = 0; i < 8; ++i) {
            chunkBuffer[i] = 3;
        }

        chunkBuffer[8] = static_cast<uint8_t>((iterationNum >> 8) & 0xFF); // High byte
        chunkBuffer[9] = static_cast<uint8_t>(iterationNum & 0xFF);

        //iterationNum++;

        // Debug output to verify sizes
        // std::cout << "BASESIZE: " << BASESIZE << std::endl;
        // std::cout << "pktlen: " << pktlen << std::endl;
        // std::cout << "buffer size: " << serializeVec.size() << std::endl;
        // std::cout << "chunkBuffer size: " << chunkBuffer.size() << std::endl;

        
        std::copy(buffer, buffer + BASESIZE, chunkBuffer.data() + 10);
            
        // record the time RTT
        // 在这里获取是否
        int sentSize = -1;
        while(sentSize < 0 ){
            sentSize = client->Send(chunkBuffer.data(),pktlen);
            // determine if the sent operation success
            if(sentSize > 0)
                NS_LOG_DEBUG(this<<" client->Send()--sentSize: " << sentSize <<" at iteration "<<iterationNum-uint16_t(0));
            else
                NS_LOG_DEBUG(this<<" client->Send() failed at iteration "<<iterationNum-uint16_t(0));
        }

        

        // Zhuoxu: Todo: print the send content of aggregator
        if(cGroup.size()>0 && sGroup.size()>0){
            NS_LOG_DEBUG("Agg send to consumer, the content is as follows: ");
            // std::cout << "------------------------------------" << std::endl;
            // for(int i = 0; i < 24; i++){
            //     std::cout << static_cast<int>(chunkBuffer[i]) << " ";
            //     if((i+1) % 8 == 0){
            //         std::cout << " the " << (i+1) / 8 << "th byte" << std::endl;
            //     }
            // }
        }
        else if(cGroup.size()==0){
            NS_LOG_DEBUG("producer send to aggregator, the content is as follows: ");
            // std::cout << "------------------------------------" << std::endl;
            // for(int i = 0; i < 24; i++){
            //     std::cout << static_cast<int>(chunkBuffer[i]) << " ";
            //     if((i+1) % 8 == 0){
            //         std::cout << " the " << (i+1) / 8 << "th byte" << std::endl;
            //     }
            // }
        }

        // if (cGroup.size()<=0 && iterationNum < maxIteration - 1){
        //     ns3::Simulator::Schedule(ns3::MilliSeconds(8), &InnetworkAggregationInterface::SendPacket, this, toStr, iterationNum + 1, serializeVec);
        // }

        
        // // for loop implementation will incur frame error
        // if (iterationNum == 0 && cGroup.size() <= 0){
        //     for(uint16_t i = 1; i < maxIteration; ++i){
        //         SendPacket(toStr, i, serializeVec);
        //     }

        // }
        
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

    void 
    InnetworkAggregationInterface::ProduceVToP (){
        for (uint8_t i = 0; i < this->sGroup.size (); ++i) {
            std::string toStr;
            Addr2Str (this->sGroup[i], toStr);
            for (uint16_t j = 0; j < this->maxIteration; ++j) {
                std::vector<uint64_t> initData(chunkSize, 88 + 11*static_cast<int>(j));
                SendResponseVTo (toStr, initData, j);
                std::cout<<"initData info:"<<std::endl;
                for(int k=0;k<5;k++){
                    std::cout<<initData[k]<<" ";
                }
                std::cout<<"iterationNum:"<<static_cast<int>(j)<<std::endl;
            }
        }
    }

    bool 
    InnetworkAggregationInterface::GetisEnd () {
        //NS_LOG_FUNCTION (this);
        return this->isEnd;
    }

    void 
    InnetworkAggregationInterface::SetisEnd (bool v) {
        //NS_LOG_FUNCTION (this);
        this->isEnd = v;
    }

    void
    InnetworkAggregationInterface::ClearChunkMap () {
        //NS_LOG_FUNCTION (this);
        
        // Zhuoxu: No need this currently.

        /*
        for (auto &tmp : this->chunkMap) {
            tmp. second. childCount = 0;
            std::vector<uint64_t> tmp1 (chunkSize, 0);
            tmp. second. data = tmp1;
        }
        */
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


}; /*namespace ns3*/
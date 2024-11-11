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

        this->iterationNumber = 0;
        DeepVectorCopy (this->sGroup, sGroup);
        DeepVectorCopy (this->cGroup, cGroup);
    }

    void
    InnetworkAggregationInterface::SetVSize (uint16_t size) {
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


        // Zhuoxu: Twist the server and client here.
        // Setup itself as clients, if it has parents
        for (uint8_t i = 0; i < this->sGroup.size(); ++i) {
            Addr2Str(sGroup[i], addrStr);
            Ptr<QuicMyClient> myclient = Create<QuicMyClient>();
            myclient->SetNode(node);
            myclient->SetRemote(Ipv4Address::ConvertFrom(sGroup[i]), m_peerPort);
            myclient->Bind(m_peerPort + i + 1);
            socketPool[addrStr] = myclient;
        }

        // Setup itself as servers, if it has children
        for (uint8_t i = 0; i < this->cGroup.size(); ++i) {
            Addr2Str(cGroup[i], addrStr);
            QuicServerHelper myserverhelper(m_peerPort);
            myserverhelper.Install(node);
            Ptr<QuicMyServer> myserver = myserverhelper.GetServer();
            myserver->SetNode(node);
            myserver->Bind(m_peerPort);
            // Zhuoxu: socket[] is owned by each node, therefore there won't be any conflict on the same port.
            socketPool[addrStr] = myserver;
        }

        NS_LOG_DEBUG("Connection Setting Finishing in: "<< Simulator::Now().GetMilliSeconds() << "ms");
    }


    //read socket buffer
    void 
    InnetworkAggregationInterface::ReceiveDataFrom (std::string fromStr) {
        NS_ASSERT ( socketPool. find (fromStr) != socketPool. end () );
        Ptr<CircularBuffer> ccircularBuffer;
        // if (this->sGroup. size () > 0 && this-> cGroup.size() > 0)
        //     NS_LOG_DEBUG(this<<" In aggregator... ReceiveDataFrom,"<<"fromStr:"<<fromStr);
        if( socketPool[fromStr] != nullptr ){
            //NS_LOG_DEBUG(this<<" socketPool[fromStr] is not nullptr---");
            
            if ( socketPool[fromStr]->GetObject<QuicMyServer>() ){
                Ptr<QuicMyServer> server = socketPool[fromStr]->GetObject<QuicMyServer>();
                // Zhuoxu: the write operation on the ccircularBuffer is integrated in the QuicMyServer::HandleRead function.
                ccircularBuffer = server->GetBuffer();
                //ccircularBuffer->print();
                // Zhuoxu: only print if it is an aggregator
                //if (this->sGroup. size () > 0 && this-> cGroup.size() > 0)
                //    NS_LOG_DEBUG(this<<" In aggregator... QuickMyServer,"<<"ccircularBufferSize:"<<ccircularBuffer->getSize());
            }
            else if( socketPool[fromStr]->GetObject<QuicMyClient>() ){
                Ptr<QuicMyClient> client = socketPool[fromStr]->GetObject<QuicMyClient>();
                ccircularBuffer = client->GetBuffer();
                // Zhuoxu: only print if it is an aggregator
                //if (this->sGroup. size () > 0 && this-> cGroup.size() > 0)
                //    NS_LOG_DEBUG(this<<" In aggregator... QuickMyClient,"<<"ccircularBufferSize:"<<ccircularBuffer->getSize());
            }
            else{
                NS_FATAL_ERROR("Application is not a QuicMyServer or QuicMyClient");
            }
        
            // process packet
            uint8_t* quicpacket = new uint8_t[pktlen];
            // If there are no data be receive then, all these process will be just triggered by the ns3::scheduler function.
            while ( ccircularBuffer->getSize() >= pktlen )
            {           
                // if ( ccircularBuffer->getNextToRead() == uint8_t(0) ){
                //     NS_LOG_DEBUG("strange request---------");
                //     //ccircularBuffer->print();
                //     //GoToReadPos(ccircularBuffer);
                //     ccircularBuffer->read(quicpacket,pktlen);
                //     // HandleRequestV(fromStr);
                // }
                // else 
                if ( ccircularBuffer->getNextToRead() == uint8_t(1) ){
                    NS_LOG_DEBUG("ReceiveDataFrom----ccircularBuffer->getNextToRead() == uint8_t(1)---------");
                    ccircularBuffer->print();
                    ccircularBuffer->read(quicpacket,pktlen);
                    //ccircularBuffer->print();
                    HandleResponseV(fromStr,quicpacket+1);
                } 
                else{
                    std::cout<< "Delete retrans content---------" <<std::endl;
                    GoToReadPos(ccircularBuffer);
                }
                    
            }

            delete [] quicpacket;
        }
        else{
            NS_LOG_DEBUG("socketPool[fromStr] is nullptr---" << socketPool[fromStr]);
        }
    }


    void 
    InnetworkAggregationInterface:: GoToReadPos( Ptr<CircularBuffer> ccircularBuffer ) {

        bool fdReadPos = false;

        while( ccircularBuffer->getSize() >= 3 && fdReadPos == false ){

            uint8_t *tmpBuf = ccircularBuffer->getNextToReadBuffer();
            uint32_t pt = 0;

            while( !(tmpBuf[pt] == uint8_t(1) ) ) {
                pt++;
            }

            uint8_t *tmpBufToDump = new uint8_t[pt];
            ccircularBuffer->read( tmpBufToDump, pt );

            if( ccircularBuffer->getSize() >= 3 && ccircularBuffer->getNextToReadBuffer()[0] == uint8_t(1) ) {
                
                    if( ccircularBuffer->getNextToReadBuffer()[0] == uint8_t(1) && ccircularBuffer->getNextToReadBuffer()[1] == ccircularBuffer->getNextToReadBuffer()[2] )
                    {
                        
                        if( ( ( ccircularBuffer->getNextToReadBuffer()[1] - uint8_t(0) >= 0 ) && ( ccircularBuffer->getNextToReadBuffer()[1]-uint8_t(0) <= 9 ) ) )
                         {
                            fdReadPos = true;
                            std::cout<< "found the chunknum to read from-----" << ccircularBuffer->getNextToReadBuffer()[0]-uint8_t(0) << "|" << ccircularBuffer->getNextToReadBuffer()[1]-uint8_t(0) << "|" << ccircularBuffer->getNextToReadBuffer()[2]-uint8_t(0) <<std::endl;
                            ccircularBuffer->print();
                            break;
                        }
                    }
                    else{
                        uint8_t*tmp1 = new uint8_t[1];
                        ccircularBuffer->read(tmp1,1);
                        delete []tmp1;
                    }
                
            }
        }
    }

    void 
    InnetworkAggregationInterface::ReceiveDataFromAll () {
        for (auto item : socketPool) {
            ReceiveDataFrom (item.first);
        }

        // The minimum time interval is 1ms in the test.
        ns3::Simulator::Schedule(ns3::MilliSeconds(4), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
    }


    // collect data from buffer for aggregation
    void 
    InnetworkAggregationInterface::HandleResponseV (std::string fromStr, uint8_t *payloadData) {
        NS_LOG_FUNCTION (this << fromStr << "---on time---");
        
        //std::cout<<"InnetworkAggregationInterface::HandleResponseV (std::string "<<fromStr<<"---on time---"<<Simulator::Now()<<std::endl;

        // 这里需要改动，chunkMap不再需要了
        // chunkNumber
        uint8_t iterationNumber = payloadData[0];

        // create new chunk for new iterationNumber
        // 确保要把所有的iterator的数据都存放在iterationMap中
        // 大小要为[iteration]
        // 还要确保收集完所有子节点的数据
        // Zhuoxu: iteration number = payloaddata[0]; 把iteration的数值抽出来，放入chunkIteration数组里面。这里直接创建数组即可，因为已经知道了maximum iteration的值。
        
        if (chunkMap.find(iterationNumber) == chunkMap.end()) {
            ReceivedChunk chunk;
            chunkMap[iterationNumber] = chunk;
        }

        //uint16_t chunkSize = static_cast<uint16_t>(BASESIZE / 8);
        std::vector<uint64_t> desPacketContent(chunkSize, 0); //the num of elements in one chunk
        
        DeserializeVector(desPacketContent, payloadData + 9, BASESIZE);

        // perform sumVector on 64bit interger
        SumVector(chunkMap[iterationNumber].data, chunkMap[iterationNumber].data, desPacketContent);
        chunkMap[iterationNumber].count++;

        // if one iterationNumber collects enough data
        if (bool(chunkMap[iterationNumber].count == cGroup.size())) {
            //std::cout<<"3333"<<chunkMap[iterationNumber].count-uint8_t(0)<<"cGroup:"<<cGroup.size()<<bool(chunkMap[iterationNumber].count == cGroup.size())<<std::endl;
            AvgVector(chunkMap[iterationNumber].data, chunkMap[iterationNumber].count); //correct only for evenly distributed networks, simply SumVector for other scenarios 
            //std::cout<<"chunkMap[iterationNumber].count == cGroup.size()  ----iterationNumber-- "<<iterationNumber-uint8_t(0)<<"---count-- "<<chunkMap[iterationNumber].count-uint8_t(0)<<std::endl;
            this->isEnd = true;
            this->currentIteration++;
            if (this->sGroup. size () > 0 && this-> cGroup.size() > 0)
                NS_LOG_DEBUG(this << "In iteration: " << iterationNumber - uint8_t(0) <<" aggregator collect all "<< cGroup.size() <<" data from the child");
            else if (this->sGroup. size () == 0)
                NS_LOG_DEBUG(this << "In iteration: " << iterationNumber - uint8_t(0) <<" consumer collect all "<< cGroup.size() <<" data from the child");
            AVG (iterationNumber);       
        }
    }

    // serialize and slice vec into chunks of k elements, that is k*8 uint8_t/chunk.
    void 
    InnetworkAggregationInterface::SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec, uint8_t iterationNumber) {
        NS_LOG_FUNCTION (this);
        //uint8_t iterationNumber = 0;
        //Ptr<QuicMyServer> server = socketPool[toStr]->GetObject<QuicMyServer>();
        uint16_t pos = 0;
        uint16_t end = vsize*sizeof(uint64_t); //transalte from uint64_t to uint8_t
        uint8_t type = 1;
        //uint8_t *serializeVec = new uint8_t[end]();
        std::vector<uint8_t> serializeVec = std::vector<uint8_t>(end,0);
        //uint8_t *chunkBuffer = new uint8_t[pktlen]();note that: uint16_t pktlen = BASESIZE + 10; So the first 10 position are for chunkNum and type.
        SerializeVector(vec,serializeVec.data());
        //if (cGroup.size()<=0){
            // while(iterationNumber < 100){
            //     SendPacketFromPro(toStr, iterationNumber, serializeVec);
                
            //     iterationNumber++;

            //     Time 
            // }
            // the push operation is done within this command
            //ns3::Simulator::Schedule(ns3::MilliSeconds(20),&InnetworkAggregationInterface::SendPacketFromPro, this, toStr, iterationNumber++, serializeVec);
        //}
        //else{
        SendPacketFromPro(toStr, iterationNumber, serializeVec);
        //}
        // 在这里schedual
        // Zhuoxu: 这段的chunkNum不用改，作用完全只是在header里面添加9个bits的头部
        //while (pos < end) {
            //uint8_t chunkNum = pos/BASESIZE;//put chunkNum ahead of the data, BASESIZE=800
            
 
            //pos += BASESIZE;
        //}
        //delete[] serializeVec;
        //delete [] chunkBuffer;
    }

    void
    InnetworkAggregationInterface::SendPacketFromPro (std::string toStr, uint8_t iterationNumber, std::vector<uint8_t> &serializeVec){
        
        NS_LOG_INFO("iteration-"<<iterationNumber-uint8_t(0));

        // Zhuoxu: change the producer to client here
        Ptr<QuicMyClient> client = socketPool[toStr]->GetObject<QuicMyClient>();
        std::vector<uint8_t> chunkBuffer = std::vector<uint8_t>(pktlen,0); //Zhuoxu: 
        uint8_t* buffer = serializeVec.data(); 
        chunkBuffer[0] = 1;
        chunkBuffer[1] = iterationNumber;
        //iterationNumber++;

        std::copy(buffer, buffer + BASESIZE, chunkBuffer.data() + 10);
            
        // record the time RTT
        // 在这里获取是否
        int sentSize = -1;
        while(sentSize < 0 ){
         sentSize = client->Send(chunkBuffer.data(),pktlen);
            // determine if the sent operation success
            if(sentSize > 0)
                NS_LOG_DEBUG(this<<"client->Send() success at iteration "<<iterationNumber-uint8_t(0));
            else
                NS_LOG_DEBUG(this<<"client->Send() failed at iteration "<<iterationNumber-uint8_t(0));
        }
        // Continue if have not yet reached the maximum iteration
        if (cGroup.size()<=0 && iterationNumber <= maxIteration){
            // Schedule the next call
            // NS_LOG_DEBUG("IterationNum-" << static_cast<int>(iterationNumber) << " Startfrom: " << Simulator::Now().GetMilliSeconds() << "ms");
            // When the schedule frequency is below than 5ms, error will happen.
            ns3::Simulator::Schedule(ns3::MilliSeconds(10), &InnetworkAggregationInterface::SendPacketFromPro, this, toStr, iterationNumber+1, serializeVec);
        }
    }


    void 
    InnetworkAggregationInterface::DelayedSend(Ptr<QuicMyServer> responseptr, uint8_t* chunkBuffer, uint32_t bufferSize)
    {
        responseptr->Send(chunkBuffer,bufferSize);
    }

    void 
    InnetworkAggregationInterface::SendResponseVToP (std::vector<uint64_t> &avg , uint8_t iterationNumber) {

        for (uint8_t i = 0; i < this->sGroup.size (); ++i) {
            std::string toStr;
            Addr2Str (this->sGroup[i], toStr);
            SendResponseVTo (toStr, avg, iterationNumber);
        }
    }

    void 
    InnetworkAggregationInterface::AVG (uint8_t iterationNumber) {
        // NS_LOG_FUNCTION (this<<this->node->GetObject<Ipv4>()<<iterationNumber);

        if (this->currentIteration == 0) {
            NS_LOG_DEBUG("[InnetworkAggregationInterface:AVG] send a new data at time "<< Simulator::Now().GetSeconds () << 
                            " currentIteration = " << this->currentIteration << " maxIteration = " << this->maxIteration);
        }
        NS_LOG_DEBUG(this<<" sGroup.size(): "<<sGroup.size());
        // std::vector<uint64_t> avg = std::vector<uint64_t>(vsize, 0);
        // WriteChunk2Vec (avg, vsize);//vsize //Zhuoxu: problematic code

        if (this->sGroup. size () <= 0) {
            std::cout<<"chunkMap.size()"<<chunkMap.size()<<std::endl;
            Time currentTime = Simulator::Now();
            // Time duration = currentTime - this->startTime;
            // std::cout<<  "Innetwork aggregation completed in : " << duration.GetMilliSeconds()  << "ms" <<std::endl;
            NS_LOG_DEBUG("IterationNum-"<<iterationNumber-uint8_t(0)<< " Innetwork aggregation completed in: " << currentTime.GetMilliSeconds() << "ms");
            //NS_LOG_INFO ("Innetwork aggregation completed in : " << duration.GetMilliSeconds() << "ms");

            // simulator end early
            // Zhuoxu: This will go wrong will iteration disorder.
            if (this->currentIteration >= this->maxIteration) {
                Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 1));
            }
            
        }
        //reply result to parent
        // NS_LOG_INFO( "AVG: " << iterationNumber-uint8_t(0));
        SendResponseVToP (chunkMap[iterationNumber].data, iterationNumber);
        // SaveResult (avg);Zhuoxu: No need this 
        //ClearChunkMap ();
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
        for (auto &tmp : this->chunkMap) {
            tmp. second. count = 0;
            std::vector<uint64_t> tmp1 (chunkSize, 0);
            tmp. second. data = tmp1;
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


}; /*namespace ns3*/
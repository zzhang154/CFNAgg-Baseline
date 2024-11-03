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
        //NS_LOG_FUNCTION (this);
        std::string addrStr;

        // setup servers
        for (uint8_t i = 0; i < this->sGroup. size (); ++i) {
            Addr2Str (sGroup[i], addrStr);
            QuicServerHelper myserverhelper(m_peerPort);
            myserverhelper.Install(node);
            Ptr<QuicMyServer> myserver = myserverhelper.GetServer();
            myserver->SetNode(node);
            myserver->Bind(m_peerPort);
            // Zhuoxu: socket[] is owned by each node, therefore there won't be any conflict on the same port.
            socketPool[addrStr] = myserver;
        }

        //setup clients
        for (uint8_t i = 0; i < this->cGroup. size (); ++i) {
            Addr2Str (cGroup[i], addrStr);
            Ptr<QuicMyClient> myclient= Create<QuicMyClient>();
            myclient->SetNode(node);
            myclient->SetRemote(Ipv4Address::ConvertFrom (cGroup[i]), m_peerPort);
            myclient->Bind(m_peerPort + i + 1);
            socketPool[addrStr] = myclient;
        }
    }


    //read socket buffer
    void 
    InnetworkAggregationInterface::ReceiveDataFrom (std::string fromStr) {
        NS_ASSERT ( socketPool. find (fromStr) != socketPool. end () );

        Ptr<CircularBuffer> ccircularBuffer;
        if( socketPool[fromStr] != nullptr ){
            
            if ( socketPool[fromStr]->GetObject<QuicMyServer>() ){
                Ptr<QuicMyServer> server = socketPool[fromStr]->GetObject<QuicMyServer>();
                // Zhuoxu: the write operation on the ccircularBuffer is integrated in the QuicMyServer::HandleRead function.
                ccircularBuffer = server->GetBuffer();
            }
            else if( socketPool[fromStr]->GetObject<QuicMyClient>() ){
                Ptr<QuicMyClient> client = socketPool[fromStr]->GetObject<QuicMyClient>();
                ccircularBuffer = client->GetBuffer();
            }
            else{
                NS_FATAL_ERROR("Application is not a QuicMyServer or QuicMyClient");
            }
        
            // process packet
            uint8_t* quicpacket = new uint8_t[pktlen];
            while ( ccircularBuffer->getSize() >= pktlen )
            {
                                   
                if ( ccircularBuffer->getNextToRead() == uint8_t(0) ){
                    ccircularBuffer->read(quicpacket,pktlen);
                    HandleRequestV(fromStr);
                }
                else if ( ccircularBuffer->getNextToRead() == uint8_t(1) ){
                    //std::cout<<"ReceiveDataFrom    ccircularBuffer->getNextToRead() == uint8_t(1)---------"<<std::endl;
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
            std::cout<< "socketPool[fromStr] is nullptr---" << socketPool[fromStr] <<std::endl;
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

        if (!isEnd || cGroup. size () == 0){
            ns3::Simulator::Schedule(ns3::MilliSeconds(50), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
        }

    }


    // collect data from buffer for aggregation
    void 
    InnetworkAggregationInterface::HandleResponseV (std::string fromStr, uint8_t *payloadData) {
        //NS_LOG_FUNCTION (this);
        
        //std::cout<<"InnetworkAggregationInterface::HandleResponseV (std::string "<<fromStr<<"---on time---"<<Simulator::Now()<<std::endl;

        // 这里需要改动，chunkMap不再需要了
        // chunkNumber
        uint8_t chunkNumber = payloadData[0];

        // create new chunk for new chunkNumber
        // 确保要把所有的iterator的数据都存放在iterationMap中
        // 大小要为[iteration]
        // 还要确保收集完所有子节点的数据
        // Zhuoxu: iteration number = payloaddata[0]; 把iteration的数值抽出来，放入chunkIteration数组里面。这里直接创建数组即可，因为已经知道了maximum iteration的值。
        
        if (chunkMap. find (chunkNumber) == chunkMap. end ()) {
            ReceivedChunk chunk;
            chunkMap[chunkNumber] = chunk;
        }

        //uint16_t chunkSize = static_cast<uint16_t>(BASESIZE / 8);
        std::vector<uint64_t> desPacketContent(chunkSize, 0); //the num of elements in one chunk
        
        DeserializeVector(desPacketContent, payloadData + 9, BASESIZE);

        // perform sumVector on 64bit interger
        SumVector (chunkMap[chunkNumber].data, chunkMap[chunkNumber].data, desPacketContent);
        chunkMap[chunkNumber]. count++;

        // if one chunkNumber collects enough data
        if ( chunkMap[chunkNumber]. count == cGroup. size () ) {
            AvgVector (chunkMap[chunkNumber].data, chunkMap[chunkNumber]. count); //correct only for evenly distributed networks, simply SumVector for other scenarios 
            //std::cout<<"chunkMap[chunkNumber]. count == cGroup. size ()  ----chunkNumber-- "<<chunkNumber-uint8_t(0)<<"---count-- "<<chunkMap[chunkNumber]. count-uint8_t(0)<<std::endl;
            AvgEnd(vsize);         
        }

    }

    void 
    InnetworkAggregationInterface::HandleRequestV (std::string fromStr) {
        //NS_LOG_FUNCTION (this);

        //std::cout<<"HandleRequestV from----"<<fromStr<<std::endl;
        // producers response
        if (cGroup. size () <= 0) {
            // add a loop here
            
            std::vector<uint64_t> randVec (vsize);
            GenetrateRandomVector (randVec, this->aggTreeLevel);
            SendResponseVTo (fromStr, randVec);
        }
        
        // aggregators deliver request
        SendRequestVToAll ();        
    }



    void 
    InnetworkAggregationInterface::SendRequestVTo (std::string toStr) {
        //NS_LOG_FUNCTION (this);        
        NS_ASSERT (socketPool. find (toStr) != socketPool. end ());
        Ptr<QuicMyClient> requestptr = socketPool[toStr]->GetObject<QuicMyClient>();
        if (!requestptr){std::cout<<"no such client to sent request!"<<std::endl;}

        // create request packet
        uint8_t* buffer = new uint8_t[pktlen];
        // not useful representations
        uint16_t request_port = requestptr->GetBindPort();
        buffer[0] = 0;
        buffer[1] = (request_port >> 8);
        buffer[2] = request_port & 0xFF;
        for (int i = 3; i < pktlen; i++){
            buffer[i]=0;
        }
        
        requestptr->Send(buffer, pktlen);
        
        delete [] buffer;
    }

    void
    InnetworkAggregationInterface::SendRequestVToAll () {
        //NS_LOG_FUNCTION (this);
        for (uint8_t i = 0; i < this->cGroup. size (); ++i) {
            // send request
            std::string toStr;
            Addr2Str (this->cGroup[i], toStr);
            //std::cout<<"SendRequestVToAllsending request to :--------"<<toStr<<std::endl;
            SendRequestVTo (toStr);
        }
        
        
        /*currentIndex = 0;   
        if (currentIndex < cGroup.size()) {
            ScheduleAndSend();
        }*/
    }

    void 
    InnetworkAggregationInterface::ScheduleAndSend() {
        //NS_LOG_FUNCTION(this);

        // 检查索引是否在有效范围内
        if (currentIndex >= cGroup.size()) {
            // 索引超出范围，停止发送和调度
            return;
        }

        // 发送请求
        std::string toStr;
        Addr2Str(cGroup[currentIndex], toStr);
        SendRequestVTo(toStr);

        // 更新索引，准备下一次发送
        currentIndex++;

        // 安排在20ms后发送下一条请求
        ns3::Simulator::Schedule(ns3::MilliSeconds(20), &InnetworkAggregationInterface::ScheduleAndSend, this);
}


    // serialize and slice vec into chunks of k elements, that is k*8 uint8_t/chunk.
    void 
    InnetworkAggregationInterface::SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec) {
        //NS_LOG_FUNCTION (this);

        Ptr<QuicMyServer> server = socketPool[toStr]->GetObject<QuicMyServer>();
        uint16_t pos = 0;
        uint16_t end = vsize*sizeof(uint64_t); //transalte from uint64_t to uint8_t
        uint8_t type = 1;
        //uint8_t *serializeVec = new uint8_t[end]();
        std::vector<uint8_t> serializeVec = std::vector<uint8_t>(end,0);
        //uint8_t *chunkBuffer = new uint8_t[pktlen]();
        std::vector<uint8_t> chunkBuffer = std::vector<uint8_t>(pktlen,0); //Zhuoxu: note that: uint16_t pktlen = BASESIZE + 10; So the first 10 position are for chunkNum and type.
        SerializeVector(vec,serializeVec.data());

        // 在这里schedual
        // Zhuoxu: 这段的chunkNum不用改，作用完全只是在header里面添加9个bits的头部
        while (pos < end) {
            uint8_t chunkNum = pos/BASESIZE;//put chunkNum ahead of the data, BASESIZE=800
            uint8_t* buffer = serializeVec.data()+pos; 
            chunkBuffer[0] = type;
            for (int i = 0;i<9;i++)
            {
                // iteration number;
                chunkBuffer[i+1] = chunkNum;
            }
            
            std::copy(buffer, buffer + BASESIZE, chunkBuffer.data() + 10);
            
            // record the time RTT
            server->Send(chunkBuffer.data(),pktlen);
 
            pos += BASESIZE;
        }
        //delete[] serializeVec;
        //delete [] chunkBuffer;
    }

    void 
    InnetworkAggregationInterface::DelayedSend(Ptr<QuicMyServer> responseptr, uint8_t* chunkBuffer, uint32_t bufferSize)
    {
        responseptr->Send(chunkBuffer,bufferSize);
    }

    void 
    InnetworkAggregationInterface::SendResponseVToP (std::vector<uint64_t> &avg ) {

        for (uint8_t i = 0; i < this->sGroup. size (); ++i) {
            std::string toStr;
            Addr2Str (this->sGroup[i], toStr);
            SendResponseVTo (toStr, avg);
        }
    }

    void 
    InnetworkAggregationInterface::AVG () {
        //NS_LOG_FUNCTION (this);

        if (this->isEnd) {
            this->isEnd = false;
            if (this->currentIteration > 0) {

                std::vector<uint64_t> avg = std::vector<uint64_t>(vsize, 0);
                WriteChunk2Vec (avg, vsize);//vsize

                if (this->sGroup. size () <= 0) {
                    Time currentTime = Simulator::Now();
                    Time duration = currentTime - this->startTime;
                    std::cout<<  "Innetwork aggregation completed in : " << duration.GetMilliSeconds()  << "ms" <<std::endl;
                    NS_LOG_INFO ("Innetwork aggregation completed in : " << duration.GetMilliSeconds() << "ms");
                }
                

                //reply result to parent
                SendResponseVToP (avg);
                SaveResult (avg);
                ClearChunkMap ();

            }

            // simulator end early
            if (this->currentIteration >= this->maxIteration) {
                Simulator::Stop (Seconds(Simulator::Now().GetSeconds() + 1));
            }

            // zhuoxu: this can only be triggered once.
            if (this->currentIteration < this->maxIteration) {
                std::cout<<"[InnetworkAggregationInterface:AVG] start a new request at time "<< Simulator::Now().GetSeconds () << 
                                " count = " << this->currentIteration << " maxIteration = " << this->maxIteration<<std::endl;
                NS_LOG_INFO ("[InnetworkAggregationInterface:AVG] start a new request at time " << Simulator::Now().GetSeconds () << 
                                " count = " << this->currentIteration << " maxIteration = " << this->maxIteration);
                this->startTime = Simulator::Now();
                SendRequestVToAll ();
            }
        }
    }

    // Zhuoxu: Need to change this, need to collect all iteration before end. i.e., collect all 100 iteration 
    void
    InnetworkAggregationInterface::AvgEnd (uint16_t size) {
        //NS_LOG_FUNCTION (this);
        //std::cout<<"AvgEnd------------"<<std::endl;
        bool res = true;
        uint8_t aggSize = cGroup. size ();
        //uint16_t chunkSize = static_cast<uint16_t>(BASESIZE / 8);
        uint8_t range = static_cast<uint8_t>(size / chunkSize + (size % chunkSize != 0));
        for (uint8_t i = 0; i < range; ++i) {
            if (chunkMap. find (i ) == chunkMap. end ()) {
                // Zhuoxu: what hanppens if res is set to false, which leads to AVG not run?
                res = false;
                break;
            }
            res &= (chunkMap[i ]. count >= aggSize);
        }
        this->isEnd = res;
        if (res)
            this->currentIteration++;
        // zhuoxu: the logic here is to finish iteration one by one. But not simultaneously.
        AVG ();
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
    InnetworkAggregationInterface::WriteChunk2Vec (std::vector<uint64_t> &vec, uint16_t size) {
        //NS_LOG_FUNCTION (this);
        //uint16_t chunkSize = static_cast<uint16_t>(BASESIZE / 8);
        uint8_t range = static_cast<uint8_t>(size / chunkSize + (size % chunkSize != 0));
        
        for (uint8_t i = 0; i < range; ++i) {
            for (uint8_t j = 0; j < chunkSize ;++j) {
                vec[i * chunkSize + j] = chunkMap[i]. data[j];
            }
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
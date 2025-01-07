#ifndef INNETWORK_AGG_INTERFACE_H
#define INNETWORK_AGG_INTERFACE_H

#include <cstdint>
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <iostream>
#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
//#include "datainnetwork.h"
//#include "consumer.h"
//#include "producer.h"
//#include "aggregator.h"


// #include "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/innetwork-task/helper/parameter.h"
// #include "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/src/innetwork-task/helper/vectorop.h"

#include "ns3/parameter.h"
#include "ns3/vectorop.h"
#include <set>
#include "ns3/TCPclient.h"
#include "ns3/TCPserver.h"


// used for datatransfer by nodes
namespace ns3 {
    typedef std::map<std::string, std::vector<uint64_t>> ChildChunkMap; // Zhuoxu: socket name -- chunk map

    class InnetworkAggregationInterface : public Object {
            uint8_t aggTreeLevel; // level of nodes in the aggregation tree or topology using the consumer as the root
            uint16_t m_peerPort; // remote port
            uint32_t vsize;
            Ptr<Node> node;
            uint16_t currentIteration;
            uint16_t maxIteration;
            std::ofstream outFile; //save result for server
            Time startTime; // record each start time

            std::vector<uint64_t> avg; // result
            std::vector<Address> cGroup; // nodes acts as clients with respect to current node
            std::vector<Address> sGroup; // nodes acts as servers with respect to current node
            // Zhuoxu: what happen if we do not use map dataset? i.e., use vector instead.

            std::map <std::string, Ptr<Application>> socketPool;
            uint8_t currentIndex;
            uint16_t iterationNum;
            std::queue<uint16_t> successIter;

            std::map<uint16_t, DataChunk> iterChunk;
            bool isEnd = false;
            std::string thisAddress;
            int printCount = 0;
            std::queue<uint16_t> compQueue;
            std::set<uint16_t> sendSuccToPSet;
            std::map<uint16_t, std::string> sendSchedSet;

        public:
            static TypeId GetTypeId (void);
            InnetworkAggregationInterface ();
            ~InnetworkAggregationInterface ();
            void SetupInnetworkAggregationInterface (uint16_t port, uint16_t itr, uint8_t aggTreeLevel, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, Ptr<Node> node, bool isEnd);
            void CreateSocketPool (std::string cc_name);
            void ReceiveDataFrom (std::string fromStr);
            void ReceiveDataFromAll ();
            int SendResponseVToP (std::vector<uint64_t> &vec , uint16_t iterationNum, std::string fromStr = "");
            void ProduceVToP (uint16_t iterationNum);
            int SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec, uint16_t iterationNum, std::string fromStr = "");
            void SetVSize (uint32_t size);
            //void AvgEnd (uint16_t size, uint16_t iterationNum);
            
            void SaveResult (std::vector<uint64_t> &vec );
            void SetOutFile (const std::string fileName);
            void Addr2Str (Address addr, std::string &str);
            int SendPacket (std::string toStr, uint16_t iterationNum, std::vector<uint8_t> &serializeVec, std::string fromStr = "");
            int SendEndPacket (std::string toStr, std::vector<uint8_t> &chunkBuffer, std::string fromStr = "");
            bool PrintCompInfo (uint16_t iterationNum);
            void PrintBufferSummary(std::vector<uint8_t>& chunkBuffer);
            void TriggerHandleRead();
            void PrintAllInfo();
            void EnableLoggingComponents();
            void DisableLoggingComponents();
            void TraceSingleNodeInfo();
            bool CheckQueueOrder(std::queue<uint16_t> q, uint16_t maxIteration);
            void UpdateQueue(std::string fromStr);
            void PrintRxBuffer(std::string chStr);

            // Zhuoxu: Todo 
            ns3::Ipv4Address GetIpAddrFromNode (Ptr<Node> node);
            uint16_t HashIpToPort(const std::string& ip);
    };

}; /*namespace ns3*/

#endif /*INNETWORK_AGG_INTERFACE_H*/
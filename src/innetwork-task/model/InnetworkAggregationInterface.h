#ifndef INNETWORK_AGG_INTERFACE_H
#define INNETWORK_AGG_INTERFACE_H

#include <cstdint>
#include <functional>
#include <unordered_map>
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
#include <unordered_set>
#include "ns3/TCPclient.h"
#include "ns3/TCPserver.h"


// used for datatransfer by nodes
namespace ns3 {
    typedef std::unordered_map<std::string, std::vector<uint64_t>> ChildChunkMap; // Zhuoxu: socket name -- chunk map

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
            std::unordered_map<uint8_t, ReceivedChunk> chunkMap; // chunk counting by chunkNumber and count
            std::unordered_map<uint16_t, ChildChunkMap> iterChMap;

            std::unordered_map <std::string, Ptr<Application>> socketPool;
            uint8_t currentIndex;
            uint16_t iterationNum;
            std::unordered_set <uint16_t> successIter;

            std::unordered_map<uint16_t, DataChunk> iterChunk;
            bool isEnd = false;
            std::string thisAddress;


        public:
            static TypeId GetTypeId (void);
            InnetworkAggregationInterface ();
            ~InnetworkAggregationInterface ();
            void SetupInnetworkAggregationInterface (uint16_t port, uint16_t itr, uint8_t aggTreeLevel, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, Ptr<Node> node, bool isEnd);
            void CreateSocketPool (std::string cc_name);
            void ReceiveDataFrom (std::string fromStr);
            void ReceiveDataFromAll ();
            void HandleResponseV (std::string fromStr, uint8_t *buffer);
            void HandleRequestV (std::string fromStr);
            void SendRequestVTo (std::string toStr);
            void SendRequestVToAll ();
            void ScheduleAndSend();
            void SendResponseVToP (std::vector<uint64_t> &vec , uint16_t iterationNum);
            void ProduceVToP (uint16_t iterationNum);
            void AVG (uint16_t iterationNum);
            void SendResponseVTo (std::string toStr, std::vector<uint64_t> &vec, uint16_t iterationNum);
            void SetVSize (uint32_t size);
            //void AvgEnd (uint16_t size, uint16_t iterationNum);
            
            bool GetisEnd ();
            void SetisEnd (bool v);
            void ClearChunkMap ();
            void SaveResult (std::vector<uint64_t> &vec );
            void SetOutFile (const std::string fileName);
            void Addr2Str (Address addr, std::string &str);
            void SendPacket (std::string toStr, uint16_t iterationNum, std::vector<uint8_t> &serializeVec);
            void SendEndPacket (std::string toStr, std::vector<uint8_t> &chunkBuffer);
            bool PrintCompInfo (uint16_t iterationNum);
            void PrintBufferSummary(std::vector<uint8_t>& chunkBuffer);
            void TriggerHandleRead();

            // Zhuoxu: Todo 
            ns3::Ipv4Address GetIpAddrFromNode (Ptr<Node> node);
            uint16_t HashIpToPort(const std::string& ip);
    };

}; /*namespace ns3*/

#endif /*INNETWORK_AGG_INTERFACE_H*/
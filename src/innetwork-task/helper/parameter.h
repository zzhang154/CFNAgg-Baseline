#ifndef INNETWORK_TASK_APPDATA_H  
#define INNETWORK_TASK_APPDATA_H  
  
#include <vector>
#include <string>
#include <unordered_set>

namespace ns3 {

    extern uint32_t BASESIZE;
    extern uint32_t chunkSize;
    extern uint32_t quicFrameSize;
    extern uint32_t pktlen;
    extern uint16_t rqtlen;
    extern uint8_t headerChr;
    extern uint16_t ITERTHRESH;
    extern int padIter;
    //extern uint32_t vsize;
    extern std::string TraceIPAddress;
    extern bool debugFlag;

    void UpdatePktlen();
    void SetBaseSize(uint32_t newBaseSize);

    //struch data chunk
    struct  ReceivedChunk {
        //count the child
        std::unordered_set<std::string> chAddr;
        uint16_t iterNum;

        //data
        std::vector<uint64_t> vec;
        //uint64_t* data;
        ReceivedChunk (uint16_t iterationNum) {
            iterNum = iterationNum;
            //uint16_t k = (uint16_t)(BASESIZE / 8);
            //std::vector<uint64_t> data = std::vector<uint64_t>(chunkSize, 0);
            // Zhuoxu: here we set the content to 44 to see if the serialization and deserialization works.
            vec = std::vector<uint64_t> (chunkSize, 0); 
            //data = vec_buffer.data();
        }

        ReceivedChunk () {
            iterNum = UINT16_MAX;
            //uint16_t k = (uint16_t)(BASESIZE / 8);
            //std::vector<uint64_t> data = std::vector<uint64_t>(chunkSize, 0);
            // Zhuoxu: here we set the content to 44 to see if the serialization and deserialization works.
            vec = std::vector<uint64_t> (chunkSize, 0); 
            //data = vec_buffer.data();
        }
    };

    struct DataChunk
    {
        std::unordered_set<std::string> chAddr;
        std::vector<uint64_t> vec; /* data */
        DataChunk () {
            vec = std::vector<uint64_t> (chunkSize, 0); 
            //data = vec_buffer.data();
        }
    };

    struct CompInfo
    {
        std::queue <std::queue<uint16_t>*> compIterQueue;
    };
    
}; /*namespace ns3*/

#endif /*INNETWORK_TASK_APPDATA_H*/
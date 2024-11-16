#ifndef INNETWORK_TASK_APPDATA_H  
#define INNETWORK_TASK_APPDATA_H  
  
#include <vector>
#include <string>
#include <unordered_set>

namespace ns3 {

    extern uint16_t BASESIZE;
    extern uint16_t chunkSize; 
    extern uint16_t pktlen;
    extern uint16_t rqtlen;
    //extern uint16_t vsize;

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

}; /*namespace ns3*/

#endif /*INNETWORK_TASK_APPDATA_H*/
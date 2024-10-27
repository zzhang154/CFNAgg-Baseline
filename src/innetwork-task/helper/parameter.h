#ifndef INNETWORK_TASK_APPDATA_H  
#define INNETWORK_TASK_APPDATA_H  
  
#include <vector>
#include <string>

namespace ns3 {

    extern uint16_t BASESIZE;
    extern uint16_t chunkSize; 
    extern uint16_t pktlen;
    extern uint16_t rqtlen;
    //extern uint16_t vsize;

    //struch data chunk
    struct ReceivedChunk {
        //count the proucer
        uint8_t count;
        //data
        std::vector<uint64_t> data;
        //uint64_t* data;
        ReceivedChunk () {
            count = 0;
            //uint16_t k = (uint16_t)(BASESIZE / 8);
            //std::vector<uint64_t> data = std::vector<uint64_t>(chunkSize, 0);
            data = std::vector<uint64_t> (chunkSize, 0); 
            //data = vec_buffer.data(); 
        }
    };

}; /*namespace ns3*/

#endif /*INNETWORK_TASK_APPDATA_H*/
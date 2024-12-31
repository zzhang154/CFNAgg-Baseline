#ifndef INNETWORK_TASK_APPDATA_H  
#define INNETWORK_TASK_APPDATA_H  
  
#include <vector>
#include <string>
#include <cstdint>
#include "parameter.h"

namespace ns3 {

    //uint16_t k=100;
    uint32_t BASESIZE = 24000; //the num of uint8_t per one chunk 800
    uint32_t chunkSize = static_cast<uint32_t>(BASESIZE/sizeof(uint64_t));//100; sizeof returns the size in bytes.
    uint32_t pktlen = BASESIZE + 10;
    uint32_t quicFrameSize = 1840;
    uint16_t rqtlen = 10;
    uint8_t headerChr = 3;
    //uint16_t k=250;
    //uint16_t BASESIZE=2000; //the num of uint8_t per one chunk
    uint16_t ITERTHRESH = 30;
    int padIter = 0;
    std::string TraceIPAddress = "10.2.8.2";


     void UpdatePktlen() {
        pktlen = BASESIZE + 10;
    }

    // Example function to change BASESIZE and update pktlen
    void SetBaseSize(uint32_t newBaseSize) {
        BASESIZE = newBaseSize;
        chunkSize = static_cast<uint32_t>(BASESIZE / sizeof(uint64_t));
        UpdatePktlen();
    }
    

}; /*namespace ns3*/

#endif /*INNETWORK_TASK_APPDATA_H*/
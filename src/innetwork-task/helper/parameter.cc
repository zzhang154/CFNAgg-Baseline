#ifndef INNETWORK_TASK_APPDATA_H  
#define INNETWORK_TASK_APPDATA_H  
  
#include <vector>
#include <string>
#include <cstdint>
#include "parameter.h"

namespace ns3 {

    //uint16_t k=100;
    uint16_t BASESIZE=800; //the num of uint8_t per one chunk 800
    uint16_t chunkSize = static_cast<uint16_t>(BASESIZE/sizeof(uint64_t));//100
    uint16_t pktlen = BASESIZE + 10;
    uint16_t rqtlen = 10;
    //uint16_t k=250;
    //uint16_t BASESIZE=2000; //the num of uint8_t per one chunk
}; /*namespace ns3*/

#endif /*INNETWORK_TASK_APPDATA_H*/
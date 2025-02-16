#ifndef INNETWORK_TASK_APPDATA_H  
#define INNETWORK_TASK_APPDATA_H  
  
#include <vector>
#include <string>
#include <cstdint>
#include <climits> // Include the climits header for CHAR_BIT
#include <unordered_set>
#include "parameter.h"
#include <unordered_map>
#include <iostream>

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
    std::string TraceIPAddress = "10.2.7.2";
    bool debugFlag = false;
    int conCount = 0;

    // for other topology, please add the additional ip address to trace.

    // std::unordered_set<std::string> ipAddressFilter = {
    //     "10.1.1.1",
    //     "10.1.2.2"
    // };

    // For ISP-50
    std::unordered_set<std::string> ipAddressFilter = {
        "10.1.1.1",
        "10.1.27.1",
        "10.2.1.2",
        "10.2.7.2",
        "10.2.21.2",
        "10.2.31.2",
        "10.1.51.1"
    };

    bool isElementInFilter(std::string ip) {
        return ipAddressFilter.find(ip) != ipAddressFilter.end();
    }

    void UpdatePktlen() {
        pktlen = BASESIZE + 10;
    }

    // Example function to change BASESIZE and update pktlen
    void SetBaseSize(uint32_t newBaseSize) {
        BASESIZE = newBaseSize * CHAR_BIT;
        chunkSize = static_cast<uint32_t>(BASESIZE / sizeof(uint64_t));
        UpdatePktlen();
    }

    std::unordered_map<std::string, std::string> ipToNodeName;
    std::unordered_map<std::string, std::string> NewToOldIpMap;
    std::string GetNodeNameFromIp(const std::string& ip) 
    {
        auto it = ipToNodeName.find(ip);
        return (it != ipToNodeName.end()) ? it->second : "IP_NOT_FOUND";
    }

    void PrintIpToNodeMap()
    {
        std::cout << "Printing ipToNodeName:" << std::endl;
        for (auto &pair : ipToNodeName) {
            std::cout << "IP: " << pair.first << " -> NodeName: " << pair.second << std::endl;
        }
    }

    void PrintNewToOldIpMap() {
        std::cout << "NewToOldIpMap contents:" << std::endl;
        for (auto &entry : NewToOldIpMap) {
            std::cout << "New IP: " << entry.first << " -> Old IP: " << entry.second << std::endl;
        }
    }
    
    // void AddToTraceRecord(PacketTraceTag tag){
    //     uint16_t iteration = tag.GetIteration();
    //     std::vector<PacketTraceTag::TraceEntry> trace = tag.GetTrace();
    //     std::string pro_name = trace[0].nodeName;
    //     traceRecord[pro_name][iteration] = tag;
    // }

}; /*namespace ns3*/

#endif /*INNETWORK_TASK_APPDATA_H*/
#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#include <map>
#include <string>
#include <cstdint>
#include "ns3/PacketTraceTag.h"  // Ensure this file defines ns3::PacketTraceTag

namespace ns3 {

    // Global trace record: <producer, iteration, PacketTraceTag>
    extern std::map<uint16_t, std::map<std::string, std::vector<PacketTraceTag>>> traceRecord;

    extern std::string currentFileName;

    // Add a PacketTraceTag to the global trace record.
    void AddToTraceRecord(PacketTraceTag tag);

    void PrintTraceRecord (std::string &fileName);

} // namespace ns3

#endif // GLOBAL_DATA_H
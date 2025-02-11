#include "global-data.h"
#include "setup.h"
#include "ns3/log.h" // Added for logging
#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <queue>
#include <vector>

NS_LOG_COMPONENT_DEFINE("GlobalData");

// Define global variables
// Zhuoxu: these global var are used by the setup.cc and innetwork-test.cc. Cannot be deleted.

const std::string Number = "-bin8";
const std::string currentDir = GetCurrentWorkingDir();

std::string routerFilePath;
std::string linkFilePath;
std::string aggGroupFilePath;

const std::string conName = "con";
const std::string proName = "pro";
const std::string fowName = "forwarder";
const std::string aggName = "agg";

uint32_t consumerNum = 0;
uint32_t producerNum = 0;
uint32_t forwarderNum = 0;
uint32_t aggregatorNum = 0;
std::string cc = "bbr";
uint16_t basetime = 1000;
uint16_t starttime = 1;
uint16_t stoptime = 500;

// global data structure for the trace record

namespace ns3 {

    // Definition of the global trace record variable.
    std::map<uint16_t, std::map<std::string, std::vector<PacketTraceTag>>> traceRecord;


    void AddToTraceRecord(PacketTraceTag tag) {
        uint16_t iteration = tag.GetIteration();
        // Get the trace entries from the tag (assumed non-empty)
        std::vector<PacketTraceTag::TraceEntry> trace = tag.GetTrace();
        // Use the first entry's nodeName as the producer (node) name.
        std::string node_name = (!trace.empty()) ? trace[0].nodeName : "UNKNOWN";

        traceRecord[iteration][node_name].push_back(tag);
    }
    

    void PrintTraceRecord() {
        std::ofstream ofs("trace_record.log");
        if (!ofs.is_open()) {
            NS_LOG_ERROR("PrintTraceRecord: Failed to open trace_record.log for writing!");
            return;
        }
        
        // Collect iteration keys from the global traceRecord (map is ordered by iteration)
        std::vector<uint16_t> keys;
        for (const auto &iterPair : traceRecord) {
            keys.push_back(iterPair.first);
        }
        
        // Determine which iterations to output.
        std::vector<uint16_t> selectedKeys;
        size_t total = keys.size();
        if (total <= 20) {
            selectedKeys = keys;
        } else {
            // Uniformly sample 20 iterations.
            // Calculate the sampling interval.
            double interval = static_cast<double>(total - 1) / (20 - 1);
            for (int i = 0; i < 20; i++) {
                size_t index = static_cast<size_t>(i * interval + 0.5);
                if (index >= total) {
                    index = total - 1;
                }
                selectedKeys.push_back(keys[index]);
            }
        }
        
        // Output only the sampled iterations.
        for (const auto &iterKey : selectedKeys) {
            // Find the corresponding iteration record.
            const auto iterIt = traceRecord.find(iterKey);
            if (iterIt == traceRecord.end()) continue;
            ofs << "iter " << iterIt->first << "\n";
            // Output each producer block for this iteration.
            for (const auto &prodPair : iterIt->second) {
                ofs << prodPair.first << "\n";
                // For each PacketTraceTag for this producer, print all its trace entries.
                for (const auto &tag : prodPair.second) {
                    for (const auto &entry : tag.GetTrace()) {
                        ofs << entry.nodeName << "(" << entry.addr << ")@"
                            << std::fixed << std::setprecision(6) << entry.t.GetSeconds() << "s\n";
                    }
                }
            }
            ofs << "\n"; // Separate each iteration block with an empty line.
        }
        
        ofs.close();
        NS_LOG_INFO("PrintTraceRecord: Trace record written to trace_record.log");
    }

} // namespace ns3
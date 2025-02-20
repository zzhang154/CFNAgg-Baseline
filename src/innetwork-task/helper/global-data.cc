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
#include <sys/stat.h>    // For stat() and mkdir()
#include <sys/types.h>

#include "ns3/myConfig.h"

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
    std::string currentFileName;

    void AddToTraceRecord(PacketTraceTag tag) {
        uint16_t iteration = tag.GetIteration();
        // Get the trace entries from the tag (assumed non-empty)
        std::vector<PacketTraceTag::TraceEntry> trace = tag.GetTrace();
        // Use the first entry's nodeName as the producer (node) name.
        std::string node_name = (!trace.empty()) ? trace[0].nodeName : "UNKNOWN";

        traceRecord[iteration][node_name].push_back(tag);
    }
    

    void PrintTraceRecord(std::string &fileName) {
        // Build folder path: "result/<fileName>"
        std::string folderPath = "result/path/" + fileName;
        struct stat info;
        if (stat(folderPath.c_str(), &info) != 0) {
            // Folder does not exist; create it.
            if (mkdir(folderPath.c_str(), 0755) != 0) {
                NS_LOG_ERROR("PrintTraceRecord: Failed to create directory " << folderPath);
                return;
            }
        }
        // Build the full file path.
        std::string filePath = folderPath + "/trace_record.log";
        
        std::ofstream ofs(filePath);
        if (!ofs.is_open()) {
            NS_LOG_ERROR("PrintTraceRecord: Failed to open " << filePath << " for writing!");
            return;
        }
        
        // Collect iteration keys from the traceRecord (assuming traceRecord is ordered).
        std::vector<uint16_t> keys;
        for (const auto &iterPair : traceRecord) {
            keys.push_back(iterPair.first);
        }
        
        // Determine which iterations to output. For example, sample up to 20 iterations.
        std::vector<uint16_t> selectedKeys;
        size_t total = keys.size();
        if (total <= 20) {
            selectedKeys = keys;
        } else {
            double interval = static_cast<double>(total - 1) / (20 - 1);
            for (int i = 0; i < 20; i++) {
                size_t index = static_cast<size_t>(i * interval + 0.5);
                if (index >= total)
                    index = total - 1;
                selectedKeys.push_back(keys[index]);
            }
        }
        
        // Output the sampled iterations.
        for (const auto &iterKey : selectedKeys) {
            auto iterIt = traceRecord.find(iterKey);
            if (iterIt == traceRecord.end())
                continue;
            ofs << "Iteration " << iterIt->first << "\n";
            for (const auto &prodPair : iterIt->second) {
                ofs << prodPair.first << "\n";
                for (const auto &tag : prodPair.second) {
                    for (const auto &entry : tag.GetTrace()) {
                        ofs << entry.nodeName << "(" << entry.addr << ")@"
                            << std::fixed << std::setprecision(6)
                            << entry.t.GetSeconds() << "s\n";
                    }
                }
                ofs << "\n";
            }
            ofs << "\n";
        }
        
        ofs.close();
        NS_LOG_INFO("PrintTraceRecord: Trace record written to " << filePath);
    }

} // namespace ns3
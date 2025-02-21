#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <queue>
#include <iostream>
#include <cstring>

// Added required NS-3 headers
#include "ns3/core-module.h"
#include "ns3/node.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include <arpa/inet.h> // for inet_ntoa
#include <cstdint>
#include <algorithm> // for std::reverse

namespace ns3 {

// Function declarations
void Addr2Str(Address addr, std::string &str);
void HelloUtils();
extern std::mutex fileMutex;
void WriteToFile(const std::string& filename, const std::string& result); 
std::string QueueToString(const std::queue<uint16_t>& queue);

std::string Ipv4AddressToString(ns3::Ipv4Address ipv4Address);
std::string reverseString(const std::string &str);
ns3::Ipv4Address GetIpAddrFromNode (Ptr<Node> node);
void EnableLoggingComponents();
void DisableLoggingComponents();

struct SimulationParams {
    uint32_t bufSize;
    std::string tcpCC;
    double lossRate;
    std::string fileName;
    uint16_t iteration;
};

class TracedTimeQueue
{
public:
    TracedTimeQueue();

    // Method to push an element
    void Push(Time t);

    // Method to pop an element
    void Pop();

    // Method to get the size of the queue
    size_t Size() const;

    // Get the underlying queue
    const std::queue<Time> &GetTimes() const;

    // Define a traced callback for the queue
    TracedCallback<const std::queue<Time>&> m_tracedTimeQueue;

private:
    // Underlying queue of Time objects
    std::queue<Time> m_times;

    // Callback method
    void TimeQueueChanged(const std::queue<Time> &times);
};

// Callback function declarations
void MyTimeQueueCallback(const std::queue<Time> &times);
void ValueChangedCallback(std::string context, uint32_t oldValue, uint32_t newValue);
void TimeChangedCallback(std::string context, Time oldValue, Time newValue);

}; /* namespace ns3 */

#endif /* UTILS_H */
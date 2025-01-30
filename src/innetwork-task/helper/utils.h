#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include "ns3/packet.h"
#include <arpa/inet.h> // for inet_ntoa
#include "ns3/ipv4-address.h"
#include <cstdint>
#include <string>
#include <mutex>
#include <cstring> 
#include "ns3/core-module.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <queue>
#include <iostream>
#include <algorithm> // for std::reverse

namespace ns3 {

// Function declarations
void Addr2Str(Address addr, std::string &str);
void HelloUtils();
extern std::mutex fileMutex;
void WriteToFile(const std::string& filename, const std::string& result); 
std::string Ipv4AddressToString(ns3::Ipv4Address ipv4Address);
std::string reverseString(const std::string &str);

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
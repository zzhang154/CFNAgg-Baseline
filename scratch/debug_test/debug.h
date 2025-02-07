#include "ns3/core-module.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

#include <queue>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TracedTimeQueue");

class TracedTimeQueue
{
public:
    TracedTimeQueue()
    {
        // Initialize the trace source
        m_tracedTimeQueue.ConnectWithoutContext(
            MakeCallback(&TracedTimeQueue::TimeQueueChanged, this));
    }

    // Method to push an element
    void Push(Time t)
    {
        m_times.push(t);
        m_tracedTimeQueue(m_times); // Trigger the trace source
    }

    // Method to pop an element
    void Pop()
    {
        if (!m_times.empty())
        {
            m_times.pop();
            m_tracedTimeQueue(m_times); // Trigger the trace source
        }
    }

    // Method to get the size of the queue
    size_t Size() const
    {
        return m_times.size();
    }

    // Get the underlying queue
    const std::queue<Time> &GetTimes() const
    {
        return m_times;
    }

    // Define a traced callback for the queue
    TracedCallback<const std::queue<Time>&> m_tracedTimeQueue;

private:
    // Underlying queue of Time objects
    std::queue<Time> m_times;

    // Callback method
    void TimeQueueChanged(const std::queue<Time> &times)
    {
        // This method will be called whenever the traced value changes
        NS_LOG_INFO("Time queue changed. Current size: " << times.size());
    }
};

void MyTimeQueueCallback(const std::queue<Time> &times)
{
    std::cout << "Time queue changed. New size: " << times.size() << std::endl;
}

void ValueChangedCallback(std::string context, uint32_t oldValue, uint32_t newValue) {
    std::cout << "Value changed from " << oldValue << " to " << newValue << " in context: " << context << std::endl;
}

void TimeChangedCallback(std::string context, Time oldValue, Time newValue) {
    std::cout << "Value changed from " << oldValue.GetSeconds() << " to " << newValue.GetSeconds() << " in context: " << context << std::endl;
}
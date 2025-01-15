/*
Zhuoxu ZHANG, 2025/01/09.
This example demonstrates how to use the TracedValue class to monitor changes to a variable.
*/
#include "ns3/core-module.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "debug.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    // Enable logging
    LogComponentEnable("TracedTimeQueue", LOG_LEVEL_INFO);

    // Create a TracedValue instance
    TracedValue<uint32_t> tracedCounter;

    // Connect a callback to the TracedValue instance with a context string
    tracedCounter.Connect(MakeCallback(&ValueChangedCallback), "Counter");

    // Perform typical operations
    tracedCounter = 10;  // Assignment
    tracedCounter++;     // Increment
    tracedCounter += 5;  // Addition assignment
    tracedCounter--;     // Decrement

    TracedValue<Time> tracedTime;
    tracedTime.Connect(MakeCallback(&TimeChangedCallback), "TimerCounter");

    // Set up a simple simulation
    Simulator::Stop(Seconds(10.0)); // Stop the simulation at 10 seconds

    // Run the simulator
    Simulator::Run();

    // Derive the current simulation time
    Time currentTime = Simulator::Now();

    // Create two Time objects
    Time t1 = Seconds(1.0);
    Time t2 = MilliSeconds(1000);

    // Compare the Time objects
    if (t1 == t2)
    {
        std::cout << "t1 is equal to t2" << std::endl;
    }
    else if (t1 < t2)
    {
        std::cout << "t1 is less than t2" << std::endl;
    }
    else if (t1 > t2)
    {
        std::cout << "t1 is greater than t2" << std::endl;
    }

    // Print the current simulation time in different units
    std::cout << "Current simulation time: " << currentTime.GetSeconds() << " seconds" << std::endl;
    std::cout << "Current simulation time: " << currentTime.GetMilliSeconds() << " milliseconds" << std::endl;
    std::cout << "Current simulation time: " << currentTime.GetNanoSeconds() << " nanoseconds" << std::endl;

    if (currentTime > MilliSeconds(2.0))
    {
        std::cout << "Current simulation time is greater than 2 MilliSeconds" << std::endl;
    }

    tracedTime = MilliSeconds(1000);
    tracedTime = Simulator::Now();
    tracedTime += Seconds(2000);

    // Create an instance of the custom class
    TracedTimeQueue tracedTimeQueue;

    // Connect a custom callback to the trace source
    tracedTimeQueue.m_tracedTimeQueue.ConnectWithoutContext(
        MakeCallback(&MyTimeQueueCallback));

    // Push and pop elements to trigger the callback
    tracedTimeQueue.Push(Seconds(1.0));
    tracedTimeQueue.Push(Seconds(2.0));
    tracedTimeQueue.Pop();

    // Clean up and exit
    Simulator::Destroy();

    return 0;
}
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/output-stream-wrapper.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <unistd.h>
#include <limits.h>

// Zhuoxu: no need to use the absolute path
#include "ns3/consumer.h"
#include "ns3/producer.h"
#include "ns3/aggregator.h"
#include "ns3/parameter.h"
#include "ns3/utils.h"

using namespace ns3;

// #include "innetwork-test.h"
#include "ns3/setup.h"

NS_LOG_COMPONENT_DEFINE("innetwork-test");

// const std::string currentDir = GetCurrentWorkingDir();
// const std::string Number = "-bin8";

// /*
// const std::string routerFilePath = currentDir + "/scratch/data/router" + Number + ".txt";
// const std::string linkFilePath = currentDir + "/scratch/data/link" + Number + ".txt";
// const std::string aggGroupFilePath = currentDir + "/scratch/data/aggtree" + Number + ".txt";
// */

// std::string routerFilePath;
// std::string linkFilePath;
// std::string aggGroupFilePath;

// const std::string conName = "con";
// const std::string proName = "pro";
// const std::string fowName = "forwarder";
// const std::string aggName = "agg";

// // Zhuoxu: this should be set automatically by the datauration file.
// uint32_t consumerNum;
// uint32_t producerNum;
// uint32_t forwarderNum;
// uint32_t aggregatorNum;
// std::string cc = "bbr";
// uint16_t basetime = 1000;
// uint16_t starttime = 1;
// uint16_t stoptime = 500;//5000

int main(int argc, char *argv[]) {
    HelloUtils();
    LogComponentEnable("TracedTimeQueue", LOG_LEVEL_ALL);
    // Create an instance of the custom class
    TracedTimeQueue tracedTimeQueue;

    // Connect a custom callback to the trace source
    tracedTimeQueue.m_tracedTimeQueue.ConnectWithoutContext(
        MakeCallback(&MyTimeQueueCallback));

    // Push and pop elements to trigger the callback
    tracedTimeQueue.Push(Seconds(1.0));
    tracedTimeQueue.Push(Seconds(2.0));
    tracedTimeQueue.Pop();
                              
    CommandLine cmd;
    uint32_t vsize = 150;
    bool topotype = 1;
    cmd.AddValue("vsize", "vector size", vsize);
    cmd.AddValue("topotype", "choose test topo type", topotype);
    cmd.AddValue("cc", "choose test congestion control", cc);
    cmd.AddValue("basetime", "set base time", basetime);
    cmd.AddValue("starttime", "set start time", starttime);
    cmd.AddValue("stoptime", "set stop time", stoptime);
    cmd.Parse(argc, argv);

    SetBaseSize(vsize);

    // std::vector<std::string> prefixFileNames = {"-bin8-no", "-bin16-no", "-bin32-no"}; // Example prefix file names
    // std::vector<std::string> prefixFileNames = {"-1-fwd-5-agg"};
    // std::vector<std::string> prefixFileNames = {"50"};
    std::vector<std::string> prefixFileNames = {"-no-fwd5"};
    // std::vector<uint16_t> iterationNumbers = {1000, 2000, 3000, 4000, 5000, 10000}; // Example iteration numbers
    std::vector<uint16_t> iterationNumbers = {5};

    const std::string outputFilename = "simulation_results.txt"; // Shared output file

    // Set the Time resolution once before the loops
    Time::SetResolution(Time::NS);

    for (std::string& prefix : prefixFileNames) {
        routerFilePath = currentDir + "/scratch/data/router" + prefix + ".txt";
        linkFilePath = currentDir + "/scratch/data/link" + prefix + ".txt";
        aggGroupFilePath = currentDir + "/scratch/data/aggtree" + prefix + ".txt";

        for (uint16_t itr : iterationNumbers) {
            std::cout << "\n\n#################### SIMULATION PARAMETERS ####################\n\n\n";
            std::cout << "Prefix: " << prefix << " Iteration: " << itr << " Vector Size: " << vsize << " Congestion Control Algorithm: " << cc << std::endl;
            std::cout << "\n\n#################### SIMULATION SET-UP ####################\n\n\n";

            LogLevel log_precision = LOG_LEVEL_LOGIC;

            LogComponentEnableAll(LOG_PREFIX_TIME);
            LogComponentEnableAll(LOG_PREFIX_FUNC);
            LogComponentEnableAll(LOG_PREFIX_NODE);

            LogComponentEnable("InnetworkAggregationInterface", log_precision);
            LogComponentEnable("Consumer", LOG_LEVEL_ALL);
            LogComponentEnable("Aggregator", LOG_LEVEL_ALL);
            LogComponentEnable("Producer", LOG_LEVEL_ALL);
            
            LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
            // LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
 
            // LogComponentEnable("TcpSocketBase", LOG_LEVEL_ALL);

            // LogComponentDisable("Consumer", LOG_LEVEL_ALL);
            // LogComponentDisable("Aggregator", LOG_LEVEL_ALL);
            // LogComponentDisable("Producer", LOG_LEVEL_ALL);
            // // LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
            // LogComponentEnable("TCPclient", LOG_LEVEL_ALL);
            // LogComponentEnable("TCPserver", LOG_LEVEL_ALL);
            // LogComponentDisable("TcpSocketBase", LOG_LEVEL_ALL);
            // LogComponentDisable("TcpRxBuffer", LOG_LEVEL_ALL);
            // LogComponentDisable("TcpTxBuffer", LOG_LEVEL_ALL);
            // LogComponentDisable("Packet", LOG_LEVEL_DEBUG);

            // Initialize node containers
            NodeContainer consumer;
            NodeContainer producer;
            NodeContainer forwarder;
            NodeContainer aggregator;

            // Reset consumer, producer, forwarder, and aggregator counts
            consumerNum = 0;
            producerNum = 0;
            forwarderNum = 0;
            aggregatorNum = 0;

            CountRouterNodes(routerFilePath);
            BuildTopo(linkFilePath, consumer, producer, forwarder, aggregator);

            uint16_t server_port = 1234;
            if (topotype == 0) {
                CreateDirectTopo(consumer, producer, itr, vsize, server_port);
            } else {
                CreateAggTreeTopo(itr, vsize, server_port);
            }

            Packet::EnablePrinting();
            Packet::EnableChecking();

            // Populate routing tables
            Ipv4GlobalRoutingHelper::PopulateRoutingTables();

            // Zhuoxu: we don't need this currently.
            // Print routing tables
            // PrintRoutingTables();

            Simulator::Stop(Seconds(15.0));
            Simulator::Run();
            Simulator::Destroy();
            Names::Clear(); // Clear the Names database

            std::string result = "Prefix: " + prefix + ", Iteration: " + std::to_string(itr) + " - Simulation completed successfully.";
            std::cout << "Prefix: " + prefix + ", Iteration: " + std::to_string(itr) + " - Simulation completed successfully." << std::endl;
            WriteToFile(outputFilename, result);

            std::cout << "\n#################### SIMULATION END ####################\n\n\n";
        }
    }

    return 0;
}

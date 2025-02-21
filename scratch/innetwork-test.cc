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
#include "ns3/global-data.h"

using namespace ns3;

// #include "innetwork-test.h"
#include "ns3/setup.h"
#include "ns3/myConfig.h"

NS_LOG_COMPONENT_DEFINE("innetwork-test");

NS_OBJECT_ENSURE_REGISTERED(TcpAIMD);

std::vector<SimulationParams> BuildExperimentList() {
    std::vector<SimulationParams> experiments;
    auto bufferSizes = MyConfig::GetBufferSizes();
    auto ccList = MyConfig::GetCongestionControls();
    auto lossRates = MyConfig::GetLossRates();
    auto fileNames = MyConfig::GetFileNames();
    auto iterationNumbers = MyConfig::GetIterationNumbers();

    for (uint32_t bufSize : bufferSizes) {
        for (const auto &tcp_cc : ccList) {
            for (double lossRate : lossRates) {
                for (const auto &fileName : fileNames) {
                    for (uint16_t itr : iterationNumbers) {
                        experiments.push_back({bufSize, tcp_cc, lossRate, fileName, itr});
                    }
                }
            }
        }
    }
    return experiments;
}

int main(int argc, char *argv[]) {
    // Load configuration parameters from file
    MyConfig::Load("src/innetwork-task/config-file/config.ini");

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
    uint32_t vsize;
    bool topotype = 1;
    cmd.AddValue("vsize", "vector size", vsize);
    cmd.AddValue("topotype", "choose test topo type", topotype);
    cmd.AddValue("cc", "choose test congestion control", cc);
    cmd.AddValue("basetime", "set base time", basetime);
    cmd.AddValue("starttime", "set start time", starttime);
    cmd.AddValue("stoptime", "set stop time", stoptime);
    cmd.Parse(argc, argv);

    vsize = MyConfig::GetVectorSize();
    SetBaseSize(vsize);

    std::vector<std::string> FileNames = MyConfig::GetFileNames();
    std::vector<uint16_t> iterationNumbers = MyConfig::GetIterationNumbers();
    // Example buffer sizes (in bytes)
    std::vector<uint32_t> bufferSizes = MyConfig::GetBufferSizes();

    const std::string outputFilename = "simulation_results.txt"; // Shared output file

    // Set the Time resolution once before the loops
    Time::SetResolution(Time::NS);

    auto experiments = BuildExperimentList();
    // Loop through each experiment.
    for (auto &exp : experiments) {
        // Apply configuration for this experiment.
        Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(exp.bufSize));
        Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(exp.bufSize));

        MyConfig::SetCongestionControl(exp.tcpCC);
        MyConfig::SetCurrentLossRate(exp.lossRate);

        // Build file paths (using a base directory currentDir, etc.)
        currentFileName = exp.fileName;
        routerFilePath = currentDir + "/scratch/data/router/router-" + exp.fileName + ".txt";
        linkFilePath = currentDir + "/scratch/data/link/link-" + exp.fileName + ".txt";
        aggGroupFilePath = currentDir + "/scratch/data/aggtree/aggtree-" + exp.fileName + ".txt";

        // Output configuration details.
        std::cout << "\n#################### SIMULATION START ####################\n\n" << std::flush;

        LogLevel log_precision = LOG_LEVEL_LOGIC;

        LogComponentEnableAll(LOG_PREFIX_TIME);
        LogComponentEnableAll(LOG_PREFIX_FUNC);
        LogComponentEnableAll(LOG_PREFIX_NODE);

        LogComponentEnable("InnetworkAggregationInterface", log_precision);
        // LogComponentEnable("Consumer", LOG_LEVEL_ALL);
        // LogComponentEnable("Aggregator", LOG_LEVEL_ALL);
        // LogComponentEnable("Producer", LOG_LEVEL_ALL);
        // LogComponentEnable("Setup", LOG_LEVEL_ALL);
        
        LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
        LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_INFO);
        // LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_ALL);

        // LogComponentEnable("TcpSocketBase", LOG_LEVEL_ALL);

        // LogComponentDisable("Consumer", LOG_LEVEL_ALL);
        // LogComponentDisable("Aggregator", LOG_LEVEL_ALL);
        // LogComponentDisable("Producer", LOG_LEVEL_ALL);
        // // LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_ALL);
        LogComponentDisable("TCPclient", LOG_LEVEL_ALL);
        LogComponentDisable("TCPserver", LOG_LEVEL_ALL);
        LogComponentDisable("TcpSocketBase", LOG_LEVEL_ALL);
        LogComponentDisable("TcpRxBuffer", LOG_LEVEL_ALL);
        LogComponentDisable("TcpTxBuffer", LOG_LEVEL_ALL);
        LogComponentDisable("Packet", LOG_LEVEL_DEBUG);
        
        // LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_WARN);
        // LogComponentEnable("TCPclient", LOG_LEVEL_WARN);
        // LogComponentEnable("TCPserver", LOG_LEVEL_WARN);
        // LogComponentEnable("TcpSocketBase", LOG_LEVEL_WARN);

        // LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_ALL);


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
            CreateDirectTopo(consumer, producer, exp.iteration, vsize, server_port);
        } else {
            CreateAggTreeTopo(exp.iteration, vsize, server_port);
        }

        Packet::EnablePrinting();
        Packet::EnableChecking();

        // Populate routing tables
        Ipv4GlobalRoutingHelper::PopulateRoutingTables();

        // Zhuoxu: we don't need this currently.
        // Print routing tables
        // PrintRoutingTables();

        Simulator::Stop(Seconds(5000.00));
        Simulator::Run();
        Simulator::Destroy();
        Names::Clear(); // Clear the Names database

        // Output configuration details.
        std::cout << "\n#################### SIMULATION CONFIG ####################\n\n" << std::flush;
        std::cout << "FileName: " << exp.fileName 
                  << "\nIteration: " << exp.iteration
                  << "\nCongestion Control: " << MyConfig::GetCongestionControl() 
                  << "\nbufSize: " << exp.bufSize 
                  << "\nlossRate: " << MyConfig::GetLossRate() 
                  << "\nVector Size: " << vsize 
                  << std::endl << std::endl << std::flush;

        // Print debug info
        PrintIpToNodeMap();
        PrintNewToOldIpMap();
        PrintTraceRecord(exp.fileName);
        std::cout << "\n#################### SIMULATION END ####################\n\n\n\n" << std::flush;
    }
    return 0;
}

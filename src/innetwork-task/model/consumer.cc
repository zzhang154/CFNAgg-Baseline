#include "consumer.h"

#include <unistd.h>  // For getcwd
#include <limits.h>  // For PATH_MAX
std::string GetCurrentWorkingDir() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}
// Global variables with paths
const std::string currentDir = GetCurrentWorkingDir();

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("Consumer");
    NS_OBJECT_ENSURE_REGISTERED (Consumer);

    TypeId 
    Consumer::GetTypeId (void) {
        static TypeId tid = TypeId ("ns3::Consumer")
            .SetParent<Application> ()
            .SetGroupName("innetwork-task")
            .AddConstructor<Consumer> ()
        ;
        return tid;
    };

    Consumer::Consumer() {
        NS_LOG_FUNCTION (this);
    }

    void 
    Consumer::SetupConsumer (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, uint16_t basetime, std::string cc_name) {
        NS_LOG_FUNCTION (this);

        // // Get the current working directory
        // char cwd[PATH_MAX];
        // if (getcwd(cwd, sizeof(cwd)) != NULL) {
        //     NS_LOG_INFO("Current working directory: " << cwd);
        // } else {
        //     NS_LOG_INFO("Error getting current working directory");
        // }

        // const std::string fileName = "/home/dd/ns-3-quic-agg-zhuoxu/ns-allinone-3.42/ns-3.42/scratch/data.txt";
        const std::string fileName = GetCurrentWorkingDir() + "/scratch/data.txt";
        this->m_peerPort = port;
        this->basetime = basetime;
        this->cc_name = cc_name;
        this->nodeInnetworkAggregationInterface = CreateObject<InnetworkAggregationInterface> ();
        this->nodeInnetworkAggregationInterface->SetupInnetworkAggregationInterface (port, itr, rank, sGroup, cGroup, GetNode (), false);
        this->nodeInnetworkAggregationInterface->SetOutFile (fileName);
        this->nodeInnetworkAggregationInterface->SetVSize (vsize);
    }

    void
    Consumer::StartApplication () {
        NS_LOG_FUNCTION (this);
        //this->nodeInnetworkAggregationInterface->CreateSocketPool (cc_name);
        ns3::Simulator::Schedule(ns3::MilliSeconds(0), &InnetworkAggregationInterface::CreateSocketPool, this->nodeInnetworkAggregationInterface, cc_name);
        // this->nodeInnetworkAggregationInterface->CreateSocketPool(cc_name);
        // this->nodeInnetworkAggregationInterface->ReceiveDataFromAll();
        ns3::Simulator::Schedule(ns3::MilliSeconds(basetime+2), &InnetworkAggregationInterface::ReceiveDataFromAll, this->nodeInnetworkAggregationInterface);
        // ns3::Simulator::Schedule(ns3::MilliSeconds(basetime+2700), &InnetworkAggregationInterface::PrintAllInfo, this->nodeInnetworkAggregationInterface);

    }

    Consumer::~Consumer () {
        NS_LOG_FUNCTION (this);
        //outFile. close ();
    }

    void 
    Consumer::DoDispose () {
        NS_LOG_FUNCTION (this);
        Application::DoDispose ();
    }
    
    void 
    Consumer::StopApplication () {
        NS_LOG_FUNCTION (this);
        NS_LOG_INFO ("Consumer Stop");

        ns3::Simulator::Stop();
    }
    
}; /*namespace ns3*/
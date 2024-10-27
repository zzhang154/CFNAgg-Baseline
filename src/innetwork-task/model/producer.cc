#include "producer.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("Producer");
    NS_OBJECT_ENSURE_REGISTERED (Producer);

    TypeId 
    Producer::GetTypeId (void) {
        static TypeId tid = TypeId ("ns3::Producer")
            .SetParent<Application> ()
            .SetGroupName("innetwork-train")
            .AddConstructor<Producer> ()
        ;
        return tid;
    };

    Producer::Producer () {
        NS_LOG_FUNCTION (this);

    }

    void 
    Producer::SetupProducer (uint16_t port, uint16_t itr, uint8_t rank, uint16_t vsize, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, uint16_t basetime, std::string cc_name) {
        NS_LOG_FUNCTION (this);
        this->m_peerPort = port;
        this->basetime = basetime;
        this->cc_name = cc_name;
        this->nodeInnetworkAggregationInterface = CreateObject<InnetworkAggregationInterface> ();
        this->nodeInnetworkAggregationInterface->SetupInnetworkAggregationInterface (port, itr, rank, sGroup, cGroup, GetNode (), false);
        this->nodeInnetworkAggregationInterface->SetVSize (vsize);
        }

    void
    Producer::StartApplication () {
        NS_LOG_FUNCTION (this);
        this->nodeInnetworkAggregationInterface->CreateSocketPool (cc_name);
        ns3::Simulator::Schedule(ns3::MilliSeconds(basetime+500), &InnetworkAggregationInterface::ReceiveDataFromAll, this->nodeInnetworkAggregationInterface);
    }

    Producer::~Producer() {
        NS_LOG_FUNCTION (this);
    }

    void 
    Producer::DoDispose () {
        NS_LOG_FUNCTION (this);
        Application::DoDispose ();

    }

    void 
    Producer::StopApplication () {

        ns3::Simulator::Stop();
        NS_LOG_FUNCTION (this);
        //Simulator::Cancel (m_sendEvent);

    }

}; /*namespace ns3*/
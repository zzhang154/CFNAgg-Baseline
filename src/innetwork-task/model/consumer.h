// consumer.h  

#ifndef INNETWORK_TASK_CONSUMER_H  

#define INNETWORK_TASK_CONSUMER_H 

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"  
#include "ns3/quic-socket.h" 
#include <fstream>
#include <cstdint>
#include "InnetworkAggregationInterface.h"

namespace ns3 {

class Consumer : public Application {
        //std::vector<uint64_t> avg;
        uint16_t m_peerPort;
        uint16_t basetime;
        std::string cc_name;
        Ptr<InnetworkAggregationInterface> nodeInnetworkAggregationInterface;

    public:
        static TypeId GetTypeId (void);
        Consumer ();
        virtual ~Consumer ();
        void SetupConsumer (uint16_t port, uint16_t itr, uint8_t rank, uint16_t vsize, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, uint16_t basetime, std::string name);

    protected:
        void DoDispose ();

    private:
        void StartApplication ();
        void StopApplication ();

}; /*Consumer class*/

}; 
#endif 
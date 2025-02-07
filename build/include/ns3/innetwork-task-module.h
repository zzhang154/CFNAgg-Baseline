#ifdef NS3_MODULE_COMPILATION 
    error "Do not include ns3 module aggregator headers from other modules these are meant only for end user scripts." 
#endif 
#ifndef NS3_MODULE_INNETWORK_TASK
    // Module headers: 
    #include <ns3/parameter.h>
    #include <ns3/utils.h>
    #include <ns3/vectorop.h>
    #include <ns3/setup.h>
    #include <ns3/PacketTraceTag.h>
    #include <ns3/aggregator.h>
    #include <ns3/consumer.h>
    #include <ns3/InnetworkAggregationInterface.h>
    #include <ns3/producer.h>
#endif 
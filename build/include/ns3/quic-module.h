#ifdef NS3_MODULE_COMPILATION 
    error "Do not include ns3 module aggregator headers from other modules these are meant only for end user scripts." 
#endif 
#ifndef NS3_MODULE_QUIC
    // Module headers: 
    #include <ns3/quic-congestion-ops.h>
    #include <ns3/quic-socket.h>
    #include <ns3/quic-socket-base.h>
    #include <ns3/quic-socket-factory.h>
    #include <ns3/quic-l4-protocol.h>
    #include <ns3/quic-socket-rx-buffer.h>
    #include <ns3/quic-socket-tx-buffer.h>
    #include <ns3/quic-socket-tx-scheduler.h>
    #include <ns3/quic-socket-tx-pfifo-scheduler.h>
    #include <ns3/quic-socket-tx-edf-scheduler.h>
    #include <ns3/quic-stream.h>
    #include <ns3/quic-stream-base.h>
    #include <ns3/quic-l5-protocol.h>
    #include <ns3/quic-stream-tx-buffer.h>
    #include <ns3/quic-stream-rx-buffer.h>
    #include <ns3/quic-header.h>
    #include <ns3/quic-subheader.h>
    #include <ns3/quic-transport-parameters.h>
    #include <ns3/quic-bbr.h>
    #include <ns3/quic-helper.h>
    #include <ns3/windowed-filter.h>
    #include <ns3/circularbuffer.h>
#endif 
# In-Network Aggregation Protocol

We have implemented a simple in-network aggregation protocol that primarily functions on a request/response content request mechanism. It aggregates content produced by producers at the aggregator level and then allows consumers to retrieve and average the data. In the protocol, the aggregator divides the peer network into multiple domains, significantly enhancing content aggregation efficiency through a hierarchical aggregation mechanism.


## Implemented Methods

1. **Network Topology Construction**: Based on the given topology structure and link information in the innetwork-test.

2. **Node Classification**: Network nodes are categorized as consumers, producers, and aggregators.

3. **QUIC Client and Server Implementation**: The main functions are to send and receive data and store packets in the circular buffer.

4. **Data Operation Helpers**: Include vector chunking, summing, averaging, copying, and global parameter passing.

5. **Circular Buffer Construction for Each Socket**: With a smaller space and higher efficiency, it stores the content from the QUIC socket's RXBUFFER for local aggregation.

6. **In-Network Aggregation Interface Implementation**: Based on the request/response content request mechanism, hierarchical aggregation and direct aggregation are implemented.

## In-Network Aggregation Interface Functions

1. **CreateSocketPool**: In data transmission, where the requester is the client and the respondent is the server, two client and server applications are constructed for each network connection and stored in the socket pool.

2. **ReceiveDataFrom**: Read the data packet content from the circular buffer, determine whether the content is a request or response, and respond accordingly.

3. **HandleResponseV**: When a response is received, deserialize the data and store it in chunks. After collecting enough data for each chunk, perform local averaging. When all data chunks within the required domain (all child nodes) are received, upload to the parent node. (Local averaging is performed because the number of nodes in each domain is the same; if not, the result would be incorrect.)
   - **Auxiliary Functions**:
     - avgend: Determine if enough packets have been collected.
     - avg: If enough packets are collected, upload to the parent node; if there is no parent node, the network aggregation ends.

4. **HandleRequestV**: When a request is received, respond to the request if the current node is a producer, and forward the request to all child nodes otherwise.
   - **Auxiliary Functions**:
     - SendResponseVToP: Assist in obtaining the requester and response data vector.
     - SendResponseVTo: Chunk the data vector and upload it to the requester.
     - SendRequestVToAll: Send a request to all child nodes.
     - SendRequestVTo: Send a request to a specific child node.

5. **Other Auxiliary Functions**: Include storing data from chunks into the result vector, storing the result vector, etc.


## Optimizations

1. We have improved some of the operational efficiency by optimizing the code structure and variable memory usage. By using wireshark and Valgrind, we found packet loss and memory errors and deficiencies for further optimization.

2. Since the quic module we use only implements the quic-bbr congestion algorithm, and defaults to TCPNewReno for congestion control sending when no congestion algorithm is specified, we have optimized network communication to get a relatively good result by adjusting the parameters of quic-bbr and quicSocketBase's transmission, including kMaxPacketsReceivedBeforeAckSend, kTimeReorderingFraction, kReorderingThreshold, kMaxTLPs, HighGain, MaxPacketSize and m_losstime. Future work can further optimize network communication through different congestion control methods, parameter settings, socket caching, traffic, and transmission control, using a more comprehensive QUIC module (such as Google's quiche).



# References and Future Implementations

Our code utilizes the QUIC module from [signetlabdei/quic-ns-3](https://github.com/signetlabdei/quic-ns-3/tree/master), which is accompanied by a simple explanation in their own paper. 

We have referenced the code from [qiangdadi/innetwork-agg-task](https://github.com/qiangdadi/innetwork-agg-task), which is based on the QUIC module implemented on ns3 using Google's quiche, as found in [SoonyangZhang/quic-on-ns3](https://github.com/SoonyangZhang/quic-on-ns3?tab=readme-ov-file).
During the installation of these codes, we encountered some library issues. We plan to attempt the use of this more comprehensive QUIC implementation in the future.


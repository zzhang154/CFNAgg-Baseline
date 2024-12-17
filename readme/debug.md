Todo:
1. // Note that this function did not implement the PSH flag
uint32_t
TcpSocketBase::SendPendingData(bool withAck)

It seems that  nPacketSent always return 0, and there is something wrong with the send function. Need to check it.

2.
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Data segment, seq=1 pkt size=536
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Expected sequence number: 1
+2.000009441s 17 TcpRxBuffer:Add(0x5625d87711f0, 0x5625d88e7ff0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17 TcpRxBuffer:Add(): Add pkt 0x5625d88e7ff0 len=536 seq=1, when NextRxSeq=1, buffsize=0
+2.000009441s 17 TcpRxBuffer:Add(): Buffered packet of seqno=1 len=536
+2.000009441s 17 TcpRxBuffer:ClearSackList(0x5625d87711f0, 537)

Check the ReceivedData(), we find that TcpRxBuffer:Add has actually add the data into the RxBuffer.


Check 1:
+2.000000000s 17 TCPserver:HandleRead(): 0x55ab61ef8380 Entering the HandleRead function ..., with Ipv4 address 10.1.1.1
+2.000000000s 17 TCPserver:HandleRead(): 0x55ab61ef8380 Printing all members of compQueue with size: 0
+2.000000000s 17 TCPserver:HandleRead(): PrintSocketInfo(socket)...
+2.000000000s 17  [node 17] TcpSocketBase:GetSockName(0x55ab61ef8ba0)
+2.000000000s 17 TCPserver:PrintSocketInfo(): Socket created with IP address: 10.2.1.2 and port: 11362
+2.000000000s 17  [node 17] TcpSocketBase:GetSockName(0x55ab61ef8ba0)
+2.000000000s 17 TCPserver:HandleRead(): Local IP: 10.2.1.2, Local Port: 11362

+2.000000000s 17  [node 17] TcpSocketBase:RecvFrom(0x55ab61ef8ba0, 48300, 4098)
+2.000000000s 17  [node 17] TcpSocketBase:Recv(0x55ab61ef8ba0)
+2.000000000s 17 TcpRxBuffer:Extract(0x55ab61ef96e0, 48300)
+2.000000000s 17 TcpRxBuffer:Extract(): Requested to extract 0 bytes from TcpRxBuffer of size=0
+2.000000000s 17  [node 17] TcpSocketBase:Recv(): m_rxBuffer->size() = 0
+2.000000000s 17  [node 17] TcpSocketBase:Recv(): m_rxBuffer->Available() = 0
+2.000000000s 17  [node 17] TcpSocketBase:RecvFrom(): packet == nullptr
+2.000000000s 17 TCPserver:HandleRead(): quit the function...
+2.000000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): maxIteration 350

Check 2:
+2.000009441s 17  [node 17] TcpSocketBase:ForwardUp(): Socket 0x55ab61f22ec0 forward up 10.1.1.1:1234 to 10.2.1.2:11362
+2.000009441s 17  [node 17] TcpSocketBase:DoForwardUp(): 0x55ab61f22ec0
+2.000009441s 17  [node 17] TcpSocketBase:ProcessOptionTimestamp(0x55ab61f22ec0, 0x55ab6207fc70)
+2.000009441s 17  [node 17] TcpSocketBase:ProcessOptionTimestamp(): 17 Got timestamp=2000 and Echo=1000
+2.000009441s 17  [node 17] TcpSocketBase:UpdateWindowSize(0x55ab61f22ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:UpdateWindowSize(): Received (scaled) window is 131072 bytes
+2.000009441s 17  [node 17] TcpSocketBase:UpdateWindowSize(): updating rWnd to 131072
+2.000009441s 17  [node 17] TcpSocketBase:ProcessEstablished(0x55ab61f22ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedAck(0x55ab61f22ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReadOptions(0x55ab61f22ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedAck(): Update bytes in flight before processing the ACK.
+2.000009441s 17  [node 17] TcpSocketBase:BytesInFlight(): Returning calculated bytesInFlight: 0
+2.000009441s 17  [node 17] TcpSocketBase:ProcessAck(0x55ab61f22ec0, 1, 0)
+2.000009441s 17  [node 17] TcpSocketBase:ProcessAck(): ACK of 1 SND.UNA=1 SND.NXT=1 in state: CA_OPEN with m_recover: 0
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(0x55ab61f22ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Data segment, seq=1 pkt size=536
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Expected sequence number: 1
+2.000009441s 17 TcpRxBuffer:Add(0x55ab61f23860, 0x55ab6209cb50, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17 TcpRxBuffer:Add(): Add pkt 0x55ab6209cb50 len=536 seq=1, when NextRxSeq=1, buffsize=0
+2.000009441s 17 TcpRxBuffer:Add(): Buffered packet of seqno=1 len=536
+2.000009441s 17 TcpRxBuffer:ClearSackList(0x55ab61f23860, 537)
+2.000009441s 17 TcpRxBuffer:Add(): Updated buffer occupancy=536 nextRxSeq=537
+2.000009441s 17  [node 17] TcpSocketBase:GetSockName(0x55ab61f22ec0)
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Local IP: 10.2.1.2, Local Port: 11362
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): expectedSeq: 1
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): m_tcb->m_rxBuffer->NextRxSequence(): 537
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): m_tcb->m_rxBuffer->Available (): 536
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): NotifyDataRecv() 
+2.000009441s 17  [node 17] TcpSocketBase:SendEmptyPacket(0x55ab61f22ec0, 4098)
+2.000009441s 17  [node 17] TcpSocketBase:AddOptions(0x55ab61f22ec0, 11362 > 1234 [ACK] Seq=1 Ack=537 Win=65535)
+2.000009441s 17  [node 17] TcpSocketBase:AddOptionTimestamp(0x55ab61f22ec0, 11362 > 1234 [ACK] Seq=1 Ack=537 Win=65535)
+2.000009441s 17  [node 17] TcpSocketBase:AddOptionTimestamp(): 17 Add option TS, ts=2000 echo=2000
+2.000009441s 17  [node 17] TcpSocketBase:AdvertisedWindowSize(0x55ab61f22ec0, 1)
+2.000009441s 17  [node 17] TcpSocketBase:AdvertisedWindowSize(): Returning AdvertisedWindowSize of 32634
+2.000009441s 17 TcpRxBuffer:GetSackListSize(0x55ab61f23860)
+2.000009441s 17  [node 17] TcpSocketBase:SendEmptyPacket(): Sending a pure ACK, acking seq 537
+2.000009441s 17  [node 17] TcpSocketBase:SendPendingData(0x55ab61f22ec0, 1)


Check 1:
+2.010000000s 17 TCPserver:HandleRead(): 0x559fb3f9f380 Entering the HandleRead function ..., with Ipv4 address 10.1.1.1
+2.010000000s 17 TCPserver:HandleRead(): 0x559fb3f9f380 Printing all members of compQueue with size: 0
+2.010000000s 17 TCPserver:HandleRead(): PrintSocketInfo(socket)...
+2.010000000s 17  [node 17] TcpSocketBase:GetSockName(0x559fb3f9fba0)
+2.010000000s 17 TCPserver:PrintSocketInfo(): Socket created with IP address: 10.2.1.2 and port: 11362
+2.010000000s 17  [node 17] TcpSocketBase:GetSockName(0x559fb3f9fba0)
+2.010000000s 17 TCPserver:HandleRead(): Local IP: 10.2.1.2, Local Port: 11362

+2.010000000s 17  [node 17] TcpSocketBase:RecvFrom(0x559fb3f9fba0, 48300, 4098)
+2.010000000s 17  [node 17] TcpSocketBase:Recv(0x559fb3f9fba0)
+2.010000000s 17 TcpRxBuffer:Extract(0x559fb3fa06e0, 48300)
+2.010000000s 17 TcpRxBuffer:Extract(): Requested to extract 0 bytes from TcpRxBuffer of size=0
+2.010000000s 17  [node 17] TcpSocketBase:Recv(): m_rxBuffer->size() = 0
+2.010000000s 17  [node 17] TcpSocketBase:Recv(): m_rxBuffer->Available() = 0
+2.010000000s 17  [node 17] TcpSocketBase:RecvFrom(): packet == nullptr
+2.010000000s 17 TCPserver:HandleRead(): quit the function...
+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): maxIteration 350

Check 2:
+1.000003216s 17  [node 17] TcpSocketBase:ForwardUp(): Socket 0x559fb3fc9ec0 forward up 10.1.1.1:1234 to 10.2.1.2:11362
+1.000003216s 17  [node 17] TcpSocketBase:DoForwardUp(): 0x559fb3fc9ec0
+1.000003216s 17  [node 17] TcpSocketBase:ProcessOptionTimestamp(0x559fb3fc9ec0, 0x559fb4001c20)
+1.000003216s 17  [node 17] TcpSocketBase:ProcessOptionTimestamp(): 17 Got timestamp=1000 and Echo=1000
+1.000003216s 17  [node 17] TcpSocketBase:EstimateRtt(): TcpSocketBase::EstimateRtt - RTT calculated from TcpOption::TS is zero, approximating to 1us.
+1.000003216s 17  [node 17] TcpSocketBase:EstimateRtt(): 0x559fb3fc9ec0+1000ns+1000ns
+1.000003216s 17  [node 17] TcpSocketBase:UpdateWindowSize(0x559fb3fc9ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(1000;1000) ns3::TcpOptionEnd(EOL))
+1.000003216s 17  [node 17] TcpSocketBase:UpdateWindowSize(): Received (scaled) window is 131072 bytes
+1.000003216s 17  [node 17] TcpSocketBase:UpdateWindowSize(): State less than ESTABLISHED; updating rWnd to 131072
+1.000003216s 17  [node 17] TcpSocketBase:ProcessSynRcvd(0x559fb3fc9ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(1000;1000) ns3::TcpOptionEnd(EOL))
+1.000003216s 17  [node 17] TcpSocketBase:ProcessSynRcvd(): SYN_RCVD -> ESTABLISHED
+1.000003216s 17  [node 17] TcpSocketBase:ReceivedAck(0x559fb3fc9ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(1000;1000) ns3::TcpOptionEnd(EOL))
+1.000003216s 17  [node 17] TcpSocketBase:ReadOptions(0x559fb3fc9ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(1000;1000) ns3::TcpOptionEnd(EOL))
+1.000003216s 17  [node 17] TcpSocketBase:ReceivedAck(): Update bytes in flight before processing the ACK.
+1.000003216s 17  [node 17] TcpSocketBase:BytesInFlight(): Returning calculated bytesInFlight: 0
+1.000003216s 17  [node 17] TcpSocketBase:ProcessAck(0x559fb3fc9ec0, 1, 0)
+1.000003216s 17  [node 17] TcpSocketBase:ProcessAck(): ACK of 1 SND.UNA=1 SND.NXT=1 in state: CA_OPEN with m_recover: 0
+1.000003216s 17  [node 17] TcpSocketBase:SendPendingData(0x559fb3fc9ec0, 1)
+1.000003216s 17  [node 17] TcpSocketBase:UpdatePacingRate(0x559fb3fc9ec0, 0x559fb3fca650)
+1.000003216s 17  [node 17] TcpSocketBase:GetTxAvailable(0x559fb3fc9ec0)
+1.000003216s 17  [node 17] TcpSocketBase:GetTxAvailable(0x559fb3fc9ec0)


CreatesocketPool:
+1.000000000s 17 TCPserver:PrintSocketInfo(): Socket created with IP address: 10.2.1.2 and port: 11362
+1.000000000s 17 TCPserver:CheckSocketState(): 0x564b15b4b380
+1.000000000s 17 TCPserver:CheckSocketState(): Socket state: LISTEN
+1.000000000s 17 TCPserver:CheckSocketState(): Socket state is LISTEN
+1.000000000s 17  [node 17] TcpSocketBase:GetSockName(0x564b15b4bba0)
+1.000000000s 17  [node 17] TcpSocketBase:GetPeerName(0x564b15b4bba0, )
+1.000000000s 17 TCPserver:LogSocketInfo(): Local IP: 10.2.1.2, Local Port: 11362
+1.000000000s 17 TCPserver:LogSocketInfo(): Peer IP: 0.0.0.0, Peer Port: 0
+1.000000000s 17  [node 17] TcpSocketBase:GetTxAvailable(0x564b15b4bba0)
myserver ptr:0x564b15b4b380
+1.000000000s 17 InnetworkAggregationInterface:CreateSocketPool(): 0x564b15b11620 CreateSocketPool: socketPool[10.1.1.1] -myserver, port: 11362

Check 1:
+2.010000000s 17 TCPserver:HandleRead(): 0x564b15b4b380 Entering the HandleRead function ..., with Ipv4 address 10.1.1.1
+2.010000000s 17 TCPserver:HandleRead(): 0x564b15b4b380 Printing all members of compQueue with size: 0
+2.010000000s 17 TCPserver:HandleRead(): PrintSocketInfo(socket)...
+2.010000000s 17  [node 17] TcpSocketBase:GetSockName(0x564b15b4bba0)
+2.010000000s 17 TCPserver:PrintSocketInfo(): Socket created with IP address: 10.2.1.2 and port: 11362
+2.010000000s 17  [node 17] TcpSocketBase:GetSockName(0x564b15b4bba0)
+2.010000000s 17 TCPserver:HandleRead(): Local IP: 10.2.1.2, Local Port: 11362

+2.010000000s 17  [node 17] TcpSocketBase:RecvFrom(0x564b15b4bba0, 48300, 4098)
+2.010000000s 17  [node 17] TcpSocketBase:Recv(0x564b15b4bba0)
+2.010000000s 17 TcpRxBuffer:Extract(0x564b15b4c6e0, 48300)

Check 2:
+2.000009441s 17  [node 17] TcpSocketBase:ForwardUp(): Socket 0x564b15b75ec0 forward up 10.1.1.1:1234 to 10.2.1.2:11362
+2.000009441s 17  [node 17] TcpSocketBase:DoForwardUp(): 0x564b15b75ec0
+2.000009441s 17  [node 17] TcpSocketBase:ProcessOptionTimestamp(0x564b15b75ec0, 0x564b15cd2c70)
+2.000009441s 17  [node 17] TcpSocketBase:ProcessOptionTimestamp(): 17 Got timestamp=2000 and Echo=1000
+2.000009441s 17  [node 17] TcpSocketBase:UpdateWindowSize(0x564b15b75ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:UpdateWindowSize(): Received (scaled) window is 131072 bytes
+2.000009441s 17  [node 17] TcpSocketBase:UpdateWindowSize(): updating rWnd to 131072
+2.000009441s 17  [node 17] TcpSocketBase:ProcessEstablished(0x564b15b75ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedAck(0x564b15b75ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReadOptions(0x564b15b75ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedAck(): Update bytes in flight before processing the ACK.
+2.000009441s 17  [node 17] TcpSocketBase:BytesInFlight(): Returning calculated bytesInFlight: 0
+2.000009441s 17  [node 17] TcpSocketBase:ProcessAck(0x564b15b75ec0, 1, 0)
+2.000009441s 17  [node 17] TcpSocketBase:ProcessAck(): ACK of 1 SND.UNA=1 SND.NXT=1 in state: CA_OPEN with m_recover: 0
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(0x564b15b75ec0, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Data segment, seq=1 pkt size=536
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Expected sequence number: 1
+2.000009441s 17 TcpRxBuffer:Add(0x564b15b76860, 0x564b15cefb50, 1234 > 11362 [ACK] Seq=1 Ack=1 Win=32768 ns3::TcpOptionTS(2000;1000) ns3::TcpOptionEnd(EOL))
+2.000009441s 17 TcpRxBuffer:Add(): Add pkt 0x564b15cefb50 len=536 seq=1, when NextRxSeq=1, buffsize=0
+2.000009441s 17 TcpRxBuffer:Add(): Buffered packet of seqno=1 len=536
+2.000009441s 17 TcpRxBuffer:ClearSackList(0x564b15b76860, 537)
+2.000009441s 17 TcpRxBuffer:Add(): Updated buffer occupancy=536 nextRxSeq=537
+2.000009441s 17  [node 17] TcpSocketBase:GetSockName(0x564b15b75ec0)
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): Local IP: 10.2.1.2, Local Port: 11362
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): s1
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): m_tcb->m_rxBuffer->NextRxSequence(): 537
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): m_tcb->m_rxBuffer->Available (): 536
+2.000009441s 17  [node 17] TcpSocketBase:ReceivedData(): NotifyDataRecv() 
+2.000009441s 17  [node 17] TcpSocketBase:SendEmptyPacket(0x564b15b75ec0, 4098)
+2.000009441s 17  [node 17] TcpSocketBase:AddOptions(0x564b15b75ec0, 11362 > 1234 [ACK] Seq=1 Ack=537 Win=65535)
+2.000009441s 17  [node 17] TcpSocketBase:AddOptionTimestamp(0x564b15b75ec0, 11362 > 1234 [ACK] Seq=1 Ack=537 Win=65535)
+2.000009441s 17  [node 17] TcpSocketBase:AddOptionTimestamp(): 17 Add option TS, ts=2000 echo=2000
+2.000009441s 17  [node 17] TcpSocketBase:AdvertisedWindowSize(0x564b15b75ec0, 1)
+2.000009441s 17  [node 17] TcpSocketBase:AdvertisedWindowSize(): Returning AdvertisedWindowSize of 32634
+2.000009441s 17 TcpRxBuffer:GetSackListSize(0x564b15b76860)
+2.000009441s 17  [node 17] TcpSocketBase:SendEmptyPacket(): Sending a pure ACK, acking seq 537
+2.000009441s 17  [node 17] TcpSocketBase:SendPendingData(0x564b15b75ec0, 1)



Mark:

line 918:

+1.000000000s 17 InnetworkAggregationInterface:CreateSocketPool(): ******cGroup[0]*********
+1.000000000s 17 InnetworkAggregationInterface:CreateSocketPool(): serverPort: 11362
+1.000000000s 17 TCPserver:TCPserver(0x564b15b4b380)
+1.000000000s 17 TCPserver:SetNode(0x564b15b4b380, 0x564b15963b40)
+1.000000000s 17 TCPserver:Bind(0x564b15b4b380)
+1.000000000s 17 TCPserver:Bind(): 0x564b15b4b380 m_socket is nullptr
+1.000000000s 17 TcpSocketBase:TcpSocketBase(0x564b15b4bba0)
+1.000000000s 17 TcpSocketBase:SetMinRto(0x564b15b4bba0, +1e+09ns)
+1.000000000s 17 TcpSocketBase:SetClockGranularity(0x564b15b4bba0, +1e+06ns)
+1.000000000s 17 TcpSocketBase:SetUseEcn(0x564b15b4bba0, 0)
+1.000000000s 17 TcpSocketBase:SetSndBufSize(0x564b15b4bba0, 131072)
+1.000000000s 17 TcpSocketBase:SetRcvBufSize(0x564b15b4bba0, 131072)
+1.000000000s 17 TcpSocketBase:SetSegSize(0x564b15b4bba0, 536)
+1.000000000s 17 TcpSocketBase:SetConnTimeout(0x564b15b4bba0, +3e+09ns)
+1.000000000s 17 TcpSocketBase:SetSynRetries(0x564b15b4bba0, 6)
+1.000000000s 17 TcpSocketBase:SetDataRetries(0x564b15b4bba0, 6)
+1.000000000s 17 TcpSocketBase:SetDelAckTimeout(0x564b15b4bba0, +2e+08ns)
+1.000000000s 17 TcpSocketBase:SetDelAckMaxCount(0x564b15b4bba0, 2)
+1.000000000s 17 TcpSocketBase:SetTcpNoDelay(0x564b15b4bba0, 1)
+1.000000000s 17 TcpSocketBase:SetPersistTimeout(0x564b15b4bba0, +6e+09ns)
+1.000000000s 17  [node 17] TcpSocketBase:SetCongestionControlAlgorithm(0x564b15b4bba0, 0x564b15b4d670)
+1.000000000s 17  [node 17] TcpSocketBase:SetRecoveryAlgorithm(0x564b15b4bba0, 0x564b15b4d590)
+1.000000000s 17  [node 17] TcpSocketBase:Bind(0x564b15b4bba0, 04-06-0a:02:01:02:62:2c)
+1.000000000s 17  [node 17] TcpSocketBase:Bind(): TcpSocketBase 0x564b15b4bba0 got an endpoint: 0x564b15b4d5e0
+1.000000000s 17  [node 17] TcpSocketBase:SetupCallback(0x564b15b4bba0)
+1.000000000s 17  [node 17] TcpSocketBase:Listen(0x564b15b4bba0)
+1.000000000s 17  [node 17] TcpSocketBase:Listen(): CLOSED -> LISTEN
+1.000000000s 17 TCPserver:Bind(): after the binding operation ...
+1.000000000s 17  [node 17] TcpSocketBase:GetSockName(0x564b15b4bba0)
+1.000000000s 17 TCPserver:PrintSocketInfo(): Socket created with IP address: 10.2.1.2 and port: 11362
+1.000000000s 17 TCPserver:CheckSocketState(): 0x564b15b4b380
+1.000000000s 17 TCPserver:CheckSocketState(): Socket state: LISTEN
+1.000000000s 17 TCPserver:CheckSocketState(): Socket state is LISTEN
+1.000000000s 17  [node 17] TcpSocketBase:GetSockName(0x564b15b4bba0)
+1.000000000s 17  [node 17] TcpSocketBase:GetPeerName(0x564b15b4bba0, )
+1.000000000s 17 TCPserver:LogSocketInfo(): Local IP: 10.2.1.2, Local Port: 11362
+1.000000000s 17 TCPserver:LogSocketInfo(): Peer IP: 0.0.0.0, Peer Port: 0
+1.000000000s 17  [node 17] TcpSocketBase:GetTxAvailable(0x564b15b4bba0)
myserver ptr:0x564b15b4b380
+1.000000000s 17 InnetworkAggregationInterface:CreateSocketPool(): 0x564b15b11620 CreateSocketPool: socketPool[10.1.1.1] -myserver, port: 11362
+1.000000000s 17 InnetworkAggregationInterface:CreateSocketPool(): ---------End-----------


check log: +2.002161761s 18 TCPserver:HandleRead(): 0x55a732139cb0 No memory available for new chunk

Todo: print the table of the info.


line 119656. +2.002157041s 17 TCPserver:HandleRead(): 0x564808aa0380 No memory available for new chunk

+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.5.1 current successIter.size(): 30

+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): isEnd false
+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.5.1 ,maxIteration 350
+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.4.1 ,maxIteration 350
+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.3.1 ,maxIteration 350
+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.2.1 ,maxIteration 350
+2.010000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.1.1 ,maxIteration 350
+2.010000000s 18 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.9.1 ,maxIteration 350
+2.010000000s 18 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.8.1 ,maxIteration 350
+2.010000000s 18 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.7.1 ,maxIteration 350
+2.010000000s 18 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.6.1 ,maxIteration 350
+2.010000000s 18 InnetworkAggregationInterface:ReceiveDataFrom(): fromStr: 10.1.10.1, Size of iterQueue: 30



猜想：rxBuffer不一样了，和之前的TCPsocketbase变了一样道理，按道理应该这样。，
验证：
socketbase地址2:
+1.000003216s 17  [node 17] TcpSocketBase:ForwardUp(): Socket 0x55870edfaba0 forward up 10.1.1.1:1234 to 10.2.1.2:11362
TcpRxBuffer id: 0x55870edfb6e0

+2.010000000s 17  [node 17] TcpSocketBase:RecvFrom(0x55870edfaba0, 48300, 4098)
+2.010000000s 17  [node 17] TcpSocketBase:Recv(0x55870edfaba0)
+2.010000000s 17  [node 17] TcpSocketBase:Recv(): m_rxBuffer->size() = 130784
+2.010000000s 17  [node 17] TcpSocketBase:Recv(): m_rxBuffer->Available() = 130784
+2.010000000s 17 TcpRxBuffer:Extract(0x55870edfb6e0, 48300)

这里为什么可以一下子读取48300个数据？
问题所在：除了10.1.5.1 以外，其它孩子节点并没有调用send函数

存在问题，明明send success了192轮的数据包，但wireshark里面显示只发送了111个interest。目前分析来看，问题应该是TxBuffer里面的包没有被发出去。
Todo:输出TxBuffer里面的情况，查看里面剩下了什么东西？
总结：目前对TCP的了解程度还不够深入，有很多细节上存在的问题还不知道，应该去阅读一下RFC文档。


异常现象：
forward up 10.1.11.1:17197 to 10.2.11.2:1234
出现了未授权的ip地址10.2.11.2
TCPclient创建有问题

+1.000000000s 18 TCPclient:LogSocketInfo(): Local IP: 10.2.12.2, Local Port: 1234
+1.000000000s 18 TCPclient:LogSocketInfo(): Peer IP: 10.1.11.1, Peer Port: 4476
client ptr:0x55870ee071d0
+1.000000000s 18 InnetworkAggregationInterface:CreateSocketPool(): 0x55870edc27c0 CreateSocketPool: socketPool[10.1.11.1] -myclient, port: 1234
+1.000000000s 18 InnetworkAggregationInterface:CreateSocketPool(): local ip: 10.2.2.2
+1.000000000s 18 InnetworkAggregationInterface:CreateSocketPool(): ---------End-----------

Todo:
在tcpsocketbase下面写获取tcprxbuffer的接口函数，

txBuffer发送时的异常现象：
170次输出
+2.006000000s 1 InnetworkAggregationInterface:SendPacket(): Print the TxBuffer of 10.1.1.1 when send success
Sent list: {[143291;144901|1610][+2.00475s]}{[144901;146511|1610][+2.00475s]}{[146511;148121|1610][+2.00488s]}{[148121;149731|1610][+2.00488s]}{[149731;151341|1610][+2.00502s]}{[151341;152951|1610][+2.00502s]}{[152951;154561|1610][+2.00515s]}{[154561;156171|1610][+2.00515s]}{[156171;157781|1610][+2.00529s]}{[157781;159391|1610][+2.00529s]}{[159391;161001|1610][+2.00534s]}{[161001;162611|1610][+2.00534s]}{[162611;164221|1610][+2.00556s]}{[164221;165831|1610][+2.00556s]}{[165831;167441|1610][+2.00569s]}{[167441;169051|1610][+2.00569s]}{[169051;170661|1610][+2.00573s]}{[170661;172271|1610][+2.00573s]}{[172271;173881|1610][+2.00573s]}{[173881;175491|1610][+2.00591s]}{[175491;177101|1610][+2.00591s]}, size = 21 Total size: 130410 m_firstByteSeq = 143291 m_sentSize = 33810 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0

171次输出
+2.007000000s 1 InnetworkAggregationInterface:SendPacket(): Print the TxBuffer of 10.1.1.1 when send success
Sent list: {[169051;170661|1610][+2.00573s]}{[170661;172271|1610][+2.00573s]}{[172271;173881|1610][+2.00573s]}{[173881;175491|1610][+2.00591s]}{[175491;177101|1610][+2.00591s]}{[177101;178711|1610][+2.00604s]}, size = 6 Total size: 106260 m_firstByteSeq = 169051 m_sentSize = 9660 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0

m_sentSize减少意味着为什么？什么时候m_sentSize才会减少？是意味着收到了ACK吗？

187次输出
+2.008000000s 1 InnetworkAggregationInterface:SendPacket(): Print the TxBuffer of 10.1.1.1 when send success
Sent list: , size = 0 Total size: 122360 m_firstByteSeq = 178711 m_sentSize = 0 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
这时，即使有很多的appSize，但是都没有推出去，原因是什么？为什么这时候sentlist为0，但不工作了。什么时候才会把appSize里面的东西推出去？？？？

下一步：打开send fail的日志，观察发生了什么事情，目前send fail的日志还没有被打开。

疑问：貌似handleread没有被触发，是什么问题？

问题定位清楚了：
在后面卡死的状态如下：

TcpTxBuffer: m_appList有数据，m_sendList没有数据，已经全部发送了
m_appList的数据没有推到m_sendList里面。

TcpRxBuffer:
+2.030000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): checking the RxBuffer content for debug...
+2.030000000s 17 InnetworkAggregationInterface:ReceiveDataFrom(): +2.030000000s 17 TcpRxBuffer:PrintRxBuffer(): 0x55b56f246880 RxBuffer: nextRxSeq=178711 size=0 availBytes=0

RxBuffer到最后是清零的状态
Todo:
目前主要存在的问题，为什么m_appList的数据没有推到m_sendList里面？查看一下RFC文档，看一下文档里面的说明情况？
有必要的话，手动将m_appList里面的数据push到sendList里面。
同时，查看一下m_appList的大小，它的最大容量是128k吗？
Quic里面也是遇到了同样的问题。

似乎是retransmit遇到了问题，
+2.007498721s 10  [node 10] TcpSocketBase:NewAck(): 0x55b56f23e290 Schedule ReTxTimeout at time 2.0075 to expire at time 3.0075
根据日志，好像重传的时间为1s。

以下设置存在问题:
src/internet/model/tcp-socket-base.cc
.AddAttribute(
                "MinRto",
                "Minimum retransmit timeout value",
                TimeValue(Seconds(1.0)), // RFC 6298 says min RTO=1 sec, but Linux uses 200ms.
                // See http://www.postel.org/pipermail/end2end-interest/2004-November/004402.html
                MakeTimeAccessor(&TcpSocketBase::SetMinRto, &TcpSocketBase::GetMinRto),
                MakeTimeChecker())

debug check:
1  [node 1] TcpSocketBase:SendDataPacket(): Send segment of size 1610

+2.006042017s 1  [node 1] TcpSocketBase:SendPendingData(): cWnd: 33810 total unAck: 32200 sent seq 177101 size 1610
+2.006042017s 1 TcpTxBuffer:BytesInFlight(): Sent size: 32200 leftOut: 0 retrans: 0
+2.006042017s 1  [node 1] TcpSocketBase:BytesInFlight(): Returning calculated bytesInFlight: 32200
+2.006042017s 1 TcpTxBuffer:NextSeg(0x55ccf5deabf0, 0)
+2.006042017s 1 TcpTxBuffer:SizeFromSequence(0x55ccf5deabf0, 178711)
+2.006042017s 1 TcpTxBuffer:NextSeg(): There is unsent data. Send it
+2.006042017s 1 TcpTxBuffer:SizeFromSequence(0x55ccf5deabf0, 178711)
+2.006042017s 1  [node 1] TcpSocketBase:SendPendingData(): availableData: 94990 availableWindow: 660 next: 178711 nextHigh: 179371 m_tcb->m_segmentSize: 1610
+2.006042017s 1  [node 1] TcpSocketBase:SendPendingData(): Preventing Silly Window Syndrome. Wait to send.
+2.006042017s 1  [node 1] TcpSocketBase:SendPendingData(): SendPendingData sent 1 segments

根据上述log，可知TcpSocket是想发送后续的data，但是此时剩余的cwnd大小已经小于单个m_segmentSize了。因此无法发送，此时程序被卡在了以下函数里面：
// Note that this function did not implement the PSH flag
uint32_t
TcpSocketBase::SendPendingData(bool withAck)
{

// Stop sending if we need to wait for a larger Tx window (prevent silly window
            // syndrome) but continue if we don't have data
            if (availableWindow < m_tcb->m_segmentSize && availableData > availableWindow)
            {
                NS_LOG_LOGIC("Preventing Silly Window Syndrome. Wait to send.");
                break; // No more
            }

目前测试结果：可能是Window的计算出了问题，查看一下两个变量的大小情况。
断点：
1  [node 1] TcpSocketBase:ForwardUp(): Socket 0x55ccf5f26590 forward up 10.2.1.2:11362 to 10.1.1.1:1234

找到问题所在：m_rWnd骤降至660，而这个数值小于1610，导致一个包都发送不出去了；而m_tcb->m_cWnd: 33810 的值一直都是33810.

目前进度：
似乎是DoForwardUp里面的header中的GetWindowsize 定义了发送端窗口的大小。这时候需要判断一下，这个窗口大小是怎么计算出来的？如果按照这个计算公式，当发送最后一个iteration(111轮的时候)，窗口大小应该会变得很大，因为这时候接受缓冲区已经空了。为什么到最后还是等于660？这个问题需要被解决。

具体查看一下sendACK的时候，windowsize是怎么被传进去的？如果这个问题解决了，感觉接下来的问题也会被解决。

void
TcpSocketBase::DoForwardUp(Ptr<Packet> packet, const Address& fromAddress, const Address& toAddress)
里面有一个UpdateWindowSize(tcpHeader)的函数，在这里m_rWnd被更新了。

定位aggregator收到10.1.1.1的包：17  [node 17] TcpSocketBase:ForwardUp(): Socket 0x564c70080d40 forward up 10.1.1.1:1234 to 10.2.1.2:11362

查看一下最后一个包是什么？是否可以发空包来告知producer窗口大小？



if (scale)
{   
    NS_LOG_DEBUG("m_rcvWindShift: " << static_cast<uint32_t>(m_rcvWindShift)
                    << " before shift, w is: " << w);
    w >>= m_rcvWindShift;
    NS_LOG_DEBUG("after shift, w is: " << w);
}

或许SendEmptyPacket这个函数可以帮助定期检查RxBuffer的状态。

参考一下void
TcpSocketBase::SetRcvBufSize(uint32_t size) 函数
里面有一个SendEmptyPacket(TcpHeader::ACK)，
说不定可以让应用层在完成的时候，触发一下这个操作。

Debug: 优化里面的一些输出。把一些没有必要的输出给取消了。
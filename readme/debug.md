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

---node1  ip---pro0---10.1.1.1---node2  ip---forwarder0---10.1.1.2
---node1  ip---pro1---10.1.2.1---node2  ip---forwarder0---10.1.1.2
---node1  ip---pro2---10.1.3.1---node2  ip---forwarder0---10.1.1.2
---node1  ip---pro3---10.1.4.1---node2  ip---forwarder0---10.1.1.2
---node1  ip---pro4---10.1.5.1---node2  ip---forwarder0---10.1.1.2
---node1  ip---pro5---10.1.6.1---node2  ip---forwarder1---10.1.6.2
---node1  ip---pro6---10.1.7.1---node2  ip---forwarder1---10.1.6.2
---node1  ip---pro7---10.1.8.1---node2  ip---forwarder1---10.1.6.2
---node1  ip---pro8---10.1.9.1---node2  ip---forwarder1---10.1.6.2
---node1  ip---pro9---10.1.10.1---node2  ip---forwarder1---10.1.6.2
---node1  ip---pro10---10.1.11.1---node2  ip---forwarder2---10.1.11.2
---node1  ip---pro11---10.1.12.1---node2  ip---forwarder2---10.1.11.2
---node1  ip---pro12---10.1.13.1---node2  ip---forwarder2---10.1.11.2
---node1  ip---pro13---10.1.14.1---node2  ip---forwarder2---10.1.11.2
---node1  ip---pro14---10.1.15.1---node2  ip---forwarder2---10.1.11.2
---node1  ip---pro15---10.1.16.1---node2  ip---forwarder3---10.1.16.2
---node1  ip---pro16---10.1.17.1---node2  ip---forwarder3---10.1.16.2
---node1  ip---pro17---10.1.18.1---node2  ip---forwarder3---10.1.16.2
---node1  ip---pro18---10.1.19.1---node2  ip---forwarder3---10.1.16.2
---node1  ip---pro19---10.1.20.1---node2  ip---forwarder3---10.1.16.2
---node1  ip---pro20---10.1.21.1---node2  ip---forwarder4---10.1.21.2
---node1  ip---pro21---10.1.22.1---node2  ip---forwarder4---10.1.21.2
---node1  ip---pro22---10.1.23.1---node2  ip---forwarder4---10.1.21.2
---node1  ip---pro23---10.1.24.1---node2  ip---forwarder4---10.1.21.2
---node1  ip---pro24---10.1.25.1---node2  ip---forwarder4---10.1.21.2
---node1  ip---pro25---10.1.26.1---node2  ip---forwarder5---10.1.26.2
---node1  ip---pro26---10.1.27.1---node2  ip---forwarder5---10.1.26.2
---node1  ip---pro27---10.1.28.1---node2  ip---forwarder5---10.1.26.2
---node1  ip---pro28---10.1.29.1---node2  ip---forwarder5---10.1.26.2
---node1  ip---pro29---10.1.30.1---node2  ip---forwarder5---10.1.26.2
---node1  ip---pro30---10.1.31.1---node2  ip---forwarder6---10.1.31.2
---node1  ip---pro31---10.1.32.1---node2  ip---forwarder6---10.1.31.2
---node1  ip---pro32---10.1.33.1---node2  ip---forwarder6---10.1.31.2
---node1  ip---pro33---10.1.34.1---node2  ip---forwarder6---10.1.31.2
---node1  ip---pro34---10.1.35.1---node2  ip---forwarder6---10.1.31.2
---node1  ip---pro35---10.1.36.1---node2  ip---forwarder7---10.1.36.2
---node1  ip---pro36---10.1.37.1---node2  ip---forwarder7---10.1.36.2
---node1  ip---pro37---10.1.38.1---node2  ip---forwarder7---10.1.36.2
---node1  ip---pro38---10.1.39.1---node2  ip---forwarder7---10.1.36.2
---node1  ip---pro39---10.1.40.1---node2  ip---forwarder7---10.1.36.2
---node1  ip---pro40---10.1.41.1---node2  ip---forwarder8---10.1.41.2
---node1  ip---pro41---10.1.42.1---node2  ip---forwarder8---10.1.41.2
---node1  ip---pro42---10.1.43.1---node2  ip---forwarder8---10.1.41.2
---node1  ip---pro43---10.1.44.1---node2  ip---forwarder8---10.1.41.2
---node1  ip---pro44---10.1.45.1---node2  ip---forwarder8---10.1.41.2
---node1  ip---pro45---10.1.46.1---node2  ip---forwarder9---10.1.46.2
---node1  ip---pro46---10.1.47.1---node2  ip---forwarder9---10.1.46.2
---node1  ip---pro47---10.1.48.1---node2  ip---forwarder9---10.1.46.2
---node1  ip---pro48---10.1.49.1---node2  ip---forwarder9---10.1.46.2
---node1  ip---pro49---10.1.50.1---node2  ip---forwarder9---10.1.46.2
---node1  ip---con0---10.1.51.1---node2  ip---forwarder13---10.1.51.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder0---10.1.1.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder1---10.1.6.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder2---10.1.11.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder3---10.1.16.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder4---10.1.21.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder5---10.1.26.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder6---10.1.31.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder7---10.1.36.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder8---10.1.41.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder9---10.1.46.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder13---10.1.51.2---node2  ip---forwarder10---10.1.52.2

trace: 75 Tcp
查看一下10.2.4.2端口的数据发包情况，貌似该端口的很早就已经满了。
0x56188023ba30 Entering the HandleRead function ..., with Ipv4 address 10.2.4.2

---node1  ip---forwarder10---10.1.52.2---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder10---10.1.52.2---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder11---10.2.166.1---node2  ip---agg14---10.2.15.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg0---10.2.1.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg1---10.2.2.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg2---10.2.3.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg3---10.2.4.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg4---10.2.5.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg5---10.2.6.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg6---10.2.7.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg7---10.2.8.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg8---10.2.9.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg9---10.2.10.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg10---10.2.11.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg11---10.2.12.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg12---10.2.13.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg13---10.2.14.2
---node1  ip---forwarder12---10.2.181.1---node2  ip---agg14---10.2.15.2

debug:
blocking status:
75 TCPserver:PrintTable(): iteration: 2893  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2891  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2890  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2889  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2888  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2887  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2886  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2885  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2884  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2883  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2882  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2881  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2880  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2879  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2878  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2877  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2876  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2875  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2874  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2873  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2872  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2871  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2870  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2869  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2868  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2867  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2866  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2865  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2864  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2863  collected child: 
10.2.2.2----10.2.10.2----

+3.740062416s 76 TCPserver:PrintTable(): iteration: 2892  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2891  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2890  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2889  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2888  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2887  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2886  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2885  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2884  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2883  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2882  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2881  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2880  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2879  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2878  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2877  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2876  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2875  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2874  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2873  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2872  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2871  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2870  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2869  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2868  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2867  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2866  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2865  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2864  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2863  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----

Todo: 把log精简一下，删除多余的log，看看这样是否可以输出更多。


Todo: 在卡住的地方把所有的Buffer和table都输出来，Block掉所有的日志信息，切换为std::cout模式。
目前卡在3.75s的话，就让所有的producer, aggregator, consumer 在3.75s的时候输出自己的信息
输出信息：
1. RxBuffer.
2. TxBuffer.
3. Application Table.

agg0--node 65 (m_sentList = End*1, appList = End*80, table = Empty)
RxBuffer = empty*5

agg1--node 66 (m_sentList = Empty, appList = 2975-2979 + End*76, table = 1799)
collected child: 
10.1.16.1----10.1.17.1----10.1.19.1----10.1.18.1----10.1.20.1----
RxBuffer = empty*5

agg2--node 67 (m_sentList = End*1, appList = End*80, table = Empty)
RxBuffer = empty*5

agg3--node 68 (m_sentList = End*1, appList = End*80, table = Empty)
RxBuffer = empty*5

agg4--node 69 (m_sentList = Empty*1, appList = 2975-2977 + End*78, table = 1799)
collected child: 
10.1.21.1----10.1.22.1----10.1.23.1----10.1.24.1----10.1.25.1----
RxBuffer = empty*5

agg5--node 70 (m_sentList = Empty*1, appList = 2975-2977 + End*78, table = 1799)
collected child:
10.1.31.1----10.1.32.1----10.1.33.1----10.1.35.1----10.1.34.1----
RxBuffer = empty*5

agg6--node 71 (m_sentList = Empty*1, appList = 2966-2968, 2998 + End*77, table = Empty)
RxBuffer = empty*5

agg7--node 72 (m_sentList = Empty*1, appList = 2975-2977 + End*78, table = 1799)
collected child:
10.1.6.1----10.1.7.1----10.1.8.1----10.1.9.1----10.1.10.1----
RxBuffer = empty*5

agg8--node 73 (m_sentList = End*1, appList = End*80, table = Empty)
RxBuffer = empty*5

agg9--node 74 (m_sentList = Empty, appList = 2975-2979 + End*76, table = 1799)
collected child: 
10.1.47.1----10.1.46.1----10.1.48.1----10.1.49.1----10.1.50.1----
RxBuffer = empty*5

agg10--node 75 (m_sentList = Empty, appList = Empty, table = ?)
RxBuffer---10.2.10.2---agg9: 2894-2974
RxBuffer---10.2.2.2---agg1: 2894-2974
other 3 RxBuffer is empty.

PrintTable:
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2968  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2908  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2893  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2891  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2890  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2889  collected child:  检查：貌似All children have collected the data for iteration 2889 in server IP: 10.2.4.2。但这样的话为什么这样agg10没有收到来自10.2.4.2的消息？查看一下send函数，查看发送了哪些消息？
10.2.2.2----10.2.10.2----
iteration: 2888  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2887  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2886  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2885  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2884  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2883  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2882  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2881  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2998  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2880  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2938  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2879  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2878  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2877  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2876  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2875  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2874  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2873  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2872  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2871  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2870  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2869  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2868  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2867  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2866  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2865  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2864  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2863  collected child: 
10.2.2.2----10.2.10.2----


agg11--node 76 (m_sentList = Empty, appList = Empty, table = ?)
RxBuffer---10.2.7.2---agg6: 2894-2965
RxBuffer---10.2.6.2---agg5: 2894-2974
RxBuffer---10.2.8.2---agg7: 2894-2974
RxBuffer---10.2.5.2---agg4: 2894-2974
RxBuffer---10.2.1.2---agg0: empty
other 3 RxBuffer is empty.

PrintTable(): iteration: 1799  collected child: 
10.2.1.2----
iteration: 2960  collected child: 
10.2.1.2----
iteration: 2900  collected child: 
10.2.1.2----
iteration: 2893  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2892  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2891  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2890  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2889  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2888  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2887  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2886  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2885  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2884  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2883  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2882  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2881  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2880  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2879  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2878  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2877  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2876  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2875  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2874  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2873  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2990  collected child: 
10.2.1.2----
iteration: 2872  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2930  collected child: 
10.2.1.2----
iteration: 2871  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2869  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2868  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2867  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2866  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2865  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2864  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----
iteration: 2863  collected child: 
10.2.5.2----10.2.8.2----10.2.6.2----10.2.7.2----

bug:
10.2.4.2 client->Send()--sentSize success: 1610 at iteration 2862
这个地方出现了断层，在2862以前，所有iteration成功发送都会显示10.2.4.2 client->Send()--sentSize success: 1610 at iteration 0-2862 （判断依据是搜索10.2.4.2 client->Send()--sentSize success: 1610 at iteration时，出现的词条数目为2863个）。

但是下一轮立刻跳转到了10.2.4.2 client->Send()--sentSize success: 1610 at iteration 2892。

是不是搞了什么操作，不处理表格里面的聚合操作了？
重点关注下面这一段的数据后续去了什么地方！感觉这里是突破口
iteration: 2878  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2877  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2876  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2875  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2874  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2873  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2872  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2871  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
iteration: 2870  collected child: 
10.1.36.1----10.1.38.1----10.1.39.1----10.1.42.1----10.1.43.1----
明明在 68 TCPserver:PrintTable()的表格里面这些数据都已经收齐完毕了，为什么

All children have collected the data for iteration 2907 in server IP: 10.2.4.2
compQueue: 2870-2871-2872-2873-2874-2875-2876-2877-2878-2879-2880-2881-2882-2883-2884-2885-2886-2887-2888-2889-2890-2891-2900-2901-2902-2903-2904-2905-2906-2907-
貌似这些iteration也有被收集到。那为什么不send这些收集好的数据呢？

log日志显示：
client->Send()--sentSize failed: -1 at iteration 2905Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2906Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2907Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2863Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2864Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2865Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2866Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 client->Send()--sentSize failed: -1 at iteration 2867Sent list: {[4960411;4962021|1610][+5.91581s]}, size = 1 Total size: 130410 m_firstByteSeq = 4960411 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0
 说明这些包一直发送失败，没有被发送出去。
 或者可以在发包前，把所有log权限打开，然后在send之后，再把log权限给关了，看看究竟发生了什么事情。
 12.22:
 可能的原因，某个iteration失败了之后，没有一直循环调用，貌似只重复了2-3次。按道理来说，程序不应该终止，应该一直重复调用send函数，才是正确的。可以检查一下是哪里出了问题。明天早上起来重点检查这个部分。
 存在的问题：为什么结束的末尾包会一直存放在sendList里面？

下一段的输出
.4.2
All children have collected the data for iteration 2901 in server IP: 10.2.4.2
All children have collected the data for iteration 2902 in server IP: 10.2.4.2
All children have collected the data for iteration 2903 in server IP: 10.2.4.2
All children have collected the data for iteration 2904 in server IP: 10.2.4.2
All children have collected the data for iteration 2905 in server IP: 10.2.4.2
All children have collected the data for iteration 2906 in server IP: 10.2.4.2
All children have collected the data for iteration 2907 in server IP: 10.2.4.2
All children have collected the data for iteration 2908 in server IP: 10.2.4.2
All children have collected the data for iteration 2909 in server IP: 10.2.4.2
All children have collected the data for iteration 2910 in server IP: 10.2.4.2
All children have collected the data for iteration 2911 in server IP: 10.2.4.2
All children have collected the data for iteration 2912 in server IP: 10.2.4.2
All children have collected the data for iteration 2913 in server IP: 10.2.4.2
All children have collected the data for iteration 2914 in server IP: 10.2.4.2
All children have collected the data for iteration 2915 in server IP: 10.2.4.2
All children have collected the data for iteration 2916 in server IP: 10.2.4.2
All children have collected the data for iteration 2917 in server IP: 10.2.4.2
All children have collected the data for iteration 2918 in server IP: 10.2.4.2
All children have collected the data for iteration 2919 in server IP: 10.2.4.2
All children have collected the data for iteration 2920 in server IP: 10.2.4.2
All children have collected the data for iteration 2921 in server IP: 10.2.4.2
All children have collected the data for iteration 2922 in server IP: 10.2.4.2
All children have collected the data for iteration 2923 in server IP: 10.2.4.2
All children have collected the data for iteration 2924 in server IP: 10.2.4.2
All children have collected the data for iteration 2925 in server IP: 10.2.4.2
All children have collected the data for iteration 2926 in server IP: 10.2.4.2
All children have collected the data for iteration 2927 in server IP: 10.2.4.2
All children have collected the data for iteration 2928 in server IP: 10.2.4.2
All children have collected the data for iteration 2929 in server IP: 10.2.4.2
All children have collected the data for iteration 2930 in server IP: 10.2.4.2
All children have collected the data for iteration 2931 in server IP: 10.2.4.2
All children have collected the data for iteration 2932 in server IP: 10.2.4.2
All children have collected the data for iteration 2933 in server IP: 10.2.4.2
All children have collected the data for iteration 2934 in server IP: 10.2.4.2
All children have collected the data for iteration 2935 in server IP: 10.2.4.2
All children have collected the data for iteration 2936 in server IP: 10.2.4.2
All children have collected the data for iteration 2937 in server IP: 10.2.4.2

直接跳过了中间部分，这是什么原因？


从下面的log看出，m_sentList[1]的包一直没有处理，原因大概率是cwnd出现了问题。于是，我们应该回溯查看于10.2.4.2相连的agg10，也就是10.2.11.2节点出现了什么问题。    

Trace the node information for 10.2.4.2
+3.289000000s 68 InnetworkAggregationInterface:PrintAllInfo(): 0x5628b6796b80 In PrintAllInfo. Local ip: 10.2.4.2
+3.289000000s 68 InnetworkAggregationInterface:PrintAllInfo(): Print TxBuffer for Peer ip: 10.2.11.2
+3.289000000s 68 TcpTxBuffer:PrintTxBuffer(): Print the content in the QuicSocketTxBuffer, m_sentList
m_sentList[1]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 02 02 0b 00 00 00 00 00 00 02 0b 00 00 00 00 00 00 02 0b 00 00 

Print the content in the QuicSocketTxBuffer, m_appList
m_appList[1]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 03 03 0b 00 00 00 00 00 00 03 0b 00 00 00 00 00 00 03 0b 00 00 
m_appList[2]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 04 04 0b 00 00 00 00 00 00 04 0b 00 00 00 00 00 00 04 0b 00 00 
m_appList[3]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 05 05 0b 00 00 00 00 00 00 05 0b 00 00 00 00 00 00 05 0b 00 00 
m_appList[4]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 06 06 0b 00 00 00 00 00 00 06 0b 00 00 00 00 00 00 06 0b 00 00 
m_appList[5]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 07 07 0b 00 00 00 00 00 00 07 0b 00 00 00 00 00 00 07 0b 00 00 
m_appList[6]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 08 08 0b 00 00 00 00 00 00 08 0b 00 00 00 00 00 00 08 0b 00 00 
m_appList[7]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 09 09 0b 00 00 00 00 00 00 09 0b 00 00 00 00 00 00 09 0b 00 00 
m_appList[8]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 0a 0a 0b 00 00 00 00 00 00 0a 0b 00 00 00 00 00 00 0a 0b 00 00 
m_appList[9]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 0b 0b 0b 00 00 00 00 00 00 0b 0b 00 00 00 00 00 00 0b 0b 00 00 
m_appList[10]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 0c 0c 0b 00 00 00 00 00 00 0c 0b 00 00 00 00 00 00 0c 0b 00 00 
m_appList[11]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 0d 0d 0b 00 00 00 00 00 00 0d 0b 00 00 00 00 00 00 0d 0b 00 00 
m_appList[12]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 0e 0e 0b 00 00 00 00 00 00 0e 0b 00 00 00 00 00 00 0e 0b 00 00 
m_appList[13]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 0f 0f 0b 00 00 00 00 00 00 0f 0b 00 00 00 00 00 00 0f 0b 00 00 
m_appList[14]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 10 10 0b 00 00 00 00 00 00 10 0b 00 00 00 00 00 00 10 0b 00 00 
m_appList[15]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 11 11 0b 00 00 00 00 00 00 11 0b 00 00 00 00 00 00 11 0b 00 00 
m_appList[16]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 12 12 0b 00 00 00 00 00 00 12 0b 00 00 00 00 00 00 12 0b 00 00 
m_appList[17]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 13 13 0b 00 00 00 00 00 00 13 0b 00 00 00 00 00 00 13 0b 00 00 
m_appList[18]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 14 14 0b 00 00 00 00 00 00 14 0b 00 00 00 00 00 00 14 0b 00 00 
m_appList[19]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 15 15 0b 00 00 00 00 00 00 15 0b 00 00 00 00 00 00 15 0b 00 00 
m_appList[20]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 16 16 0b 00 00 00 00 00 00 16 0b 00 00 00 00 00 00 16 0b 00 00 
m_appList[21]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 17 17 0b 00 00 00 00 00 00 17 0b 00 00 00 00 00 00 17 0b 00 00 
m_appList[22]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 18 18 0b 00 00 00 00 00 00 18 0b 00 00 00 00 00 00 18 0b 00 00 
m_appList[23]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 19 19 0b 00 00 00 00 00 00 19 0b 00 00 00 00 00 00 19 0b 00 00 
m_appList[24]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 1a 1a 0b 00 00 00 00 00 00 1a 0b 00 00 00 00 00 00 1a 0b 00 00 
m_appList[25]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 1b 1b 0b 00 00 00 00 00 00 1b 0b 00 00 00 00 00 00 1b 0b 00 00 
m_appList[26]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 1c 1c 0b 00 00 00 00 00 00 1c 0b 00 00 00 00 00 00 1c 0b 00 00 
m_appList[27]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 1d 1d 0b 00 00 00 00 00 00 1d 0b 00 00 00 00 00 00 1d 0b 00 00 
m_appList[28]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 1e 1e 0b 00 00 00 00 00 00 1e 0b 00 00 00 00 00 00 1e 0b 00 00 
m_appList[29]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 1f 1f 0b 00 00 00 00 00 00 1f 0b 00 00 00 00 00 00 1f 0b 00 00 
m_appList[30]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 20 20 0b 00 00 00 00 00 00 20 0b 00 00 00 00 00 00 20 0b 00 00 
m_appList[31]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 21 21 0b 00 00 00 00 00 00 21 0b 00 00 00 00 00 00 21 0b 00 00 
m_appList[32]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 22 22 0b 00 00 00 00 00 00 22 0b 00 00 00 00 00 00 22 0b 00 00 
m_appList[33]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 23 23 0b 00 00 00 00 00 00 23 0b 00 00 00 00 00 00 23 0b 00 00 
m_appList[34]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 24 24 0b 00 00 00 00 00 00 24 0b 00 00 00 00 00 00 24 0b 00 00 
m_appList[35]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 25 25 0b 00 00 00 00 00 00 25 0b 00 00 00 00 00 00 25 0b 00 00 
m_appList[36]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 26 26 0b 00 00 00 00 00 00 26 0b 00 00 00 00 00 00 26 0b 00 00 
m_appList[37]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 27 27 0b 00 00 00 00 00 00 27 0b 00 00 00 00 00 00 27 0b 00 00 
m_appList[38]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 28 28 0b 00 00 00 00 00 00 28 0b 00 00 00 00 00 00 28 0b 00 00 
m_appList[39]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 29 29 0b 00 00 00 00 00 00 29 0b 00 00 00 00 00 00 29 0b 00 00 
m_appList[40]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 2a 2a 0b 00 00 00 00 00 00 2a 0b 00 00 00 00 00 00 2a 0b 00 00 
m_appList[41]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 2b 2b 0b 00 00 00 00 00 00 2b 0b 00 00 00 00 00 00 2b 0b 00 00 
m_appList[42]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 2c 2c 0b 00 00 00 00 00 00 2c 0b 00 00 00 00 00 00 2c 0b 00 00 
m_appList[43]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 2d 2d 0b 00 00 00 00 00 00 2d 0b 00 00 00 00 00 00 2d 0b 00 00 
m_appList[44]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 2e 2e 0b 00 00 00 00 00 00 2e 0b 00 00 00 00 00 00 2e 0b 00 00 
m_appList[45]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 4c 4c 0b 00 00 00 00 00 00 4c 0b 00 00 00 00 00 00 4c 0b 00 00 
m_appList[46]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 5c 5c 0b 00 00 00 00 00 00 5c 0b 00 00 00 00 00 00 5c 0b 00 00 
m_appList[47]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 7a 7a 0b 00 00 00 00 00 00 7a 0b 00 00 00 00 00 00 7a 0b 00 00 
m_appList[48]:
Packet bytes: 03 03 03 03 03 03 03 03 0b 98 98 0b 00 00 00 00 00 00 98 0b 00 00 00 00 00 00 98 0b 00 00 
m_appList[49]:
Packet bytes: 03 03 03 03 03 03 03 03 0b b6 b6 0b 00 00 00 00 00 00 b6 0b 00 00 00 00 00 00 b6 0b 00 00 
m_appList[50]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[51]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[52]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[53]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[54]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[55]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[56]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[57]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[58]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[59]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[60]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[61]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[62]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[63]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[64]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[65]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[66]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[67]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[68]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[69]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[70]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[71]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[72]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[73]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[74]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[75]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[76]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[77]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[78]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[79]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 
m_appList[80]:
Packet bytes: 07 07 07 07 07 07 07 07 07 07 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 09 

74, 75是级别最高的aggregator，观察一下他们是什么原因被堵塞了。
目前来看，他们的sentList和appList都是空的。

+4.700000000s 75 TCPserver:PrintTable(): 
iteration: 2981  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2982  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2983  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2993  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2866  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2984  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2867  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2985  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2868  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2986  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2869  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2987  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2870  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2988  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2989  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2872  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2863  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2990  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2873  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2864  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2991  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2874  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2865  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2992  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2875  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2876  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2877  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2878  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2879  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2938  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2880  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2871  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2998  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2881  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2882  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2883  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2884  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2885  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2886  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2887  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2888  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2889  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2890  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2891  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2893  collected child: 
10.2.2.2----10.2.10.2----
iteration: 2908  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2968  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 1799  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2979  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2969  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2970  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2971  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2972  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2973  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2974  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2975  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2976  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2977  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2978  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
iteration: 2980  collected child: 
10.2.3.2----10.2.4.2----10.2.9.2----
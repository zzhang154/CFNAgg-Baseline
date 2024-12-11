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


Check 2:

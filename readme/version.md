Comment for v4.0
Quic-Agg-v4.0:
1. producer now push all the data in a "for" loop
2. Quic will treat this as a whole process. And the stream now will cut random within one iteration. This means that data can only be recovered after all streams finish their transmission.

Todo List:
1. Cut the stream number in Quic to 1 to see what happens
2. Alternatively, set a counter for each iteration, and mark for every 1500 B.

current configuration:

iteration: 150
producer: 10
aggregator: 2
consumer:1

All packet receive by 10.2.1.2, sender: 10.1.1.1

packet-1: [1499B+3b], [1499B+3b], [72B+6b]
***********START*********
3 3 3 3 3 3 3 3  the 1th byte
0 0 88 0 0 0 0 0  the 2th byte
......
0 0 88 0 0 0 0 0  the 1500th byte
0 0 88 0 0 3 3 3  the 1501th byte
3 3 3 3 3 0 1 99  the 1502th byte
0 0 0 0 0 0 0 99  the 1503th byte
......
0 0 0 0 0 0 0 99  the 3001th byte
0 0 3 3 3 3 3 3  the 3002th byte
3 3 0 2 110 0 0 0  the 3003th byte
0 0 0 0 110 0 0 0  the 3004th byte
......
0 0 0 0 110 0 0 0  the 3074th byte
0 0
***********END*********

packet-2:[0], [0], [6b]
***********START*********
0 0 110 0 0 0
***********END*********

packet-3:[0], [0], [1426B+3b]
***********START*********
0 0 0 0 110 0 0 0  the 1th byte
......
0 0 0 0 110 0 0 0  the 1426th byte
0 0 0 0 110 0 0
***********END*********

packet-4:[1500B+5b], [143B+4b], [0]
***********START*********
0 0 0 0 0 88 0 0  the 1th byte
......
0 0 0 0 0 88 0 0  the 1500th byte
0 0 0 0 0 0 0 0  the 1501th byte
0 0 99 0 0 0 0 0  the 1502th byte
......
0 0 99 0 0 0 0 0  the 1644th byte
0 0 99 0 0 0
***********END*********

packet-5:[0], [1356B+4b], [73B+1b]
***********START*********
0 0 0 0 99 0 0 0  the 1th byte
......
0 0 0 0 99 0 0 0  the 1356th byte
0 0 0 0 0 0 0 0  the 1357th byte
0 110 0 0 0 0 0 0  the 1358th byte
......
0 110 0 0 0 0 0 0  the 1430th byte
0 110
***********END*********

packet-6:[0], [0], [1426B+7b]
***********START*********
0 0 0 0 0 0 0 110  the 1th byte
......
0 0 0 0 0 0 0 110  the 1426th byte
0 0 0 0 0 0 0
***********END*********

Observation: when the data of one iteration reach the size of half of 24010bit = 1500B, it will automatically turn into the content of the next iteration.

ns3::QuicSubHeader (|STREAM010|
|Stream Id 1|
|Length 24594|
) Payload Fragment [0:12005] Payload Fragment [0:12005] Payload Fragment [0:584]

first stream
ns3::QuicSubHeader (|STREAM010|
|Stream Id 2|
|Length 13158|
) Payload Fragment [12005:24010] Payload Fragment [12005:13158]ns3::QuicSubHeader (|STREAM010|

+2.026890882s 17 QuicMyServer:HandleRead(): 0x564474169950 Copy data size: 6

ns3::QuicSubHeader (|STREAM110|
|Stream Id 2|
|Offset 12005|
|Length 574|
) Payload Fragment [12005:12579]ns3::QuicSubHeader (|STREAM110|

----------------------------
Quic-Agg-v4.1:
1. fix the bug for change the vsize from uint16_t to uint32_t, such that we can test for large data.

----------------------------
Quic-Agg-v4.1.1:
1. It seems that the chunkSize has wrongly set to uint16_t, which actually not send the 300,000 vector size at all. Now we fix that.

2. Just really be careful that all variable in the "parameters.cc" should change to uint32_t.


---------------------------
Quic-Agg-v5.0. ToDo List:
1. InnetworkAggregationInterface.cc
(1) Control the action of QuicMyServer. Check the status of iterChunkMap, if it is full, then stop sending the data.
(2) Alternatively, maintain the available space of iterChunkMap. It means that once the data of one iteration has been processed, it should triggered the send operation. But does this mean that the server should send packets to their children node as notification?
(3) call the handle read function when the one iteration finish.
(4) for handle read triggered automatically by QuicSocket, it should verify the iteration space threshold. Otherwise, just return directly without doing anything. 

2. QuicMyServer.cc:
(1) only call "packet = socket->RecvFrom (from)", when the iterChunkMap (the table) has available space. Let say constraint is 30 iterations for example.

check the discussion record with Xinjiao:
我感觉你想的和DK想的可能不是一个东西。关于agg满了以后让producer不要再发的这个东西。首先，是程序的缓存满，那就不readfrom；然后不读之后接收缓冲区会满，那这个就会让pro那边的rtt变大的，那边自动就会调整发包策略了。所以一般不会返回-1啊。。只要能放到send buffer里面，就能发，时间长短的问题。我们不就是想让时间变长吗。。这就是把数据丢给了quic，让他自适应的发啊。这不是DK一开始想要的么

3. Bug to be fixed.
wrongly send large data packet

(1) Now, only can run 117 iteration. Don't know why the left 30+ iteration data have not received? When both the SendBuf and RecBuf is INT32_MAX. Note that all the packets of 150 iterations have been sent successfully.

(2) When sendBuf both the SendBuf and RecBuf is pktlen * 10 or pktlen * 30. Only 8 iterations can run.

(3) SendBuf is INT32_MAX and RecBuf is pktlen * 30, only 107 iterations can run. After that, there are constantly small packets received by QuicMyServer, the sizes are as follows: 508, 1316, 1252, 572, 1284, 1316. It seems that these packets are not contained anything, maybe just some notification packet.

(4) Wrongly design the server side socket and buffer.

Quic-Agg-v5.0. Done

Bug lists.
config: producer-10
(1) after iteration 1523, only 10.1.1.1 and 10.1.6.1 can read from the receiving buffer.
 
the last iteration finish:
+2.183000000s 0 InnetworkAggregationInterface:PrintCompInfo(): IterationNum-1521, Consumer completed in: 2183ms

(2) +2.185839114s 17 QuicSocketRxBuffer:Extract(): Added packet of size 1184466
+2.185839114s 17 QuicSocketRxBuffer:Extract(): Added packet of size 1184466

+2.185839114s 17 QuicMyServer:HandleRead(): 0x561912a743d0 Copy data size: 1186326

location for the bug:
17 QuicMyServer:HandleRead(): Print data from 10.1.1.1 for iteration- 1522 group

Todo:
Control the send function of each byte sent.

Solution:
1. to solve this problem, use the flush send strategy. Not to wait.

LOG Breakpoint checkmessage:

+2.185839114s 17 QuicMyServer:HandleRead(): Print data from 10.1.1.1 for iteration- 1521 group
mark
client->Send()--sentSize: 1610 at iteration 1523
+2.000000001s 1 QuicStreamBase:SendPendingData
s 1 QuicStreamBase:SendPendingData()
problem breakpoint:
+2.186195354s 17 QuicMyServer:CheckChComp(): 0x56448d15b890 All children have collected the data for iteration 1523 in the m_bindIp: 10.1.5.1
check the break point of the receive 17 QuicStreamBase:SetStreamStateRecvIf(0x56448d205cd0)
2zQbEBctvrP.2dP

// Zhuoxu: we should modify this function so that it will not generate small frames.
Ptr<Packet>
QuicStreamTxBuffer::NextSequence (uint32_t numBytes, const SequenceNumber32 seq)
Zhuoxu: Solve this problem.

check this function to see what happens.


check the packet send size, detect error.
insert error message if the packet size exist some threshold.

Todo: 11.27
QuicSocketBase::AppendingRx (Ptr<Packet> frame, Address &address)
What happens if the appendix is rejected? In this way, we have to ask for retransmission.

Check how the reject is called...
Will the rejected packet also received the ACK? Just traced back to see what happen exactly...

Todo: 11.28
Ptr<Packet> QuicSocketTxBuffer::NextSequence (uint32_t numBytes,
                                              const SequenceNumber32 seq)

We can add an iteration item in the Ptr<QuicSocketTxItem>, to bind the packet number and iteration number together.


Todo: 11.29
1. SendBuffer 128k. INF is wrong.
2. Condition send, based on whether SendBuff has available space.
3. Check whether ACK has info of remained RecvBuff space.
4. If 3 is right, then check whether some var will change. See whether there is flow control function.

problem:

QuicSocketBase:SetReTxTimeout(): Schedule ReTxTimeout at time 2.04145 to expire at time 14.0829

a too high expire time.


feedback to the producer (from the aggregator):

+2.069697090s 9 QuicL5Protocol:DisgregateRecv(): subheader |ACK|
|Largest Acknowledged 313|
|Ack Delay 3657|
|Ack Block Count 1|
|First Ack Block 313|
|Gap 312|
|Additional Ack Block 293|
 dataSizeByte 12 remaining 0 frame size 0

 Here, it seems that packet 287 has been received....

 producer的所有350轮次都收到了，但是aggregator的处理有问题

 检查一下producer端在没有收到ACK时，究竟有没有触发重传？以及触发重传时的检查机制是怎么样的？是检查之前发送过的包是否都收到ACK还是怎么样？

 Ptr<QuicSocketTxItem> retx->m_retrans = true; 可以利用该表项判断是否是重传的包

 打印包：
 17 QuicStreamBase:Recv(): Print the content of the frame from 10.1.1.1 Packet，
 参考这个，看一下能否打印是否重传的信息。
 
 目前似乎重传信息只会存储在buffer里面，并不会体现在quic 头部里面。那么我们要做的应该是把重传的信息放在头部里面？

 ToDo: 在DoRetransmit函数里面，往头部里面添加一个重传的标志位，表明这是重传的包。
 疑问：按照现在的设计，是会一次性往buffer里面推很多包，直到buffer不能再推为止。

 这样就需要考虑以下的问题：发生重传的时候，是对面接受端的buffer满了，这时候后续的包到达了，也能处理，但是由于table也被占用着，是否会导致出现死锁问题。比如后续到达的包由于无法被table读入，导致其一直堆积在buffer里面，这个时候buffer也满了。而此时重传的包即使到了，也没有足够的空位去容纳。
 
 如何

 header is added in the quic-l4-protocol and we should also parse in the quic-l4-protocol to check whether it is a transmit packet. And output this information.

 Todo: We should check that we add the retransmit info into the quicheader or quicSubheader class.

 We find that in QuicSocketBase::SendDataPacket (SequenceNumber32 packetNumber,
                                uint32_t maxSize, bool withAck),
it creates 


Current problem: in the quic-socket-base.cc QuicSocketBase::SendDataPacket, we set the Retransmission flag correctly. But in the void QuicL4Protocol::ForwardUp (Ptr<Socket> sock), when we deserialize the result, it constantly get the same one. Maybe we should just directly change the application content in the packet to testify. See the print function and change the first 8 bytes in application content. Do this tomorrow.

12.3
在stream receive上，如果被drop掉了packet，会将abort的信号添加至quic-subheader里面。
+2.026669778s 17 QuicStreamBase:Recv(): Buffering unordered received frame - offset 420210, frame offset 629510
+2.026669778s 17 QuicStreamBase:Recv(): Dropping packet as it could not be inserted in RX buffer
+2.026669778s 17 QuicL5Protocol:SignalAbortConnection(0x5565ad155660)
+2.026669778s 17 QuicSocketBase:AbortConnection(0x5565ad151e40)
+2.026669778s 17 QuicSocketBase:AbortConnection(): Abort connection 0 because Aborting connection due to full RX buffer


12.3 Todo List:
1. 打印所有的Buffer状态

Markdown the error:
+2.000073594s 17 QuicSocketBase:AppendingRx(): Received AppendingRx of size 1610from--10.1.1.1
+2.000073594s 17 QuicSocketBase:AppendingRx(): Packet number: Packet bytes: 03 03 03 03 03 03 03 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

Even though I set the schedule function of the handleRead(), aggregator 17 still don't work after 260 iteration. It seems that aggregator 17 is blocked by something.
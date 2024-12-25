State in 10.2.11.2.

+3.740000000s 75 InnetworkAggregationInterface:PrintAllInfo(): Print RxBuffer for Peer ip: 10.2.4.2
+3.740000000s 75 InnetworkAggregationInterface:PrintAllInfo(): +3.740000000s 75 TcpRxBuffer:PrintRxBuffer(): 0x556aa3c644d0 RxBuffer: nextRxSeq=4611041 size=0 availBytes=0
+3.740000000s 75 TcpRxBuffer:PrintRxBuffer(): RxBuffer is empty

Comment: RxBuffer for 10.2.4.2 is empty.

+4.700000000s 75 TCPserver:PrintTable(): 
iteration: 1799  collected child: 
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

Conclusion:
Data of 10.2.4.2 doesn't send to 10.2.4.2, especially for iteration 2863 to 2907.
The empty RxBuffer in 10.2.11.2. eliminates the possibility that send operation in 10.2.4.2 fails because of the full RxBuffer. So there must be other reason. 

Check the send success log for 10.2.4.2

 client->Send()--sentSize failed: -1 at iteration 2863Sent list: {[4962021;4963631|1610][+5.92584s]}, size = 1 Total size: 130410 m_firstByteSeq = 4962021 m_sentSize = 1610 m_retransOut = 0 m_lostOut = 0 m_sackedOut = 0

 It constantly shows send error, check the cwnd of this time.


原因：

问题找到了。从2863轮开始，后面的轮次会先到达，会产生乱序的现象。
问题来了？为什么会乱序？

是不是应用层的table数据结构有问题
首先，application的table用的数据结构是队列，这个是一定要确定的，一定要用一个先进先出的队列。
其次，查看一下successIter的记录，这里要更改一下successIter的数据结构（目前好像是set，set是无法记录先后顺序）。应该将其修改为queue，然后观察一下乱序的情况，是从哪个节点开始第一次出现乱序的。以及，乱序的轮次第一次出现是什么时候？这些信息都应该被记录下来。

TCPserver里面的checkComp函数有问题，不能轮训地检测，只能是从第一个开始检测。否则就会乱序。


12.25改动：
1. 将unorder_map改成map
2. 将
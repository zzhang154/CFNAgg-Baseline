Comment for v4.0

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

Experiment Result:
producer-10, iteration-150
ns3::Simulator::Schedule(ns3::MilliSeconds(10), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.200000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 200ms

ns3::Simulator::Schedule(ns3::MilliSeconds(3), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.202000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 202ms

producer-50, iteration-150
ns3::Simulator::Schedule(ns3::MilliSeconds(5), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.215000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 215ms

ns3::Simulator::Schedule(ns3::MilliSeconds(10), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.210000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 210ms

ns3::Simulator::Schedule(ns3::MilliSeconds(3), &InnetworkAggregationInterface::ReceiveDataFromAll, this);
+2.193000000s 0 InnetworkAggregationInterface:ReceiveDataFromAll(): All iteration-150 completed in: 193ms


Todo List for 2025/1/7:
Add 2ms time after each aggregator complete the collection of all children. Only do this for aggregator, but not for producer. Producer can send the data ASAP.

Compelte log, in file "src/applications/model/TCPserver.cc".
ns3::Simulator::Schedule(ns3::MilliSeconds(2), &TCPserver::DoChComp, this, iterNum);

Todo List for 2025/1/8:
Changes should be done in the TCPserver.c file.

Todo List for 2025/1/9:
TraceValue for the following content.
1. RTT for send data of each iteration, both application layer and transport layer.

supplement(2025/1/21)
(1) sample should be cut lower. otherwise, the size of the log would be 1.1GB. (√)
(2) iteration should be larger than 10k, to see what's happening.
(3) only record some of the node, i.e., one node for each layer. To accelerate the process. We don't need to print out the output of all the nodes.
?? IP address seems to be not consist with the initial output.
(4) should point out when does the node finish the last ACK. otherwise, it seems that the x-axis of the time constantly grows.
(5) also see the RTT of consumer side, i.e., ACK packet of consumer, not neccessarily all data packets larger than 1210.

2. Throught put of consumer, both app layer and transport layer.
(1) app layer means the timestamp when the data is successfully sent by the TCPserver and received by the TCPclient. Both side should be coincide.
(2) Similarly, for transport layer, we should record the timestamp when the packet is removed from the TxBuffer and be received by the RxBuffer. This should be distinguished.
(3) We use a sliding window to record the timestamp of the recent received packet, with 'iteration' as the meta-data. The current sliding window is 20ms.
(4) The throughput measurement should be implemented in the TCPserver class.(Quic should also add this function.)



The above should also be done in 2025/01/10.

Pending to be solved (2025/1/10):
1. Hungary algorithm cannot solve the current problem. The objective function should be re-defined.
2. Objection function: minimize the cost.
(1) individual cost --- within the cluster, only calculate the distance of nodes within the cluster.
(2) total cost --- sum of all cost of individual cluster.
3. Game theory solution
Use game theory to abtain the optimal solution, which can lower the complexity.
4. Distance calculation
use a proximal graph distance query approach (KDD'13), instead of using O(n^3) to calculate all pairs distance.

Bug for unable to use utils.h (2025/1/14):
1. Reason:
Forget to add the utils.h and utils.cc files into the CMakeLists.txt. Function can be called after this operation is done.
Path  
/home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/src/innetwork-task/CMakeLists.txt


Bug for log (2025/1/14), to be fixed on 2025/1/15 (√).
" current node con0
+0.000000000s -1 Consumer:Consumer(0x564091641230)
+0.000000000s -1 Consumer:SetupConsumer(0x564091641230)
+1.000000000s 0 Consumer:StartApplication(0x564091641230)
Consumer:DoDispose(0x564091641230)
Consumer:~Consumer(0x564091641230)
-------Node 10.1.9.1 Connection Setting Finishing in: 1000ms-----------

Prefix: -bin8, Iteration: 100 - Simulation completed successfully.

#################### SIMULATION END ####################"

Why there is no completion info show in the log file?
It seems that when we put the code into the loop block, the bug occurs.

Answer: 
the following variable should not be decleared more than once. i.e., 
std::string routerFilePath = currentDir + "/scratch/config/router" + prefix + ".txt";

routerFilePath = currentDir + "/scratch/config/router" + prefix + ".txt";
linkFilePath = currentDir + "/scratch/config/link" + prefix + ".txt";
aggGropuFilePath = currentDir + "/scratch/config/aggtree" + prefix + ".txt";



Bug for extra line in the log (2025/1/15), have been fixed on 2025/1/15 (√).

Location: src/applications/model/TCPserver.cc

void 
TCPserver::CallSendEmptyPacket()
{
  NS_LOG_FUNCTION (this << " CallSendEmptyPacket");
  // Zhuoxu: extra std::endl comes from this.
  // std::cout << std::endl;
  m_socket->GetObject<TcpSocketBase>()->CallSendEmptyPacketACK();
}

the extra std::endl is caused by the std::cout << std::endl; within the CallSendEmptyPacket() function.


Todo List for 2025/1/16:
1. Print the routing of the ISP-50. Since we have to make comparison for ICN forwarder selection.
2. How do the TCP's Dijstra algorithm read topology? File.


Record List for 2025/1/20: 
1. How does routing be computed in TCP?
Using populated global routing table, related function is implemented in file "src/internet/model/global-route-manager-impl.cc". It adopts the "Dijkstra SPF calculation" to do the routing table computation.

2. How to print the routing table after the routing is completed?
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/output-stream-wrapper.h"

void PrintRoutingTables();

// Print routing tables
  PrintRoutingTables();

We adopt the function provided by 4o and added in the "scratch/innetwork-test.cc" file.


Todo List for 2025/1/20:
1. How to make the result of the bandwidth change. This problem cannot be solved.

Todo List for 2025/1/22:
1. Output the Tx and Rx Buffer volume change of each host.
2. Whether forwarder has?


Bug for 2025/1/23:
Trace for reason why the IP address is wrong?

+1.000000000s 71 TCPserver:PrintSocketInfo(): Socket info. IP address: 10.2.9.2 and port: 32625
IP address is wrong in TCPserver:PrintSocketInfo() function (×)!
update on 2025/1/24: output on TCPserver side is right.

this->nodeInnetworkAggregationInterface->SetupInnetworkAggregationInterface (port, itr, rank, sGroup, cGroup, GetNode (), false);

there is problem with GetNode (), IP address error. Check how GetNode return in Producer class, maybe check the innetwork_test.cc file.

How to derive IP address of a NodeContainer class?
consumer.Get(0)->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal()

hint: 
0x55de0d0d0940 IP: 10.2.20.1 in RttEndRecord...0x55de0d0d0940 local IP: 10.2.20.1
in Createagg ...
Node IP Address: 10.2.19.2
Node IP Address: 10.2.20.1

It seems that we output the different dim of the IPv4 address. Maybe use another different method can we obtain the right IPv4 address.

Fix:
Previously, in src/internet/model/tcp-socket-base.cc file, method "src/internet/model/tcp-socket-base.cc" is used m_endPoint->GetLocalAddress() and this method would return the wrong IP address.
We should use GetNode() method and get the IPv4 address we config in the beginning.

Bug:
it seems that there is retransmission packet?? But the isRetransmission flag is always false.
Why iteration 20 has 34 send records? This is strange.
To fix 2025/01/25:
we only need to record the time record of the first RTT, 

bug for 2025/01/26:
why there is no any output log within this function?
void 
TcpSocketBase::UpdateTP(uint32_t pktSize)


bug for 2025/01/27:
why in /home/dd/tcp-agg/ns-allinone-3.42/ns-3.42/log/tcp-testing.log
In address: 10.2.27.2
In address: 10.2.27.2
In address: 10.2.27.2
In address: 10.2.27.2
In address: 10.2.27.2
why the destructor will be called multiple times.
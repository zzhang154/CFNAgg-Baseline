Todo List for 2025/1/7:
Add 2ms time after each aggregator complete the collection of all children. Only do this for aggregator, but not for producer. Producer can send the data ASAP.

Todo List for 2025/1/8:
Changes should be done in the TCPserver.c file.

Todo List for 2025/1/9:
TraceValue for the following content.
1. RTT for send data of each iteration, both application layer and transport layer.
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
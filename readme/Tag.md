2025/02/05, for path tracing.
Todo:
1. packet parse.
tcp-socket-base class should be able to recognize the packet received and sent. This can be done via scaning the first 10 bytes of the packet and recognize the flag, then scratch out the iteration number.

2. iteration number for tag.
After recognize the current iteration number, find the record from the m_lastTraceTag list and inherit the tag info for the corresponding iteration number.

3. m_lastTraceTag management.
When sent packet for a specific iteration, we can delete the m_lastTraceTag record of that iteration. Since it has completed the process of this iteration and will never need this iteration info after then.

4. ?? How to do with the forwarder?
It seems that there is no code for controling the behavior of forwarders. How can we do this?


2025/02/08:
1. The tag should be sent in the file "src/applications/model/TCPclient.cc". And also in the function "int
TCPclient::Send(const uint8_t* buffer, size_t len)", we should add the tag into it.

  Ptr<Packet> p = Create<ns3::Packet>(buffer,len);
  // agg the tag here.
  int sentSize = m_socket->Send (p,0);

But in this way, we need to specify another producer_send function, since other component, i.e., aggregator and consumer needn't do this.

The difficult point is that how to keep the record of the old tag histroy?
Also, how to merge multiple tag when packets are aggregated into one single node.

Today's finish:
Tag generate and by-pass via the forwarder.
Now, forwarder can see the record.
Bug: there is something wrong with the Time.print and operator << function. See how can print the operator in a normal way.

To finish on 2025/02/09:
:用全局变量来记录
:每个iteration来区别
:将属于同一个pro0的同一个iteration的记录放入其中即可
:全局变量放在parameters.cc文件

bugs report 2025/02/09:

In function "src/innetwork-task/helper/setup.cc":
bool ForwarderPromiscRxCallback(Ptr<NetDevice> device, Ptr<const Packet> packet,
                                uint16_t protocol, const Address &sender,
                                const Address &receiver, NetDevice::PacketType packetType)

Ipv4Address nodeIp = ipv4->GetAddress(1, 0).GetLocal();
            tag.AddEntry(nodeIp, Simulator::Now());

Here, we must use ipv4->GetAddress(1, 0), instead of ipv4->GetAddress(0, 0), in order to get the correct ipv4 address.

2025/02/09 Todo list:
1. add a member tag in the interface, which stores the tag of the children. 
2. a pointer in both TCPclient and TCPserver that can access to its interface.


2025/02/10 Todo list:
But fix:
currently, the aggregator has not been activated. Why?
There is no log w.r.t aggregator.

Ans:
The aggtree has not been build correctly. Previous, aggTree only has pro0 and con0.

2025/02/10 Todo list:
Do a receive record, currently, the received record should be put in the application layter, in order to access the iteration number easily.

should use in TCPserver.cc:
GetLocalAddress().GetIpv4()

About the forwarder:
void
PointToPointNetDevice::Receive(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    // std::cout << "PointToPointNetDevice::Receive(Ptr<Packet> packet)" << std::endl;
    uint16_t protocol = 0;

Here, I add a std::cout to output the information when the packet is transfered via the device.


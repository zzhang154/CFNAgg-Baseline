/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007,2008,2009 INRIA, UDCAST
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Amine Ismail <amine.ismail@sophia.inria.fr>
 *                      <amine.ismail@udcast.com>
 *
 */

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "ns3/internet-module.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/PacketTraceTag.h"

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup quicclientserver
 *
 * \brief A QUIC client. Sends QUIC packet carrying sequence number and time stamp
 *  in their payloads
 *
 */
class TCPclient : public Application
{
public:
  Ptr<Socket> m_socket=nullptr ; //!< Socket

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TCPclient ();

  virtual ~TCPclient ();
  void Bind (uint16_t port);
  //void Start();
  void SetRemote (Address ip, uint16_t port);
  void SetRemote (Address addr);
  void SetNode (Ptr<Node> node);
  int Send(const uint8_t* buffer, size_t len, bool isProducer);
  void RecvPacket(Ptr<Socket> socket);
  InetSocketAddress GetLocalAddress() const;
  Ptr<Socket> GetSocket();
  void CreateSocket(Ptr<Node> node,uint16_t port);
  uint16_t GetBindPort();
  Address GetBindAddress();
  Ipv4Address GetIpv4LocalAddress();
  void SetCongestionControlAlgorithm(std::string cc_name);

  void connectToserver();
  void ReceiveData();
  void PrintSocketInfo(Ptr<Socket> socket);
  void CheckSocketState();
  void LogSocketInfo();
  void SetSendTag(PacketTraceTag tag);

  void SetIpTable();
  
  
protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);  

  uint32_t m_count; //!< Maximum number of packets the application will send
  uint32_t m_rcount; //!< Maximum number of packets the application will send
  Time m_interval; //!< Packet inter-send time
  uint32_t m_size; //!< Size of the sent packet (including the SeqTsHeader)

  uint32_t m_sent; //!< Counter for sent packets
  EventId m_sendEvent; //!< Event to send the next packet

  uint32_t m_numStreams;
  uint32_t m_lastUsedStream;
  
  
  bool m_running=false;


  Address m_bindIp; //!< local address
  uint16_t m_bindPort;//local port to receive from the server
  uint16_t m_peerPort=0; //!< Remote peer port
  Address m_peerIp; //!< Remote peer address
  Address m_peerAddress; //!< Remote peer address


  Ptr<Node> m_node = nullptr;
  std::string congestionControlAlgorithm = "reno";

  // Zhuoxu: DIY var
  PacketTraceTag m_sendTag;
};

} // namespace ns3

#endif /* TCP_CLIENT_H */

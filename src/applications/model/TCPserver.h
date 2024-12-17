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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "ns3/application.h"
#include "ns3/internet-module.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include "packet-loss-counter.h"
#include <iostream>
#include <fstream>
#include <queue>

// Zhuoxu: add this in order to use chunkMap
#include "ns3/parameter.h"
#include "ns3/vectorop.h"
// #include "ns3/utils.h"
#include <string.h>
#include <unordered_map>

namespace ns3 {
/**
 * \ingroup applications
 * \defgroup quicclientserver QuicClientServer
 */

/**
 * \ingroup quicclientserver
 *
 * \brief A QUIC server, receives QUIC packets from a remote host.
 *
 * QUIC packets carry a 32bits sequence number followed by a 64bits time
 * stamp in their payloads. The application uses the sequence number
 * to determine if a packet is lost, and the time stamp to compute the delay.
 */

enum PacketState {
    FIRST_PACKET,
    SECOND_PACKET
};

class TCPserver : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  
  static TypeId GetTypeId (void);
  TCPserver(Ptr<Node> node); // Modified constructor
  TCPserver(); // Default constructor
  virtual ~TCPserver ();
  /**
   * \brief Returns the number of lost packets
   * \return the number of lost packets
   */
  uint32_t GetLost (void) const;

  /**
   * \brief Returns the number of received packets
   * \return the number of received packets
   */
  uint64_t GetReceived (void) const;

  void PrintSocketInfo(Ptr<Socket> socket);

  /**
   * \brief Returns the size of the window used for checking loss.
   * \return the size of the window used for checking loss.
   */
  uint16_t GetPacketWindowSize () const;

  /**
   * \brief Set the size of the window used for checking loss. This value should
   *  be a multiple of 8
   * \param size the size of the window used for checking loss. This value should
   *  be a multiple of 8
   */
  void SetPacketWindowSize (uint16_t size);
  Ptr<Socket> GetSocket();
  void Bind (uint16_t port);
  void RecvPacket(Ptr<Socket> socket);
  void SetNode (Ptr<Node> node);
  //
  void CreateSocket(Ptr<Node> node,uint16_t port);
  //void SendToAddress(const uint8_t* buffer, size_t len,const InetSocketAddress& dest);
  int Send(const uint8_t* buffer, uint32_t len);
  Address GetBindAddress();
  InetSocketAddress GetLocalAddress() const;
  void SetCongestionControlAlgorithm(std::string cc_name);

  // Zhuoxu: relative function w.r.t the interface.cc
  uint16_t GetCompIterNum();
  std::queue<uint16_t> GetCompIterQueue();
  void ClearCompQueue();
  void ReleaseMap(uint16_t iterationNum);
  void SetcGroupSize(uint16_t size);
  void CheckChComp(uint16_t iterationNum); 
  void PrintBuffInfo_8(uint8_t* buffer, uint32_t packetSize);
  void PrintState();
  void SetIterChunkPtr(std::unordered_map<uint16_t, DataChunk>* iterChunk);
  int CheckMemory();
  void CallHandleRead();
  void Addr2Str (Address addr, std::string &str);
  void SetIpAddrStr(std::string ipAddrStr);
  void LogSocketInfo();
  void CheckSocketState();
  void PrintTable();
  void CallSendEmptyPacket();

protected:
  virtual void DoDispose (void);
  virtual void DoInitialize(void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);
  void ProcessPerPkt();

  bool CheckHeader(uint8_t* packetContent, int len);
  bool CheckReTransmit(uint8_t* packetContent);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);
  
  Ptr<Socket> m_socket=nullptr ; //!< IPv4 Socket
  Ptr<Socket> m_rxsocket=nullptr ; //!< IPv4 Socket for responding the requests
  Ptr<Socket> m_socket6; //!< IPv6 Socket
  uint64_t m_received; //!< Number of received packets
  PacketLossCounter m_lossCounter; //!< Lost packet counter
  
  uint16_t m_port; //!< Port on which we listen for incoming packets.
  Address m_bindIp;
  Address m_peerAddress;

  Time m_txTs; //!< Time at which the last packet with header was received
  // uint32_t m_currentSequenceNumber; //!< SN of the last packet with header
  // uint32_t m_hSize; //!< Size of the last header received

  std::string m_outFilename;
  std::ofstream m_outFile;

  Ptr<Node> m_node = nullptr;
  Ptr<Packet> p = nullptr;
  std::string congestionControlAlgorithm = "reno";

  // Zhuoxu: add two more states here to distinguish between the first and second packet.
  PacketState m_packetState = FIRST_PACKET;
  std::queue<uint16_t> compQueue;

  // Zhuoxu: add data structure chunkmap here to store data of all the child individully.
  std::unordered_map<uint16_t, ReceivedChunk> iterChunkMap;
  // Zhuoxu: create a buffer to store the first and second received data.
  std::unordered_map<std::string, uint16_t> m_iterationMap;
  std::unordered_map<std::string, uint8_t*> m_bufferMap;
  std::unordered_map<std::string, uint32_t> m_bufferPtrMap;
  uint8_t* m_buffer = nullptr;
  uint32_t m_bufferPtr = 0;
  uint32_t m_pktPtr = 0;
  uint16_t m_iteration = 0;
  uint16_t cGroupSize;
  std::string ipAddressStr;
  uint16_t iterThresh = ITERTHRESH;

  std::unordered_map<uint16_t, DataChunk>* iterChunkPtr;
};

} // namespace ns3

#endif /* TCP_SERVER_H */
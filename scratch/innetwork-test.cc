/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 SIGNET Lab, Department of Information Engineering, University of Padova
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
 * Authors: Alvise De Biasio <alvise.debiasio@gmail.com>
 *          Federico Chiariotti <whatever@blbl.it>
 *          Michele Polese <michele.polese@gmail.com>
 *          Davide Marcato <davidemarcato@outlook.com>
 *          
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/quic-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>

// Zhuoxu: no need to use the absolute path
#include "ns3/consumer.h"
#include "ns3/producer.h"
#include "ns3/aggregator.h"
#include "ns3/parameter.h"

#include <unistd.h>
#include <limits.h>
#include <string>
#include <climits> // For CHAR_BIT

using namespace ns3;

// Function to get the current working directory
std::string GetCurrentWorkingDir() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}
const std::string currentDir = GetCurrentWorkingDir();

NS_LOG_COMPONENT_DEFINE("innetwork-test");

const std::string routerFilePath = currentDir + "/scratch/config/router50.txt";
const std::string linkFilePath = currentDir + "/scratch/config/link50.txt";
const std::string aggGropuFilePath = currentDir + "/scratch/config/aggtree50.txt";
const std::string conName = "con";
const std::string proName = "pro";
const std::string fowName = "forwarder";
const std::string aggName = "agg";

// Zhuoxu: this should be set automatically by the configuration file.
uint32_t consumerNum;
uint32_t producerNum;
uint32_t forwarderNum;
uint32_t aggregatorNum;
std::string cc = "bbr";
uint16_t basetime = 1000;
uint16_t starttime = 1;
uint16_t stoptime = 500;//5000

// name nodes
void NameNodes(NodeContainer &nodes, std::string baseName) {
    //Name cons
    for (uint8_t i = 0; i < nodes.GetN(); ++i)
    {
        std::string nodeName = baseName + std::to_string(i);
        if (!Names::Find<Node> (nodeName)) {
            Names::Add(nodeName, nodes.Get(i));
            //std::cout << "Name added successfully: " << nodeName << std::endl;

        } else {
            std::cout << "Error: Name already exists: " << nodeName << std::endl;
        }
    }
}

void CountRouterNodes(const std::string& filename) {

    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Read the file line by line
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("con") == 0) {
            ++consumerNum;
        } else if (line.find("pro") == 0) {
            ++producerNum;
        } else if (line.find("for") == 0) {
            ++forwarderNum;
        } else if (line.find("agg") == 0) {
            ++aggregatorNum;
        }
    }

    // Close the file
    file.close();

    // Output the results
    std::cout << "uint32_t consumerNum = " << consumerNum << ";" << std::endl;
    std::cout << "uint32_t producerNum = " << producerNum << ";" << std::endl;
    std::cout << "uint32_t forwarderNum = " << forwarderNum << ";" << std::endl;
    std::cout << "uint32_t aggregatorNum = " << aggregatorNum << ";" << std::endl;
}

// read link.txt to create links
void BuildTopo(const std::string &linkFile,  NodeContainer &consumer, NodeContainer &producer, 
            NodeContainer &forwarder, NodeContainer &aggregator) {
    
    consumer.Create(consumerNum);
    producer.Create(producerNum);
    forwarder.Create(forwarderNum);
    aggregator.Create(aggregatorNum);

    // name nodes
    NameNodes (consumer, conName);
    NameNodes (producer, proName);
    NameNodes (forwarder, fowName);
    NameNodes (aggregator, aggName);
    //std::cout << "name end" << std::endl;

    // install internet stack
    InternetStackHelper stack;
    stack.Install (consumer);
    stack.Install (producer);
    stack.Install (forwarder);
    stack.Install (aggregator);
    //std::cout << "install internet end" << std::endl;

    std::ifstream infile (linkFile);
    std::string line;
    // create p2p links
    PointToPointHelper p2p;
    Ipv4AddressHelper address;
    uint32_t ia = 1, ic = 1;
    
    while (std::getline (infile, line)) {
        std::istringstream iss (line);
        std::string node1, node2, dataRate, delay, queueSize;
        double lossRate;
        if (!(iss >> node1 >> node2 >> dataRate >> lossRate >> delay >> queueSize)) {
            //std::cout << "iss >> node1 >> node2 >> dataRate >> lossRate >> delay >> queueSize" << std::endl;
            //std::cout<<node1<<"------------------"<<node2<<"------------------"<<dataRate<<"------------------"<<lossRate<<"------------------"<<delay<<"------------------"<<queueSize<<std::endl;
            continue; // jump error-format lines
        }
        //std::cout<<"Installing nodes------------------"<<std::endl;
        //std::cout<<node1<<"------------------"<<node2<<"------------------"<<dataRate<<"------------------"<<lossRate<<"------------------"<<delay<<"------------------"<<queueSize<<std::endl;
        p2p.SetDeviceAttribute("DataRate", StringValue (dataRate));
        p2p.SetChannelAttribute("Delay", StringValue (delay));
        p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue (queueSize + "p"));//maxsize=30packets
        //std::cout << "set p2p linkAttribute end" << std::endl;

        if (Names::Find<Node> (node1) == nullptr || Names::Find<Node> (node2) == nullptr) {
            std::cout << "Name does not exist, node1 : " << node1 << " node2 : " << node2 << std::endl;
        }


        NodeContainer n1n2 = NodeContainer (Names::Find<Node> (node1), Names::Find<Node> (node2));
        // install p2p
        NetDeviceContainer d1d2 = p2p.Install (n1n2);
        
        // assign ip address
        std::string ipBaseAddr;
        if (node1.find ("agg") != std::string::npos || node2.find ("agg") != std::string::npos) {
            ipBaseAddr = "10.2." + std::to_string (ia) + ".0";
            ++ia;

        } else {
            ipBaseAddr = "10.1." + std::to_string (ic) + ".0";
            ++ic;
        }

        address.SetBase(Ipv4Address(ipBaseAddr.c_str()), "255.255.255.0");
        address.Assign(d1d2);
        std::cout<<"---node1  ip---"<<node1<<"---"<<n1n2.Get(0)->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal()<<"---node2  ip---"<<node2<<"---"<<n1n2.Get(1)->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal()<<std::endl;
        
    }
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    p2p.EnablePcapAll("/home/dd/wiresharkRecordFile/");
    infile.close ();
}

void Createpro (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, std::vector<Address> &sGroup, 
                std::vector<Address> &cGroup, Ptr<Node> node) {
    Ptr<Producer> producer = CreateObject<Producer> ();
    //cGroup.resize(0);
    cGroup.clear();
    node->AddApplication (producer);
    producer->SetupProducer (port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    producer->SetStartTime (Seconds(starttime));
    producer->SetStopTime (Seconds(stoptime));
}

void Createagg (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize, std::vector<Address> &sGroup, 
                                        std::vector<Address> &cGroup, Ptr<Node> node) {
    Ptr<Aggregator> aggragator = CreateObject<Aggregator> ();
    node->AddApplication (aggragator);
    aggragator->SetupAggregator (port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    aggragator->SetStartTime (Seconds(starttime));
    aggragator->SetStopTime (Seconds(stoptime));
}

void Createcon (uint16_t port, uint16_t itr, uint8_t rank, uint32_t vsize,
                    std::vector<Address> &sGroup, std::vector<Address> &cGroup, 
                    Ptr<Node> node) {
    Ptr<Consumer> consumer = CreateObject<Consumer> ();
    node->AddApplication (consumer);
    consumer->SetupConsumer (port, itr, rank, vsize, sGroup, cGroup, basetime, cc);
    consumer->SetStartTime (Seconds(starttime));
    consumer->SetStopTime (Seconds(stoptime));
}

void CreateAggGroup (const std::string aggGroupFile, 
                        std::unordered_map<std::string, std::vector<std::string>> &aggGroups) {
    std::ifstream infile (aggGroupFile);
    std::string line;
    while (std::getline (infile, line)) {
        std::istringstream iss (line);
        std::vector<std::string> nodes;
        std::string nodeName, childName;
        iss >> nodeName;
        nodeName. pop_back (); // delete ':' character
    
        while (iss >> childName) {
            nodes. push_back (childName);
        }
        aggGroups[nodeName] = nodes;
    }
}

void CreateAggTree (std::string &nodeName, std::vector<Address> pa,
                std::unordered_map<std::string, std::vector<std::string>> &aggGroups,
                uint8_t rank, uint16_t itr, uint32_t vsize, uint16_t server_port) {
    std::vector<Address> sGroup;
    Ptr<Ipv4> ipv4 = Names::Find<Node> (nodeName)->GetObject<Ipv4>(); // find ipv4 address of the root node 'con0' for the first time
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
    Ipv4Address addr = iaddr.GetLocal ();
    sGroup. push_back (addr);

    std::vector<Address> cGroup;  // store the address of the child nodes
    for (std::string &str : aggGroups[nodeName]) {
        rank += 1;
        CreateAggTree (str, sGroup, aggGroups, rank, itr, vsize, server_port);
        ipv4 = Names::Find<Node> (str)->GetObject<Ipv4>();
        iaddr = ipv4->GetAddress (1, 0);
        addr = iaddr.GetLocal ();
        cGroup. push_back (addr);
    }
    //std::cout << "current node " << nodeName << std::endl;
    if (nodeName. find ("con") != std::string::npos)
        // for each object, we have to specify its connection. So here we pass the address of the parent node and children nodes for construction.
        Createcon(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node> (nodeName));
    if (nodeName. find ("agg") != std::string::npos)
        Createagg(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node> (nodeName));
    if (nodeName. find ("pro") != std::string::npos)
        Createpro(server_port, itr, rank, vsize, pa, cGroup, Names::Find<Node> (nodeName));
}

void CreateAggTreeTopo (uint16_t itr, uint32_t vsize, uint16_t server_port) {
    // get Aggragator Group
    std::unordered_map<std::string, std::vector<std::string>> aggGroups;
    CreateAggGroup (aggGropuFilePath, aggGroups);
    // create agg tree
    std::string root = "con0";
    CreateAggTree (root, std::vector<Address>(), aggGroups, 0, itr, vsize, server_port);
}


void CreateDirectTopo (NodeContainer &cons, NodeContainer &pros, uint16_t itr, 
                       uint32_t vsize, uint16_t server_port) {
    std::vector<Address> consumers;
    Ptr<Ipv4> ipv4 = cons. Get (0)->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress (1, 0);
    Ipv4Address addr = iaddr.GetLocal ();
    consumers. push_back (addr);

    std::vector<Address> producers;
    for (uint8_t i = 0; i < pros. GetN (); ++i) { //
        ipv4 = pros. Get (i)->GetObject<Ipv4>();
        iaddr = ipv4->GetAddress (1, 0);
        addr = iaddr.GetLocal ();
        producers. push_back (addr);
    }

    uint8_t rank = 0;
    std::vector<Address> nullV;
    for (uint8_t i = 0; i < pros. GetN (); ++i) {
        Ptr<Producer> producer = CreateObject<Producer> ();
        pros. Get (i)->AddApplication (producer);
        producer->SetupProducer (server_port, 0, rank, vsize, consumers, nullV, basetime * 2, cc);
        rank++;
        producer->SetStartTime (Seconds(starttime));
        producer->SetStopTime (Seconds(stoptime));
    }
    

    Ptr<Consumer> consumer = CreateObject<Consumer> ();
    cons. Get (0)->AddApplication (consumer);
    consumer->SetupConsumer (server_port, itr, rank, vsize, nullV, producers, basetime, cc);
    consumer->SetStartTime (Seconds(starttime));
    consumer->SetStopTime (Seconds(stoptime));
    //std::cout << consumer <<std::endl;
}

int
main (int argc, char *argv[])
{
    CommandLine cmd;
    uint16_t itr = 3;//1000 Zhuoxu: now this number should set to one.
    uint32_t vsize = 300000;
    bool topotype = 1;
    cmd.AddValue("itr", "max iteration consumer performed", itr);
    cmd.AddValue("vsize", "vector size", vsize);
    cmd.AddValue("topotype", "choose test topo type", topotype);
    cmd.AddValue("cc", "choose test congestion control", cc);
    cmd.AddValue("basetime", "set base time", basetime);
    cmd.AddValue("starttime", "set start time", starttime);
    cmd.AddValue("stoptime", "set stop time", stoptime);
    cmd.Parse (argc, argv);

    ns3::SetBaseSize(vsize * sizeof(uint8_t) * CHAR_BIT);

    std::cout
        << "\n\n#################### SIMULATION PARAMETERS ####################\n\n\n";
    
    std::cout
        << "Topotype: "<<topotype<< " Vector Size: "<<vsize<< " Congestion Control Algorithm: "<< cc <<std::endl;

    std::cout
        << "\n\n#################### SIMULATION SET-UP ####################\n\n\n";

    LogLevel log_precision = LOG_LEVEL_LOGIC;
    Time::SetResolution (Time::NS);
    // Disable all log levels initially
    LogComponentDisableAll(LOG_LEVEL_ALL);

    LogComponentEnableAll (LOG_PREFIX_TIME);
    LogComponentEnableAll (LOG_PREFIX_FUNC);
    LogComponentEnableAll (LOG_PREFIX_NODE);
    //LogComponentEnable ("QuicMyClient", log_precision);
    LogComponentEnable ("QuicMyServer", log_precision);
    LogComponentEnable("QuicMyServer", LOG_LEVEL_INFO);
    // LogComponentDisable("QuicMyServer", LOG_LEVEL_FUNCTION);


    //LogComponentEnable ("QuicSocketTxScheduler", log_precision);
    //LogComponentEnable ("Consumer", log_precision);
    //LogComponentEnable ("Producer", log_precision);
    //LogComponentEnable ("Aggregator", log_precision);
    // LogComponentEnable ("QuicSocketBase", log_precision);
    //LogComponentEnable("CircularBuffer",log_precision);
    //LogComponentEnable ("InnetworkAggregationInterface", log_precision);
    //LogComponentEnable("QuicL5Protocol",log_precision);

    // Enable logging for the Consumer component
    // LogComponentEnable("Consumer", LOG_LEVEL_INFO);
    // LogComponentEnable("Aggregator", LOG_LEVEL_INFO);
    
    // LogComponentEnable("Producer", LOG_LEVEL_INFO);
    // LogComponentEnable("Consumer", LOG_LEVEL_FUNCTION);

    // Enable logging debug for some component
    // LogComponentEnable("Consumer", LOG_LEVEL_DEBUG);
    // LogComponentEnable("Aggregator", LOG_LEVEL_DEBUG);
    // LogComponentEnable("Producer", LOG_LEVEL_DEBUG);
    LogComponentEnable("InnetworkAggregationInterface", LOG_LEVEL_INFO);

    // zhuoxu: disable the QuicSubheader log
    ns3::LogComponentDisable("QuicSubheader", ns3::LOG_LEVEL_ALL);

    //Ensure that LOG_LEVEL_FUNCTION and other levels are not enabled
    LogComponentDisable("Consumer", LOG_LEVEL_FUNCTION);
    LogComponentDisable("Aggregator", LOG_LEVEL_FUNCTION);
    LogComponentDisable("Producer", LOG_LEVEL_FUNCTION);
    LogComponentDisable("InnetworkAggregationInterface", LOG_LEVEL_FUNCTION);
    
    NodeContainer consumer;
    NodeContainer producer;
    NodeContainer forwarder;
    NodeContainer aggregator;

    CountRouterNodes(routerFilePath);
    BuildTopo(linkFilePath, consumer, producer, forwarder, aggregator);
    //LogLevel log_precision = LOG_LEVEL_INFO; 
    uint16_t server_port=1234;
    if (topotype == 0) {
        CreateDirectTopo (consumer, producer, itr, vsize, server_port);

    } else {
        CreateAggTreeTopo (itr, vsize, server_port);
    }


    // PointToPointHelper pointToPoint;
    // pointToPoint.EnablePcapAll  ("/home/hxq/ns-allinone-3.37/ns-3.37/topo/topo");

    Packet::EnablePrinting ();
    Packet::EnableChecking ();
    // run
    //Simulator::Stop (Seconds (50000));
    Simulator::Run();
    
    //end
    Simulator::Destroy();
    std::cout
      << "\n\n#################### SIMULATION END ####################\n\n\n";
    return 0;
}

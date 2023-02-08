/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h" 


// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{

  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (5);

  PointToPointHelper pointToPoint,pointToPoint1,pointToPoint2,pointToPoint3;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Kbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("50ms"));
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("100Kbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("5ms"));
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("100Kbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("5ms"));
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("100Kbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("5ms"));



  NetDeviceContainer devices,devices1,devices2,devices3;
  devices = pointToPoint.Install (nodes.Get(0),nodes.Get(1));
  devices1 = pointToPoint1.Install (nodes.Get(1),nodes.Get(2));
  devices2 = pointToPoint2.Install (nodes.Get(2),nodes.Get(3));
  devices3 = pointToPoint3.Install (nodes.Get(2),nodes.Get(4));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address,address1,address2,address3;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  address1.SetBase ("10.1.2.0", "255.255.255.0");
  address2.SetBase ("10.1.3.0", "255.255.255.0");
  address3.SetBase ("10.1.4.0", "255.255.255.0");


  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
    Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);
      Ipv4InterfaceContainer interfaces3 = address3.Assign (devices3);

  UdpEchoServerHelper echoServer (9),echoServer1(10);
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (3));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  ApplicationContainer serverApps1 = echoServer1.Install (nodes.Get (4));
  serverApps1.Start (Seconds (1.0));
  serverApps1.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces3.GetAddress (1), 10); //Specifies where the echo message goes
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  
  UdpEchoClientHelper echoClient1 (interfaces2.GetAddress (1), 9); //Specifies where the echo message goes
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  
  ApplicationContainer clientApps1 = echoClient1.Install (nodes.Get (1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  AnimationInterface anim ("FIRST.xml"); //3 Animation interface

  

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

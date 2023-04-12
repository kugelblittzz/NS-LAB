
//p2p1
//no
//d01|    d23
//n1---n2---n3 p3p4
//| \d4 | /d5|
//n4---n5---n6 p2p5
//| /d6  |d7 \ |
//n7---n8---n9 p2p6
 //     
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;

int main(int argc, char *argv[]){

  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes,l01,l12,l14,l23,l25,l36,l45,l47,l56,l58,l69,l78,l89;
  nodes.Create (10);
  // l01.Add(nodes.Get(0),nodes.Get(1));
  // l12.Add(nodes.Get(1),nodes.Get(2));
  // l14.Add(nodes.Get(1),nodes.Get(4));
  // l23.Add(nodes.Get(2),nodes.Get(3));
  // l25.Add(nodes.Get(2),nodes.Get(5));
  // l36.Add(nodes.Get(3),nodes.Get(6));
  // l45.Add(nodes.Get(4),nodes.Get(5));
  // l47.Add(nodes.Get(4),nodes.Get(7));
  // l56.Add(nodes.Get(5),nodes.Get(6));
  // l58.Add(nodes.Get(5),nodes.Get(8));
  // l69.Add(nodes.Get(6),nodes.Get(9));
  // l78.Add(nodes.Get(7),nodes.Get(8));
  // l89.Add(nodes.Get(8),nodes.Get(9));
  
   PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
 
  NetDeviceContainer d01,d12,d14,d23,d25,d36;
  d01 = pointToPoint.Install (nodes.Get(0),nodes.Get(1));
  d12= pointToPoint.Install (nodes.Get(1),nodes.Get(2));
  d14= pointToPoint.Install (nodes.Get(1),nodes.Get(4));
  d23= pointToPoint.Install (nodes.Get(2),nodes.Get(3));
  d25= pointToPoint.Install (nodes.Get(2),nodes.Get(5));
  d36= pointToPoint.Install (nodes.Get(3),nodes.Get(6));
  NetDeviceContainer d45= pointToPoint.Install (nodes.Get(4),nodes.Get(5));
  NetDeviceContainer d47= pointToPoint.Install (nodes.Get(4),nodes.Get(7));
  NetDeviceContainer d56= pointToPoint.Install (nodes.Get(5),nodes.Get(6));
  NetDeviceContainer d58= pointToPoint.Install (nodes.Get(5),nodes.Get(8));
  NetDeviceContainer d69= pointToPoint.Install (nodes.Get(6),nodes.Get(9));
  NetDeviceContainer d78= pointToPoint.Install (nodes.Get(7),nodes.Get(8));
  NetDeviceContainer d89= pointToPoint.Install (nodes.Get(8),nodes.Get(9));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  
  Ipv4InterfaceContainer if01 = address.Assign (d01);
  Ipv4InterfaceContainer if12 = address.Assign (d12);
  Ipv4InterfaceContainer if14 = address.Assign (d14);
  Ipv4InterfaceContainer if23 = address.Assign (d23);
  Ipv4InterfaceContainer if25 = address.Assign (d25);
  Ipv4InterfaceContainer if36 = address.Assign (d36);
  Ipv4InterfaceContainer if45 = address.Assign (d45);
  Ipv4InterfaceContainer if47 = address.Assign (d47);
  Ipv4InterfaceContainer if56 = address.Assign (d56);
  Ipv4InterfaceContainer if58 = address.Assign (d58);
  Ipv4InterfaceContainer if69 = address.Assign (d69);
  Ipv4InterfaceContainer if78 = address.Assign (d78);
  Ipv4InterfaceContainer if89 = address.Assign (d89);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (9));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (if89.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (3));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  AnimationInterface anim("l5q2.xml");
  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("l5q2.tr"));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

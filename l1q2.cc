//
// n0-------n2-------n1
//          |
//          |
///
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lab1q1");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (4);

  //0 and 2(link1)
  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));
  pointToPoint1.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("3p"));

  //1 and 2 (link2)
  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("4ms"));
  pointToPoint2.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("3p"));
  //2 and 3 (link2)
  PointToPointHelper pointToPoint3;
  pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint3.SetChannelAttribute ("Delay", StringValue ("4ms"));
  pointToPoint3.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("3p"));  
  NetDeviceContainer device1 = pointToPoint1.Install (nodes.Get(0), nodes.Get(2));
  NetDeviceContainer device2 = pointToPoint2.Install (nodes.Get(1), nodes.Get(2));
  NetDeviceContainer device3 = pointToPoint3.Install (nodes.Get(3), nodes.Get(2));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  
  Ipv4InterfaceContainer if1 = address.Assign (device1);
  Ipv4InterfaceContainer if2 = address.Assign (device2);
  Ipv4InterfaceContainer if3 = address.Assign (device3);
  
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get(2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  

  UdpEchoClientHelper echoClient (if3.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (3));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps2 = echoClient.Install (nodes.Get (0));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
  
  ApplicationContainer clientApps3 = echoClient.Install (nodes.Get (3));
  clientApps3.Start (Seconds (2.0));
  clientApps3.Stop (Seconds (10.0));
 
 AnimationInterface anim("l1q2.xml");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

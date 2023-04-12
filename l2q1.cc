#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
// Default Network Topology
//
//       10.1.1.0       10.1.3.0
// n0-------------n1-------------- n2   n3   n4   n5
//                  point-to-point  |    |    |    |
//                                  ================
//                                    LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int main (int argc, char *argv[])
{
   // std::string animeFile="l2q2.xml";
  uint32_t nCsma = 4;

      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer p2pNodes;
  p2pNodes.Create (3);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (2));
  csmaNodes.Create (nCsma);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices1,p2pDevices2;
  p2pDevices1 = pointToPoint.Install (p2pNodes.Get(0),p2pNodes.Get(1));
  p2pDevices2 = pointToPoint.Install (p2pNodes.Get(1),p2pNodes.Get(2));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (p2pNodes.Get (1));
  stack.Install (csmaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces1,p2pInterfaces2;
  p2pInterfaces1 = address.Assign (p2pDevices1);
  p2pInterfaces2 = address.Assign (p2pDevices2);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (15.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (12.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("l2q1.tr"));

  // pointToPoint.EnablePcapAll ("second");
  // csma.EnablePcap ("second", csmaDevices.Get (1), true);
  AnimationInterface anim ("l2q1.xml");
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

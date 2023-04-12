#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RipNgSimpleRouting");

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv6> ()->SetDown (interfaceA);
  nodeB->GetObject<Ipv6> ()->SetDown (interfaceB);
}

int main (int argc, char **argv)
{
  bool verbose = false;
  bool printRoutingTables = false;
  bool showPings = false;
  std::string SplitHorizon ("PoisonReverse");

  CommandLine cmd (__FILE__);
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds", printRoutingTables);
  cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
  cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)", SplitHorizon);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("RipNgSimpleRouting", LOG_LEVEL_INFO);
      LogComponentEnable ("RipNg", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv6L4Protocol", LOG_LEVEL_INFO);
      LogComponentEnable ("Ipv6Interface", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv6L4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("NdiscCache", LOG_LEVEL_ALL);
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
    }

  if (showPings)
    {
      LogComponentEnable ("Ping6Application", LOG_LEVEL_INFO);
    }

  if (SplitHorizon == "NoSplitHorizon")
    {
      Config::SetDefault ("ns3::RipNg::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));
    }
  else if (SplitHorizon == "SplitHorizon")
    {
      Config::SetDefault ("ns3::RipNg::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));
    }
  else
    {
      Config::SetDefault ("ns3::RipNg::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));
    }

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> src = CreateObject<Node> ();
  Names::Add ("SrcNode", src);
  Ptr<Node> dst = CreateObject<Node> ();
  Names::Add ("DstNode", dst);
  Ptr<Node> a = CreateObject<Node> ();
  Names::Add ("RouterA", a);
  Ptr<Node> b = CreateObject<Node> ();
  Names::Add ("RouterB", b);
  Ptr<Node> c = CreateObject<Node> ();
  Names::Add ("RouterC", c);
  Ptr<Node> d = CreateObject<Node> ();
  Names::Add ("RouterD", d);
  Ptr<Node> e = CreateObject<Node> ();
  Names::Add ("RouterE", e);
  NodeContainer net1 (src, a);
  NodeContainer net2 (a, b);
  NodeContainer net3 (a, c);
  NodeContainer net4 (b, c);
  NodeContainer net5 (c, d);
  NodeContainer net6 (b, d);
  NodeContainer net7 (d, dst);
  NodeContainer net8 (b, e);
  NodeContainer net9 (d, e);
  NodeContainer routers (a, b, c, d,e);
  NodeContainer nodes (src, dst);


  NS_LOG_INFO ("Create channels.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  NetDeviceContainer ndc1 = csma.Install (net1);
  NetDeviceContainer ndc2 = csma.Install (net2);
  NetDeviceContainer ndc3 = csma.Install (net3);
  NetDeviceContainer ndc4 = csma.Install (net4);
  NetDeviceContainer ndc5 = csma.Install (net5);
  NetDeviceContainer ndc6 = csma.Install (net6);
  NetDeviceContainer ndc7 = csma.Install (net7);
  NetDeviceContainer ndc8 = csma.Install (net8);
  NetDeviceContainer ndc9 = csma.Install (net9);

  NS_LOG_INFO ("Create IPv6 and routing");
  RipNgHelper ripNgRouting;

  // Rule of thumb:
  // Interfaces are added sequentially, starting from 0
  // However, interface 0 is always the loopback...
  ripNgRouting.ExcludeInterface (a, 1);
  ripNgRouting.ExcludeInterface (d, 3);

  ripNgRouting.SetInterfaceMetric (c, 3, 10);
  ripNgRouting.SetInterfaceMetric (d, 1, 10);

  Ipv6ListRoutingHelper listRH;
  listRH.Add (ripNgRouting, 0);
  Ipv6StaticRoutingHelper staticRh;
  listRH.Add (staticRh, 5);

  InternetStackHelper internetv6;
  internetv6.SetIpv4StackInstall (false);
  internetv6.SetRoutingHelper (listRH);
  internetv6.Install (routers);

  InternetStackHelper internetv6Nodes;
  internetv6Nodes.SetIpv4StackInstall (false);
  internetv6Nodes.Install (nodes);

  // Assign addresses.
  // The source and destination networks have global addresses
  // The "core" network just needs link-local addresses for routing.
  // We assign global addresses to the routers as well to receive
  // ICMPv6 errors.
  NS_LOG_INFO ("Assign IPv6 Addresses.");
  Ipv6AddressHelper ipv6;

  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic1 = ipv6.Assign (ndc1);
  iic1.SetForwarding (1, true);
  iic1.SetDefaultRouteInAllNodes (1);

  ipv6.SetBase (Ipv6Address ("2001:0:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic2 = ipv6.Assign (ndc2);
  iic2.SetForwarding (0, true);
  iic2.SetForwarding (1, true);

  ipv6.SetBase (Ipv6Address ("2001:0:2::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic3 = ipv6.Assign (ndc3);
  iic3.SetForwarding (0, true);
  iic3.SetForwarding (1, true);

  ipv6.SetBase (Ipv6Address ("2001:0:3::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic4 = ipv6.Assign (ndc4);
  iic4.SetForwarding (0, true);
  iic4.SetForwarding (1, true);

  ipv6.SetBase (Ipv6Address ("2001:0:4::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic5 = ipv6.Assign (ndc5);
  iic5.SetForwarding (0, true);
  iic5.SetForwarding (1, true);

  ipv6.SetBase (Ipv6Address ("2001:0:5::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic6 = ipv6.Assign (ndc6);
  iic6.SetForwarding (0, true);
  iic6.SetForwarding (1, true);
  
      ipv6.SetBase (Ipv6Address ("2001:0:6::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic8 = ipv6.Assign (ndc8);
  iic8.SetForwarding (0, true);
  iic8.SetForwarding (1, true);
    ipv6.SetBase (Ipv6Address ("2001:0:7::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic9 = ipv6.Assign (ndc9);
  iic9.SetForwarding (0, true);
  iic9.SetForwarding (1, true);

  ipv6.SetBase (Ipv6Address ("2001:2::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic7 = ipv6.Assign (ndc7);
  iic7.SetForwarding (0, true);
  iic7.SetDefaultRouteInAllNodes (0);

  if (printRoutingTables)
    {
      RipNgHelper routingHelper;

      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);

      routingHelper.PrintRoutingTableAt (Seconds (30.0), a, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (30.0), b, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (30.0), c, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (30.0), d, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (30.0), e, routingStream);

      routingHelper.PrintRoutingTableAt (Seconds (60.0), a, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (60.0), b, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (60.0), c, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (60.0), d, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (60.0), e, routingStream);

      routingHelper.PrintRoutingTableAt (Seconds (90.0), a, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (90.0), b, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (90.0), c, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (90.0), d, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (90.0), e, routingStream);
    }

  NS_LOG_INFO ("Create Applications.");
  uint32_t packetSize = 1024;
  uint32_t maxPacketCount = 100;
  Time interPacketInterval = Seconds (1.0);
  Ping6Helper ping6;

  ping6.SetLocal (iic1.GetAddress (0, 1));
  ping6.SetRemote (iic7.GetAddress (1, 1));
  ping6.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  ping6.SetAttribute ("Interval", TimeValue (interPacketInterval));
  ping6.SetAttribute ("PacketSize", UintegerValue (packetSize));
  ApplicationContainer apps = ping6.Install (src);
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (110.0));

  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("ripng-simple-routing.tr"));
  csma.EnablePcapAll ("ripng-simple-routing", true);

  Simulator::Schedule (Seconds (40), &TearDownLink, b, d, 3, 2);
  
  AnimationInterface anim ("l6q1.xml");

  /* Now, do the actual simulation. */
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (120));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}


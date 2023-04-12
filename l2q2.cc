#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-layout-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int main (int argc, char *argv[])
{
   uint32_t nSpokes = 11;
   NS_LOG_INFO ("Build star topology.");
   PointToPointHelper pointToPoint;
   pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

   PointToPointStarHelper star (nSpokes, pointToPoint);

   NS_LOG_INFO ("Install internet stack on all nodes.");
   InternetStackHelper internet;
   star.InstallStack (internet);

   NS_LOG_INFO ("Assign IP Addresses.");
   star.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"));
   NS_LOG_INFO ("Create applications.");
   /* Create a packet sink on the star "hub" to receive packets.*/

   uint16_t port = 50000;
   Address hubLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
   PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
   ApplicationContainer hubApp = packetSinkHelper.Install (star.GetHub ());
   hubApp.Start (Seconds (1.0));
   hubApp.Stop (Seconds (10.0));

   /* Create OnOff applications to send TCP to the hub, one on each spoke node.*/
   OnOffHelper onOffHelper ("ns3::TcpSocketFactory", Address ());
   onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
   onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

   ApplicationContainer spokeApps;

   for (uint32_t i = 0; i < star.SpokeCount (); ++i)
   {
      AddressValue remoteAddress (InetSocketAddress (star.GetHubIpv4Address (i), port));
      onOffHelper.SetAttribute ("Remote", remoteAddress);
      spokeApps.Add (onOffHelper.Install (star.GetSpokeNode (i)));
   }
   spokeApps.Start (Seconds (1.0));
   spokeApps.Stop (Seconds (10.0));
   NS_LOG_INFO ("Enable static global routing.");
   /* Turn on global static routing so we can actually be routed across the star.*/
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
   star.BoundingBox(0,0,100,100);
   AnimationInterface anim ("l2q2.xml");
    Simulator::Run ();
  Simulator::Destroy ();
  
  return 0;
}

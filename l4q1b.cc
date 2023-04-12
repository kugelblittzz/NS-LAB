
//
//       10.1.1.0        
//    n0----------n1   n2   n3   n4
//                |    |     |    |
//              ======================
//               Lan  10.2.1.0
//
//   n0 originates UDP broadcast to 255.255.255.255/discard port, which 
//   is replicated and received on both n1 and n2
//     n1 is source
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("l4q1b");

int 
main (int argc, char *argv[])
{
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this

  LogComponentEnable ("l4q1b", LOG_LEVEL_INFO);
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);


  // Allow the user to override any of the defaults and the above
  // Bind()s at run-time, via command-line arguments
  //CommandLine cmd (__FILE__);
  //cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (5);
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer c1 = NodeContainer (c.Get (1), c.Get (2),c.Get (3),c.Get (4));

  NS_LOG_INFO ("Build Topology.");
  PointToPointHelper pp;
  pp.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pp.SetChannelAttribute ("Delay", StringValue ("2ms"));
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  NetDeviceContainer n0 = pp.Install (c0);
  NetDeviceContainer n1 = csma.Install (c1);

  InternetStackHelper internet;
  internet.Install (c);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.0.0", "255.255.255.0");
  ipv4.Assign (n0);
  ipv4.SetBase ("192.168.1.0", "255.255.255.0");
  ipv4.Assign (n1);


  // RFC 863 discard port ("9") indicates packet should be thrown away
  // by the system.  We allow this silent discard to be overridden
  // by the PacketSink application.
  uint16_t port = 9;

  // Create the OnOff application to send UDP datagrams of size
  // 512 bytes (default) at a rate of 500 Kb/s (default) from n0
  NS_LOG_INFO ("Create Applications.");
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("255.255.255.255"), port)));
  onoff.SetConstantRate (DataRate ("500kb/s"));

  ApplicationContainer app = onoff.Install (c0.Get (1));
  // Start the application
  app.Start (Seconds (1.0));
  app.Stop (Seconds (10.0));

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  app = sink.Install (c0.Get (0));
  app.Add (sink.Install (c1.Get (1)));
  app.Start (Seconds (1.0));
  app.Stop (Seconds (10.0));

  // Configure ascii tracing of all enqueue, dequeue, and NetDevice receive 
  // events on all devices.  Trace output will be sent to the file 
  // "csma-one-subnet.tr"
  // AsciiTraceHelper ascii;
  // csma.EnableAsciiAll (ascii.CreateFileStream ("csma-broadcast.tr"));

  // Also configure some tcpdump traces; each interface will be traced
  // The output files will be named 
  // csma-broadcast-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -tt -r" command 
  // csma.EnablePcapAll ("csma-broadcast", false);
    AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("l4q1b.tr"));

  NS_LOG_INFO ("Run Simulation.");
    AnimationInterface anim ("l4q1b.xml");

  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

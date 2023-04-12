// n0    n1      n3     n4       n5
// |    |      |     |       |
// |    |      |     |       |
//=============================

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("l5q1");
int main(int argc, char *argv[]){
  LogComponentEnable("l5q1",LOG_LEVEL_INFO);

  //uint32_t nCsma = 5;
uint32_t maxBytes = 512;
 std::string datarate="0.5Mbps";
  std::string delay="2ms";
/*
  CommandLine cmd (__FILE__);
  cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
    cmd.AddValue ("datarate",
                "the datarate", datarate);
    cmd.AddValue ("delay",
                "the delay", delay);
  cmd.Parse (argc, argv);
  */
  
  NodeContainer csmaNodes;
  csmaNodes.Create(5);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (csmaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  uint16_t port = 9; // well-known echo port number
BulkSendHelper source ("ns3::TcpSocketFactory",
InetSocketAddress (csmaInterfaces.GetAddress (1), port));
// Set the amount of data to send in bytes. Zero is unlimited.
source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
ApplicationContainer sourceApps = source.Install (csmaNodes.Get (0));
sourceApps.Start (Seconds (0.0));
sourceApps.Stop (Seconds (10.0));

PacketSinkHelper sink ("ns3::TcpSocketFactory",
InetSocketAddress (Ipv4Address::GetAny (), port));
ApplicationContainer sinkApps = sink.Install (csmaNodes.Get (1));
sinkApps.Start (Seconds (0.0));
sinkApps.Stop (Seconds (10.0));

NS_LOG_INFO ("Run Simulation.");

AnimationInterface anim("l5q1.xml");
anim.SetConstantPosition ( csmaNodes.Get(0), 1, 2);
anim.SetConstantPosition (csmaNodes.Get(1), 2, 2);
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("l5q1.tr"));
Simulator::Stop (Seconds (10.0));
Simulator::Run ();
Simulator::Destroy ();
NS_LOG_INFO ("Done.");
Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;

  return 0;
}

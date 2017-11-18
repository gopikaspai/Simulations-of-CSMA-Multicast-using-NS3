// Network topology
//
//                     Lan1
//                 ===========
//                 |    |    | 
//       n0   n1   n2   n3   n4   n5   n6
//       |    |    |         |     |    |
//       ===========         ============
//           Lan0          Lan2


// - Multicast source is at node n0;
// - Multicast is forwarded by node n2 to LAN1 and by n4 to LAN2 ;
// - Nodes n0, n1, n2, n3, n4, n5 and n6 receive the multicast frame.
// - Node n6 listens for the data 

#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CsmaMulticastExample");

int 
main (int argc, char *argv[])
{	
  
  LogComponentEnable ("CsmaMulticastExample", LOG_LEVEL_INFO);

  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix"));//Selecting the encapsulation mode

  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  CommandLine cmd;               
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (7); // Creating Nodes 
  // We will later want three subcontainers for these nodes, for the three LANs
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2)); // Three nodes in each container
  NodeContainer c1 = NodeContainer (c.Get (2), c.Get (3), c.Get (4));
  NodeContainer c2 = NodeContainer (c.Get (4), c.Get (5), c.Get (6));

  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000))); // Setting the channel attributes data flow rate and time
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  // We will use these NetDevice containers later, for IP addressing
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN
  NetDeviceContainer nd2 = csma.Install (c2);  // Third LAN

  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c); 

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4Addr.Assign (nd0);
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4Addr.Assign (nd1);
  ipv4Addr.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4Addr.Assign (nd2);

  NS_LOG_INFO ("Configure multicasting.");
  //
  // Now we can configure multicasting.  As described above, the multicast 
  // source is at node zero, for which we assign the IP address as 10.1.1.1 
  // We need to define a multicast group to send packets too.  This
  // can be any multicast address from 224.0.0.0 through 239.255.255.255
  // (avoiding the reserved routing protocol addresses).
  //

  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  // Now, we will set up multicast routing.  We need to do three things:
  // 1) Configure a (static) multicast route on node n2
  // 2) Set up a default multicast route on the sender n0 
  // 3) Have node n4 join the multicast group
  // We have a helper that can help us with static multicast
  Ipv4StaticRoutingHelper multicast;

  // 1) Configure a (static) multicast route on node n2 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (2);  // Node 2 acts as a router
  Ptr<NetDevice> inputIf = nd0.Get (2);  // nd0: NetDeviceContainer 0 i.e LAN1, where n2 acts as an input interface for LAN2
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd1.Get (0));  // (we only need one NetDevice here)
  
   multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                            multicastGroup, inputIf, outputDevices);
  
   Ptr<Node> multicastRouter1 = c.Get (4);  // Node 4 acts as a router
  Ptr<NetDevice> inputIf1 = nd1.Get (4);  // nd1: NetDeviceContainer 1 i.e LAN2, where n4 acts as an input interface for LAN3
  NetDeviceContainer outputDevices1;  // A container of output NetDevices
  outputDevices1.Add (nd2.Get (0));  // (we only need one NetDevice here)
  
  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                            multicastGroup, inputIf, outputDevices);
      
   // 2) Set up a default multicast route on the sender n0 
  Ptr<Node> sender = c.Get (0); // Node 0 acts as the sender 
  Ptr<NetDevice> senderIf = nd0.Get (0); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
                            
    // 2) Set up a default multicast route on the sender n0 
  Ptr<Node> sender1 = c.Get (2); // Node 0 acts as the sender 
  Ptr<NetDevice> senderIf1 = nd1.Get (2); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
  // 2) Set up a default multicast route on the sender n0 
  Ptr<Node> sender2 = c.Get (4); // Node 0 acts as the sender 
  Ptr<NetDevice> senderIf2 = nd2.Get (4); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
                               
  //
  // Create an OnOff application to send UDP datagrams from node zero to the
  // multicast group (node six will be listening).
  //
  NS_LOG_INFO ("Create Applications.");

  uint16_t multicastPort = 9;   // Discard port (RFC 863)

  // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff ("ns3::UdpSocketFactory", Address (InetSocketAddress (multicastGroup, multicastPort)));
  onoff.SetConstantRate (DataRate ("255b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));

  ApplicationContainer srcC = onoff.Install (c0.Get (0));

  //
  // Tell the application when to start and stop.
  //
  srcC.Start (Seconds (1.));
  srcC.Stop (Seconds (10.));

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

  ApplicationContainer sinkC = sink.Install (c1.Get (2)); // Node n2
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (12.0));
  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Ascii trace output will be sent to the file "csma-multicast.tr"
  //
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("csma-multicast.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     csma-multicast-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
   csma.EnablePcapAll ("csma-multicast", true);

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

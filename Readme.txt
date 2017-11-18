                                           Simulations of CSMA-Multicast scenarios in NS3
     - Gopika S Pai                                      -----------------------------------------------


Abstract:
---------
Multicast is group communication where information is addressed to a group of destination computers simultaneously.Multicasting can allow you to reach many end users while utilizing only one data stream, decreasing the amount of bandwidth and saving resources.
CSMA(Carrier Sense Multiple Access)is the protocol for carrier transmission access in Ethernet networks. On Ethernet each device can try to send a frame at anytime. Each device senses if the line is idle and therefore ready to sned the frame.
Here, in each CSMA Multicast scenario, I have tried to implement different topologies in Multicast networks considering CSMA protocols.

Introduction:
-------------
There are three fundamental types of IPv4 addresses: unicast, broadcast, and multicast.A unicast address is designed to transmit a packet to a single destination. A broadcast address is used to send a datagram to an entire subnetwork. A multicast address is
designed to enable the delivery of datagrams to a set of hosts that have been configured as members of a multicast group in various scattered subnetworks.

     # Multicast:
                Multicast is group communication where information is addressed to a group of destination computers simultaneously.It is similar to broadcast in the sense it's target is a number of machines in the network, but not all.Multicast is only directed to a group of hosts in the network.The most common transport layer protocol to use multicast addressing is User Datagram Protocol (UDP). By its nature, UDP is not reliable—messages may be lost or delivered out of order.
                Multicast networks are used in softwares, such as:
                    * Whatsapp
                    * Sending Email
                    * Teleconferencing
                    * Videoconferencing
                and many more..
     # NS3 (Network Simulator 3): It is a free software, publicly available under the GNU GPLv2 license for research and development.
     
Simulation Scenario:
--------------------




  # Scenario 1:
  
  
  
  
  
  
  
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
   // For the users to find it convenient to turn on explicit debugging
  // for selected modules
  
  LogComponentEnable ("CsmaMulticastExample", LOG_LEVEL_INFO);
  
  //Selecting the encapsulation mode

  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix"));  

  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  
  CommandLine cmd;               
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  
  // Creating Nodes 
  
  c.Create (7); 
  
  
  // We will later want three subcontainers for these nodes, for the three LANs
  
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2)); // Three nodes in each container
  NodeContainer c1 = NodeContainer (c.Get (2), c.Get (3), c.Get (4));
  NodeContainer c2 = NodeContainer (c.Get (4), c.Get (5), c.Get (6));

  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  
  // Setting the channel attributes data flow rate and time
  
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000))); 
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  // We will use these NetDevice containers later, for IP addressing
  
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN
  NetDeviceContainer nd2 = csma.Install (c2);  // Third LAN

  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c); 

   // Assigning the IP Addresses

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
  // 3) Have node n6 join the multicast group
  // We have a helper that can help us with static multicast
  
  Ipv4StaticRoutingHelper multicast;

  // 1) Configure a (static) multicast route on node n4 (multicastRouter)
  
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
                            
    // 2) Set up a default multicast route on the sender n2 
  
  Ptr<Node> sender1 = c.Get (2); // Node 2 acts as the sender 
  Ptr<NetDevice> senderIf1 = nd1.Get (2); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
  // 2) Set up a default multicast route on the sender n4 
  
  Ptr<Node> sender2 = c.Get (4); // Node 4 acts as the sender 
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
  
  // Onoff acts as when to send the packets and when to block 
   
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
  Simulator::Run (); // To run the simulation
  Simulator::Destroy (); 
  NS_LOG_INFO ("Done.");
}
     
     
     
     
     
     
     
     
     
     
          # Scenario 2 :
     
     
     
     
     
     
     
     
     
     
     
     
     
// Network topology
//
//                                     Lan1
//                           ==========================
//                           |     |     |      |     |
//       n0   n1   n2   n3   n4    n5    n6     n7    n8
//       |    |    |    |    |
//       ======================
//           Lan0
//
// - Multicast source is at node n0;
// - Multicast forwarded by node n4 to LAN1;
// - Nodes n0, n1, n2, n3, n4, n5, n6, n7 and n8 receive the multicast frame.
// - Node n8 listens for the data 

.............................. (same as Scenario 1)


  NS_LOG_INFO ("Create nodes.");
  NodeContainer c; 
  
  c.Create (9);// Creating Nodes 
  
  // We will later want two subcontainers of these nodes, for the two LANs
  
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2), c.Get (3), c.Get (4));
  NodeContainer c1 = NodeContainer (c.Get (4), c.Get (5), c.Get (6), c.Get (7), c.Get (8));

  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));// Setting the channel attributes data flow rate and time
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  // We will use these NetDevice containers later, for IP addressing
  
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN

  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4Addr.Assign (nd0);
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4Addr.Assign (nd1);

  NS_LOG_INFO ("Configure multicasting.");
  
  //
  // Now we can configure multicasting.  As described above, the multicast 
  // source is at node zero, which we assigned the IP address of 10.1.1.1 
  // earlier.  We need to define a multicast group to send packets to.  This
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

  // 1) Configure a (static) multicast route on node n4 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (4);  // Node 4 acts as a router
  Ptr<NetDevice> inputIf = nd0.Get (4);  // nd0: NetDeviceContainer 0 i.e LAN1, where n4 acts as an input interface for LAN2
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd1.Get (0));  // (we only need one NetDevice here)

  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                               multicastGroup, inputIf, outputDevices);

  // 2) Set up a default multicast route on the sender n0 
  Ptr<Node> sender = c.Get (0); // Node n0 acts as the sender
  Ptr<NetDevice> senderIf = nd0.Get (0);
  multicast.SetDefaultMulticastRoute (sender, senderIf);

 .................(same as Scenario 1)











     # Scenario 3:
     
     
     
     
     
     
     
     
     
     
   
     
     
// Network topology
//
//                     Lan1                 Lan3                    Lan5
//                 ============        ==============         ================
//                 |    |    |          |          |           |      |     |
//       n0   n1   n2   n3   n4   n5   n6          n7   n8    n9     n10   n11   n12    n13     n14
//       |    |    |         |    |     |          |     |     |            |     |      |      |
//       ===========         ============          =============            ======================
//           Lan0              Lan2                     Lan4                       Lan6
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

  
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix")); //Selecting the encapsulation mode

  // Allow the user to override any of the defaults at
  // run-time, via command-line 
  
  CommandLine cmd;
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (15);// Creating Nodes 
  
  // We will later want seven subcontainers for these nodes, for the seven LANs, each with different number of nodes
  
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2));
  NodeContainer c1 = NodeContainer (c.Get (2), c.Get (3), c.Get (4));
  NodeContainer c2 = NodeContainer (c.Get (4), c.Get (5), c.Get (6));
  NodeContainer c3 = NodeContainer (c.Get (6), c.Get (7));
  NodeContainer c4 = NodeContainer (c.Get (7), c.Get (8), c.Get (9));
  NodeContainer c5 = NodeContainer (c.Get (9), c.Get (10), c.Get (11));
  NodeContainer c6 = NodeContainer (c.Get (11), c.Get (12), c.Get (13), c.Get(14));
  
  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));// Setting the channel attributes data flow rate and time
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  // We will use these NetDevice containers later, for IP addressing
  
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN
  NetDeviceContainer nd2 = csma.Install (c2);  // Third LAN
  NetDeviceContainer nd3 = csma.Install (c3);  // fourth LAN
  NetDeviceContainer nd4 = csma.Install (c4);  // Fifth LAN
  NetDeviceContainer nd5 = csma.Install (c5);  // Sixth LAN
  NetDeviceContainer nd6 = csma.Install (c6);  // Seventh LAN
  
  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c);

   // Assigning the IP addresses
   
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4Addr.Assign (nd0);
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4Addr.Assign (nd1);
  ipv4Addr.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4Addr.Assign (nd2);
  ipv4Addr.SetBase ("10.1.4.0", "255.255.255.0");
  ipv4Addr.Assign (nd3);
  ipv4Addr.SetBase ("10.1.5.0", "255.255.255.0");
  ipv4Addr.Assign (nd4);
  ipv4Addr.SetBase ("10.1.6.0", "255.255.255.0");
  ipv4Addr.Assign (nd5);
  ipv4Addr.SetBase ("10.1.7.0", "255.255.255.0");
  ipv4Addr.Assign (nd6);
  

  NS_LOG_INFO ("Configure multicasting.");
  //
  // Now we can configure multicasting.  As described above, the multicast 
  // source is at node zero, which we assigned the IP address of 10.1.1.1 
  // earlier.  We need to define a multicast group to send packets to.  This
  // can be any multicast address from 224.0.0.0 through 239.255.255.255
  // (avoiding the reserved routing protocol addresses).
  //

  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  // Now, we will set up multicast routing.  We need to do three things:
  // 1) Configure a (static) multicast route on node n2, n4, n6 , n7, n9,n11
  // 2) Set up a default multicast route on the sender n0,n2,n4,n6,n7,n9,n11
  // 3) Have node n11 join the multicast group
  // We have a helper that can help us with static multicast
  Ipv4StaticRoutingHelper multicast;

  // 1) Configure a (static) multicast route on node n2 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (2);   // Node 2 acts as a router
  Ptr<NetDevice> inputIf = nd0.Get (2);   // nd0: NetDeviceContainer 0 i.e LAN1, where n2 acts as an input interface for LAN2
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
    
    
   Ptr<Node> multicastRouter2 = c.Get (6);  // Node 6 acts as a router
  Ptr<NetDevice> inputIf2 = nd2.Get (6);  // nd2: NetDeviceContainer 2 i.e LAN3, where n6 acts as an input interface for LAN3
  NetDeviceContainer outputDevices2;  // A container of output NetDevices
  outputDevices2.Add (nd3.Get (0));  // (we only need one NetDevice here)
  
  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                            multicastGroup, inputIf, outputDevices);
  
   Ptr<Node> multicastRouter3 = c.Get (7);  // Node 7 acts as a router
  Ptr<NetDevice> inputIf3 = nd3.Get (7);  // nd3: NetDeviceContainer 3 i.e LAN4, where n7 acts as an input interface for LAN5
  NetDeviceContainer outputDevices3;  // A container of output NetDevices
  outputDevices3.Add (nd4.Get (0));  // (we only need one NetDevice here)
  
  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                            multicastGroup, inputIf, outputDevices);
                            
  Ptr<Node> multicastRouter4 = c.Get (9);  // Node 9 acts as a router
  Ptr<NetDevice> inputIf4 = nd4.Get (9);  // nd4: NetDeviceContainer 4 i.e LAN5, where n9 acts as an input interface for LAN6
  NetDeviceContainer outputDevices4;  // A container of output NetDevices
  outputDevices4.Add (nd5.Get (0));  // (we only need one NetDevice here)
  
  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                            multicastGroup, inputIf, outputDevices);
  
  Ptr<Node> multicastRouter5 = c.Get (11);  // Node 11 acts as a router
  Ptr<NetDevice> inputIf5 = nd5.Get (11);  // nd5: NetDeviceContainer 5 i.e LAN6
  NetDeviceContainer outputDevices5;  // A container of output NetDevices
  outputDevices5.Add (nd6.Get (0));  // (we only need one NetDevice here)
  
  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                            multicastGroup, inputIf, outputDevices);
    
                                
  // 2) Set up a default multicast route on the sender n0,n2,n4,n6,n7,n9,n11
  Ptr<Node> sender = c.Get (0);
  Ptr<NetDevice> senderIf = nd0.Get (0);
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
  Ptr<Node> sender1 = c.Get (2); // Node 2 acts as the sender 
  Ptr<NetDevice> senderIf1 = nd1.Get (2); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);


   Ptr<Node> sender2 = c.Get (4); // Node 4 acts as the sender 
  Ptr<NetDevice> senderIf2 = nd2.Get (4); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
  Ptr<Node> sender3 = c.Get (6); // Node 6 acts as the sender 
  Ptr<NetDevice> senderIf3 = nd3.Get (6); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
  Ptr<Node> sender4 = c.Get (7); // Node 7 acts as the sender 
  Ptr<NetDevice> senderIf4 = nd4.Get (7); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
  Ptr<Node> sender5 = c.Get (9); // Node 9 acts as the sender 
  Ptr<NetDevice> senderIf5 = nd5.Get (9); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);
  
    
  Ptr<Node> sender6 = c.Get (11); // Node 9 acts as the sender 
  Ptr<NetDevice> senderIf6 = nd6.Get (11); 
  multicast.SetDefaultMulticastRoute (sender, senderIf);



  //
  // Create an OnOff application to send UDP datagrams from node zero to the
  // multicast group (node four will be listening).
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
 

  NS_LOG_INFO ("Configure Tracing.");
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
  csma.EnablePcapAll ("csma-multicast", false);

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}









             # Scenario 4 :
             
             
             
             
             
             
             
             
             
  
  
 // Network topology
//
//                 n5   n6   LAN 2
//                 |     |       
//            =============
//            |        
//            |      Lan1
//            |     ===========
//            |     |    |    | 
//       n0   n1   n2   n3   n4
//       |    |    |
//       ===========
//           Lan0  
//                
//                     

// - Multicast source is at node n0;
// - Multicast forwarded by node n2 onto LAN1;
// - Nodes n0, n1, n2, n3, and n4 receive the multicast frame.
// - Node n4 listens for the data 

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

  
  
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix"));

  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (7);  //Creating the nodes
  // We will later want two subcontainers of these nodes, for the two LANs
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2));
  NodeContainer c2 = NodeContainer (c.Get (1), c.Get (5), c.Get (6));
  NodeContainer c1 = NodeContainer (c.Get (2), c.Get (3), c.Get (4));
  

  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
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
  // source is at node zero, which we assigned the IP address of 10.1.1.1 
  // earlier.  We need to define a multicast group to send packets to.  This
  // can be any multicast address from 224.0.0.0 through 239.255.255.255
  // (avoiding the reserved routing protocol addresses).
  //

  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  // Now, we will set up multicast routing.  We need to do three things:
  // 1) Configure a (static) multicast route on node n2
  // 2) Set up a default multicast route on the sender n0 
  // 3) Have node n4 and n6 join the multicast group
  // We have a helper that can help us with static multicast
  Ipv4StaticRoutingHelper multicast;

  /* 1) Configure a (static) multicast route on node n2 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (2);  // Node 2 acts as a router
  Ptr<NetDevice> inputIf = nd0.Get (2);  // nd0: NetDeviceContainer 0 i.e LAN1, where n2 acts as an input interface for LAN2
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd1.Get (0));  // (we only need one NetDevice here)*/
  
  // 1) Configure a (static) multicast route on node n1 (multicastRouter)
  Ptr<Node> multicastRouter1 = c.Get (1);  // Node 1 acts as a router
  Ptr<NetDevice> inputIf1 = nd0.Get (1);  // nd1: NetDeviceContainer 1 i.e LAN2, where n1 acts as an input interface for LAN3
  NetDeviceContainer outputDevices1;  // A container of output NetDevices
  outputDevices1.Add (nd2.Get (0));  // (we only need one NetDevice here)

  multicast.AddMulticastRoute (multicastRouter1, multicastSource, 
                               multicastGroup, inputIf1, outputDevices1);
                            

  // 2) Set up a default multicast route on the sender n0 
  Ptr<Node> sender = c.Get (0);
  Ptr<NetDevice> senderIf = nd0.Get (0);
  multicast.SetDefaultMulticastRoute (sender, senderIf);

  //
  // Create an OnOff application to send UDP datagrams from node zero to the
  // multicast group (node four will be listening).
  //
  NS_LOG_INFO ("Create Applications.");

  uint16_t multicastPort = 9;   // Discard port (RFC 863)

  // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (multicastGroup, multicastPort)));
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

  ApplicationContainer sinkC = sink.Install (c2.Get (2)); // Node n4 
  // Start the sink
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (10.0));

  NS_LOG_INFO ("Configure Tracing.");
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
  csma.EnablePcapAll ("csma-multicast", false);

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

             
             
             
             
             
             
             
             
    .....................Thank You..............
             
             
             
             
             
             
             
             
             
             
             
             
             
             
             
   
     
     
     
     
     

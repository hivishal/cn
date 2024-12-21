#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("StarTopologyExample");

int main (int argc, char *argv[])
{
    // Parse command line arguments
    CommandLine cmd (__FILE__);
    cmd.Parse (argc, argv);

    // Set simulation time resolution to nanoseconds
    Time::SetResolution (Time::NS);

    // Create 6 nodes (1 central node + 5 client nodes)
    NodeContainer nodes;
    nodes.Create (6);

    // Set up Point-to-Point link for star topology
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    // Install devices and set up links from central node (node 0) to other nodes (nodes 1 to 5)
    NetDeviceContainer devices[5];
    for (int i = 0; i < 5; ++i) {
        devices[i] = pointToPoint.Install (nodes.Get (0), nodes.Get (i + 1)); // Node 0 is central
    }

    // Install the Internet stack on all nodes
    InternetStackHelper stack;
    stack.Install (nodes);

    // Assign IP addresses to all devices
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces[5];
    for (int i = 0; i < 5; ++i) {
        interfaces[i] = address.Assign (devices[i]);
    }

    // Set up the UDP Echo Server on Node 0 (central node)
    UdpEchoServerHelper echoServer (9); // Server listens on port 9
    ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));

    // Set up UDP Echo Client applications on nodes 1 to 5 (client nodes)
    for (int i = 0; i < 5; ++i) {
        UdpEchoClientHelper echoClient (interfaces[i].GetAddress (0), 9); // Send to Node 0 (central)
        echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
        echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
        echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

        ApplicationContainer clientApps = echoClient.Install (nodes.Get (i + 1)); // Clients on nodes 1 to 5
        clientApps.Start (Seconds (2.0));
        clientApps.Stop (Seconds (10.0));
    }

    // Set up graphical animation using NetAnim
    AnimationInterface anim ("star_topology.xml");
    anim.SetConstantPosition (nodes.Get (0), 300, 300); // Central node at (300, 300)
    for (int i = 0; i < 5; ++i) {
        anim.SetConstantPosition (nodes.Get (i + 1), 100, 200 + (i * 100)); // Clients at different Y positions
    }

    // Enable Ascii tracing for packet analysis
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("trace.tr"));

    // Run the simulation
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}

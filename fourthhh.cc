#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    // Set simulation time resolution
    Time::SetResolution(Time::NS);

    // Create a NodeContainer for 5 nodes (example)
    NodeContainer nodes;
    nodes.Create(5);

    // Set up Point-to-Point links
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // Install devices and set up links between nodes in a bus topology
    NetDeviceContainer devices;
    for (uint32_t i = 0; i < nodes.GetN() - 1; ++i) {
        NetDeviceContainer linkDevices = pointToPoint.Install(nodes.Get(i), nodes.Get(i + 1));
        devices.Add(linkDevices);
    }

    // Install the Internet stack on all nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Create a UDP echo server application on Node 4 (last node in the bus)
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(4));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Create a UDP echo client application on Node 0 (first node in the bus)
    UdpEchoClientHelper echoClient(interfaces.GetAddress(4), 9); // Targeting Node 4
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Enable Ascii tracing for packet analysis
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("bus_topology.tr"));

    // Enable Pcap tracing
    pointToPoint.EnablePcapAll("bus_topology");

    // Enable NetAnim for graphical visualization
    AnimationInterface anim("bus_topology.xml");
    anim.SetConstantPosition(nodes.Get(0), 0, 0);  // Position Node 0 at (0, 0)
    anim.SetConstantPosition(nodes.Get(1), 5, 0);  // Position Node 1 at (5, 0)
    anim.SetConstantPosition(nodes.Get(2), 10, 0); // Position Node 2 at (10, 0)
    anim.SetConstantPosition(nodes.Get(3), 15, 0); // Position Node 3 at (15, 0)
    anim.SetConstantPosition(nodes.Get(4), 20, 0); // Position Node 4 at (20, 0)

    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    Time::SetResolution(Time::NS);
    
    // Create nodes
    NodeContainer nodes;
    nodes.Create(3);
    
    // Set up point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    
    // Create NetDeviceContainers for links
    NetDeviceContainer device0;
    device0 = p2p.Install(nodes.Get(0), nodes.Get(1)); // Connect node 0 to node 1

    NetDeviceContainer device1;
    device1 = p2p.Install(nodes.Get(2), nodes.Get(1)); // Connect node 2 to node 1
    
    // Install Internet stack on nodes
    InternetStackHelper stack;
    stack.Install(nodes);
    
    // Assign IP addresses to the links
    Ipv4AddressHelper address1;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface0 = address1.Assign(device0);

    Ipv4AddressHelper address2;
    address2.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface1 = address2.Assign(device1);
    
    // Set up UDP echo server on node 1
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApp = echoServer.Install(nodes.Get(1));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(10.0));
    
    // Set up UDP echo client on node 0
    UdpEchoClientHelper echoClient0(interface0.GetAddress(1), 9);
    echoClient0.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient0.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient0.SetAttribute("PacketSize", UintegerValue(1024));
    
    ApplicationContainer clientApp0 = echoClient0.Install(nodes.Get(0));
    clientApp0.Start(Seconds(2.0));
    clientApp0.Stop(Seconds(10.0));

    // Set up UDP echo client on node 2
    UdpEchoClientHelper echoClient1(interface1.GetAddress(1), 9);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));
    
    ApplicationContainer clientApp1 = echoClient1.Install(nodes.Get(2));
    clientApp1.Start(Seconds(2.0));
    clientApp1.Stop(Seconds(10.0));

    // Enable Ascii trace
    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll(ascii.CreateFileStream("s2c.tr"));
    
    // Set up animation
    AnimationInterface anim("2.xml");
    anim.SetConstantPosition(nodes.Get(0), 0, 10);
    anim.SetConstantPosition(nodes.Get(1), 10, 10);
    anim.SetConstantPosition(nodes.Get(2), 20, 10);
    
    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();
    
    return 0;
}


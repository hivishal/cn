#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"

using namespace ns3 ;
int main(int argc, char*argv[]){
	Time::SetResolution(Time::NS);
	NodeContainer nodes ;
	nodes.Create(2);
	
	PointToPointHelper pointToPoint ;
	pointToPoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));
	
	NetDeviceContainer device ;
	device = pointToPoint.Install(nodes);
	InternetStackHelper stack ;
	stack.Install(nodes);
	
	Ipv4AddressHelper address ;
	address.SetBase("10.1.1.0","255.255.255.0");
	Ipv4InterfaceContainer interfaces = address.Assign(device);
	
	UdpEchoServerHelper EchoServer(9);
	ApplicationContainer ServerApp = EchoServer.Install(nodes.Get(1));
	ServerApp.Start(Seconds(1.0));
	ServerApp.Stop(Seconds(10.0));
	
	UdpEchoClientHelper EchoClient(interfaces.GetAddress(1),9);
	EchoClient.SetAttribute("MaxPackets",UintegerValue(1));
	EchoClient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
	EchoClient.SetAttribute("PacketSize",UintegerValue(1024));
	
	ApplicationContainer ClientApp = EchoClient.Install(nodes.Get(0));
	ClientApp.Start(Seconds(2.0));
	ClientApp.Stop(Seconds(10.0));
	
	AsciiTraceHelper ascii;
	pointToPoint.EnableAsciiAll(ascii.CreateFileStream("first.tr"));
	
	
	AnimationInterface anim("first.xml");
	anim.SetConstantPosition(nodes.Get(0),10,10);
	anim.SetConstantPosition(nodes.Get(1),15,10);
	
	
	Simulator::Run();
	Simulator::Destroy() ;
	
	return 0 ;


}

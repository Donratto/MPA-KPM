/**-*-Mode:C++;c-file-style:"gnu";indent-tabs-mode:nil;-*-*/
/*
*Thisprogramisfreesoftware;youcanredistributeitand/ormodify
*itunderthetermsoftheGNUGeneralPublicLicenseversion2as
*publishedbytheFreeSoftwareFoundation;
*
*Thisprogramisdistributedinthehopethatitwillbeuseful,
*butWITHOUTANYWARRANTY;withouteventheimpliedwarrantyof
*MERCHANTABILITYorFITNESSFORAPARTICULARPURPOSE.Seethe
*GNUGeneralPublicLicenseformoredetails.
*
*YoushouldhavereceivedacopyoftheGNUGeneralPublicLicense
*alongwiththisprogram;ifnot,writetotheFreeSoftware
*Foundation,Inc.,59TemplePlace,Suite330,Boston,MA02111-1307USA
*/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main(int argc,char *argv[])
{
CommandLine cmd;
cmd.Parse(argc,argv);

Time::SetResolution(Time::NS);

//string logLevel = "LOG_LEVEL_DEBUG";
//LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_ERROR);LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_ERROR);
//LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_WARN);LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_WARN);
//LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_DEBUG);LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_DEBUG);
//LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
//LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_FUNCTION);LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_FUNCTION);
//LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_LOGIC);LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_LOGIC);
LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_ALL);LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_ALL);

NodeContainer nodes;
nodes.Create(2);

PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));

NetDeviceContainer devices;
devices=pointToPoint.Install(nodes);

InternetStackHelper stack;
stack.Install(nodes);

Ipv4AddressHelper address;
address.SetBase("10.1.1.0","255.255.255.0");

Ipv4InterfaceContainer interfaces=address.Assign(devices);
UdpEchoServerHelper echoServer(9);

ApplicationContainer serverApps=echoServer.Install(nodes.Get(1));
serverApps.Start(Seconds(1.0));
serverApps.Stop(Seconds(10.0));

UdpEchoClientHelper echoClient(interfaces.GetAddress(1),9);
echoClient.SetAttribute("MaxPackets",UintegerValue(1));
echoClient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
echoClient.SetAttribute("PacketSize",UintegerValue(1024));

ApplicationContainer clientApps=echoClient.Install(nodes.Get(0));
clientApps.Start(Seconds(2.0));
serverApps.Start(Seconds(1.0));
clientApps.Stop(Seconds(10.0));

Simulator::Run();
Simulator::Destroy();
return 0;
}

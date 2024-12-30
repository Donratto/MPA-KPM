/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
// main source of inspiration "cttc-nr-demo"
#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/buildings-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"

#include "ns3/nr-radio-environment-map-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3; 

/*
 * With this line, we will be able to see the logs of the file by enabling the
 * component "CttcNrDemo"
 */
NS_LOG_COMPONENT_DEFINE("CttcNrDemo");

int
main(int argc, char* argv[])
{
    std::string remMode = "None";
    std::string typeOfRem = "DlRem";
    
    // Scenario parameters (that we will use inside this script):
    uint16_t gNbNum = 2;
    uint16_t ueNum = 5;
    bool logging = true;
    bool doubleOperationalBand = true;
    bool xmlAnim = false;

    // Traffic parameters (that we will use inside this script):
    uint32_t udpPacketSizeULL = 100;
    uint32_t udpPacketSizeBe = 1252;
    uint32_t lambdaULL = 10000;
    uint32_t lambdaBe = 10000;

    // Simulation parameters. Please don't use double to indicate seconds; use
    // ns-3 Time values which use integers to avoid portability issues.
    Time simTime = MilliSeconds(1000);
    Time AppStartTime = MilliSeconds(400);

    // NR parameters. We will take the input from the command line, and then we
    // will pass them inside the NR module.
    uint16_t numerologyBwp1 = 4;
    double centralFrequencyBand1 = 26.5e9;
    double bandwidthBand1 = 100e6;
    uint16_t numerologyBwp2 = 2;
    double centralFrequencyBand2 = 27e9;
    double bandwidthBand2 = 100e6;
    double totalTxPower = 4;

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";

    /*
     * From here, we instruct the ns3::CommandLine class of all the input parameters
     * that we may accept as input, as well as their description, and the storage
     * variable.
     * to view these commands in cmd write ""./ns3 run scratch/<script_name> -- --PrintHelp"
     */
    CommandLine cmd(__FILE__);
    
    cmd.AddValue("gNbNum", "The number of gNbs in multiple-ue topology", gNbNum);
    cmd.AddValue("ueNum", "The number of UE in multiple-ue topology", ueNum);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.AddValue("doubleOperationalBand",
                 "If true, simulate two operational bands with one CC for each band,"
                 "and each CC will have 1 BWP that spans the entire CC.",
                 doubleOperationalBand);
    cmd.AddValue("packetSizeUll",
                 "packet size in bytes to be used by ultra low latency traffic",
                 udpPacketSizeULL);
    cmd.AddValue("packetSizeBe",
                 "packet size in bytes to be used by best effort traffic",
                 udpPacketSizeBe);
    cmd.AddValue("lambdaUll",
                 "Number of UDP packets in one second for ultra low latency traffic",
                 lambdaULL);
    cmd.AddValue("lambdaBe",
                 "Number of UDP packets in one second for best effor traffic",
                 lambdaBe);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("numerologyBwp1", "The numerology to be used in bandwidth part 1", numerologyBwp1);
    cmd.AddValue("centralFrequencyBand1",
                 "The system frequency to be used in band 1",
                 centralFrequencyBand1);
    cmd.AddValue("bandwidthBand1", "The system bandwidth to be used in band 1", bandwidthBand1);
    cmd.AddValue("numerologyBwp2", "The numerology to be used in bandwidth part 2", numerologyBwp2);
    cmd.AddValue("centralFrequencyBand2",
                 "The system frequency to be used in band 2",
                 centralFrequencyBand2);
    cmd.AddValue("bandwidthBand2", "The system bandwidth to be used in band 2", bandwidthBand2);
    cmd.AddValue("totalTxPower",
                 "total tx power that will be proportionally assigned to"
                 " bands, CCs and bandwidth parts depending on each BWP bandwidth ",
                 totalTxPower);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);
    cmd.AddValue("remMode",
                 "What type of REM map to use: BeamShape, CoverageArea, UeCoverage or None.\n"
                 "BeamShape shows beams that are configured in a user's script. "
                 "Coverage area is used to show worst-case SINR and best-case SNR maps "
                 "considering that at each point of the map the best beam is used "
                 "towards that point from the serving gNB and also of all the interfering"
                 "gNBs in the case of worst-case SINR."
                 "UeCoverage is similar to the previous, just that it is showing the "
                 "uplink coverage.",
                 remMode);
    cmd.AddValue("typeOfRem",
                 "The type of Rem to generate (DL or UL) in the case of BeamShape option. Choose among "
                 "'DlRem', 'UlRem'.",typeOfRem);
    cmd.AddValue("xmlAnim",
                 "Set 'true' to generate xml file describing communication network for NetAnim,"
                 "'false' will generate only overview, no metadata",xmlAnim);

                 

    // Parse the command line
    cmd.Parse(argc, argv);

    /*
     * Check if the frequency is in the allowed range.
     * If you need to add other checks, here is the best position to put them.
     */
    NS_ABORT_IF(centralFrequencyBand1 < 0.5e9 && centralFrequencyBand1 > 100e9);
    NS_ABORT_IF(centralFrequencyBand2 < 0.5e9 && centralFrequencyBand2 > 100e9);

    if (logging)
    {
        LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
        LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
        LogComponentEnable("LtePdcp", LOG_LEVEL_INFO);
    }
    

    /*
     * Default values for the simulation. We are progressively removing all
     * the instances of SetDefault, but we need it for legacy code (LTE)
     */
    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    /*
     * Create the scenario. In our examples, we heavily use helpers that setup
     * the gnbs and ue following a pre-defined pattern. Please have a look at the
     * GridScenarioHelper documentation to see how the nodes will be distributed.
     */
    int64_t randomStream = 1;
    GridScenarioHelper gridScenario;
    gridScenario.SetRows(2);
    gridScenario.SetColumns(gNbNum);
    gridScenario.SetHorizontalBsDistance(10.0);
    gridScenario.SetVerticalBsDistance(5.0);
    gridScenario.SetBsHeight(1.5);
    gridScenario.SetUtHeight(1.5);
    // must be set before BS number
    gridScenario.SetSectorization(GridScenarioHelper::SINGLE);
    gridScenario.SetBsNumber(gNbNum);
    gridScenario.SetUtNumber(ueNum);
    gridScenario.SetScenarioHeight(3); // Create a 3x3 scenario where the UE will
    gridScenario.SetScenarioLength(3); // be distribuited.
    randomStream += gridScenario.AssignStreams(randomStream);
    gridScenario.CreateScenario();

    /*
     * Create one NodeContainer for UEs.
     */
    NodeContainer ueContainer;

    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Node> ue = gridScenario.GetUserTerminals().Get(j);
        ueContainer.Add(ue);
    }

    /*
     * TODO: Add a print, or a plot, that shows the scenario.
     */

    /*
     * Setup the NR module. We create the various helpers needed for the
     * NR simulation:
     * - EpcHelper, which will setup the core network
     * - IdealBeamformingHelper, which takes care of the beamforming part
     * - NrHelper, which takes care of creating and connecting the various
     * part of the NR stack
     */
    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();

    // Put the pointers inside nrHelper
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(epcHelper);

    /*
     * Spectrum division. We create two operational bands, each of them containing
     * one component carrier, and each CC containing a single bandwidth part
     * centered at the frequency specified by the input parameters.
     * Each spectrum part length is, as well, specified by the input parameters.
     * Both operational bands will use the StreetCanyon channel modeling.
     */
    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1; // in this example, both bands have a single CC

    // Create the configuration for the CcBwpHelper. SimpleOperationBandConf creates
    // a single BWP per CC
    CcBwpCreator::SimpleOperationBandConf bandConf1(centralFrequencyBand1,
                                                    bandwidthBand1,
                                                    numCcPerBand,
                                                    BandwidthPartInfo::UMi_StreetCanyon);
    CcBwpCreator::SimpleOperationBandConf bandConf2(centralFrequencyBand2,
                                                    bandwidthBand2,
                                                    numCcPerBand,
                                                    BandwidthPartInfo::UMi_StreetCanyon);

    // By using the configuration created, it is time to make the operation bands
    OperationBandInfo band1 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf1);
    OperationBandInfo band2 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf2);

    /*
     * The configured spectrum division is:
     * ------------Band1--------------|--------------Band2-----------------
     * ------------CC1----------------|--------------CC2-------------------
     * ------------BWP1---------------|--------------BWP2------------------
     */

    /*
     * Attributes of ThreeGppChannelModel still cannot be set in our way.
     * TODO: Coordinate with Tommaso
     */
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    nrHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));

    /*
     * Initialize channel and pathloss, plus other things inside band1. If needed,
     * the band configuration can be done manually, but we leave it for more
     * sophisticated examples. For the moment, this method will take care
     * of all the spectrum initialization needs.
     */
    nrHelper->InitializeOperationBand(&band1);

    /*
     * Start to account for the bandwidth used by the example, as well as
     * the total power that has to be divided among the BWPs.
     */
    double x = pow(10, totalTxPower / 10);
    double totalBandwidth = bandwidthBand1;

    /*
     * if not single band simulation, initialize and setup power in the second band
     */
    if (doubleOperationalBand)
    {
        // Initialize channel and pathloss, plus other things inside band2
        nrHelper->InitializeOperationBand(&band2);
        totalBandwidth += bandwidthBand2;
        allBwps = CcBwpCreator::GetAllBwps({band1, band2});
    }
    else
    {
        allBwps = CcBwpCreator::GetAllBwps({band1});
    }

    /*
     * allBwps contains all the spectrum configuration needed for the nrHelper.
     *
     * Now, we can setup the attributes. We can have three kind of attributes:
     * (i) parameters that are valid for all the bandwidth parts and applies to
     * all nodes, (ii) parameters that are valid for all the bandwidth parts
     * and applies to some node only, and (iii) parameters that are different for
     * every bandwidth parts. The approach is:
     *
     * - for (i): Configure the attribute through the helper, and then install;
     * - for (ii): Configure the attribute through the helper, and then install
     * for the first set of nodes. Then, change the attribute through the helper,
     * and install again;
     * - for (iii): Install, and then configure the attributes by retrieving
     * the pointer needed, and calling "SetAttribute" on top of such pointer.
     *
     */

    Packet::EnableChecking();
    Packet::EnablePrinting();

    /*
     *  Case (i): Attributes valid for all the nodes
     */
    // Beamforming method
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(DirectPathBeamforming::GetTypeId()));

    // Core latency
    epcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    uint32_t bwpIdForLowLat = 0;
    //uint32_t bwpIdForVoice = 0;
    if (doubleOperationalBand)
    {
        //bwpIdForVoice = 1;
        bwpIdForLowLat = 0;
    }

    // gNb routing between Bearer and bandwidh part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB",
                                                 UintegerValue(bwpIdForLowLat));
   //nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForLowLat));
   // nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    /*
     * We miss many other parameters. By default, not configuring them is equivalent
     * to use the default values. Please, have a look at the documentation to see
     * what are the default values for all the attributes you are not seeing here.
     */

    /*
     * Case (ii): Attributes valid for a subset of the nodes
     */

    // NOT PRESENT IN THIS SIMPLE EXAMPLE

    /*
     * We have configured the attributes we needed. Now, install and get the pointers
     * to the NetDevices, which contains all the NR stack:
     */

    NetDeviceContainer gnbNetDev =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice(ueContainer, allBwps);

    randomStream += nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);
    /*
     * Case (iii): Go node for node and change the attributes we have to setup
     * per-node.
     */

    // Get the first netdevice (gnbNetDev.Get (0)) and the first bandwidth part (0)
    // and set the attribute.
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)
        ->SetAttribute("Numerology", UintegerValue(numerologyBwp1));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)
        ->SetAttribute("TxPower", DoubleValue(10 * log10((bandwidthBand1 / totalBandwidth) * x)));

    if (doubleOperationalBand)
    {
        // Get the first netdevice (gnbNetDev.Get (0)) and the second bandwidth part (1)
        // and set the attribute.
        nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)
            ->SetAttribute("Numerology", UintegerValue(numerologyBwp2));
        nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)
            ->SetTxPower(10 * log10((bandwidthBand2 / totalBandwidth) * x));
    }

    // When all the configuration is done, explicitly call UpdateConfig ()

    for (auto it = gnbNetDev.Begin(); it != gnbNetDev.End(); ++it)
    {
        DynamicCast<NrGnbNetDevice>(*it)->UpdateConfig();
    }

    for (auto it = ueNetDev.Begin(); it != ueNetDev.End(); ++it)
    {
        DynamicCast<NrUeNetDevice>(*it)->UpdateConfig();
    }


    // From here, it is standard NS3. In the future, we will create helpers
    // for this part as well.

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(gridScenario.GetUserTerminals());

    Ipv4InterfaceContainer ueIpIface =
        epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            gridScenario.GetUserTerminals().Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to the closest eNB
    nrHelper->AttachToClosestEnb(ueNetDev, gnbNetDev);
    /*
    nrHelper->AttachToEnb(ueNetDev.Get(0),gnbNetDev.Get(0));
    nrHelper->AttachToEnb(ueNetDev.Get(1),gnbNetDev.Get(0));
    nrHelper->AttachToEnb(ueNetDev.Get(2),gnbNetDev.Get(0));
    nrHelper->AttachToEnb(ueNetDev.Get(3),gnbNetDev.Get(1));
    nrHelper->AttachToEnb(ueNetDev.Get(4),gnbNetDev.Get(1));
    */
    
    /*
     * Traffic part. Install low-latency  traffic.
     */
    uint16_t dlPort = 1234;
    uint16_t exchangePort = 1235;

    ApplicationContainer serverApps;

    PacketSinkHelper tcpSinkHelper("ns3::TcpSocketFactory",
                               InetSocketAddress(Ipv4Address::GetAny(), dlPort));
    serverApps.Add(tcpSinkHelper.Install(ueContainer));

    // The bearer that will carry low latency traffic
    EpsBearer lowLatBearer(EpsBearer::NGBR_LOW_LAT_EMBB);

    // The filter for the low-latency traffic
    Ptr<EpcTft> lowLatTft = Create<EpcTft>();
    EpcTft::PacketFilter dlpfLowLat;
    dlpfLowLat.localPortStart = dlPort;
    dlpfLowLat.localPortEnd = dlPort;
    lowLatTft->Add(dlpfLowLat);


    /*
     * Let's install the applications!
     */
    ApplicationContainer clientApps;

    for (uint32_t i = 0; i < ueContainer.GetN()-2; ++i)
    {
        Ptr<Node> ue = ueContainer.Get(i);
        Ptr<NetDevice> ueDevice = ueNetDev.Get(i);
        Ipv4Address ueAddress = ueIpIface.GetAddress(i);

        BulkSendHelper tcpClientHelper("ns3::TcpSocketFactory",
                               InetSocketAddress(ueAddress, dlPort));
        tcpClientHelper.SetAttribute("MaxBytes", UintegerValue(0)); // Send unlimited data

        // The client, who is transmitting, is installed in the remote host,
        // with destination address set to the address of the UE
        clientApps.Add(tcpClientHelper.Install(remoteHost));

        // Activate a dedicated bearer for the traffic type
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, lowLatBearer, lowLatTft);
    }



    // File exchange ue 4/5
    BulkSendHelper fileExchangeSender("ns3::TcpSocketFactory", InetSocketAddress(ueIpIface.GetAddress(ueContainer.GetN()-1), exchangePort));
    fileExchangeSender.SetAttribute("MaxBytes", UintegerValue(0)); // Unlimited
    ApplicationContainer exchangeApps = fileExchangeSender.Install(ueContainer.Get(ueContainer.GetN()-2));

    PacketSinkHelper fileExchangeSink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), exchangePort));
    ApplicationContainer exchangeSinkApps = fileExchangeSink.Install(ueContainer.Get(ueContainer.GetN()-1));
    


    /*
    */
    // start TCP server and client apps
    serverApps.Start(AppStartTime);
    clientApps.Start(AppStartTime);
    exchangeApps.Start((AppStartTime));
    exchangeSinkApps.Start(AppStartTime);
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);
    exchangeApps.Stop(simTime);
    exchangeSinkApps.Stop(simTime);

    // enable the traces provided by the nr module
    //nrHelper->EnableTraces();

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(gridScenario.GetUserTerminals());
    endpointNodes.Add(gridScenario.GetBaseStations());

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

//
// Animation
//snaha schovani tvorby animace za if, ale to uplne nefuguje
//pouze se vytvori mapa s pozicemi zarizeni
    
    
        //nrHelper->EnableTraces(); //required in order to show simulated communication

        unsigned long long testValue = 0xFFFFFFFFFFFFFFFF;
        AnimationInterface::SetConstantPosition(remoteHost, 10, 7.5);
        AnimationInterface::SetConstantPosition(pgw, 10, 5);
        AnimationInterface::SetConstantPosition(NodeList::GetNode(8),5,5);
        AnimationInterface::SetConstantPosition(NodeList::GetNode(9), 0,5);
        AnimationInterface anim("xmlAnim.xml");
        for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Node> n = *i;
        anim.UpdateNodeSize(n,0.25,0.25);
    }

        anim.UpdateNodeDescription(8, "SGW");
        anim.UpdateNodeDescription(9, "MME");
        anim.UpdateNodeColor(10, 255, 200, 0);
        /// Optional steps
        anim.SetMobilityPollInterval (Seconds (1)); // step 1
        //anim.EnablePacketMetadata(true); // step 5

        //*R
        anim.SetMaxPktsPerTraceFile(testValue);
        anim.UpdateNodeDescription(pgw, "PGW");
        anim.UpdateNodeDescription(remoteHost, "Remote_Host");

        for (uint32_t u = 0; u < ueNetDev.GetN(); ++u)
        {
            anim.UpdateNodeDescription(ueContainer.Get(u), "ue_" + std::to_string(u));
            //AnimationInterface::SetConstantPosition(ueContainer.Get(u), 10 + 15 * u, 20);
            anim.UpdateNodeColor(ueContainer.Get(u), 127, 127, 127); // Optional
        }

        for (uint32_t u = 0; u < gnbNetDev.GetN(); ++u)
        {
            anim.UpdateNodeDescription(gridScenario.GetBaseStations().Get(u), "gnb_" + std::to_string(u));
            //AnimationInterface::SetConstantPosition(gridScenario.GetBaseStations().Get(u), 10 + 40 * u, 30);
            anim.UpdateNodeColor(gridScenario.GetBaseStations().Get(u), 0, 255, 0); // Optional
            //AnimationInterface::SetConstantPosition(gridScenario.GetBaseStations().Get(u), 10 + 40 * u, 30);
        }
        if (xmlAnim != false){
            nrHelper->EnableTraces();
            anim.EnablePacketMetadata(true);
    }

        //access to nodes  via indexes
/*
        NS_LOG_UNCOND("PGW");
        NS_LOG_UNCOND(epcHelper->GetPgwNode());
        NS_LOG_UNCOND(NodeList::GetNode(7));
        NS_LOG_UNCOND("SGW");
        NS_LOG_UNCOND(epcHelper->GetSgwNode());
        NS_LOG_UNCOND(NodeList::GetNode(8));
        NS_LOG_UNCOND("MME");
        NS_LOG_UNCOND(NodeList::GetNode(9));
        for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Node> n = *i;
        uint16_t it = std::distance( NodeList::Begin(), i );
        NS_LOG_UNCOND(it);
        NS_LOG_UNCOND(n);
    }
 */
    
    





    if (remMode != "None")
    {
    
        
// Rem parameters
        double xMin = -40.0;
        double xMax = 80.0;
        uint16_t xRes = 50;
        double yMin = -70.0;
        double yMax = 50.0;
        uint16_t yRes = 50;
        double z = 1.5;


         uint16_t remBwpId = 0;
        // Radio Environment Map Generation for ccId 0
        Ptr<NrRadioEnvironmentMapHelper> remHelper = CreateObject<NrRadioEnvironmentMapHelper>();
        remHelper->SetMinX(xMin);
        remHelper->SetMaxX(xMax);
        remHelper->SetResX(xRes);
        remHelper->SetMinY(yMin);
        remHelper->SetMaxY(yMax);
        remHelper->SetResY(yRes);
        remHelper->SetZ(z);
        remHelper->SetSimTag(simTag);

        gnbNetDev.Get(0)
            ->GetObject<NrGnbNetDevice>()
            ->GetPhy(remBwpId)
            ->GetSpectrumPhy()
            ->GetBeamManager()
            ->ChangeBeamformingVector(ueNetDev.Get(0));


            gnbNetDev.Get(1)  //ueNetDev, gnbNetDev
                ->GetObject<NrGnbNetDevice>()
                ->GetPhy(remBwpId)
                ->GetSpectrumPhy()
                ->GetBeamManager()
                ->ChangeBeamformingVector(ueNetDev.Get(1));

        /*
        */
        if (remMode == "BeamShape")
        {
            remHelper->SetRemMode(NrRadioEnvironmentMapHelper::BEAM_SHAPE);

            if (typeOfRem == "DlRem")
            {
                remHelper->CreateRem(gnbNetDev, ueNetDev.Get(0), remBwpId);
            }
            else if (typeOfRem == "UlRem")
            {
                remHelper->CreateRem(ueNetDev, gnbNetDev.Get(0), remBwpId);
            }
            else
            {
                NS_ABORT_MSG("typeOfRem not supported. "
                             "Choose among 'DlRem', 'UlRem'.");
            }
        }
        else if (remMode == "CoverageArea")
        {
            remHelper->SetRemMode(NrRadioEnvironmentMapHelper::COVERAGE_AREA);
            remHelper->CreateRem(gnbNetDev, ueNetDev.Get(0), remBwpId);


        }
        else if (remMode == "UeCoverage")
        {
            remHelper->SetRemMode(NrRadioEnvironmentMapHelper::UE_COVERAGE);
            remHelper->CreateRem(ueNetDev, gnbNetDev.Get(0), remBwpId);
        }
        else
        {
            NS_ABORT_MSG("Not supported remMode.");
        }
        }


    Simulator::Stop(simTime);
    Simulator::Run();

    /*
     * To check what was installed in the memory, i.e., BWPs of eNb Device, and its configuration.
     * Example is: Node 1 -> Device 0 -> BandwidthPartMap -> {0,1} BWPs -> NrGnbPhy -> Numerology,
    GtkConfigStore config;
    config.ConfigureAttributes ();
    */

    // Print per-flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;
    double averageFlowPacketLoss = 0.0;

    std::ofstream outFile;
    std::string filename = outputDir + "/" + simTag;
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    double flowDuration = (simTime - AppStartTime).GetSeconds();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
       Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }
        outFile << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "
                << t.destinationAddress << ":" << t.destinationPort << ") proto "
                << protoStream.str() << "\n";
        outFile << "  Tx Packets: " << i->second.txPackets << "\n";
        outFile << "  Tx Bytes:   " << i->second.txBytes << "\n";
        outFile << "  TxOffered:  " << i->second.txBytes * 8.0 / flowDuration / 1000.0 / 1000.0
                << " Mbps\n";
        outFile << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            averageFlowThroughput += i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;
            averageFlowPacketLoss += (((i->second.txPackets - i->second.rxPackets) * 1.0) 
                                            / i->second.txPackets) * 100;

            outFile << "  Throughput: " << i->second.rxBytes * 8.0 / flowDuration / 1000 / 1000
                    << " Mbps\n";
            outFile << "  Mean delay:  "
                    << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            // outFile << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << "
            // Mbps \n";
            outFile << "  Mean jitter:  "
                    << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
            outFile << "  Lost Packets: " << i->second.txPackets - i->second.rxPackets << std::endl;
            outFile << "  Packet loss: " << (((i->second.txPackets - i->second.rxPackets) * 1.0) 
                                            / i->second.txPackets) * 100 << "%" << std::endl;
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms\n";
            outFile << "  Mean jitter: 0 ms\n";
            outFile << "  Lost Packets: 0";
            outFile << "  Packet loss: 0";
        }
        outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }

    double meanFlowThroughput = averageFlowThroughput / stats.size();
    double meanFlowDelay = averageFlowDelay / stats.size();
    double meanFlowPacketLoss = averageFlowPacketLoss / stats.size();

    outFile << "\n\n  Mean flow throughput: " << meanFlowThroughput << "\n";
    outFile << "  Mean flow delay: " << meanFlowDelay << "\n";
    outFile << "  Mean flow packet loss: " << meanFlowPacketLoss << "%\n";

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();

    if (argc == 0)
    {
        double toleranceMeanFlowThroughput = 0.0001 * 56.258560;
        double toleranceMeanFlowDelay = 0.0001 * 0.553292;

        if (meanFlowThroughput >= 56.258560 - toleranceMeanFlowThroughput &&
            meanFlowThroughput <= 56.258560 + toleranceMeanFlowThroughput &&
            meanFlowDelay >= 0.553292 - toleranceMeanFlowDelay &&
            meanFlowDelay <= 0.553292 + toleranceMeanFlowDelay)
        {
            return EXIT_SUCCESS;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    else if (argc == 1 and ueNum == 9) // called from examples-to-run.py with these parameters
    {
        double toleranceMeanFlowThroughput = 0.0001 * 47.858536;
        double toleranceMeanFlowDelay = 0.0001 * 10.504189;

        if (meanFlowThroughput >= 47.858536 - toleranceMeanFlowThroughput &&
            meanFlowThroughput <= 47.858536 + toleranceMeanFlowThroughput &&
            meanFlowDelay >= 10.504189 - toleranceMeanFlowDelay &&
            meanFlowDelay <= 10.504189 + toleranceMeanFlowDelay)
        {
            return EXIT_SUCCESS;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    else
    {
        return EXIT_SUCCESS; // we dont check other parameters configurations at the moment
    }
}

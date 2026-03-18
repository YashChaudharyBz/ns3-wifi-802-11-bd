#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"

#include "ns3/wave-bd-mac-helper.h"
#include "ns3/wifi-80211bd-helper.h"
#include "ns3/yans-wifi-bd-helper.h"

#include <map>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace ns3;

// std::map<uint32_t, std::ofstream*> g_txFiles;
// std::map<uint32_t, std::ofstream*> g_rxFiles;

std::map<uint32_t, uint32_t> g_voTxCount;
std::map<std::pair<uint32_t, uint32_t>, uint32_t> g_voRxCount;
std::map<std::pair<uint32_t, uint32_t>, double> g_voDelay;
std::map<uint32_t, uint32_t> g_viTxCount;
std::map<std::pair<uint32_t, uint32_t>, uint32_t> g_viRxCount;
std::map<std::pair<uint32_t, uint32_t>, double> g_viDelay;


// void CreateFolder(std::string path) {
//     if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
//         if (errno != EEXIST) {
//             NS_FATAL_ERROR("Could not create directory: " << path);
//         }
//     }
// }

// std::string g_resultsFolder = "results_default";

// void CheckAndOpenFile(uint32_t nodeId) {
//     if (g_txFiles.find(nodeId) == g_txFiles.end()) {
//         std::stringstream ssTx, ssRx;
//         // Files are now created INSIDE the factors folder
//         ssTx << g_resultsFolder << "/node-" << nodeId << "-tx.csv";
//         ssRx << g_resultsFolder << "/node-" << nodeId << "-rx.csv";
        
//         g_txFiles[nodeId] = new std::ofstream(ssTx.str().c_str(), std::ios::out);
//         g_rxFiles[nodeId] = new std::ofstream(ssRx.str().c_str(), std::ios::out);
        
//         *g_txFiles[nodeId] << "Serial,Sender,Time\n";
//         *g_rxFiles[nodeId] << "Serial,Sender,Receiver,Repeat,txTime,rxTime\n";
//     }
// }

// TX Sink: Always writes to the Sender's file
void EnqueueSink (uint32_t seq, uint32_t sender, int ac) {
    // CheckAndOpenFile(sender);
    // *g_txFiles[sender] << seq << "," << sender << "," 
    //                    << Simulator::Now().GetSeconds() << "\n";
    if(ac == AC_VO) {
        g_voTxCount[sender] ++;
    }
    else if(ac == AC_VI) {
        g_viTxCount[sender] ++;
    }
    // else {
    //     printf("### %d, %d: %d", seq, sender, ac);
    // }
}

// RX Sink: Always writes to the Receiver's file
void DequeueSink (uint32_t seq, uint32_t sender, uint32_t receiver, double txTime, double rxTime, bool isRep, int ac) {
    // CheckAndOpenFile(receiver);
    // *g_rxFiles[receiver] << seq << "," << sender << "," << receiver << "," 
    //                      << isRep << "," << txTime << "," << rxTime << "\n";
    if(!isRep) {
        if(ac == AC_VO) {
            g_voRxCount[{sender, receiver}] ++;
            g_voDelay[{sender, receiver}] += (rxTime - txTime);
        }
        else if(ac == AC_VI) {
            g_viRxCount[{sender, receiver}] ++;
            g_viDelay[{sender, receiver}] += (rxTime - txTime);
        }
        // else {
        //     printf("### %d, %d: %d", seq, sender, ac);
        // }
    }
}



// Global function to schedule randomized Poisson traffic for AC_VO
void ScheduleNextPoisson (Ptr<Socket> socket, uint32_t pktSize, double lambda) {
    Ptr<Packet> pkt = Create<Packet> (pktSize / 8); // Convert bits to bytes
    socket->SetIpTos (0xe0); // AC_VO
    socket->Send (pkt);

    // Exponential inter-arrival time for Poisson process: 1/lambda
    Ptr<ExponentialRandomVariable> x = CreateObject<ExponentialRandomVariable> ();
    x->SetAttribute ("Mean", DoubleValue (1.0 / lambda));
    Time nextInterval = Seconds (x->GetValue ());

    Simulator::Schedule (nextInterval, &ScheduleNextPoisson, socket, pktSize, lambda);
}

// Global function to schedule Deterministic traffic for AC_VI
void ScheduleNextDeterministic (Ptr<Socket> socket, uint32_t pktSize, double lambda) {
    Ptr<Packet> pkt = Create<Packet> (pktSize / 8); 
    socket->SetIpTos (0xa0); // AC_VI
    socket->Send (pkt);

    // Deterministic interval: 1/lambda
    Time nextInterval = Seconds (1.0 / lambda);
    Simulator::Schedule (nextInterval, &ScheduleNextDeterministic, socket, pktSize, lambda);
}


void RecreateExp(uint32_t nVehicles, double lambda0, double lambda1, uint32_t packetSizeBits, double dataRateMbps, std::ofstream *outFile) {
    // LogComponentEnable("OcbWifiMacBd", LOG_LEVEL_ALL);
    // Default values

    
    // std::stringstream folderName;
    // folderName << "scratch/80211bd-experiment/" << "exp_N" << nVehicles << "_L0_" << lambda0 << "_L1_" << lambda1 << "_P" << packetSizeBits;
    // g_resultsFolder = folderName.str();
    // CreateFolder(g_resultsFolder);
    
    NodeContainer vehicles;
    vehicles.Create (nVehicles);

    // 2. Mobility: Random Rectangle Area for N vehicles
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"),
                                  "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=6.0]"));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (vehicles);

    // 3. 802.11bd PHY/MAC Setup
    YansWifiChannelHelper channelHelper = YansWifiChannelHelper::Default ();
    Ptr<YansWifiChannel> channel = channelHelper.Create();
    Ptr<TwoRayGroundPropagationLossModel> loss = CreateObject<TwoRayGroundPropagationLossModel>();
    loss->SetFrequency(5.86e9);
    loss->SetMinDistance(1.0);
    loss->SetHeightAboveZ(1.5);
    loss->SetSystemLoss(3.0);
    channel->SetPropagationLossModel(loss);
    YansWifiPhyBdHelper phy;
    phy.Set ("TxPowerStart", DoubleValue (20.0));
    phy.Set ("TxPowerEnd", DoubleValue (20.0));
    phy.SetChannel (channel);

    Wifi80211bdHelper wifi80211bd = Wifi80211bdHelper::Default ();
    std::stringstream ssRate;
    ssRate << "OfdmRate" << dataRateMbps << "MbpsBW10MHz"; 
    wifi80211bd.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                        "DataMode", StringValue (ssRate.str ()),
                                        "ControlMode", StringValue (ssRate.str ()),
                                        "NonUnicastMode", StringValue(ssRate.str ()));

    QosBdWaveMacHelper mac = QosBdWaveMacHelper::Default ();
    mac.SetType("ns3::OcbWifiMacBd", "Rep_VO", IntegerValue(3), "Rep_VI", IntegerValue(3));
    NetDeviceContainer devices = wifi80211bd.Install (phy, mac, vehicles);


    // *. Trace for Send/Recieve
    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/Mac/$ns3::OcbWifiMacBd/WaveBdMacTx", 
                                MakeCallback (&EnqueueSink));
    Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/Mac/$ns3::OcbWifiMacBd/WaveBdMacRx", 
                                MakeCallback (&DequeueSink));

    // 4. Stack & IP
    InternetStackHelper stack;
    stack.Install (vehicles);
    Ipv4AddressHelper address;
    // Should allow 1022 addresses
    address.SetBase ("10.1.0.0", "255.255.252.0");
    address.Assign (devices);

    // 5. Setup Sockets for every vehicle
    uint16_t port = 9000;
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

    for (uint32_t i = 0; i < nVehicles; ++i) {
        // Broadcaster Socket
        Ptr<Socket> sourceSocket = Socket::CreateSocket (vehicles.Get (i), tid);
        sourceSocket->Connect (InetSocketAddress (Ipv4Address::GetBroadcast (), port));
        sourceSocket->SetAllowBroadcast (true);

        // Schedule first packets with a small random jitter to avoid massive t=0 collisions
        Ptr<UniformRandomVariable> jitter = CreateObject<UniformRandomVariable> ();
        Simulator::Schedule (Seconds (jitter->GetValue (0.1, 1.0)), 
                            &ScheduleNextPoisson, sourceSocket, packetSizeBits, lambda0);
        Simulator::Schedule (Seconds (jitter->GetValue (0.1, 1.0)), 
                            &ScheduleNextDeterministic, sourceSocket, packetSizeBits, lambda1);
    }

    // 6. Run
    Simulator::Stop (Seconds (10));
    Simulator::Run ();

    // for (auto const& [id, file] : g_txFiles) {
    //     file->close();
    //     delete file;
    // }
    // for (auto const& [id, file] : g_rxFiles) {
    //     file->close();
    //     delete file;
    // }

    Simulator::Destroy ();


    uint32_t totalVoTransmissions = 0, totalViTransmissions = 0;
    uint32_t totalVoReceptions = 0, totalViReceptions = 0;
    double totalVoDelaySum = 0, totalViDelaySum = 0;
    for (auto const& [nodeId, count] : g_voTxCount) {
        totalVoTransmissions += count;
    }
    for (auto const& [pair, count] : g_voRxCount) {
        totalVoReceptions += count;
    }
    for (auto const& [pair, delay] : g_voDelay) {
        totalVoDelaySum += delay;
    }
    for (auto const& [nodeId, count] : g_viTxCount) {
        totalViTransmissions += count;
    }
    for (auto const& [pair, count] : g_viRxCount) {
        totalViReceptions += count;
    }
    for (auto const& [pair, delay] : g_viDelay) {
        totalViDelaySum += delay;
    }

    double voPSR = (totalVoTransmissions > 0) ? 
        (double)totalVoReceptions / (totalVoTransmissions * (nVehicles - 1)) : 0;
    double voAvgDelay = (totalVoReceptions > 0) ? 
        (totalVoDelaySum / totalVoReceptions) : 0;
    double viPSR = (totalViTransmissions > 0) ? 
        (double)totalViReceptions / (totalViTransmissions * (nVehicles - 1)) : 0;
    double viAvgDelay = (totalViReceptions > 0) ? 
        (totalViDelaySum / totalViReceptions) : 0;
    voAvgDelay *= 1000;
    viAvgDelay *= 1000;
    
    // std::cout << "--- Global Results ---" << std::endl;
    // std::cout << "Global PSR (AC_VO): " << voPSR << std::endl;
    // std::cout << "Global Avg Delay (AC_VO): " << voAvgDelay * 1000 << " ms" << std::endl;

    // std::cout << "Global PSR (AC_VI): " << viPSR << std::endl;
    // std::cout << "Global Avg Delay (AC_VI): " << viAvgDelay * 1000 << " ms" << std::endl;

    *outFile << voPSR << "," << voAvgDelay << "," << viPSR << "," << viAvgDelay << "\n";


}
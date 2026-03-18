#include "bd-recreate-exp.h"
#include <filesystem>

int main(int argc, char* argv[]) {
    uint32_t nVehicles = 200;
    double lambda0 = 30.0;
    double lambda1 = 30.0;
    uint32_t packetSizeBits = 400;
    double dataRateMbps = 12.0;
    std::string fileName = "results.csv";
    std::string folderName = "scratch/80211bd-experiment/";
    // std::string factor = "nVehicles";

    // std::vector<uint32_t> nVehiclesList = {50,100,150,200,250,300,350};
    // std::vector<double> lambda0List = {15,20,25,30,35,40,45};
    // std::vector<double> lambda1List = lambda0List;
    // std::vector<uint32_t> packetSizeBitsList = {100,200,400,800,1600};
    // std::vector<double> dataRateMbpsList = {3,6,9,12,18,24,27};
    
    CommandLine cmd;
    cmd.AddValue ("nVehicles", "Number of vehicles [50-500]", nVehicles);
    cmd.AddValue ("lambda0", "Rate for AC_VO (Poisson)", lambda0);
    cmd.AddValue ("lambda1", "Rate for AC_VI (Deterministic)", lambda1);
    cmd.AddValue ("packetSize", "Packet size in bits", packetSizeBits);
    cmd.AddValue ("dataRate", "Channel Data Rate in Mbps", dataRateMbps);
    cmd.AddValue ("fileName", "Name of file to generate output", fileName);
    cmd.AddValue ("folderName", "Name of folder to generate output", folderName);
    // cmd.AddValue ("factor", "Name of the factor to vary from nVehicles, lambda0, lambda1, packetSizeBits, dataRateMbps", factor);
    cmd.Parse (argc, argv);

    // if(factor != "nVehicles") {
    //     nVehiclesList = {nVehicles};
    // }
    // if(factor != "lambda0") {
    //     lambda0List = {lambda0};
    // }
    // if(factor != "lambda1") {
    //     lambda1List = {lambda1};
    // }
    // if(factor != "packetSizeBits") {
    //     packetSizeBitsList = {packetSizeBits};
    // }
    // if(factor != "dataRateMbps") {
    //     dataRateMbpsList = {dataRateMbps};
    // }

    fileName = folderName + fileName;
    if(std::filesystem::exists(fileName)) {
        printf("%s exists, skipping execution\n", fileName.c_str());
        return 0;
    }
    std::ofstream *outFile = new std::ofstream(fileName.c_str(), std::ios::out);
    *outFile << "nVehicles,lambda0,lambda1,packetSizeBits,dataRate,voPSR,voDelay,viPSR,viDelay\n";
    // for(auto n: nVehiclesList) {
    //     for(auto l0: lambda0List) {
    //         for(auto l1: lambda1List) {
    //             for(auto p: packetSizeBitsList) {
    //                 for(auto d: dataRateMbpsList) {
                        // *outFile << n << "," << l0 << "," << l1 << "," << p << "," << d << ",";
                        // RecreateExp(n, l0, l1, p, d, outFile);
    //                 }
    //             }
    //         }
    //     }
    // }

    *outFile << nVehicles << "," << lambda0 << "," << lambda1 << "," << packetSizeBits << "," << dataRateMbps << ",";
    RecreateExp(nVehicles, lambda0, lambda1, packetSizeBits, dataRateMbps, outFile);
    outFile->close();
    delete outFile;
    return 0;
}
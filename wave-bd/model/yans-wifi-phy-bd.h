#ifndef YANS_WIFI_PHY_BD_H
#define YANS_WIFI_PHY_BD_H

#include "ns3/yans-wifi-phy.h"

namespace ns3
{

class YansWifiPhyBd: public YansWifiPhy{
public:
    static TypeId GetTypeId();

    YansWifiPhyBd();
    ~YansWifiPhyBd() override;
    void StartTx(Ptr<const WifiPpdu> ppdu) override;
private:
    void StartSingle(Ptr<const WifiPpdu> ppdu);
};

} // namespace ns3



#endif // YANS_WIFI_PHY_BD_H
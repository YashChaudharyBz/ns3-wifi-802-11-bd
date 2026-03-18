#ifndef YANS_WIFI_BD_HELPER_H
#define YANS_WIFI_BD_HELPER_H

#include "ns3/yans-wifi-helper.h"

namespace ns3
{

class YansWifiPhyBdHelper: public WifiPhyHelper {
public:
    YansWifiPhyBdHelper();
    void SetChannel(Ptr<YansWifiChannel> channel);
    void SetChannel(std::string channelName);
private:
    std::vector<Ptr<WifiPhy>> Create(Ptr<Node> node, Ptr<WifiNetDevice> device) const override;
    Ptr<YansWifiChannel> m_channel;
};

} // namespace ns3


#endif // YANS_WIFI_BD_HELPER_H
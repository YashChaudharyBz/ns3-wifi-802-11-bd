#include "ns3/wifi-80211bd-helper.h"
#include "ns3/string.h"

namespace ns3
{
Wifi80211bdHelper::Wifi80211bdHelper()
{
}

Wifi80211bdHelper::~Wifi80211bdHelper()
{
}

Wifi80211bdHelper
Wifi80211bdHelper::Default()
{
    Wifi80211bdHelper helper;
    helper.SetStandard(WIFI_STANDARD_80211p);
    helper.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                   "DataMode",
                                   StringValue("OfdmRate6MbpsBW10MHz"),
                                   "ControlMode",
                                   StringValue("OfdmRate6MbpsBW10MHz"),
                                   "NonUnicastMode",
                                   StringValue("OfdmRate6MbpsBW10MHz"));
    return helper;
}

} // namespace ns3

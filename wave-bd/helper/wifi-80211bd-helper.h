#ifndef WIFI_80211_BD_HELPER_H
#define WIFI_80211_BD_HELPER_H

#include "ns3/wifi-80211p-helper.h"

namespace ns3
{

class Wifi80211bdHelper: public Wifi80211pHelper {
public:
    Wifi80211bdHelper();
    ~Wifi80211bdHelper() override;

    /**
     * Default() still declare the Wifi Standard as WIFI_STANDARD_80211p
     */
    static Wifi80211bdHelper Default();
};

} // namespace ns3


#endif  // WIFI_80211_BD_HELPER_H
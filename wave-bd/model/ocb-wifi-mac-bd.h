#ifndef OCB_WIFI_MAC_BD_H
#define OCB_WIFI_MAC_BD_H

#include "ns3/ocb-wifi-mac.h"
#include "bd-rep-manager.h"

namespace ns3
{

class OcbWifiMacBd: public OcbWifiMac {
public:
    static TypeId GetTypeId();
    OcbWifiMacBd();
    ~OcbWifiMacBd() override;
    void AddReceiveVscCallback(OrganizationIdentifier oi, VscCallback cb);
    void RemoveReceiveVscCallback(OrganizationIdentifier oi);
    void Enqueue(Ptr<Packet> packet, Mac48Address to) override;
    void ConfigureStandard(WifiStandard standard) override;

    int  GetRepBe () const { return m_rep[0]; }
    void SetRepBe (int v) { m_rep[0] = v; }
    int  GetRepBk () const { return m_rep[1]; }
    void SetRepBk (int v) { m_rep[1] = v; }
    int  GetRepVi () const { return m_rep[2]; }
    void SetRepVi (int v) { m_rep[2] = v; }
    int  GetRepVo () const { return m_rep[3]; }
    void SetRepVo (int v) { m_rep[3] = v; }

    typedef void (* TxCallback)(uint32_t seq, uint32_t sender, int);
    typedef void (* RxCallback)(uint32_t seq, uint32_t sender, uint32_t receiver, double txTime, double rxTime, bool isRep, int);

private:
    void Receive(Ptr<const WifiMpdu> mpdu, uint8_t linkId) override;
    int m_rep[4];
    Ptr<BdRepManager> m_repManager;
    VendorSpecificContentManager m_vscManager;

    TracedCallback<uint32_t, uint32_t, int> m_enqueueTrace;
    TracedCallback<uint32_t, uint32_t, uint32_t, double, double, bool, int> m_rxTrace;
};

} // namespace ns3

#endif  // OCB_WIFI_MAC_BD_H

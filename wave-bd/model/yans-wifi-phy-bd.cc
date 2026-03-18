#include "yans-wifi-phy-bd.h"
#include "bd-tag.h"
#include "ns3/simulator.h"
#include "ns3/wifi-psdu.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("YansWifiPhyBd");
NS_OBJECT_ENSURE_REGISTERED(YansWifiPhyBd);

TypeId
YansWifiPhyBd::GetTypeId()
{
    static TypeId tid = TypeId("ns3::YansWifiPhyBd")
                            .SetParent<WifiPhy>()
                            .SetGroupName("WaveBd")
                            .AddConstructor<YansWifiPhyBd>();
    return tid;
}

YansWifiPhyBd::YansWifiPhyBd()
{
    NS_LOG_FUNCTION(this);
}

YansWifiPhyBd::~YansWifiPhyBd()
{
    NS_LOG_FUNCTION(this);
}




void
YansWifiPhyBd::StartTx(Ptr<const WifiPpdu> ppdu)
{
    NS_LOG_FUNCTION(this);
    Time txDuration = ppdu->GetTxDuration () + GetSifs();
    Time accumulatedDelay = Seconds(0);
    BdTag tag;
    int repCount = 1;
    Ptr<const WifiPsdu> psdu = ppdu->GetPsdu ();
    Ptr<const Packet> packet = psdu->GetPacket();
    if(packet->PeekPacketTag(tag)) {
        repCount = tag.GetRep();
    }
    NS_LOG_DEBUG("DEBUG: txDuration = " << txDuration);
    for(int i = 0; i < repCount; i ++) {
        Simulator::Schedule(accumulatedDelay, &YansWifiPhyBd::StartSingle, this, ppdu);
        accumulatedDelay += txDuration;
    }
}

void
YansWifiPhyBd::StartSingle(Ptr<const WifiPpdu> ppdu)
{
    YansWifiPhy::StartTx(ppdu);
}

} // namespace ns3

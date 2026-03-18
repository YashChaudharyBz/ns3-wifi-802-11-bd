#include "ocb-wifi-mac-bd.h"
#include "wave-bd-frame-exchange-manager.h"
#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("OcbWifiMacBd");
NS_OBJECT_ENSURE_REGISTERED(OcbWifiMacBd);
const static Mac48Address WILDCARD_BSSID = Mac48Address::GetBroadcast();

TypeId
OcbWifiMacBd::GetTypeId()
{
    static TypeId tid = TypeId("ns3::OcbWifiMacBd")
                        .SetParent<OcbWifiMac>()
                        .SetGroupName("WaveBd")
                        .AddConstructor<OcbWifiMacBd>()
                        .AddAttribute("Rep_BE", "Retransission for best effort channel",
                        IntegerValue(1), MakeIntegerAccessor(&OcbWifiMacBd::GetRepBe, &OcbWifiMacBd::SetRepBe), MakeIntegerChecker<int>())
                        .AddAttribute("Rep_BK", "Retransission for background channel",
                        IntegerValue(1), MakeIntegerAccessor(&OcbWifiMacBd::GetRepBk, &OcbWifiMacBd::SetRepBk), MakeIntegerChecker<int>())
                        .AddAttribute("Rep_VI", "Retransission for video channel",
                        IntegerValue(1), MakeIntegerAccessor(&OcbWifiMacBd::GetRepVi, &OcbWifiMacBd::SetRepVi), MakeIntegerChecker<int>())
                        .AddAttribute("Rep_VO", "Retransission for voice     channel",
                        IntegerValue(1), MakeIntegerAccessor(&OcbWifiMacBd::GetRepVo, &OcbWifiMacBd::SetRepVo), MakeIntegerChecker<int>())
                        .AddTraceSource ("WaveBdMacTx", 
                        "Fired when a packet enters the Enqueue function",
                        MakeTraceSourceAccessor (&OcbWifiMacBd::m_enqueueTrace),
                        "ns3::OcbWifiMacBd::TxCallback")
                        .AddTraceSource ("WaveBdMacRx", 
                        "Fired when a packet is successfully received",
                        MakeTraceSourceAccessor (&OcbWifiMacBd::m_rxTrace),
                        "ns3::OcbWifiMacBd::RxCallback");
    return tid;
}

OcbWifiMacBd::OcbWifiMacBd()
    :m_repManager(CreateObject<BdRepManager>())
{
    NS_LOG_FUNCTION(this);
    SetTypeOfStation(OCB);
    AggregateObject(m_repManager);
    if(GetQosSupported()) {
        ConfigureEdca(15, 31, 2, AC_VO);
        ConfigureEdca(15, 31, 3, AC_VI);
    }
}

OcbWifiMacBd::~OcbWifiMacBd()
{
    NS_LOG_FUNCTION(this);
}

void
OcbWifiMacBd::AddReceiveVscCallback(OrganizationIdentifier oi, VscCallback cb)
{
    NS_LOG_FUNCTION(this << oi << &cb);
    m_vscManager.RegisterVscCallback(oi, cb);
}

void
OcbWifiMacBd::RemoveReceiveVscCallback(OrganizationIdentifier oi)
{
    NS_LOG_FUNCTION(this << oi);
    m_vscManager.DeregisterVscCallback(oi);
}

void
OcbWifiMacBd::Enqueue(Ptr<Packet> packet, Mac48Address to)
{
    // NS_LOG_FUNCTION(this);
    int ac = 0;
    int rep = 1;
    NS_LOG_DEBUG("OcbWifiMacBd::Enqueue: QosSupported: " << GetQosSupported());
    if (GetQosSupported())
    {
        int tid = QosUtilsGetTidForPacket(packet);
        ac = QosUtilsMapTidToAc(tid);
        NS_LOG_DEBUG("OcbWifiMacBd::Enqueue: tid = " << tid << " ac = " << ac);
        rep = m_rep[ac];
    }
    if(to.IsBroadcast() && rep > 1)
        m_repManager->BdRepeat(packet, rep);
    
    Mac48Address myMac = GetAddress (); 
    uint8_t buffer[6];
    myMac.CopyTo (buffer);
    uint32_t senderId = (uint32_t(buffer[4]) << 8) | uint32_t(buffer[5]);

    BdTag tag;
    int seq = -1;
    if(packet->PeekPacketTag(tag)) {
        seq = tag.GetSeq();
    }

    m_enqueueTrace(seq, senderId, ac);

    OcbWifiMac::Enqueue(packet, to);
}

void
OcbWifiMacBd::ConfigureStandard(WifiStandard standard)
{
    NS_LOG_FUNCTION(this << standard);
    NS_ASSERT(standard == WIFI_STANDARD_80211p);

    if (GetNLinks() == 0)
    {
        WifiMac::SetWifiPhys({nullptr});
    }

    auto& link = GetLink(SINGLE_LINK_OP_ID);
    link.channelAccessManager = CreateObject<ChannelAccessManager>();

    uint32_t cwmin = 15;
    uint32_t cwmax = 1023;

    if (!GetQosSupported())
    {
        ConfigureEdca(cwmin, cwmax, 2, AC_BE_NQOS);
    }
    else
    {
        ConfigureEdca(cwmin, cwmax, 2, AC_VO);
        ConfigureEdca(cwmin, cwmax, 3, AC_VI);
        ConfigureEdca(cwmin, cwmax, 6, AC_BE);
        ConfigureEdca(cwmin, cwmax, 9, AC_BK);
    }

    // Setup FrameExchangeManager 
    auto feManager = CreateObject<WaveBdFrameExchangeManager>();
    feManager->SetWifiMac(this);
    feManager->SetMacTxMiddle(m_txMiddle);
    feManager->SetMacRxMiddle(m_rxMiddle);
    feManager->SetAddress(GetAddress());
    link.channelAccessManager->SetupFrameExchangeManager(feManager);
    if (auto phy = GetWifiPhy(); phy != nullptr)
    {
        feManager->SetWifiPhy(phy);
        link.channelAccessManager->SetupPhyListener(phy);
    }
    link.feManager = feManager;
}

void
OcbWifiMacBd::Receive(Ptr<const WifiMpdu> mpdu, uint8_t linkId)
{
    NS_LOG_FUNCTION(this << *mpdu << +linkId);
    const WifiMacHeader* hdr = &mpdu->GetHeader();
    Ptr<Packet> packet = mpdu->GetPacket()->Copy();
    NS_ASSERT(!hdr->IsCtl());
    NS_ASSERT(hdr->GetAddr3() == WILDCARD_BSSID);

    Mac48Address from = hdr->GetAddr2();
    Mac48Address to = GetAddress();
    
    uint8_t buffer[6];
    to.CopyTo (buffer);
    uint32_t senderId = (uint32_t(buffer[4]) << 8) | uint32_t(buffer[5]);
    from.CopyTo(buffer);
    uint32_t receiverId = (uint32_t(buffer[4]) << 8) | uint32_t(buffer[5]);
    bool isRep = m_repManager->HandleRepeat(packet, from);
    BdTag tag;
    int seq = -1;
    int ac = 0;
    double txTime, rxTime;
    
    if(packet->PeekPacketTag(tag)) {
        seq = tag.GetSeq();
        txTime = tag.GetTime();
        rxTime = Simulator::Now().GetSeconds();
        ac = tag.GetAc();
    }

    NS_LOG_DEBUG("OcbWifiMacBd::Receive: Trace is here!");
    m_rxTrace(seq, senderId, receiverId, txTime, rxTime, isRep, ac);

    // Repetition Check
    if(isRep) {
        NS_LOG_DEBUG("OcbWifiMacBd::Receive: Duplicate packet");
        return ;
    }
    
    if (GetWifiRemoteStationManager()->IsBrandNew(from))
    {
        if (GetHtSupported() || GetVhtSupported(SINGLE_LINK_OP_ID))
        {
            GetWifiRemoteStationManager()->AddAllSupportedMcs(from);
            GetWifiRemoteStationManager()->AddStationHtCapabilities(
                from,
                GetHtCapabilities(SINGLE_LINK_OP_ID));
            }
            if (GetVhtSupported(SINGLE_LINK_OP_ID))
            {
                GetWifiRemoteStationManager()->AddStationVhtCapabilities(
                    from,
                    GetVhtCapabilities(SINGLE_LINK_OP_ID));
                }
                GetWifiRemoteStationManager()->AddAllSupportedModes(from);
                GetWifiRemoteStationManager()->RecordDisassociated(from);
            }
            
            if (hdr->IsData())
            {
                if (hdr->IsQosData() && hdr->IsQosAmsdu())
                {
                    NS_LOG_DEBUG("Received A-MSDU from" << from);
                    DeaggregateAmsduAndForward(mpdu);
                }
                else
                {
                    ForwardUp(packet, from, to);
                }
                return;
            }
            
            if (to != GetAddress() && !to.IsGroup())
            {
                NS_LOG_LOGIC("the management frame is not for us");
                NotifyRxDrop(packet);
                return;
            }
            
            if (hdr->IsMgt() && hdr->IsAction())
            {
                VendorSpecificActionHeader vsaHdr;
                packet->PeekHeader(vsaHdr);
                if (vsaHdr.GetCategory() == CATEGORY_OF_VSA)
                {
                    VendorSpecificActionHeader vsa;
                    packet->RemoveHeader(vsa);
                    OrganizationIdentifier oi = vsa.GetOrganizationIdentifier();
                    VscCallback cb = m_vscManager.FindVscCallback(oi);
                    
                    if (cb.IsNull())
                    {
                        NS_LOG_DEBUG("cannot find VscCallback for OrganizationIdentifier=" << oi);
                        return;
                    }
                    bool succeed = cb(this, oi, packet, from);
                    
                    if (!succeed)
                    {
                        NS_LOG_DEBUG("vsc callback could not handle the packet successfully");
                    }
                    
                    return;
                }
            }
            WifiMac::Receive(Create<WifiMpdu>(packet, *hdr), linkId);
        }
        
    } // namespace ns3
    
    
    
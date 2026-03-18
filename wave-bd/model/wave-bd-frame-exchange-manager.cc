#include "wave-bd-frame-exchange-manager.h"

#include "ns3/higher-tx-tag.h"

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/qos-blocked-destinations.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/wifi-protection.h"

#include "bd-tag.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("WaveBdFrameExchangeManager");

NS_OBJECT_ENSURE_REGISTERED(WaveBdFrameExchangeManager);

TypeId
WaveBdFrameExchangeManager::GetTypeId()
{
    static TypeId tid = TypeId("ns3::WaveBdFrameExchangeManager")
                            .SetParent<QosFrameExchangeManager>()
                            .AddConstructor<WaveBdFrameExchangeManager>()
                            .SetGroupName("WaveBd");
    return tid;
}

WaveBdFrameExchangeManager::WaveBdFrameExchangeManager()
{
    NS_LOG_FUNCTION(this);
}

WaveBdFrameExchangeManager::~WaveBdFrameExchangeManager()
{
    NS_LOG_FUNCTION_NOARGS();
}

void
WaveBdFrameExchangeManager::SetWaveNetDevice(Ptr<WaveNetDevice> device)
{
    m_scheduler = device->GetChannelScheduler();
    m_coordinator = device->GetChannelCoordinator();
    NS_ASSERT(m_scheduler && m_coordinator);
}

WifiTxVector
WaveBdFrameExchangeManager::GetDataTxVector(Ptr<const WifiMpdu> item) const
{
    NS_LOG_FUNCTION(this << *item);
    HigherLayerTxVectorTag datatag;
    bool found;
    found = ConstCast<Packet>(item->GetPacket())->PeekPacketTag(datatag);
    // if high layer has not controlled transmit parameters, the real transmit parameters
    // will be determined by MAC layer itself.
    if (!found)
    {
        return m_mac->GetWifiRemoteStationManager()->GetDataTxVector(item->GetHeader(),
                                                                     m_allowedWidth);
    }

    // if high layer has set the transmit parameters with non-adaption mode,
    // the real transmit parameters are determined by high layer.
    if (!datatag.IsAdaptable())
    {
        return datatag.GetTxVector();
    }

    // if high layer has set the transmit parameters with non-adaption mode,
    // the real transmit parameters are determined by both high layer and MAC layer.
    WifiTxVector txHigher = datatag.GetTxVector();
    WifiTxVector txMac =
        m_mac->GetWifiRemoteStationManager()->GetDataTxVector(item->GetHeader(), m_allowedWidth);
    WifiTxVector txAdapter;
    txAdapter.SetChannelWidth(10);
    // the DataRate set by higher layer is the minimum data rate
    // which is the lower bound for the actual data rate.
    if (txHigher.GetMode().GetDataRate(txHigher.GetChannelWidth()) >
        txMac.GetMode().GetDataRate(txMac.GetChannelWidth()))
    {
        txAdapter.SetMode(txHigher.GetMode());
        txAdapter.SetPreambleType(txHigher.GetPreambleType());
    }
    else
    {
        txAdapter.SetMode(txMac.GetMode());
        txAdapter.SetPreambleType(txMac.GetPreambleType());
    }
    // the TxPwr_Level set by higher layer is the maximum transmit
    // power which is the upper bound for the actual transmit power;
    txAdapter.SetTxPowerLevel(std::min(txHigher.GetTxPowerLevel(), txMac.GetTxPowerLevel()));


    // set 802.11bd repetitions if available
    Ptr<const Packet> pkt = item->GetPacket();
    BdTag tag;
    if(pkt->PeekPacketTag(tag)) {
        txAdapter.SetRep(tag.GetRep());
    }

    return txAdapter;
}

bool
WaveBdFrameExchangeManager::StartTransmission(Ptr<Txop> dcf, uint16_t allowedWidth)
{
    NS_LOG_FUNCTION(this << dcf << allowedWidth);

    uint32_t curChannel = m_phy->GetChannelNumber();
    // if current channel access is not AlternatingAccess, just do as FrameExchangeManager.
    if (!m_scheduler || !m_scheduler->IsAlternatingAccessAssigned(curChannel))
    {
        return FrameExchangeManager::StartTransmission(dcf, allowedWidth);
    }

    m_txTimer.Cancel();
    m_dcf = dcf;
    m_allowedWidth = allowedWidth;

    Ptr<WifiMacQueue> queue = dcf->GetWifiMacQueue();

    if (queue->IsEmpty())
    {
        NS_LOG_DEBUG("Queue empty");
        m_dcf->NotifyChannelReleased(0);
        m_dcf = nullptr;
        return false;
    }

    m_dcf->NotifyChannelAccessed(0);
    Ptr<WifiMpdu> mpdu = queue->PeekFirstAvailable(0);
    NS_ASSERT(mpdu);

    // assign a sequence number if this is not a fragment nor a retransmission
    if (!mpdu->IsFragment() && !mpdu->GetHeader().IsRetry())
    {
        uint16_t sequence = m_txMiddle->GetNextSequenceNumberFor(&mpdu->GetHeader());
        mpdu->GetHeader().SetSequenceNumber(sequence);
    }

    WifiTxParameters txParams;
    txParams.m_txVector = GetDataTxVector(mpdu);
    NS_LOG_DEBUG("TxVector: " << txParams.m_txVector);
    Time remainingTime = m_coordinator->NeedTimeToGuardInterval();

    if (!TryAddMpdu(mpdu, txParams, remainingTime))
    {
        // The attempt for this transmission will be canceled;
        // and this packet will be pending for next transmission by QosTxop class
        NS_LOG_DEBUG("Because the required transmission time exceeds the remainingTime = "
                     << remainingTime.As(Time::MS)
                     << ", currently this packet will not be transmitted.");
    }
    else
    {
        SendMpduWithProtection(mpdu, txParams);
        return true;
    }
    return false;
}

void
WaveBdFrameExchangeManager::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_scheduler = nullptr;
    m_coordinator = nullptr;
    FrameExchangeManager::DoDispose();
}

} // namespace ns3

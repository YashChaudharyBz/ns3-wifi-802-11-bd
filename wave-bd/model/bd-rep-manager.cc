#include "bd-rep-manager.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/qos-utils.h"

NS_LOG_COMPONENT_DEFINE("BdRepManager");

namespace ns3 {

TypeId
BdRepManager::GetTypeId()
{
    static TypeId tid = TypeId("ns3::BdRepManager")
        .SetParent<Object>()
        .SetGroupName("WaveBd")
        .AddConstructor<BdRepManager>()
        .AddAttribute("Timeout", "The timeout for repetition check", TimeValue(Seconds(0)),
                      MakeTimeAccessor(&BdRepManager::m_timeout), MakeTimeChecker());
    return tid;
}

BdRepManager::BdRepManager()
    :m_timeout(Seconds(2)),
    seq(0)
{
    NS_LOG_FUNCTION(this);
}

BdRepManager::BdRepManager(Time timeout)
    :m_timeout(timeout),
    seq(0)
{
    NS_LOG_FUNCTION(this);
}

BdRepManager::~BdRepManager()
{
    NS_LOG_FUNCTION(this);
}

void BdRepManager::BdRepeat(Ptr<Packet> packet, int rep)
{
    BdTag tag;
    tag.SetRep(rep);
    tag.SetSeq(seq);
    tag.SetTime(Simulator::Now().GetSeconds());
    int tid = QosUtilsGetTidForPacket(packet);
    AcIndex ac = QosUtilsMapTidToAc(tid);
    tag.SetAc(ac);
    packet->AddPacketTag(tag);
    NS_LOG_DEBUG("BdRepManager::BdRepeat: Tag added with sequence " << seq);
    seq ++;
    if(seq > 1e8)   seq = 0;
}

bool
BdRepManager::ReduceCount(Ptr<Packet> packet)
{
    BdTag tag;
    packet->PeekPacketTag(tag);
    if(tag.GetRep() == 0) {
        return false;
    }
    tag.SetRep(tag.GetRep()-1);
    packet->AddPacketTag(tag);
    return true;
}

bool BdRepManager::HandleRepeat(Ptr<Packet> packet, Mac48Address to)
{
    BdTag tag;
    if(!packet->PeekPacketTag(tag)) {
        return false;
    }
    int seqNo = tag.GetSeq();
    auto it = m_repMap.find({seqNo, to});
    bool isRep = (it != m_repMap.end());
    Time now = Simulator::Now();
    if(!isRep) {
        while(!m_repBuffer.empty()) {
            auto [s, rAdd, t] = m_repBuffer.front();
            if(now - t > m_timeout) {
                m_repBuffer.pop();
                m_repMap.erase({s, rAdd});
                NS_LOG_DEBUG("BdRepManager::HandleRepeat: Old packet(" << s << ", " << rAdd << ", " << t << ")");
            }
            else {
                break;
            }
        }
        m_repMap[{seqNo, to}] = now;
        m_repBuffer.push({seqNo, to, now});
        NS_LOG_DEBUG("BdRepManager::HandleRepeat: New packet(" << seqNo << ", " << to << ", " << now << ")");
    }
    else {
        NS_LOG_DEBUG("BdRepManager::HandleRepeat: Packet Repeat("<< seqNo << ", " << to << ")");
    }
    return isRep;
}

}
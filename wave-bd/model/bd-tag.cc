#include "bd-tag.h"
#include "ns3/tag-buffer.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/qos-utils.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("BdTag");

BdTag::BdTag()
    : m_seq(0), m_rep(0), m_txTime(0.0), m_ac(AC_BE) {
    NS_LOG_FUNCTION(this);
}

BdTag::~BdTag() {
    NS_LOG_FUNCTION(this);
}

void BdTag::SetSeq(int seq) {
    m_seq = seq;
}

int BdTag::GetSeq() {
    return m_seq;
}

void BdTag::SetRep(int rep) {
    m_rep = rep;
}

int BdTag::GetRep() {
    return m_rep;
}

void
BdTag::SetAc(AcIndex ac)
{
    m_ac = ac;
}

AcIndex
BdTag::GetAc()
{
    return m_ac;
}

void
BdTag::SetTime(double time)
{
    m_txTime = time;
}

double
BdTag::GetTime()
{
    return m_txTime;
}

void BdTag::Serialize(TagBuffer i) const {
    NS_LOG_FUNCTION(this);
    i.WriteU32(m_seq);
    i.WriteU32(m_rep);
    i.WriteDouble(m_txTime);
    i.WriteU8(static_cast<uint8_t>(m_ac));
}

void BdTag::Deserialize(TagBuffer i) {
    NS_LOG_FUNCTION(this);
    m_seq = i.ReadU32();
    m_rep = i.ReadU32();
    m_txTime = i.ReadDouble();
    m_ac = static_cast<AcIndex>(i.ReadU8());
}

uint32_t BdTag::GetSerializedSize() const {
    return sizeof(uint32_t) + sizeof(uint32_t) + sizeof(double) + sizeof(AcIndex);
}

TypeId BdTag::GetInstanceTypeId() const {
    return GetTypeId();
}

void BdTag::Print(std::ostream& os) const {
    os << "BdTag - Seq: " << m_seq << ", Rep: " << m_rep;
}

TypeId BdTag::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::BdTag")
        .SetParent<Tag>()
        .SetGroupName("WaveBd")
        .AddConstructor<BdTag>()
        .AddAttribute("Seq", "The sequence number", IntegerValue(0),
                      MakeIntegerAccessor(&BdTag::m_seq), MakeIntegerChecker<int>())
        .AddAttribute("Rep", "The repetition value", IntegerValue(0),
                      MakeIntegerAccessor(&BdTag::m_rep), MakeIntegerChecker<int>());
    return tid;
}

} // namespace ns3

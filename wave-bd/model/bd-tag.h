#ifndef BD_TAG_H
#define BD_TAG_H

#include "ns3/tag.h"
#include "ns3/qos-utils.h"

namespace ns3 {

class BdTag : public Tag {
public:
    BdTag();
    virtual ~BdTag();

    void SetSeq(int seq);
    int GetSeq();
    void SetRep(int rep);
    int GetRep();
    void SetAc(AcIndex ac);
    AcIndex GetAc();
    void SetTime(double time);
    double GetTime();

    static TypeId GetTypeId();
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual uint32_t GetSerializedSize() const;
    virtual TypeId GetInstanceTypeId() const;
    virtual void Print(std::ostream& os) const;
private:
    int m_seq;
    int m_rep;
    double m_txTime;
    AcIndex m_ac;
};

}   // namespace ns3

#endif  // BD_TAG_H
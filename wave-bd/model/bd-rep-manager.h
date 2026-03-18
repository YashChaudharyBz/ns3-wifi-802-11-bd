#ifndef BD_REP_MANAGER_H
#define BD_REP_MANAGER_H

#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/timer.h"
#include "bd-tag.h"
#include <queue>

namespace ns3 {

class BdRepManager : public Object {
public:
    static TypeId GetTypeId();

    BdRepManager();
    BdRepManager(ns3::Time timeout);
    ~BdRepManager() override;

    void BdRepeat(Ptr<Packet> packet, int rep);
    bool ReduceCount(Ptr<Packet> packet);
    bool HandleRepeat(Ptr<Packet> packet, Mac48Address to);
private:
    std::map<std::pair<int, Mac48Address>, ns3::Time> m_repMap;
    std::queue<std::tuple<int, Mac48Address, ns3::Time>> m_repBuffer;
    Time m_timeout;
    int seq;
};

}   // namespace ns3

#endif  // BD_REP_MANAGER_H
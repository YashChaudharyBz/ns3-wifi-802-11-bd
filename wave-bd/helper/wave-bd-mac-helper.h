#ifndef WAVE_BD_MAC_HELPER_H
#define WAVE_BD_MAC_HELPER_H

#include "ns3/wave-mac-helper.h"

namespace ns3
{

class NqosBdWaveMacHelper: public NqosWaveMacHelper {
public:
    NqosBdWaveMacHelper();
    ~NqosBdWaveMacHelper() override;
    static NqosBdWaveMacHelper Default();

    template <typename... Ts>
    void SetType(std::string type, Ts&&... args);
};

class QosBdWaveMacHelper: public QosWaveMacHelper {
public:
    QosBdWaveMacHelper();
    ~QosBdWaveMacHelper() override;

    static QosBdWaveMacHelper Default();

    template <typename... Ts>
    void SetType(std::string type, Ts&&... args);
};



template <typename... Ts>
void
NqosBdWaveMacHelper::SetType(std::string type, Ts&&... args)
{
    if (type != "ns3::OcbWifiMacBd")
    {
        NS_FATAL_ERROR("NqosBdWaveMacHelper shall set OcbWifiMacBd");
    }
    WifiMacHelper::SetType("ns3::OcbWifiMacBd", std::forward<Ts>(args)...);
}

template <typename... Ts>
void
QosBdWaveMacHelper::SetType(std::string type, Ts&&... args)
{
    if (type != "ns3::OcbWifiMacBd")
    {
        NS_FATAL_ERROR("QosBdWaveMacHelper shall set OcbWifiMacBd");
    }
    WifiMacHelper::SetType("ns3::OcbWifiMacBd", std::forward<Ts>(args)...);
}

} // namespace ns3


#endif  //WAVE_BD_MAC_HELPER_H
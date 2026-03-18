#include "ns3/wave-bd-mac-helper.h"
#include "ns3/boolean.h"
#include "ns3/integer.h"

namespace ns3
{


NqosBdWaveMacHelper::NqosBdWaveMacHelper()
{
}

NqosBdWaveMacHelper::~NqosBdWaveMacHelper()
{
}

NqosBdWaveMacHelper
NqosBdWaveMacHelper::Default()
{
    NqosBdWaveMacHelper helper;
    helper.SetType("ns3::OcbWifiMacBd", "QosSupported", BooleanValue(false),
                    "Rep_BE", IntegerValue(3));
    return helper;
}


QosBdWaveMacHelper::QosBdWaveMacHelper()
{
}

QosBdWaveMacHelper::~QosBdWaveMacHelper()
{
}

QosBdWaveMacHelper
QosBdWaveMacHelper::Default()
{
    QosBdWaveMacHelper helper;
    helper.SetType("ns3::OcbWifiMacBd", "QosSupported", BooleanValue(true), 
                    "Rep_VO", IntegerValue(3), "Rep_VI", IntegerValue(2));
    return helper;
}

} // namespace ns3


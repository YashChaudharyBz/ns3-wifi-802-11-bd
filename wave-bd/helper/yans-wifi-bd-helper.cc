#include "ns3/yans-wifi-bd-helper.h"
#include "ns3/yans-wifi-phy-bd.h"

#include "ns3/wifi-helper.h"
#include "ns3/error-rate-model.h"
#include "ns3/frame-capture-model.h"
#include "ns3/interference-helper.h"
#include "ns3/names.h"
#include "ns3/preamble-detection-model.h"
#include "ns3/wifi-net-device.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("YansWifiBdHelper");

YansWifiPhyBdHelper::YansWifiPhyBdHelper()  
    : WifiPhyHelper(1),
      m_channel(nullptr)
{
    m_phy.at(0).SetTypeId("ns3::YansWifiPhyBd");
    SetInterferenceHelper("ns3::InterferenceHelper");
    SetErrorRateModel("ns3::TableBasedErrorRateModel");
}

void
YansWifiPhyBdHelper::SetChannel(Ptr<YansWifiChannel> channel)
{
    m_channel = channel;
}

void
YansWifiPhyBdHelper::SetChannel(std::string channelName)
{
    Ptr<YansWifiChannel> channel = Names::Find<YansWifiChannel>(channelName);
    m_channel = channel;
}

std::vector<Ptr<WifiPhy>>
YansWifiPhyBdHelper::Create(Ptr<Node> node, Ptr<WifiNetDevice> device) const
{
    NS_LOG_DEBUG("YansWifiPhyBdHelper::Create called!");
    Ptr<YansWifiPhyBd> phy = m_phy.at(0).Create<YansWifiPhyBd>();
    Ptr<InterferenceHelper> interference = m_interferenceHelper.Create<InterferenceHelper>();
    phy->SetInterferenceHelper(interference);
    Ptr<ErrorRateModel> error = m_errorRateModel.at(0).Create<ErrorRateModel>();
    phy->SetErrorRateModel(error);
    if (m_frameCaptureModel.at(0).IsTypeIdSet())
    {
        auto frameCapture = m_frameCaptureModel.at(0).Create<FrameCaptureModel>();
        phy->SetFrameCaptureModel(frameCapture);
    }
    if (m_preambleDetectionModel.at(0).IsTypeIdSet())
    {
        auto preambleDetection = m_preambleDetectionModel.at(0).Create<PreambleDetectionModel>();
        phy->SetPreambleDetectionModel(preambleDetection);
    }
    phy->SetChannel(m_channel);
    phy->SetDevice(device);
    return std::vector<Ptr<WifiPhy>>({phy});
}

} // namespace ns3

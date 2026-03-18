#ifndef WAVE_BD_FRAME_EXCHANGE_MANAGER_H
#define WAVE_BD_FRAME_EXCHANGE_MANAGER_H

#include "ns3/wave-net-device.h"

#include "ns3/qos-frame-exchange-manager.h"

namespace ns3
{

class WaveBdFrameExchangeManager : public QosFrameExchangeManager
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    WaveBdFrameExchangeManager();
    ~WaveBdFrameExchangeManager() override;

    // Overridden from FrameExchangeManager
    bool StartTransmission(Ptr<Txop> dcf, uint16_t allowedWidth) override;

    /**
     * \param device WaveNetDevice associated with WaveFrameExchangeManager
     */
    void SetWaveNetDevice(Ptr<WaveNetDevice> device);

  protected:
    // Overridden from FrameExchangeManager
    void DoDispose() override;

  private:
    /**
     * Return a TXVECTOR for the DATA frame given the destination.
     * The function consults WifiRemoteStationManager, which controls the rate
     * to different destinations.
     *
     * \param item the item being asked for TXVECTOR
     * \return TXVECTOR for the given item
     */
    virtual WifiTxVector GetDataTxVector(Ptr<const WifiMpdu> item) const;

    Ptr<ChannelScheduler> m_scheduler;     ///< the channel scheduler
    Ptr<ChannelCoordinator> m_coordinator; ///< the channel coordinator
};

} // namespace ns3

#endif /* WAVE_BD_FRAME_EXCHANGE_MANAGER_H */

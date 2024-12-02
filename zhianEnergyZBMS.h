#ifndef ZHIAN_ENERGY_Z_BMS_H
#define ZHIAN_ENERGY_Z_BMS_H
#include "Arduino.h"
#include <AsyncDelay.h>
#include <SPI.h>
#include <mcp2515.h>
#include "zhianEnergyZBMDclassDefinitions.h"

class ZhianEnergyZBMS
{
public:
    ZhianEnergyZBMS_PF22_ChargingRequest chargingRequest;
    ZhianEnergyZBMS_PF24_BatteryAlarm alarm;
    ZhianEnergyZBMS_PF26_BatteryOperation operation;
    ZhianEnergyZBMS_PF80_FixedValue fixedValue;
    ZhianEnergyZBMS_PF82_CellTemperature temperature;
    ZhianEnergyZBMS_PF84_CellVoltage voltage;
    ZhianEnergyZBMS_PF86_CirculationTimes circulationTimes;
    ZhianEnergyZBMS_PF86_OtherSop otherSop;

    void begin(uint8_t mcpCsPin),
        loop();

    void startCellTempRequest(uint16_t requestInterval),
        stopCellTempRequest();

    void startCellVoltRequest(uint16_t requestInterval),
        stopCellVoltRequest();

private:
    String alltudo[6];
    MCP2515 *mcp2515;

    struct can_frame
        heartBeatData,
        cellVoltCanFrame,
        cellTempCanFrame,
        circulationCanFrame,
        fixedValueCanFrame,
        otherSopCanFrame;

    AsyncDelay
        heartBeatDelay{2000, AsyncDelay::MILLIS},
        cellTempDelay,
        cellVoltDelay,
        battFirstReadDelay,
        battOutDelay;

    int8_t
        battFirstRead = -1;

    bool
        cellTempRequestFlag = false,
        cellVoltRequestFlag = false;

    void sendHeartBeat(),
        messageReceiverLoop(),
        battOutLoop(),
        pf22Case(can_frame &msg),
        pf24Case(can_frame &msg),
        pf26Case(can_frame &msg),
        pf81Case(can_frame &msg),
        pf83Case(can_frame &msg),
        pf85Case(can_frame &msg),
        pf87Case(can_frame &msg),
        pf89Case(can_frame &msg);
};

#endif
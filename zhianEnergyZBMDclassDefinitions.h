#ifndef ZHIAN_ENERGY_Z_BMS_CLASS_DEFINITIONS_H
#define ZHIAN_ENERGY_Z_BMS_CLASS_DEFINITIONS_H
#include "Arduino.h"

struct ZhianEnergyZBMS_PF22_ChargingRequest
{
    uint16_t
        chargingVoltage,
        maxChargingCurrent,
        highestCellVoltage,
        chargingState;
};

struct ZhianEnergyZBMS_PF24_BatteryAlarm
{
    uint16_t
        alarm,
        attention;
};

struct ZhianEnergyZBMS_PF26_BatteryOperation
{
    int16_t
        batteryVoltage = 0,
        batteryCurrent,
        batterySOC,
        batterySOH,
        battery15sSOP;
};

struct ZhianEnergyZBMS_PF80_FixedValue
{
    uint16_t
        packRatedVoltage,
        packRatedCapacity,
        maxDischargeCurrent,
        maxChargeCurrent;

    uint8_t
        softwareDate[4],    // 0: year, 1: month, 2: day, 3: hour
        canVersion[2],      // 0: major, 1: minor
        hardwareVersion[2], // 0: major, 1: minor
        softwareVersion[2], // 0: major, 1: minor
        cellCount,
        cellType, // 0: LiFePO4, 1: LCO, 2: Other's , SSLB
        cellTempCount,
        envTempCount,
        otherTempCount;
};

struct ZhianEnergyZBMS_PF80_OtherSop
{
    uint16_t
        sop500ms,
        sop3s;
};

struct ZhianEnergyZBMS_PF82_CellTemperature
{
    uint8_t
        sensor[8];
};

struct ZhianEnergyZBMS_PF84_CellVoltage
{
    uint16_t
        cell[18];
};

struct ZhianEnergyZBMS_PF86_CirculationTimes
{
    uint16_t
        circulationTimes;
};

#endif
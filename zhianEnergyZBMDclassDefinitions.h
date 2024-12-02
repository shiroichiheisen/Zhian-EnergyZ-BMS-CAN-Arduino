#ifndef ZHIAN_ENERGY_Z_BMS_CLASS_DEFINITIONS_H
#define ZHIAN_ENERGY_Z_BMS_CLASS_DEFINITIONS_H
#include "Arduino.h"

class ZhianEnergyZBMS_PF22_ChargingRequest
{
public:
    uint16_t
        chargingVoltage,
        maxChargingCurrent,
        highestCellVoltage,
        chargingState;
};

class ZhianEnergyZBMS_PF24_BatteryAlarm
{
public:
    uint16_t
        alarm,
        attention;
};

class ZhianEnergyZBMS_PF26_BatteryOperation
{
public:
    uint16_t
        batteryVoltage = 0,
        batteryCurrent,
        batterySOC,
        batterySOH,
        battery15sSOP;
};

class ZhianEnergyZBMS_PF80_FixedValue
{
public:
    uint32_t
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

class ZhianEnergyZBMS_PF86_CirculationTimes
{
public:
    uint16_t
        circulationTimes;
};

class ZhianEnergyZBMS_PF86_OtherSop
{
public:
    uint16_t
        sop500ms,
        sop3s;
};

class ZhianEnergyZBMS_PF82_CellTemperature
{
public:
    uint8_t
        cell[8];
};

class ZhianEnergyZBMS_PF84_CellVoltage
{
public:
    uint16_t
        cell[18];
};
#endif
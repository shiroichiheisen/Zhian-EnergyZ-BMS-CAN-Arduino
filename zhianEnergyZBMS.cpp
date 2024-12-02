#include "zhianEnergyZBMS.h"

void ZhianEnergyZBMS::begin(uint8_t mcpCsPin)
{
    mcp2515 = new MCP2515(mcpCsPin);

    mcp2515->reset();
    mcp2515->setConfigMode();
    mcp2515->setBitrate(CAN_1000KBPS, MCP_8MHZ);
    mcp2515->setNormalMode();

    memset(heartBeatData.data, 0, 8);
    memset(voltage.cell, 0, sizeof(voltage.cell));
    memset(cellVoltCanFrame.data, 0, 8);
    memset(cellTempCanFrame.data, 0, 8);
    memset(circulationCanFrame.data, 0, 8);

    heartBeatData.can_id = 0x984300FF;
    heartBeatData.can_dlc = 8;
    heartBeatData.data[3] = 0x01;
    heartBeatData.data[7] = 0x01;

    cellVoltCanFrame.can_id = 0x988400FF;
    cellVoltCanFrame.can_dlc = 8;

    cellTempCanFrame.can_id = 0x988200FF;
    cellTempCanFrame.can_dlc = 8;

    circulationCanFrame.can_id = 0x988600FF;
    circulationCanFrame.can_dlc = 8;

    fixedValueCanFrame.can_id = 0x988000FF;
    fixedValueCanFrame.can_dlc = 8;
    fixedValueCanFrame.data[0] = 0x4;

    otherSopCanFrame.can_id = 0x988800FF;
    otherSopCanFrame.can_dlc = 8;
}

void ZhianEnergyZBMS::startCellTempRequest(uint16_t requestInterval)
{
    cellTempRequestFlag = true;
    cellTempDelay.start(requestInterval, AsyncDelay::MILLIS);
}

void ZhianEnergyZBMS::stopCellTempRequest()
{
    cellTempRequestFlag = false;
}

void ZhianEnergyZBMS::startCellVoltRequest(uint16_t requestInterval)
{
    cellVoltRequestFlag = true;
    cellVoltDelay.start(requestInterval, AsyncDelay::MILLIS);
}

void ZhianEnergyZBMS::stopCellVoltRequest()
{
    cellVoltRequestFlag = false;
}

void ZhianEnergyZBMS::battOutLoop()
{

    if (battOutDelay.isExpired() && battFirstRead != -1 && operation.batteryVoltage != 0)
    {
        fixedValueCanFrame.data[0] = 0x04;
        battFirstRead = -1;

        memset(&chargingRequest, 0, sizeof(chargingRequest));
        memset(&alarm, 0, sizeof(alarm));
        memset(&operation, 0, sizeof(operation));
        memset(&fixedValue, 0, sizeof(fixedValue));
        memset(&temperature, 0, sizeof(temperature));
        memset(&voltage, 0, sizeof(voltage));
        memset(&circulationTimes, 0, sizeof(circulationTimes));
        memset(&otherSop, 0, sizeof(otherSop));
    }
}

void ZhianEnergyZBMS::sendHeartBeat()
{
    if (heartBeatDelay.isExpired())
    {
        mcp2515->sendMessage(&heartBeatData);
        heartBeatDelay.restart();
    }

    if (battFirstRead == -1)
        return;

    if (cellVoltRequestFlag)
        if (cellVoltDelay.isExpired())
        {
            mcp2515->sendMessage(&cellVoltCanFrame);
            cellVoltDelay.restart();
        }

    if (cellTempRequestFlag)
        if (cellTempDelay.isExpired())
        {
            mcp2515->sendMessage(&cellTempCanFrame);
            cellTempDelay.restart();
        }
}

void ZhianEnergyZBMS::pf22Case(can_frame &msg)
{
    chargingRequest.chargingVoltage = (msg.data[1] << 8 | msg.data[0]);
    chargingRequest.maxChargingCurrent = (msg.data[3] << 8 | msg.data[2]) / 100;
    chargingRequest.highestCellVoltage = (msg.data[5] << 8 | msg.data[4]);
    chargingRequest.chargingState = (msg.data[7] << 8 | msg.data[6]);
    battOutDelay.start(1500, AsyncDelay::MILLIS);

    if (battFirstRead == -1)
    {
        battFirstReadDelay.start(50, AsyncDelay::MILLIS);
        battFirstRead = 1;
    }
}

void ZhianEnergyZBMS::pf24Case(can_frame &msg)
{
    alarm.alarm = (msg.data[1] << 8 | msg.data[0]);
    alarm.attention = (msg.data[3] << 8 | msg.data[2]);
}

void ZhianEnergyZBMS::pf26Case(can_frame &msg)
{
    operation.batteryVoltage = (msg.data[1] << 8 | msg.data[0]);
    operation.batteryCurrent = (msg.data[3] << 8 | msg.data[2]) / 100;
    operation.batterySOC = msg.data[4];
    operation.batterySOH = msg.data[5];
    operation.battery15sSOP = (msg.data[7] << 8 | msg.data[6]);
}

void ZhianEnergyZBMS::pf83Case(can_frame &msg)
{
    for (int i = 0; i < msg.can_dlc; i++)
        temperature.cell[i] = msg.data[i] - 40;
}

void ZhianEnergyZBMS::pf85Case(can_frame &msg)
{
    for (int i = 0; i < msg.can_dlc; i++)
    {
        switch (msg.data[0])
        {
        case 1:
            voltage.cell[0] = (msg.data[5] << 8 | msg.data[4]);
            voltage.cell[1] = (msg.data[7] << 8 | msg.data[6]);
            break;

        case 2:
            voltage.cell[2] = (msg.data[2] << 8 | msg.data[1]);
            voltage.cell[3] = (msg.data[4] << 8 | msg.data[3]);
            voltage.cell[4] = (msg.data[6] << 8 | msg.data[5]);
            voltage.cell[5] = msg.data[7];
            break;

        case 3:
            voltage.cell[5] |= msg.data[1] << 8;
            voltage.cell[6] = (msg.data[3] << 8 | msg.data[2]);
            voltage.cell[7] = (msg.data[5] << 8 | msg.data[4]);
            voltage.cell[8] = (msg.data[7] << 8 | msg.data[6]);
            break;

        case 4:
            voltage.cell[9] = (msg.data[2] << 8 | msg.data[1]);
            voltage.cell[10] = (msg.data[4] << 8 | msg.data[3]);
            voltage.cell[11] = (msg.data[6] << 8 | msg.data[5]);
            voltage.cell[12] = msg.data[7];
            break;

        case 5:
            voltage.cell[12] |= msg.data[1] << 8;
            voltage.cell[13] = (msg.data[3] << 8 | msg.data[2]);
            voltage.cell[14] = (msg.data[5] << 8 | msg.data[4]);
            voltage.cell[15] = (msg.data[7] << 8 | msg.data[6]);
            break;

        case 6:
            voltage.cell[16] = (msg.data[2] << 8 | msg.data[1]);
            voltage.cell[17] = (msg.data[4] << 8 | msg.data[3]);
            break;

        default:
            break;
        }
    }
}

void ZhianEnergyZBMS::pf87Case(can_frame &msg)
{
    circulationTimes.circulationTimes = (msg.data[1] << 8 | msg.data[0]);
}

void ZhianEnergyZBMS::pf89Case(can_frame &msg)
{
    otherSop.sop500ms = (msg.data[1] << 8 | msg.data[0]);
    otherSop.sop3s = (msg.data[3] << 8 | msg.data[2]);
}

void ZhianEnergyZBMS::pf81Case(can_frame &msg)
{
    switch (msg.data[0])
    {
    case 0x04:
    {
        fixedValue.hardwareVersion[0] = msg.data[4];
        fixedValue.hardwareVersion[1] = msg.data[5];
        break;
    }

    case 0x05:
    {
        fixedValue.softwareVersion[0] = msg.data[4];
        fixedValue.softwareVersion[1] = msg.data[5];
        break;
    }

    case 0x06:
    {
        fixedValue.softwareDate[0] = msg.data[4];
        fixedValue.softwareDate[1] = msg.data[5];
        fixedValue.softwareDate[2] = msg.data[6];
        fixedValue.softwareDate[3] = msg.data[7];
        break;
    }

    case 0x07:
    {
        fixedValue.canVersion[0] = msg.data[4];
        fixedValue.canVersion[1] = msg.data[5];
        break;
    }

    case 0x08:
    {
        fixedValue.cellCount = msg.data[4];
        break;
    }

    case 0x09:
    {
        fixedValue.cellType = msg.data[4];
        break;
    }

    case 0x0A:
    {
        fixedValue.cellTempCount = msg.data[4];
        break;
    }

    case 0x0B:
    {
        fixedValue.envTempCount = msg.data[4];
        break;
    }

    case 0x0C:
    {
        fixedValue.otherTempCount = msg.data[4];
        break;
    }

    case 0x0E:
    {
        fixedValue.packRatedVoltage = (msg.data[5] << 8 | msg.data[4]);
        break;
    }

    case 0x0F:
    {
        fixedValue.packRatedCapacity = (msg.data[5] << 8 | msg.data[4]);
        break;
    }

    case 0x10:
    {
        fixedValue.maxDischargeCurrent = (msg.data[5] << 8 | msg.data[4]);
        break;
    }

    case 0x11:
    {
        fixedValue.maxChargeCurrent = (msg.data[5] << 8 | msg.data[4]);
        break;
    }

    default:
        break;
    }
}

void ZhianEnergyZBMS::messageReceiverLoop()
{
    if (battFirstRead > 0)
    {
        if (battFirstReadDelay.isExpired())
        {
            if (battFirstRead >= 3)
            {
                mcp2515->sendMessage(&fixedValueCanFrame);
                fixedValueCanFrame.data[0] += 1;

                if (fixedValueCanFrame.data[0] == 0x0D)
                    fixedValueCanFrame.data[0] = 0x0E;
            }

            if (battFirstRead == 1)
                mcp2515->sendMessage(&circulationCanFrame);

            if (battFirstRead == 2)
                mcp2515->sendMessage(&otherSopCanFrame);

            battFirstRead += 1;
            if (battFirstRead > 18)
                battFirstRead = 0;

            battFirstReadDelay.restart();
        }
    }

    if (mcp2515->checkReceive())
    {
        can_frame msg;
        mcp2515->readMessage(&msg);
        uint8_t batPfId = (msg.can_id >> 16) & 0xFF;

        switch (batPfId)
        {
        case 0x22:
            pf22Case(msg);
            break;

        case 0x24:
            pf24Case(msg);
            break;

        case 0x26:
            pf26Case(msg);
            break;

        case 0x81:
            pf81Case(msg);
            break;

        case 0x83:
            pf83Case(msg);
            break;

        case 0x85:
            pf85Case(msg);
            break;

        case 0x87:
            pf87Case(msg);
            break;

        case 0x89:
            pf89Case(msg);
            break;

        case 0x28:
            break;

        default:
            break;
        }
    }
}

void ZhianEnergyZBMS::loop()
{
    sendHeartBeat();
    messageReceiverLoop();
    battOutLoop();
}
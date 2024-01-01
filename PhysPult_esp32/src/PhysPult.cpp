#include "PhysPult.h"

PhysPult::PhysPult(void (*state)(PhysPult&)) : State(state)
{
    for(uint8_t i = 0; i < LightingLedCountTotal; i++)
    {
        LightingLeds[i] = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);
    }
}

void PhysPult::AcceptMessage(PhysPultMessage msg)
{
    if(msg.NumericData.length() < 6)
    {
        Serial.println("Error: not enough numeric data for output. Skipping update.");
    }
    else
    {
        Speed           = (uint8_t)msg.NumericData[0];

        TmValue         = (uint8_t)msg.NumericData[1];
        NmValue         = (uint8_t)msg.NumericData[2];
        TcValue         = (uint8_t)msg.NumericData[3];

        BatteryVoltage  = (uint8_t)msg.NumericData[4];
        SupplyVoltage   = (uint8_t)msg.NumericData[5];

        // EnginesCurrent  = msg.NumericData[???]; // todo: implement
    }

    if(msg.BinaryData.length() < OutRegistersCount)
    {
        Serial.println("Error: not enough binary data for output registers. Skipping update.");
    }
    else
    {
        // memcpy(OutRegisters, msg.BinaryData.c_str(), OutRegistersCount);
        for(uint8_t i = 0; i < OutRegistersCount; i++)
        {
            OutRegisters[i] = msg.BinaryData[i];
        }
    }

    Serial.println("Message accepted");
}

PhysPultMessage PhysPult::MessageToSend()
{
    PhysPultMessage msg;
    
    msg.NumericData += CranePosition;
    
    msg.BinaryData.reserve(InRegistersCount);
    // memcpy((char*)msg.BinaryData.c_str(), InRegisters, InRegistersCount);

    for(uint8_t i = 0; i < InRegistersCount; i++)
    {
        msg.BinaryData += InRegisters[i];
    }

    return msg;
}
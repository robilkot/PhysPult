#include "PhysPult.h"

PhysPult::PhysPult(void (*state)(PhysPult&)) : State(state)
{
    for(uint8_t i = 0; i < LightingLedCount1; i++)
    {
        LightingLeds1[i] = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);
    }
    for(uint8_t i = 0; i < LightingLedCount2; i++)
    {
        LightingLeds2[i] = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);
    }
    for(uint8_t i = 0; i < GaugesLightingLedCount; i++)
    {
        GaugesLeds[i] = CHSV(LightingColorHue, LightingColorSat, LightingColorvalue);
    }
}

void PhysPult::AcceptMessage(PhysPultMessage msg)
{
    if(msg.Type != PhysPultMessageTypes::WORKING)
    {
        return;
    }

    msg.NumericData.reserve(7);
    
    Speed           = msg.NumericData[0];

    TmValue         = msg.NumericData[1];
    NmValue         = msg.NumericData[2];
    TcValue         = msg.NumericData[3];

    BatteryVoltage  = msg.NumericData[4];
    SupplyVoltage   = msg.NumericData[5];

    EnginesCurrent  = msg.NumericData[6];
    
    msg.NumericData.reserve(OutRegistersCount);

    for(uint8_t i = 0; i < OutRegistersCount; i++)
    {
        OutRegisters[i] = msg.BinaryData[i];
    }
    
    // Serial.println("Message accepted");
}

PhysPultMessage PhysPult::MessageToSend()
{
    PhysPultMessage msg;
    
    msg.Type == PhysPultMessageTypes::WORKING;

    msg.NumericData.emplace_back(CranePosition);
    
    msg.BinaryData.reserve(InRegistersCount);

    for(uint8_t i = 0; i < InRegistersCount; i++)
    {
        msg.BinaryData.emplace_back(InRegisters[i]);
    }

    return msg;
}

void PhysPult::Reset()
{
    Speed = 0;
    BatteryVoltage = 0;
    TmValue = 0;
    NmValue = 0;
    TcValue = 0;
    SupplyVoltage = 0;
    EnginesCurrent = 0;

    for(auto& r : OutRegisters)
    {
        r = 0;
    }
    for(auto& r : InRegisters)
    {
        r = 0;
    }
}
#pragma once

#include "Hardware/Hardware.h"
#include "Communication/PultMessage.h"
#include "Communication/Communicator.h"

class Pult
{
    private:
    static Hardware hardware;
    static Communicator communicator;
    static uint8_t speed;
    // todo kv, pb, other stuff

    static void on_message(PultMessage& msg)
    {
        if(msg.Type == PultMessageTypes::DEBUG)
        {
            return;
        }

        msg.NumericData.reserve(7);
        
        speed = msg.NumericData[0];
        hardware.tm_position = msg.NumericData[1];
        hardware.nm_position = msg.NumericData[2];
        hardware.tc_position = msg.NumericData[3];
        hardware.battery_voltage = msg.NumericData[4];
        hardware.supply_voltage = msg.NumericData[5];
        hardware.engines_current = msg.NumericData[6];

        // todo set out registers values
        
        msg.BinaryData.reserve(OutRegistersCount);

        for(uint8_t i = 0; i < OutRegistersCount; i++)
            hardware.registers_out[i] = msg.BinaryData[i];

        PultMessage msgToSend;

        msgToSend.Type = PultMessageTypes::WORKING;
        msgToSend.NumericData.emplace_back(hardware.crane_position);
        msgToSend.BinaryData.reserve(InRegistersCount);

        for(auto reg : hardware.registers_in)
            msgToSend.BinaryData.emplace_back(reg);

        communicator.send(msgToSend.ToString());
    } 

    static void on_disconnect()
    {
        reset();
    }

    static void reset()
    {
        speed = 0;
        hardware.battery_voltage = 0;
        hardware.tm_position = 0;
        hardware.nm_position = 0;
        hardware.tc_position = 0;
        hardware.supply_voltage = 0;
        hardware.engines_current = 0;

        for(auto& r : hardware.registers_out)
            r = 0;
    }

    public:
    static void start()
    {
        communicator.set_on_message(on_message);
        communicator.set_on_disconnect(on_disconnect);

        xTaskCreate(
            [](void* param) { hardware.start(); },
            "background_hw_function",
            8000,
            nullptr,
            1,  // Priority
            nullptr
        );

        xTaskCreate(
            [](void* param) { communicator.start(); },
            "background_net_function",
            8000,
            nullptr,
            1,  // Priority
            nullptr
        );
    }
};
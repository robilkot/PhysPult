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

    void reset()
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

    void accept_work_message(const WorkPultMessage& msg)
    {
        speed = msg.NumericData[0];
        hardware.tm_position = msg.NumericData[1];
        hardware.nm_position = msg.NumericData[2];
        hardware.tc_position = msg.NumericData[3];
        hardware.battery_voltage = msg.NumericData[4];
        hardware.supply_voltage = msg.NumericData[5];
        hardware.engines_current = msg.NumericData[6];

        // todo set out registers values

        for(uint8_t i = 0; i < OutRegistersCount; i++)
            hardware.registers_out[i] = msg.BinaryData[i];

        WorkPultMessage response;

        response.NumericData.emplace_back(hardware.crane_position);
        response.BinaryData.reserve(InRegistersCount);

        for(auto reg : hardware.registers_in)
            response.BinaryData.emplace_back(reg);

        communicator.send(response);
    }

    void accept_debug_message(DebugPultMessage& msg)
    {
        bool success = true;

        switch (msg.action)
        {
        case DebugActions::RESET: {
            reset();
            log_i("Reset.");
            break;
        }
        case DebugActions::REGISTER_OUT_ENABLE: {
            for(auto out_index : msg.params) {
                if(out_index > (OutRegistersCount * 8 - 1)) {
                    success = false;
                    log_i("Pin index %d out of range", out_index);
                    break;
                }

                auto register_index = out_index / 8;
                auto pin_index = out_index % 8;
                hardware.registers_out[register_index] |= (1 << pin_index);
                log_i("Pin %d enabled.", out_index);
            }
            break;
        }
        case DebugActions::REGISTER_OUT_DISABLE: {
            for(auto out_index : msg.params) {
                if(out_index > (OutRegistersCount * 8 - 1)) {
                    success = false;
                    log_i("Pin %d out of range", out_index);
                    break;
                }

                auto register_index = out_index / 8;
                auto pin_index = out_index % 8;
                hardware.registers_out[register_index] &= !(1 << pin_index);
                log_i("Pin %d disabled.", out_index);
            }
            break;
        }
        case DebugActions::TOGGLE_LIGHTING: {
            hardware.lighting_enabled = !hardware.lighting_enabled;
            log_i("Lighting set to %d", hardware.lighting_enabled);
            break;
        }

        default:
            log_i("Debug action of unknown type.");
            success = false;
        }

        DebugPultMessage response;
        response.action = success ? DebugActions::OK : DebugActions::ERROR;
        communicator.send(response);
    }

    void accept_message(PultMessage& msg)
    {
        switch (msg.get_type())
        {
        case 'W': {
            accept_work_message(static_cast<WorkPultMessage&>(msg));
            break;
        }
        case 'D': {
            accept_debug_message(static_cast<DebugPultMessage&>(msg));
            break;
        }
        default:
            log_w("Message with type %s ignored", msg.get_type());
            break;
        }
    }

    public:
    void start()
    {
        communicator.set_on_message([this](PultMessage& msg) { accept_message(msg); });
        communicator.set_on_disconnect([this](void) { reset(); });

        xTaskCreate(
            [](void* param) { hardware.start(); },
            "hw_function",
            8000,
            nullptr,
            1,  // Priority
            nullptr
        );

        xTaskCreate(
            [](void* param) { communicator.start(); },
            "net_function",
            8000,
            nullptr,
            1,  // Priority
            nullptr
        );
    }
};
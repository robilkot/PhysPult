#pragma once

#include "Hardware/Hardware.h"
#include "Communication/PultMessage.h"
#include "Communication/Communicator.h"
#include <map>

class Pult
{
    private:
    static std::vector<uint8_t> digit_pins;
    static std::array<std::vector<uint8_t>, 10> symbols_left;
    static std::array<std::vector<uint8_t>, 10> symbols_right;
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

        hardware.clear_output();

        display_number(communicator.device_number);
    }

    void display_number(uint8_t number) {
        auto left_digit = (number / 10) % 10;
        auto right_digit = number % 10;

        for(auto pin_index : symbols_left[left_digit]) {
            hardware.set_output(pin_index, true);
        }
        for(auto pin_index : symbols_right[right_digit]) {
            hardware.set_output(pin_index, true);
        }
    }

    void accept_work_message(WorkPultMessage& msg)
    {

        speed = msg.numeric_data[0];
        hardware.tm_position = msg.numeric_data[1];
        hardware.nm_position = msg.numeric_data[2];
        hardware.tc_position = msg.numeric_data[3];
        hardware.battery_voltage = msg.numeric_data[4];
        hardware.supply_voltage = msg.numeric_data[5];
        hardware.engines_current = msg.numeric_data[6];

        hardware.clear_output();

        // for(auto output : msg.BinaryData)
        //     hardware.set_output(output, true);
            
        display_number(speed);

        WorkPultMessage response;

        response.numeric_data[0] = hardware.crane_position;

        for(int i = 0; i < InRegistersCount; i++)
            response.binary_data[i] = hardware.registers_in[i];

        communicator.send(response);
        
    }

    void accept_debug_message(DebugPultMessage& msg)
    {
        bool success = true;

        switch (msg.action)
        {
        case DebugActions::RESET: {
            reset();
            log_d("Reset.");
            break;
        }
        case DebugActions::REGISTER_OUT_ENABLE: {
            for(auto out_index : msg.params) {
                if(out_index > (OutRegistersCount * 8 - 1)) {
                    success = false;
                    log_d("Pin index %d out of range", out_index);
                    break;
                }

                hardware.set_output(out_index, true);
                log_d("Pin %d enabled.", out_index);
            }
            break;
        }
        case DebugActions::REGISTER_OUT_DISABLE: {
            for(auto out_index : msg.params) {
                if(out_index > (OutRegistersCount * 8 - 1)) {
                    success = false;
                    log_d("Pin %d out of range", out_index);
                    break;
                }

                hardware.set_output(out_index, false);
                log_d("Pin %d disabled.", out_index);
            }
            break;
        }
        case DebugActions::TOGGLE_LIGHTING: {
            hardware.lighting_enabled = !hardware.lighting_enabled;
            log_d("Lighting set to %d", hardware.lighting_enabled);
            break;
        }

        default:
            log_d("Debug action of unknown type.");
            success = false;
        }

        DebugPultMessage response;
        response.action = success ? DebugActions::OK : DebugActions::ERROR;
        communicator.send(response);
    }

    void accept_message(PultMessage& msg)
    {
        // auto start = esp_timer_get_time();

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
        
        // auto end = esp_timer_get_time();
        // log_d("%lld ms", (end - start) / 1000);
    }

    public:
    void start()
    {
        reset();

        communicator.set_on_message([this](PultMessage& msg) { accept_message(msg); });
        communicator.set_on_disconnect([this](void) { reset(); });
        communicator.set_on_ip_changed([this](int ip) { display_number(communicator.device_number); });

        xTaskCreatePinnedToCore(
            [](void* param) { hardware.start(); },
            "hw_function",
            8000,
            nullptr,
            1,  // Priority
            nullptr,
            1
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
#include "Pult.h"

Communicator Pult::communicator;
Hardware Pult::hardware;
std::array<uint8_t, 14> Pult::digit_pins = { // Happens to be a range [26, 39]
    31, 33, 29, 37, 27, 30, 28, 26, 34, 39, 38, 36, 32, 35
};
std::array<std::vector<uint8_t>, 10> Pult::symbols_left = {{
        {31, 33, 29, 37, 27, 30},
        {37, 27},
        {37, 29, 28, 31, 30},
        {37, 29, 30, 28, 27},
        {33, 28, 37, 27},
        {29, 33, 28, 27, 30},
        {29, 33, 28, 27, 30, 31},
        {37, 29, 27},
        {31, 33, 29, 37, 27, 30, 28},
        {33, 29, 37, 27, 30, 28}
    }};
std::array<std::vector<uint8_t>, 10>  Pult::symbols_right = {{
        {26, 34, 39, 38, 36, 32},
        {38, 36},
        {38, 39, 35, 26, 32},
        {38, 39, 32, 35, 36},
        {34, 35, 38, 36},
        {39, 34, 35, 36, 32},
        {39, 34, 35, 36, 32, 26},
        {38, 39, 36},
        {26, 34, 39, 38, 36, 32, 35},
        {34, 39, 38, 36, 32, 35}
    }};

void Pult::reset()
{
    hardware.battery_voltage = 0;
    hardware.tm_position = 0;
    hardware.nm_position = 0;
    hardware.tc_position = 0;
    hardware.supply_voltage = 0;
    hardware.engines_current = 0;

    hardware.clear_output();

    display_symbols(communicator.device_number);
}

void Pult::display_symbols(uint8_t number) {
    std::lock_guard<std::mutex> lock(hardware.mutex);

    for(auto pin_index : digit_pins) {
        hardware.set_output(pin_index, false);
    }

    auto left_digit = (number / 10) % 10;
    auto right_digit = number % 10;

    for(auto pin_index : symbols_left[left_digit]) {
        hardware.set_output(pin_index, true);
    }
    for(auto pin_index : symbols_right[right_digit]) {
        hardware.set_output(pin_index, true);
    }
}

ReverserPosition Pult::get_reverser_position()
{
    ReverserPosition position;
    
    // Reverser shaft is attached to pins 3, 4 of register 6
    auto reverser_bits = hardware.registers_in[5] & B00110000;

    // Defined by hardware connections
    switch (reverser_bits)
    {
    case B00010000: {
        position = ReverserPosition::F;
        break;
    }
    case B00110000: {
        position = ReverserPosition::O;
        break;
    }
    case B00100000: {
        position = ReverserPosition::R;
        break;
    }
    default:
        log_w("Unknown reverser position (%d).", reverser_bits);
        break;
    }
    
    return position;
} 


ControllerPosition Pult::get_controller_position()
{
    ControllerPosition position;

    // Main shaft is attached to last 4 bits of register 6
    auto controller_bits = hardware.registers_in[5] & B00001111;

    // Defined by hardware connections
    switch (controller_bits)
    {
    case B00001000: {
        position = ControllerPosition::T2;
        break;
    }
    case B00001001: {
        position = ControllerPosition::T1A;
        break;
    }
    case B00001011: {
        position = ControllerPosition::T1;
        break;
    }
    case B00000111: {
        position = ControllerPosition::O;
        break;
    }
    case B00000011: {
        position = ControllerPosition::X1;
        break;
    }
    case B00000001: {
        position = ControllerPosition::X2;
        break;
    }
    case B00000000: {
        position = ControllerPosition::X3;
        break;
    }
    case B00001111: {
        log_v("Intermediate controller position (%d).", controller_bits);
        break;
    }
    default:
        log_v("Unknown controller position (%d).", controller_bits);
        break;
    }

    return position;
}


void Pult::accept_work_message(WorkPultMessage& msg)
{
    auto speed = msg.numeric_data[0];
    hardware.tm_position = msg.numeric_data[1];
    hardware.nm_position = msg.numeric_data[2];
    hardware.tc_position = msg.numeric_data[3];
    hardware.battery_voltage = msg.numeric_data[4];
    hardware.supply_voltage = msg.numeric_data[5];
    hardware.engines_current = msg.numeric_data[6];

    for(int i = 0; i < OutRegistersCount; i++) {
        auto register_value = msg.binary_data[i];

        for(int k = 0; k < 8; k++)
        {
            hardware.set_output(8 * i + k, (register_value >> k) & 1);
        }
    }

    display_symbols(speed);

    WorkPultMessage response;

    response.numeric_data.emplace_back(hardware.crane_position);
    response.numeric_data.emplace_back((uint8_t)get_reverser_position());
    response.numeric_data.emplace_back((uint8_t)get_controller_position());

    for(int i = 0; i < InRegistersCount; i++)
        response.binary_data.emplace_back(hardware.registers_in[i]);

    communicator.send(response);
}

void Pult::accept_debug_message(DebugPultMessage& msg)
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
        hardware.toggle_lighting = !hardware.toggle_lighting;
        log_d("Lighting set to %d", hardware.toggle_lighting);
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

void Pult::accept_config_message(ConfigPultMessage& msg) {
    assert(false && "Config messages not implemented yet");    
}

void Pult::start()
{
    reset();

    communicator.set_on_message([this](PultMessage& msg) { msg.apply(*this); });
    communicator.set_on_disconnect([this](void) { reset(); });
    communicator.set_on_ip_changed([this](int ip) { display_symbols(ip); });

    xTaskCreatePinnedToCore(
        [](void* param) { hardware.start(); },
        "hw_function",
        2100, // takes 1892
        nullptr,
        1,  // Priority
        nullptr,
        1
    );

    xTaskCreate(
        [](void* param) { communicator.start(); },
        "net_function",
        2500, // takes 2084 but grows to 2356
        nullptr,
        1,  // Priority
        nullptr
    );
}
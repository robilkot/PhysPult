#include "Pult.h"

FeatureFlags Pult::feature_flags = FeatureFlags::None;
std::shared_ptr<Communicator> Pult::communicator;
Hardware Pult::hardware;
TaskHandle_t Pult::state_monitor;
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

void Pult::set_communicator(std::shared_ptr<Communicator> communicator_)
{
    Pult::communicator = communicator_;
}

void Pult::reset()
{
    hardware.battery_voltage = 0;
    hardware.tm_position = 0;
    hardware.nm_position = 0;
    hardware.tc_position = 0;
    hardware.supply_voltage = 0;
    hardware.engines_current = 0;

    hardware.clear_output();

    display_symbols(communicator->get_device_number());
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
        position = ReverserPosition::Intermediate;
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
    default:
        position = ControllerPosition::Intermediate;
        break;
    }

    return position;
}

void Pult::accept_state_changed_message(const StateChangePultMessage& msg)
{
    for(auto pin : msg.pins_enabled) {
        hardware.set_output(pin, true);
    }
    for(auto pin : msg.pins_disabled) {
        hardware.set_output(pin, false);
    }
    for(auto pair : msg.new_values) {
        switch (pair.first.output)
        {
        case OutputStateKeys::Speed:
            display_symbols(pair.second);
            break;
        case OutputStateKeys::BatteryVoltage:
            hardware.battery_voltage = pair.second;
            break;
        case OutputStateKeys::SupplyVoltage:
            hardware.supply_voltage = pair.second;
            break;
        case OutputStateKeys::EnginesCurrent:
            hardware.engines_current = pair.second;
            break;
        case OutputStateKeys::TM:
            hardware.tm_position = pair.second;
            break;
        case OutputStateKeys::NM:
            hardware.nm_position = pair.second;
            break;
        case OutputStateKeys::TC:
            hardware.tc_position = pair.second;
            break;
        default:
            log_e("Unknown input key.");
            break;
        }
    }
}

// Returns full inputs lits
void Pult::accept_state_request_message(const StateRequestMessage& msg)
{
    StateChangePultMessage response;

    if(has_flag(feature_flags, FeatureFlags::Controller)) {
        response.new_values.emplace_back(StateKeys{ .input = InputStateKeys::Controller }, (int16_t)get_controller_position());
    }
    if(has_flag(feature_flags, FeatureFlags::Reverser)) {
        response.new_values.emplace_back(StateKeys{ .input = InputStateKeys::Reverser }, (int16_t)get_reverser_position());
    }
    if(has_flag(feature_flags, FeatureFlags::Crane)) {
        response.new_values.emplace_back(StateKeys{ .input = InputStateKeys::Crane }, (int16_t)hardware.crane_position);
    }

    if(has_flag(feature_flags, FeatureFlags::InputRegisters)) {
        for(int i = 0; i < InRegistersCount; i++) {
            auto register_value = hardware.registers_in[i];

            for(int k = 0; k < 8; k++)
            {
                if((register_value >> k) & 1) {
                    response.pins_enabled.emplace_back(8 * i + k);
                } else {
                    response.pins_disabled.emplace_back(8 * i + k);
                }
            }
        }
    }

    communicator->send(response);
}

void Pult::accept_debug_message(const DebugPultMessage& msg)
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
        log_e("Debug action of unknown type.");
        success = false;
    }

    communicator->send(success ? DebugPultMessage::Ok() : DebugPultMessage::Error());
}

void Pult::accept_config_message(const ConfigPultMessage& msg) {
    for(const auto& pair : msg.values) {
        switch (pair.first)
        {
        case ConfigActions::ENABLE_FEATURE: {
            auto feature = pair.second.feature_flag;
            feature_flags = feature_flags | feature;
            log_i("Enabled feature %d.", pair.second);
            break;
        }
        case ConfigActions::DISABLE_FEATURE: {
            auto feature = pair.second.feature_flag;
            feature_flags = feature_flags & ~feature;
            log_i("Disabled feature %d.", pair.second);
            break;
        }
        case ConfigActions::SET_LIGHTING_H: {
            LightingColor.h = pair.second.number;
            log_i("Set lighting hue to %d.", pair.second);
            break;
        }
        case ConfigActions::SET_LIGHTING_S: {
            LightingColor.s = pair.second.number;
            log_i("Set lighting saturation to %d.", pair.second);
            break;
        }
        case ConfigActions::SET_LIGHTING_V: {
            LightingColor.v = pair.second.number;
            log_i("Set lighting base value to %d.", pair.second);
            break;
        }

        default:
            log_e("Config action of unknown type.");
            communicator->send(DebugPultMessage::Error());
        }
    }
}

void Pult::monitor_state()
{
    ControllerPosition controller_position_p = get_controller_position();
    ReverserPosition reverser_position_p = get_reverser_position();
    uint8_t crane_position_p = hardware.crane_position;
    uint8_t registers_in_p[InRegistersCount];
    std::copy(hardware.registers_in, hardware.registers_in + InRegistersCount, registers_in_p);

    while(true) {
        vTaskDelay(pdMS_TO_TICKS(1));
        bool update_needed = false;
        StateChangePultMessage update;

        if(has_flag(feature_flags, FeatureFlags::Crane)) {
            if(hardware.crane_position != crane_position_p) {
                update_needed = true;
                crane_position_p = hardware.crane_position;
                update.new_values.emplace_back(StateKeys{ .input = InputStateKeys::Crane }, (int16_t)hardware.crane_position);
            }
        } 

        if(has_flag(feature_flags, FeatureFlags::Controller)) {
            auto controller_position = get_controller_position(); 
            if(controller_position != controller_position_p) {
                update_needed = true;
                controller_position_p = controller_position;
                if(controller_position != ControllerPosition::Intermediate) {
                    update.new_values.emplace_back(StateKeys{ .input = InputStateKeys::Controller }, (int16_t)controller_position);
                }
            }
        }

        if(has_flag(feature_flags, FeatureFlags::Reverser)) {
            auto reverser_position = get_reverser_position(); 
            if(reverser_position != reverser_position_p) {
                update_needed = true;
                reverser_position_p = reverser_position;
                if(reverser_position != ReverserPosition::Intermediate) {
                    update.new_values.emplace_back(StateKeys{ .input = InputStateKeys::Reverser }, (int16_t)reverser_position);
                }
            }
        }
        
        if(has_flag(feature_flags, FeatureFlags::InputRegisters)) {
            for(int i = 0; i < InRegistersCount; i++) {
                auto old_register_value = registers_in_p[i];
                auto new_register_value = hardware.registers_in[i];

                for(int k = 0; k < 8; k++)
                {
                    auto old_value = old_register_value >> k & 1;
                    auto new_value = new_register_value >> k & 1;

                    if(old_value ^ new_value) {
                        update_needed = true;

                        if(new_value) {
                            update.pins_enabled.emplace_back(8 * i + k);
                        } else {
                            update.pins_disabled.emplace_back(8 * i + k);
                        }
                    }
                }
            }
        }

        if(update_needed) {
            std::copy(hardware.registers_in, hardware.registers_in + InRegistersCount, registers_in_p);
            communicator->send(update);
        }
    }
}

void Pult::start()
{
    reset();

    communicator->set_on_message([this](const PultMessage& message) { message.apply(*this); });
    communicator->set_on_device_number_changed([this](int number) { display_symbols(number); });
    communicator->set_on_connect([this](void) {
        xTaskCreatePinnedToCore(
            [](void* param) { monitor_state(); },
            "pult_state_monitor",
            8000, // takes ???
            nullptr,
            1,  // Priority
            &state_monitor,
            1
        );
        
        communicator->send(StateRequestMessage());
        });
    communicator->set_on_disconnect([this](void) {
        if(state_monitor) {
            vTaskDelete(state_monitor);
        }
        reset();
        });

    xTaskCreatePinnedToCore(
        [](void* param) { hardware.start(); },
        "pult_hardware",
        2500, // takes 1892
        nullptr,
        1,  // Priority
        nullptr,
        1
    );

    xTaskCreatePinnedToCore(
        [](void* param) { communicator->start(); },
        "pult_communicator",
        3500, // takes 2084 but grows to 2356
        nullptr,
        5,  // Priority
        nullptr,
        0
    );
}
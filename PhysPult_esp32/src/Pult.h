#pragma once

#include "FeatureFlags.h"
#include "Hardware/Hardware.h"
#include "Communication/PultMessage.h"
#include "Communication/Communicator.h"

#include <memory>

class Communicator;
class PultMessage;
class ConfigPultMessage;
class DebugPultMessage;
class StateChangePultMessage;
class StateRequestMessage;
class WebsocketsCommunicator;

enum class ControllerPosition : uint8_t {
    O, // Neutral
    X1,
    X2,
    X3,
    T1,
    T1A,
    T2,
    Intermediate, // In case measurements are incorrect
};

enum class ReverserPosition : uint8_t {
    O, // Neutral
    F, // Forward
    R, // Rear
    Intermediate, // In case measurements are incorrect
};

class Pult
{
    private:
    static FeatureFlags feature_flags;
    static std::array<uint8_t, 14> digit_pins;
    static std::array<std::vector<uint8_t>, 10> symbols_left;
    static std::array<std::vector<uint8_t>, 10> symbols_right;
    static Hardware hardware;
    static std::shared_ptr<Communicator> communicator;
    static TaskHandle_t state_monitor;

    static void monitor_state();
    static void reset();
    static void display_symbols(uint8_t number);
    static ReverserPosition get_reverser_position();
    static ControllerPosition get_controller_position();

    public:
    Pult() = delete;
    static void set_communicator(std::shared_ptr<Communicator> communicator);

    static void accept_config_message(const ConfigPultMessage& msg);
    static void accept_debug_message(const DebugPultMessage& pult_msg);
    static void accept_state_changed_message(const StateChangePultMessage& msg);
    static void accept_state_request_message(const StateRequestMessage& msg);

    static void start();
};
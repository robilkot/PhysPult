#pragma once

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

enum class FeatureFlags {
    Controller = 0,
    Reverser = 1,
    Crane = 2,
    InputRegisters = 4,
};

inline FeatureFlags operator|(FeatureFlags a, FeatureFlags b)
{
    return static_cast<FeatureFlags>(static_cast<int>(a) | static_cast<int>(b));
}
inline bool operator&(FeatureFlags source, FeatureFlags target)
{
    return static_cast<bool>((static_cast<int>(source) & static_cast<int>(target)) == static_cast<int>(target));
}


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
    static void set_communicator(std::shared_ptr<Communicator> communicator);

    void accept_config_message(const ConfigPultMessage& msg);
    void accept_debug_message(const DebugPultMessage& pult_msg);
    void accept_state_changed_message(const StateChangePultMessage& msg);
    void accept_state_request_message(const StateRequestMessage& msg);

    void start();
};
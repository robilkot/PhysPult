#pragma once

#include "Hardware/Hardware.h"
#include "Communication/PultMessage.h"
#include "Communication/Communicator.h"

class WorkPultMessage;
class ConfigPultMessage;
class DebugPultMessage;
class Communicator;

class Pult
{
    private:
    static std::array<uint8_t, 14> digit_pins;
    static std::array<std::vector<uint8_t>, 10> symbols_left;
    static std::array<std::vector<uint8_t>, 10> symbols_right;
    static Hardware hardware;
    static Communicator communicator;

    // todo kv, pb, other stuff

    void reset();
    void display_symbols(uint8_t number);
    ReverserPosition get_reverser_position();
    ControllerPosition get_controller_position();

    public:
    void accept_work_message(WorkPultMessage& pult_msg);
    void accept_config_message(ConfigPultMessage& msg);
    void accept_debug_message(DebugPultMessage& pult_msg);

    void start();
};
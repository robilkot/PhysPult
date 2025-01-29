#pragma once

#include "PultMessage.h"

class PultMessage;
class WorkPultMessage;
class DebugPultMessage;
class ConfigPultMessage;
class StateChangePultMessage;

class PultMessageFactory {
    private:
    const static char delimiter = ';';
    static std::shared_ptr<DebugPultMessage> create_debug_message(const std::string& str, size_t delimIndex);
    static std::shared_ptr<ConfigPultMessage> create_config_message(const std::string& str, size_t delimIndex);
    static std::shared_ptr<StateChangePultMessage> create_state_changed_message(const std::string& str, size_t delimIndex);

    public:
    static std::shared_ptr<PultMessage> Create(std::string str);
};

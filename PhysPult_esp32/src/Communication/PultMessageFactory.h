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
    static WorkPultMessage* create_work_message(String& str, int& delimIndex);
    static DebugPultMessage* create_debug_message(String str, int& delimIndex);
    static ConfigPultMessage* create_config_message(String str, int& delimIndex);
    static StateChangePultMessage* create_state_changed_message(String& str, int& delimIndex);

    public:
    static std::unique_ptr<PultMessage> Create(String str);
};

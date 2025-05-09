#include "PultMessage.h"

void StateChangePultMessage::apply() const {
    Pult::accept_state_changed_message(*this);
}

void StateRequestMessage::apply() const {
    Pult::accept_state_request_message(*this);
}

void ConfigPultMessage::apply() const {
    Pult::accept_config_message(*this);
}

void DebugPultMessage::apply() const {
    Pult::accept_debug_message(*this);
}

std::string StateChangePultMessage::to_string() const {
    std::string output{get_type()};

    output += ';';

    for(auto i : pins_enabled)
    {
        output += std::to_string(i);
        output += ',';
    }
    if(pins_enabled.size() == 0) {
        output += ';';
    }

    output[output.length() - 1] = ';';

    for(auto i : pins_disabled)
    {
        output += std::to_string(i);
        output += ',';
    }
    if(pins_disabled.size() == 0) {
        output += ';';
    }

    output[output.length() - 1] = ';';

    
    for(auto i : new_values)
    {
        output += std::to_string((int)(i.first.output));
        output += '/';
        output += std::to_string(i.second);
        output += ',';
    }
    if(new_values.size() == 0) {
        output += ';';
    }

    output[output.length() - 1] = ';';

    return output;
}

std::string DebugPultMessage::to_string() const {
    std::string output{get_type()};
    output += ';';
    output += std::to_string((int)action);
    output += ';';
    for(auto param : params) {
        output += std::to_string((int)action);
        output += ',';
    }
    output[output.length() - 1] = ';';
    return output;
}
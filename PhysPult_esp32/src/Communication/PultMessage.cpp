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

String StateChangePultMessage::to_string() const {
    String output{get_type()};

    output += ';';

    for(auto i : pins_enabled)
    {
        output += String(i);
        output += ',';
    }
    if(pins_enabled.size() == 0) {
        output += ';';
    }

    output[output.length() - 1] = ';';

    for(auto i : pins_disabled)
    {
        output += String(i);
        output += ',';
    }
    if(pins_disabled.size() == 0) {
        output += ';';
    }

    output[output.length() - 1] = ';';

    
    for(auto i : new_values)
    {
        output += String((int)(i.first.output));
        output += '/';
        output += String(i.second);
        output += ',';
    }
    if(new_values.size() == 0) {
        output += ';';
    }

    output[output.length() - 1] = ';';

    return output;
}

String DebugPultMessage::to_string() const {
    String output(get_type());
    output += ';';
    output += (int)action;
    output += ';';
    for(auto param : params) {
        output += (int)action;
        output += ',';
    }
    output[output.length() - 1] = ';';
    return output;
}
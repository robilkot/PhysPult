#include "PultMessage.h"

String WorkPultMessage::to_string() {
    String output(get_type());
    output += ';';

    for(auto i : numeric_data)
    {
        output += String(i);
        output += ',';
    }

    output[output.length() - 1] = ';';

    for(auto i : binary_data)
    {
        output += String(i);
        output += ',';
    }

    output[output.length() - 1] = ';';

    return output;
}

char WorkPultMessage::get_type() {
    return 'W';
}

void WorkPultMessage::apply(Pult& pult) {
    pult.accept_work_message(*this);
}



String ConfigPultMessage::to_string() {
    String output(get_type());
    output += ';';
    return output;
}

char ConfigPultMessage::get_type() {
    return 'C';
}

void ConfigPultMessage::apply(Pult& pult) {
    pult.accept_config_message(*this);
}



String DebugPultMessage::to_string() {
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

char DebugPultMessage::get_type() {
    return 'D';
}

void DebugPultMessage::apply(Pult& pult) {
    pult.accept_debug_message(*this);
}
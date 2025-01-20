#include "PultMessageFactory.h"


StateChangePultMessage* PultMessageFactory::create_state_changed_message(String& str, int& delimIndex)
{
    auto msg = new StateChangePultMessage;

    // pins_enabled
    int secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    auto pins_enabled_string = str.substring(delimIndex + 1, secondDelimIndex);

    int commaIndex = -1;
    do {
        int secondCommaIndex = pins_enabled_string.indexOf(',', commaIndex + 1); 
        
        String binarySubstring;
        if(secondCommaIndex != -1)
        {
            binarySubstring = pins_enabled_string.substring(commaIndex + 1, secondCommaIndex);
        }
        else {
            binarySubstring = pins_enabled_string.substring(commaIndex + 1);
        }
        msg->pins_enabled.emplace_back(atoi(binarySubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    // pins_disabled
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    auto pins_disabled_string = str.substring(delimIndex + 1, secondDelimIndex);

    commaIndex = -1;
    do {
        int secondCommaIndex = pins_disabled_string.indexOf(',', commaIndex + 1); 
        
        String binarySubstring;
        if(secondCommaIndex != -1)
        {
            binarySubstring = pins_disabled_string.substring(commaIndex + 1, secondCommaIndex);
        }
        else {
            binarySubstring = pins_disabled_string.substring(commaIndex + 1);
        }
        msg->pins_disabled.emplace_back(atoi(binarySubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    // key value pairs
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    auto key_value_pairs_string = str.substring(delimIndex + 1, secondDelimIndex);

    commaIndex = -1;
    do {
        int secondCommaIndex = key_value_pairs_string.indexOf(',', commaIndex + 1); 
        
        String pair_substr;
        if(secondCommaIndex != -1)
        {
            pair_substr = key_value_pairs_string.substring(commaIndex + 1, secondCommaIndex);
        }
        else {
            pair_substr = key_value_pairs_string.substring(commaIndex + 1);
        }

        auto separator_index = pair_substr.indexOf('/');
        auto key = pair_substr.substring(0, separator_index - 1);
        auto value = pair_substr.substring(separator_index + 1);

        auto pair = std::make_pair(StateKeys { .input = (InputStateKeys)atoi(key.c_str()) }, atoi(value.c_str()));
        msg->new_values.emplace_back(pair);

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    return msg;
}

WorkPultMessage* PultMessageFactory::create_work_message(String& str, int& delimIndex)
{
    auto msg = new WorkPultMessage;

    // Parse numeric data
    int secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    auto numericDataString = str.substring(delimIndex + 1, secondDelimIndex);

    int commaIndex = -1;
    do {
        int secondCommaIndex = numericDataString.indexOf(',', commaIndex + 1); 
        
        String numberSubstring;
        if(secondCommaIndex != -1)
        {
            numberSubstring = numericDataString.substring(commaIndex + 1, secondCommaIndex);
        }
        else {
            numberSubstring = numericDataString.substring(commaIndex + 1);
        }
        msg->numeric_data.emplace_back(atoi(numberSubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    // Parse binary data
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    
    auto binaryDataString = str.substring(delimIndex + 1, secondDelimIndex);

    commaIndex = -1;
    do {
        int secondCommaIndex = binaryDataString.indexOf(',', commaIndex + 1); 
        
        String binarySubstring;
        if(secondCommaIndex != -1)
        {
            binarySubstring = binaryDataString.substring(commaIndex + 1, secondCommaIndex);
        }
        else {
            binarySubstring = binaryDataString.substring(commaIndex + 1);
        }
        msg->binary_data.emplace_back(atoi(binarySubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    return msg;
}

DebugPultMessage* PultMessageFactory::create_debug_message(String str, int& delimIndex)
{
    auto msg = new DebugPultMessage;

    // Parse action
    int secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    auto actionString = str.substring(delimIndex + 1, secondDelimIndex);
    msg->action = (DebugActions)atoi(actionString.c_str());

    // Parse params
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
    auto paramsString = str.substring(delimIndex + 1, secondDelimIndex);

    int commaIndex = -1;
    do {
        int secondCommaIndex = paramsString.indexOf(',', commaIndex + 1); 
        
        String numberSubstring;
        if(secondCommaIndex != -1)
        {
            numberSubstring = paramsString.substring(commaIndex + 1, secondCommaIndex);
        }
        else {
            numberSubstring = paramsString.substring(commaIndex + 1);
        }
        msg->params.emplace_back(atoi(numberSubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    return msg;
}

ConfigPultMessage* PultMessageFactory::create_config_message(String str, int& delimIndex)
{
    assert(false && "Config messages not implemented yet");    
}

std::unique_ptr<PultMessage> PultMessageFactory::Create(String str)
{
    int delimIndex = str.indexOf(delimiter);
    if(delimIndex <= 0) {
        throw std::invalid_argument("Wrong message format");
    }

    PultMessage* result;

    switch(str[0]) {
        case 'S': {
            result = create_state_changed_message(str, delimIndex);
            break;
        }
        case 'R': {
            result = new StateRequestMessage;
            break;
        }
        case 'W': {
            result = create_work_message(str, delimIndex);
            break;
        }
        case 'D': {
            result = create_debug_message(str, delimIndex);
            break;
        }
        case 'C': {
            result = new ConfigPultMessage;
            break;
        }
        default: {
            throw std::invalid_argument("Unknown message type");
            break;
        }
    }
    
    return std::unique_ptr<PultMessage>(result);
}
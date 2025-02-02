#include "PultMessageFactory.h"


std::shared_ptr<StateChangePultMessage> PultMessageFactory::create_state_changed_message(const std::string& str, size_t delimIndex)
{
    auto msg = std::make_shared<StateChangePultMessage>();

    // pins_enabled
    int secondDelimIndex = str.find(delimiter, delimIndex + 1);
    auto pins_enabled_string = str.substr(delimIndex + 1, secondDelimIndex - delimIndex - 1);

    int commaIndex = -1;
    do {
        int secondCommaIndex = pins_enabled_string.find(',', commaIndex + 1); 
        
        std::string binarySubstring;
        if(secondCommaIndex != -1)
        {
            binarySubstring = pins_enabled_string.substr(commaIndex + 1, secondCommaIndex - commaIndex - 1);
        }
        else {
            binarySubstring = pins_enabled_string.substr(commaIndex + 1);
        }
        msg->pins_enabled.emplace_back(atoi(binarySubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    // pins_disabled
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.find(delimiter, delimIndex + 1);
    auto pins_disabled_string = str.substr(delimIndex + 1, secondDelimIndex - delimIndex - 1);

    commaIndex = -1;
    do {
        int secondCommaIndex = pins_disabled_string.find(',', commaIndex + 1); 
        
        std::string binarySubstring;
        if(secondCommaIndex != -1)
        {
            binarySubstring = pins_disabled_string.substr(commaIndex + 1, secondCommaIndex - commaIndex - 1);
        }
        else {
            binarySubstring = pins_disabled_string.substr(commaIndex + 1);
        }
        msg->pins_disabled.emplace_back(atoi(binarySubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    // key value pairs
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.find(delimiter, delimIndex + 1);
    auto key_value_pairs_string = str.substr(delimIndex + 1, secondDelimIndex - delimIndex - 1);

    commaIndex = -1;
    do {
        int secondCommaIndex = key_value_pairs_string.find(',', commaIndex + 1); 
        
        std::string pair_substr;
        if(secondCommaIndex != -1)
        {
            pair_substr = key_value_pairs_string.substr(commaIndex + 1, secondCommaIndex - commaIndex - 1);
        }
        else {
            pair_substr = key_value_pairs_string.substr(commaIndex + 1);
        }

        if(pair_substr.length() < 3) {
            break;
        }
        auto separator_index = pair_substr.find('/');
        auto key = std::stoi(pair_substr.substr(0, separator_index));
        auto value = std::stoi(pair_substr.substr(separator_index + 1));

        auto pair = std::make_pair(StateKeys { .output = (OutputStateKeys)key }, value);
        msg->new_values.emplace_back(pair);

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    return msg;
}

std::shared_ptr<DebugPultMessage> PultMessageFactory::create_debug_message(const std::string& str, size_t delimIndex)
{
    auto msg = std::make_shared<DebugPultMessage>();

    // Parse action
    int secondDelimIndex = str.find(delimiter, delimIndex + 1);
    auto actionString = str.substr(delimIndex + 1, secondDelimIndex - delimIndex - 1);
    msg->action = (DebugActions)atoi(actionString.c_str());

    // Parse params
    delimIndex = secondDelimIndex;
    secondDelimIndex = str.find(delimiter, delimIndex + 1);
    auto paramsString = str.substr(delimIndex + 1, secondDelimIndex - delimIndex - 1);

    int commaIndex = -1;
    do {
        int secondCommaIndex = paramsString.find(',', commaIndex + 1); 
        
        std::string numberSubstring;
        if(secondCommaIndex != -1)
        {
            numberSubstring = paramsString.substr(commaIndex + 1, secondCommaIndex - commaIndex - 1);
        }
        else {
            numberSubstring = paramsString.substr(commaIndex + 1);
        }
        msg->params.emplace_back(atoi(numberSubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    return msg;
}

std::shared_ptr<ConfigPultMessage> PultMessageFactory::create_config_message(const std::string& str, size_t delimIndex)
{
    auto msg = std::make_shared<ConfigPultMessage>();

    // key value pairs
    auto secondDelimIndex = str.find(delimiter, delimIndex + 1);
    auto key_value_pairs_string = str.substr(delimIndex + 1, secondDelimIndex - delimIndex - 1);

    auto commaIndex = -1;
    do {
        int secondCommaIndex = key_value_pairs_string.find(',', commaIndex + 1); 
        
        std::string pair_substr;
        if(secondCommaIndex != -1)
        {
            pair_substr = key_value_pairs_string.substr(commaIndex + 1, secondCommaIndex - commaIndex - 1);
        }
        else {
            pair_substr = key_value_pairs_string.substr(commaIndex + 1);
        }

        if(pair_substr.length() < 3) {
            break;
        }

        auto separator_index = pair_substr.find('/');
        auto key = std::stoi(pair_substr.substr(0, separator_index));
        auto value = std::stoi(pair_substr.substr(separator_index + 1));

        auto pair = std::make_pair((PultPreferencesKeys)key, PultPreferenceValue{ .number = value });
        msg->values.emplace_back(pair);

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    return msg;
}

std::shared_ptr<PultMessage> PultMessageFactory::Create(std::string str)
{
    auto delimIndex = str.find(delimiter);
    if(delimIndex == std::string::npos) {
        throw std::invalid_argument("Wrong message format");
    }

    std::shared_ptr<PultMessage> result;

    switch(str[0]) {
        case 'S': {
            result = create_state_changed_message(str, delimIndex);
            break;
        }
        case 'R': {
            result = std::make_shared<StateRequestMessage>();
            break;
        }
        case 'D': {
            result = create_debug_message(str, delimIndex);
            break;
        }
        case 'C': {
            result = create_config_message(str, delimIndex);
            break;
        }
        default: {
            throw std::invalid_argument("Unknown message type");
            break;
        }
    }
    
    return result;
}
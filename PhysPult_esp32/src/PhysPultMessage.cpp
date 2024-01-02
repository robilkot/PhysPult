#include "PhysPultMessage.h"

String PhysPultMessage::ToString()
{
    String output;

    output += Type;
    output += ';';

    for(auto i : NumericData)
    {
        output += String(i);
        output += ',';
    }

    output[output.length() - 1] = ';';

    for(auto i : BinaryData)
    {
        output += String(i);
        output += ',';
    }

    output[output.length() - 1] = ';';

    return output;
}

PhysPultMessage::PhysPultMessage() { }

PhysPultMessage::PhysPultMessage(String msg)
{
    int delimIndex = msg.indexOf(Delimiter);
    if(delimIndex > 0)
    {
        Type = msg[0];
    }

    // No parsing required
    if(Type != PhysPultMessageTypes::WORKING)
    {
        return;
    }

    // Parse numeric data
    int secondDelimIndex = msg.indexOf(Delimiter, delimIndex + 1);
    auto numericDataString = msg.substring(delimIndex + 1, secondDelimIndex);

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
        NumericData.emplace_back(atoi(numberSubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);

    // Parse binary data
    delimIndex = secondDelimIndex;
    secondDelimIndex = msg.indexOf(Delimiter, delimIndex + 1);
    
    auto binaryDataString = msg.substring(delimIndex + 1, secondDelimIndex);

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
        BinaryData.emplace_back(atoi(binarySubstring.c_str()));

        commaIndex = secondCommaIndex;
    } while(commaIndex != -1);
}
#include "PhysPultMessage.h"

String PhysPultMessage::ToString()
{
    return Type + ';' + NumericData + ';' + BinaryData + ';';
}

PhysPultMessage::PhysPultMessage() { }

PhysPultMessage::PhysPultMessage(String msg)
{
    int delimIndex = msg.indexOf(Delimiter);
    if(delimIndex > 0)
    {
        Type = msg[0];
    }

    int secondDelimIndex = msg.indexOf(Delimiter, delimIndex + 1);
    NumericData = msg.substring(delimIndex + 1, secondDelimIndex);

    delimIndex = secondDelimIndex;
    secondDelimIndex = msg.indexOf(Delimiter, delimIndex + 1);
    
    BinaryData = msg.substring(delimIndex + 1, secondDelimIndex);
}
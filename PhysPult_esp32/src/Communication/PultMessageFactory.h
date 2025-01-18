#include "PultMessage.h"


class PultMessageFactory {
    private:
    const static char delimiter = ';';

    public:
    static std::unique_ptr<PultMessage> Create(String str)
    {
        int delimIndex = str.indexOf(delimiter);
        if(delimIndex <= 0) {
            throw std::invalid_argument("Wrong message format");
        }

        PultMessage* result;

        switch(str[0]) {
            case 'W': {
                auto msg = new WorkPultMessage;

                // Parse numeric data
                int secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
                auto numericDataString = str.substring(delimIndex + 1, secondDelimIndex);

                int commaIndex = -1;
                int numeric_index = 0;
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
                    msg->numeric_data[numeric_index] = atoi(numberSubstring.c_str());

                    commaIndex = secondCommaIndex;
                    numeric_index++;
                } while(commaIndex != -1);

                // Parse binary data
                delimIndex = secondDelimIndex;
                secondDelimIndex = str.indexOf(delimiter, delimIndex + 1);
                
                auto binaryDataString = str.substring(delimIndex + 1, secondDelimIndex);

                commaIndex = -1;
                int binary_index = 0;
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
                    msg->binary_data[binary_index] = atoi(binarySubstring.c_str());

                    commaIndex = secondCommaIndex;
                    binary_index++;
                } while(commaIndex != -1);

                result = msg;
                break;
            }
            case 'D': {
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

                result = msg;
                break;
            }
            case 'C': {
                auto msg = new ConfigPultMessage;

                result = msg;
                break;
            }
            default: {
                throw std::invalid_argument("Unknown message type");
                break;
            }
        }
        
        // log_d("%s", result->to_string());
        return std::unique_ptr<PultMessage>(result);
    }
};

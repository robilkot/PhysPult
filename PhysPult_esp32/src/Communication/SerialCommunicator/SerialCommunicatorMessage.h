#pragma once

#include <Communication/PultMessage.h>
#include <Communication/PultMessageFactory.h>
#include "crc32/crc32.h"

// PDU format:
// ---------------------------------------------------------
// |   4   |      4      |      4      |     ...     |  1  |
// ---------------------------------------------------------
// | crc32 | seq. number | ack. number | PultMessage | \n  |
// ---------------------------------------------------------
//
// Encapsulates PultMessage to provide mechanism for ack-ing messages and validation

class SerialCommunicatorMessage
{
    private:
    uint32_t crc;
    uint32_t sequence_number;
    uint32_t ack_number;
    std::shared_ptr<PultMessage> message; // Not good considering PultMessage is mutable. Whatever for now

    uint32_t get_crc() const
    {
        auto bytes = to_bytes();

        return crc32<IEEE8023_CRC32_POLYNOMIAL>(0xFFFFFFFF, bytes.begin() + 4, bytes.end());
    }

    public:
    SerialCommunicatorMessage(const std::vector<uint8_t>& bytes)
    {
        if(bytes.size() < 12) {
            return;
        }
        try {
            for (auto i = 0; i < 4; i++)
            {
                crc |= bytes[i] << (8 * (3 - i));
            }
            for (auto i = 0; i < 4; i++)
            {
                sequence_number |= bytes[i + 4] << (8 * (3 - i));
            }
            for (auto i = 0; i < 4; i++)
            {
                ack_number |= bytes[i + 8] << (8 * (3 - i));
            }

            size_t size = bytes.size() - 12;
            std::string message_str;
            message_str.reserve(size);

            for(auto it = bytes.begin() + 12; it != bytes.end(); it++)
            {
                message_str += (char)*it;
            }

            message = PultMessageFactory::Create(message_str);
        }
        catch(const std::exception& ex)
        {
            log_w("invalid serial message received: %s", ex.what());
        }
    }

    SerialCommunicatorMessage(std::shared_ptr<PultMessage> message, uint32_t sequence, uint32_t ack)
    : message(message), sequence_number(sequence), ack_number(ack)
    { 
        crc = get_crc();
    }

    std::shared_ptr<PultMessage> get_message()
    {
        return message; // mutable, not good
    }
    uint32_t get_sequence_number()
    {
        return sequence_number;
    }
    uint32_t get_ack_number()
    {
        return ack_number;
    }

    std::vector<uint8_t> to_bytes() const
    {
        std::string msg_string;
        if(message) {
            msg_string = message->to_string();
        }

        std::vector<uint8_t> output;
        output.reserve(12 + msg_string.size());
        
        for(auto i = 3; i >= 0; i--)
        {
            output.emplace_back(static_cast<uint8_t>((crc >> (i * 8)) & 255));
        }
        for(auto i = 3; i >= 0; i--)
        {
            output.emplace_back(static_cast<uint8_t>((sequence_number >> (i * 8)) & 255));
        }
        for(auto i = 3; i >= 0; i--)
        {
            output.emplace_back(static_cast<uint8_t>((ack_number >> (i * 8)) & 255));
        }
        for(auto c : msg_string)
        {
            output.emplace_back(c);
        }

        return output;
    }

    bool is_valid() const
    {
        return get_crc() == crc;
    }
};
#pragma once

#include "crc32/crc32.h"

// PDU format:
// -------------------------------------------------------------------------------
// |      1      |   4   |      4      |      4      |     ...     |      1      |
// -------------------------------------------------------------------------------
// | 0b01111110  | crc32 | seq. number | ack. number |   content   | 0b10000001  |
// -------------------------------------------------------------------------------
// CRC is calculated started from seq. number, finishing with content. Start and stop bits are not used for CRC.

class SerialCommunicatorMessage
{
    private:
    bool valid;
    uint32_t crc;
    uint32_t sequence_number;
    uint32_t ack_number;
    std::string content;

    uint32_t get_crc() const
    {
        auto bytes = to_bytes();

        // todo INCORRECT CRC !!!
        return crc32<IEEE8023_CRC32_POLYNOMIAL>(0xFFFFFFFF, bytes.begin() + 5, bytes.end() - 1);
    }

    public:
    const static uint8_t start_byte = B01111110;
    const static uint8_t stop_byte = B10000001;

    // Accepts byte array not including start and stop bytes
    SerialCommunicatorMessage(const std::vector<uint8_t>& bytes)
    {
        if(bytes.size() < 14) {
            log_w("invalid serial message: less than 12 bytes");
            return;
        }

        std::string message_str;

        try {
            for (auto i = 0; i < 4; i++)
            {
                crc |= bytes[i + 1] << (8 * (3 - i));
            }
            for (auto i = 0; i < 4; i++)
            {
                sequence_number |= bytes[i + 5] << (8 * (3 - i));
            }
            for (auto i = 0; i < 4; i++)
            {
                ack_number |= bytes[i + 9] << (8 * (3 - i));
            }

            size_t size = bytes.size() - 14;
            message_str.reserve(size);

            for(auto it = bytes.begin() + 13; it != bytes.end() - 1; it++)
            {
                message_str += (char)*it;
            }
        }
        catch(const std::exception& ex)
        {
            log_w("invalid serial message: %s", ex.what());
        }

        valid = crc == get_crc();
    }

    SerialCommunicatorMessage(std::string content, uint32_t sequence, uint32_t ack)
    : content(content), sequence_number(sequence), ack_number(ack)
    { 
        crc = get_crc();

        valid = true;
    }

    std::string get_content() const
    {
        return content;
    }
    uint32_t get_sequence_number() const
    {
        return sequence_number;
    }
    uint32_t get_ack_number() const
    {
        return ack_number;
    }

    std::vector<uint8_t> to_bytes() const
    {
        std::vector<uint8_t> output;
        output.reserve(1 + 12 + content.size() + 1);
        
        // start byte
        output.emplace_back(start_byte);

        // crc
        for(auto i = 3; i >= 0; i--)
        {
            output.emplace_back(static_cast<uint8_t>((crc >> (i * 8)) & 255));
        }
        // seq
        for(auto i = 3; i >= 0; i--)
        {
            output.emplace_back(static_cast<uint8_t>((sequence_number >> (i * 8)) & 255));
        }
        // ack
        for(auto i = 3; i >= 0; i--)
        {
            output.emplace_back(static_cast<uint8_t>((ack_number >> (i * 8)) & 255));
        }
        // content
        for(auto c : content)
        {
            output.emplace_back(c);
        }

        // stop byte
        output.emplace_back(stop_byte);

        return output;
    }

    bool is_valid() const
    {
        return valid;
    }
};
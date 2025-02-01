#pragma once

#include "crc32/crc32.h"
#include <vector>

// PDU format:
// -----------------------------------------------------------------------------------------------------------------
// |      1      |      4      |      4      |      4      |      3      |      1      |   0..255    |      1      |
// -----------------------------------------------------------------------------------------------------------------
// |  B00000010  |    crc32    | seq. number | ack. number |      0      |  c. length  |   content   |  B00000011  |
// -----------------------------------------------------------------------------------------------------------------
// CRC32 is calculated started for seq. number, ack. number, c. length and content

class SerialCommunicatorMessage
{
    private:
    bool valid = false;
    uint32_t crc = 0;
    uint32_t sequence_number = 0;
    uint32_t ack_number = 0;
    uint32_t content_length = 0;

    std::string content {};

    uint32_t calculate_crc() const
    {
        auto bytes = to_bytes();
        auto p_begin = bytes.data();

        return crc32(p_begin + 5, bytes.size() - 5 - 1);
    }

    public:
    const static uint8_t start_byte = B00000010;
    const static uint8_t stop_byte = B00000011;

    // Accepts byte array not including start and stop bytes
    SerialCommunicatorMessage(const std::vector<uint8_t>& bytes)
    {
        if(bytes.size() < 18) {
            std::string msg{bytes.begin(), bytes.end()};
            log_w("invalid serial message: less than 18 bytes (%lu bytes): %s", bytes.size(), msg);
            return;
        }

        std::string message_str;

        for (auto i = 0; i < 4; i++)
        {
            crc |= bytes[i + 1] << (8 * i);
        }
        for (auto i = 0; i < 4; i++)
        {
            sequence_number |= bytes[i + 5] << (8 * i);
        }
        for (auto i = 0; i < 4; i++)
        {
            ack_number |= bytes[i + 9] << (8 * i);
        }
        for (auto i = 0; i < 4; i++)
        {
            content_length |= bytes[i + 13] << (8 * i);
        }

        size_t size = bytes.size() - 18;
        content.reserve(size);

        for (auto it = bytes.begin() + 17; it != bytes.end() - 1; it++)
        {
            content += (char)*it;
        }

        valid = crc == calculate_crc();
    }

    SerialCommunicatorMessage(std::string content, uint32_t sequence, uint32_t ack)
    : content(content), sequence_number(sequence), ack_number(ack), content_length(content.size()), valid(true)
    { 
        crc = calculate_crc();
    }

    uint32_t get_crc() const { return crc; }
    uint32_t get_sequence_number() const { return sequence_number; }
    uint32_t get_ack_number() const { return ack_number; }
    uint32_t get_content_length() const { return content_length; }
    std::string get_content() const { return content; }
    bool is_valid() const { return valid; }

    std::vector<uint8_t> to_bytes() const
    {
        std::vector<uint8_t> output;
        output.reserve(1 + 16 + content.size() + 1);

        // start byte
        output.emplace_back(start_byte);

        // crc
        for (auto i = 0; i < 4; i++)
        {
            output.emplace_back(static_cast<uint8_t>((crc >> (i * 8)) & 255));
        }
        // seq
        for (auto i = 0; i < 4; i++)
        {
            output.emplace_back(static_cast<uint8_t>((sequence_number >> (i * 8)) & 255));
        }
        // ack
        for (auto i = 0; i < 4; i++)
        {
            output.emplace_back(static_cast<uint8_t>((ack_number >> (i * 8)) & 255));
        }
        // length
        for (auto i = 0; i < 4; i++)
        {
            output.emplace_back(static_cast<uint8_t>((content_length >> (i * 8)) & 255));
        }
        // content
        for (auto c : content)
        {
            output.emplace_back(c);
        }

        // stop byte
        output.emplace_back(stop_byte);

        return output;
    }

};
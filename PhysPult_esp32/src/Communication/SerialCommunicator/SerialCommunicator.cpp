#include "SerialCommunicator.h"
#include <driver/uart.h>
#include <rtc_wdt.h>

extern const uint8_t SerialCommunicatorMessage::stop_byte;
extern const uint8_t SerialCommunicatorMessage::start_byte;

int SerialCommunicator::get_device_number() { return 0; }
void SerialCommunicator::set_on_message(OnPultMessage handler) { on_message = handler; }
void SerialCommunicator::set_on_disconnect(OnDisconnect handler) { on_disconnect = handler; }
void SerialCommunicator::set_on_connect(OnConnect handler) { on_connect = handler; }
void SerialCommunicator::set_on_device_number_changed(OnDeviceNumberChanged handler) { on_device_number_changed = handler; }

void SerialCommunicator::resetInputBuffer()
{
    input_buffer.clear();
    expected_content_length = 0;
}


void SerialCommunicator::accept_pending_receive()
{

    uint8_t read{};
    while(xQueueReceive(uart_rx_queue, &read, 0))
    {
        // If content length is read
        if(input_buffer.size() == 17)
        {
            auto read_length = input_buffer[16];
            expected_content_length = read_length <= 255 ? read_length : 0;
        }

        switch (read)
        {
        case SerialCommunicatorMessage::start_byte:
        {
            input_buffer.emplace_back(read);
            break;
        }
        case SerialCommunicatorMessage::stop_byte:
        {
            if(input_buffer.size() == 0)
            {
                resetInputBuffer();
            }
            else {
                input_buffer.emplace_back(read);

                int contentBytes = (int)input_buffer.size() - 18;
                if(contentBytes > 0)
                // If expected number was corrupter or we've reached length, reset
                // if(expected_content_length == 0 || contentBytes >= expected_content_length)
                {
                    SerialCommunicatorMessage msg(input_buffer);

                    deincapsulate_pult_message(msg);
                    resetInputBuffer();
                } else
                {
                    log_w("contentBytes: %d < %lu", contentBytes, expected_content_length);
                }
            }
            break;
        }
        default:
        {
            if(input_buffer.size() > 0)
            {
                input_buffer.emplace_back(read);
            }
            break;
        }
        }
    }
}

void SerialCommunicator::deincapsulate_pult_message(const SerialCommunicatorMessage& message)
{
    if(message.is_valid())
    {
        const auto& content = message.get_content();
        on_message(PultMessageFactory::Create(content));
    }
    else {
        on_invalid_message(message);
    }
}

void SerialCommunicator::on_invalid_message(const SerialCommunicatorMessage& message)
{
    log_w("invalid pult message: seq %lu, ack %lu, crc %lu (%s))", message.get_sequence_number(), message.get_ack_number(), message.get_crc(), message.get_content().c_str());
    send(std::make_shared<StateRequestMessage>());
}

void SerialCommunicator::send_pending_message()
{
    if (!transmit_queue.empty())
    {
        const auto& msg = transmit_queue.front();

        // log_i("sending: crc: %lu, seq: %lu, ack: %lu, len: %lu (%s)", msg.get_crc(), msg.get_sequence_number(), msg.get_ack_number(), msg.get_content_length(), msg.get_content().c_str());

        const auto& bytes = msg.to_bytes();

        for(auto b : bytes) {
            Serial.write(b);
        }
        uart_write_bytes(UART_NUM_0, bytes.data(), bytes.size());

        // todo: mutex
        transmit_queue.pop();
    }
}

void SerialCommunicator::send(std::shared_ptr<PultMessage> msg)
{
    static uint32_t seq = 1;

    assert(msg);

    if(seq == 0) {
        seq++;
    }

    SerialCommunicatorMessage serial_msg(msg->to_string(), seq, 0);

    seq++;

    // todo: mutex
    transmit_queue.push(serial_msg);
}

void IRAM_ATTR SerialCommunicator::uart_read_isr()
{
    size_t buffered_len = Serial.available();
    
    uint8_t* buf = new uint8_t[buffered_len];
    if(uart_read_bytes(UART_NUM_0, buf, buffered_len, 0) > 0)
    {
        for(size_t i = 0; i < buffered_len; i++) {
            xQueueSendFromISR(uart_rx_queue, buf + i, NULL);
        }
    }

    delete buf;
}

void SerialCommunicator::start()
{
    Serial.onReceive([this]() {
        uart_read_isr();
    });

    log_i("callback for serial set-up. waiting for messages.");
    on_connect();
    
    while (true)
    {
        yieldIfNecessary();
        accept_pending_receive();
        send_pending_message();
    }
}
void SerialCommunicator::stop()
{
    Serial.onReceive(NULL);
    log_i("callback for serial cleared.");
}
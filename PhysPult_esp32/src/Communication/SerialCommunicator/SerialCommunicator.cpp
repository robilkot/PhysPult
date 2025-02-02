#include "SerialCommunicator.h"
#include <driver/uart.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

extern const uint8_t SerialCommunicatorMessage::stop_byte;
extern const uint8_t SerialCommunicatorMessage::start_byte;

QueueHandle_t SerialCommunicator::uart_rx_queue = xQueueCreate(1024, sizeof(uint8_t));

int SerialCommunicator::get_device_number() { return 0; }
void SerialCommunicator::set_on_message(OnPultMessage handler) { on_message = handler; }
void SerialCommunicator::set_on_disconnect(OnDisconnect handler) { on_disconnect = handler; }
void SerialCommunicator::set_on_connect(OnConnect handler) { on_connect = handler; }
void SerialCommunicator::set_on_device_number_changed(OnDeviceNumberChanged handler) { on_device_number_changed = handler; }

void SerialCommunicator::reset_input_buffer()
{
    input_buffer.clear();
}

void SerialCommunicator::accept_pending_receive()
{

    uint8_t read{};
    while(xQueueReceive(uart_rx_queue, &read, 0))
    {
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
                reset_input_buffer();
            }
            else {
                input_buffer.emplace_back(read);

                int contentBytes = (int)input_buffer.size() - 18;
                if(contentBytes > 0)
                {
                    SerialCommunicatorMessage msg(input_buffer);

                    deincapsulate_pult_message(msg);
                    reset_input_buffer();
                } else
                {
                    // log_w("contentBytes: %d < %lu", contentBytes, expected_content_length);
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
    log_w("invalid message: seq %lu, ack %lu, crc %lu (%s))", message.get_sequence_number(), message.get_ack_number(), message.get_crc(), message.get_content().c_str());
    send(std::make_shared<StateRequestMessage>());
}

void SerialCommunicator::send_pending_message()
{
    if (!transmit_queue.empty())
    {
        const auto& msg = transmit_queue.front();

        // log_i("sending: crc: %lu, seq: %lu, ack: %lu, len: %lu (%s)", msg.get_crc(), msg.get_sequence_number(), msg.get_ack_number(), msg.get_content_length(), msg.get_content().c_str());

        const auto& bytes = msg.to_bytes();

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
    // auto start = esp_timer_get_time();
    
    size_t buffered_len = Serial.available();
    
    uint8_t* buf = new uint8_t[buffered_len];
    Serial.readBytes(buf, buffered_len);

    for(size_t i = 0; i < buffered_len; i++) {
        xQueueSendFromISR(uart_rx_queue, buf + i, NULL);
    }

    delete buf;

    // auto end = esp_timer_get_time();
    // ESP_DRAM_LOGE("*", "isr %lld mcs", (end - start));
}

void SerialCommunicator::start()
{
    // todo: this disables IWDT, not good
    TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE; // Unlock timer config.
    TIMERG1.wdt_feed = 1; // Reset feed count.
    TIMERG1.wdt_config0.en = 0; // Disable timer.
    TIMERG1.wdt_wprotect = 0; // Lock timer config.

    Serial.onReceive(uart_read_isr);

    log_i("isr for uart set-up. waiting for messages.");
    on_connect();

    send(std::make_shared<StateRequestMessage>());
    
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
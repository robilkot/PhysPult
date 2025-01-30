#include "SerialCommunicator.h"

int SerialCommunicator::get_device_number()
{
    return 0;
}
void SerialCommunicator::set_on_message(OnPultMessage handler)
{
    on_message = handler;
}
void SerialCommunicator::set_on_disconnect(OnDisconnect handler)
{
    on_disconnect = handler;
}
void SerialCommunicator::set_on_connect(OnConnect handler)
{
    on_connect = handler;
}
void SerialCommunicator::set_on_device_number_changed(OnDeviceNumberChanged handler)
{
    on_device_number_changed = handler;
}

void SerialCommunicator::accept_pending_receive()
{
    if(!pending_receive)
    {
        return;
    }
 
    // To avoid corrupting input buffer
    std::lock_guard<std::mutex> lock(receive_buffer_lock);

    while(Serial.available())
    {
        auto read = Serial.read();

        if(read == -1) {
            log_w("invalid reading from serial port");
        }

        input_buffer.emplace_back(read);

        if(read == '\n')
        {
            auto msg = SerialCommunicatorMessage(input_buffer);
            input_buffer.clear();
            pending_receive = false;
            
            if(msg.is_valid())
            {
                on_message(*msg.get_message());
            }
            else
            {
                on_invalid_message(msg);
            }

            break; // todo: review this function
        }
    }
}

void SerialCommunicator::on_invalid_message(const SerialCommunicatorMessage& msg)
{
    log_w("invalid serial message received. sending state request");
    send(std::make_shared<StateRequestMessage>());
}

void SerialCommunicator::send_pending_message()
{
    if(transmit_queue.empty())
    {
        return;
    }

    // todo: some queue to support ack-ing
    const auto& bytes = transmit_queue.front().to_bytes();

    for(auto b : bytes) {
        Serial.write(b);
    }

    {
        std::lock_guard<std::mutex> lock(transmit_buffer_lock);
        transmit_queue.pop(); // will need to change if acking is implemented
    }
}

void SerialCommunicator::send(std::shared_ptr<PultMessage> msg)
{
    assert(msg);
    SerialCommunicatorMessage serial_msg(msg, 0, 0);

    {
        std::lock_guard<std::mutex> lock(transmit_buffer_lock);
        transmit_queue.push(serial_msg);
    }
}
void SerialCommunicator::start()
{
    Serial.onReceive([this]() {
        pending_receive = true;
    });

    log_i("callback for serial set-up ok. waiting for messages.");
    
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
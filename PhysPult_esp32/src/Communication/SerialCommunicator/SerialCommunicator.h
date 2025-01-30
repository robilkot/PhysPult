#pragma once

#include <Communication/Communicator.h>
#include <queue>
#include <memory>
#include "SerialCommunicatorMessage.h"
#include "Communication/PultMessageFactory.h"

class PultMessage;

class SerialCommunicator : public Communicator
{
    private:
    std::mutex receive_buffer_lock{};
    std::mutex transmit_buffer_lock{};
    std::queue<SerialCommunicatorMessage> transmit_queue{};
    bool pending_receive = false;

    // TODO implement max size
    // size_t input_buffer_max_size = 1024;
    std::vector<uint8_t> input_buffer{};
    bool message_started = false;

    OnPultMessage on_message;
    OnDisconnect on_disconnect;
    OnConnect on_connect;
    OnDeviceNumberChanged on_device_number_changed;

    void on_invalid_message(const SerialCommunicatorMessage& msg);
    void send_pending_message();
    void accept_pending_receive();
    void deincapsulate_pult_message(const SerialCommunicatorMessage& message);

    public:
    int get_device_number() override;
    void set_on_message(OnPultMessage handler) override;
    void set_on_connect(OnConnect handler) override;
    void set_on_disconnect(OnDisconnect handler) override;
    void set_on_device_number_changed(OnDeviceNumberChanged handler) override;
    void send(std::shared_ptr<PultMessage> msg) override;
    void start() override;
    void stop() override;
};
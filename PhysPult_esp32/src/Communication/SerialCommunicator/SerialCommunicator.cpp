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
void SerialCommunicator::send(PultMessage& msg)
{
    // todo
}
void SerialCommunicator::start()
{
    Serial.onReceive([this]() {
        move_next();
    });
}
void SerialCommunicator::stop()
{
    Serial.onReceive(NULL);
}
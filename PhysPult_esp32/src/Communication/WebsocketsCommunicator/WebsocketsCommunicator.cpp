#include "WebsocketsCommunicator.h"

int WebsocketsCommunicator::get_device_number()
{
    return device_number;
}
void WebsocketsCommunicator::accept_client()
{
    log_i("Waiting for client.");
    
    while(server.available()) {
        vTaskDelay(pdMS_TO_TICKS(5));
        
        if(server.poll()) {
            client = server.accept();
            break;
        }
    }
    // todo: what if server not available
    
    on_connect();

    client.onMessage([&] (websockets::WebsocketsMessage msg) {
        on_message(*PultMessageFactory::Create(msg.rawData()));
    });
    client.onEvent([&] (websockets::WebsocketsEvent event, websockets::WSInterfaceString payload) {
        switch(event) {
            case websockets::WebsocketsEvent::ConnectionOpened: {
                log_i("Client connected.");
                break;
            }
            case websockets::WebsocketsEvent::GotPing: {
                client.pong();
                log_i("Got ping");
                break;
            }
            case websockets::WebsocketsEvent::GotPong: {
                log_i("Got pong");
                break;
            }
            case websockets::WebsocketsEvent::ConnectionClosed: {
                log_i("Client disconnected: %d", client.getCloseReason());

                on_disconnect();
                accept_client();
                break;
            }
        }
        });
    log_i("Client connected.");
}

void WebsocketsCommunicator::set_on_message(OnPultMessage handler)
{
    on_message = handler;
}
void WebsocketsCommunicator::set_on_disconnect(OnDisconnect handler)
{
    on_disconnect = handler;
}
void WebsocketsCommunicator::set_on_connect(OnConnect handler)
{
    on_connect = handler;
}
void WebsocketsCommunicator::set_on_device_number_changed(OnDeviceNumberChanged handler)
{
    on_ip_changed = handler;
}

void WebsocketsCommunicator::connect_to_network()
{
    WiFi.disconnect();
    WiFi.begin(WifiSsid, WifiPassword);

    uint8_t connectionDisplayTimer = 1;
    const uint8_t connectionTimerLimit = 50;

    for(; connectionDisplayTimer <= connectionTimerLimit; connectionDisplayTimer++) 
    {
        if(WiFi.status() == WL_CONNECTED)
        {
            break;
        }

        // todo: don't use delay
        delay(100);
    }

    if(connectionDisplayTimer == connectionTimerLimit)
    {
        log_e("Couldn't connect to network.");
    }

    auto ip = WiFi.localIP().toString(); 

    // todo: This doesn't work with 3 digits though
    device_number = atoi(ip.substring(ip.length() - 2).c_str());
    on_ip_changed(device_number);

    server.listen(NetworkPort);
    log_i("Server is %savailable at %s", server.available() ? "" : "not ", ip);
}

void WebsocketsCommunicator::send(std::shared_ptr<PultMessage> msg)
{
    assert(msg);
    client.send(msg->to_string().c_str());
}

void WebsocketsCommunicator::start()
{
    connect_to_network();
    
    accept_client();

    while(true)
    {
        yieldIfNecessary();
        client.poll();  
    }
}

void WebsocketsCommunicator::stop()
{
    client.close();
}
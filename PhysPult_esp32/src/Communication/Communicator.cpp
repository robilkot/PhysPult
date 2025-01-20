#include "Communicator.h"


Communicator::Communicator() { }

void Communicator::accept_client()
{
    log_i("Waiting for client.");
    // todo: use poll to not block
    client = server.accept();

    client.onMessage([&](websockets::WebsocketsMessage msg) {
        on_message(*PultMessageFactory::Create(msg.data()));
    });
    client.onEvent([&](websockets::WebsocketsEvent event, websockets::WSInterfaceString payload) {
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

void Communicator::set_on_message(std::function<void(PultMessage&)> handler)
{
    on_message = handler;
}
void Communicator::set_on_disconnect(std::function<void()> handler)
{
    on_disconnect = handler;
}
void Communicator::set_on_ip_changed(std::function<void(int)> handler)
{
    on_ip_changed = handler;
}

void Communicator::connect_to_network()
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

void Communicator::send(PultMessage& msg)
{
    client.send(msg.to_string());
}

void Communicator::start()
{
    connect_to_network();
    
    accept_client();

    while(true)
    {
        client.poll();

        assert(client.available() && "Client should be available when polling");           
    }
}
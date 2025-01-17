#pragma once

#include <functional>
#include <ArduinoWebsockets.h>
#include <WifiConstants.h>
#include "PultMessage.h"
#include <Constants.h>


class Communicator
{
    private:
    std::function<void(PultMessage&)> on_message;
    std::function<void()> on_disconnect;
    websockets::WebsocketsClient client;

    public:
    websockets::WebsocketsServer server;
    uint8_t device_number;

    Communicator() { }

    void set_on_message(std::function<void(PultMessage&)> handler)
    {
        on_message = handler;
    }
    void set_on_disconnect(std::function<void()> handler)
    {
        on_disconnect = handler;
    }

    void connect_to_network()
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

            Serial.print(".");
            // todo: don't use delay
            delay(100);
        }

        if(connectionDisplayTimer == connectionTimerLimit)
        {
            Serial.print("Couldn't connect to network.");
        }

        auto ip = WiFi.localIP().toString(); 
        Serial.println("\nConnected to network. IPv4:");
        Serial.println(ip);

        // todo: This doesn't work with 3 digits though
        device_number = atoi(ip.substring(ip.length() - 2).c_str());

        server.listen(NetworkPort);
        Serial.print("Server is ");
        if(server.available() == false)
        {
            Serial.print("not ");
        }
        Serial.println("available.");

        Serial.println("Network initialization complete");
    }

    void wait_for_client()
    {
        Serial.println("Waiting for client.");
        client = server.accept();
        Serial.println("Client accepted.");
    }

    void send(const websockets::WSInterfaceString msg)
    {
        client.send(msg);
    }
    
    void start()
    {
        connect_to_network();
        
        wait_for_client();

        while(true)
        {
            if(client.available())
            {
                websockets::WebsocketsMessage ws_message = client.readBlocking();

                PultMessage message(ws_message.data());

                on_message(message);
            }
            else
            {
                on_disconnect();
                
                wait_for_client();
            }
        }
    }
};
#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(uint16_t port, const char *ws_path)
    : server(port), ws(ws_path)
{
    ws.onEvent([this](AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len)
               { this->onWebSocketEvent(server, client, type, arg, data, len); });
}

void WebSocketServer::begin(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.addHandler(&ws);

    server.begin();
    Serial.println("WebSocket server started");
}

void WebSocketServer::onWebSocketEvent(AsyncWebSocket *server,
                                       AsyncWebSocketClient *client,
                                       AwsEventType type,
                                       void *arg,
                                       uint8_t *data,
                                       size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.println("New client connected");
        client->text("Welcome to WebSocket Server!");
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.println("Client disconnected");
    }
    else if (type == WS_EVT_DATA)
    {
        char msg[len + 1];
        memcpy(msg, data, len);
        msg[len] = '\0';

        Serial.println("Message received from client:");
        Serial.println(msg);

        if (messageCallback)
        {
            messageCallback(msg);
        }

        client->text("Echo: " + String(msg));
    }
}

void WebSocketServer::sendMsg(const char *msg)
{
    ws.textAll(msg);
    Serial.println("Message sent to all clients: " + String(msg));
}

void WebSocketServer::onMessage(void (*callback)(const char *msg))
{
    messageCallback = callback;
}

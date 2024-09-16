#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WebSocketServer
{
public:
    WebSocketServer(uint16_t port, const char *ws_path);

    void begin(const char *ssid, const char *password);

    void sendMsg(const char *msg);

    void SendBIN_TO_BASE64(uint8_t *data, size_t len, bool debug);

    // Send binary data
    void sendBIN(uint8_t *data, size_t len);

    void onMessage(void (*callback)(const char *msg));

private:
    void onWebSocketEvent(AsyncWebSocket *server,
                          AsyncWebSocketClient *client,
                          AwsEventType type,
                          void *arg,
                          uint8_t *data,
                          size_t len);

    AsyncWebServer server;
    AsyncWebSocket ws;

    void (*messageCallback)(const char *msg) = nullptr;
};

#endif

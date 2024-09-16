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

        if (messageCallback)
        {
            messageCallback(msg);
        }
    }
}

void WebSocketServer::sendMsg(const char *msg)
{
    ws.textAll(msg);
    Serial.println("Message sent to all clients: " + String(msg));
}

void base64_encode(uint8_t *data, size_t len, char *out)
{
    static const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i, j;
    for (i = 0, j = 0; i < len; i += 3)
    {
        out[j++] = base64_table[(data[i] & 0xFC) >> 2];
        out[j++] = base64_table[((data[i] & 0x03) << 4) | ((data[i + 1] & 0xF0) >> 4)];
        out[j++] = base64_table[((data[i + 1] & 0x0F) << 2) | ((data[i + 2] & 0xC0) >> 6)];
        out[j++] = base64_table[data[i + 2] & 0x3F];
    }
    for (i -= 3; i < len; i++)
    {
        out[j++] = base64_table[(data[i] & 0xFC) >> 2];
    }
    while (j % 4)
    {
        out[j++] = '=';
    }
    out[j] = '\0';
}

void base64_decode(const char *data, uint8_t *out, size_t len)
{
    static const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i, j;
    for (i = 0, j = 0; i < len; i += 4)
    {
        out[j++] = (strchr(base64_table, data[i]) - base64_table) << 2 | (strchr(base64_table, data[i + 1]) - base64_table) >> 4;
        out[j++] = (strchr(base64_table, data[i + 1]) - base64_table) << 4 | (strchr(base64_table, data[i + 2]) - base64_table) >> 2;
        out[j++] = (strchr(base64_table, data[i + 2]) - base64_table) << 6 | strchr(base64_table, data[i + 3]) - base64_table;
    }
    if (data[i - 2] == '=')
    {
        j -= 2;
    }
    else if (data[i - 1] == '=')
    {
        j--;
    }
    out[j] = '\0';
}

void WebSocketServer::SendBIN_TO_BASE64(uint8_t *data, size_t len, bool debug = false)
{
    char base64[len * 2 + 1];
    base64_encode(data, len, base64);
    if (debug)
    {
        Serial.println("Base64 data: " + String(base64));
    }
    ws.textAll(base64);
    Serial.println("Base64 data sent to all clients");
}

void WebSocketServer::sendBIN(uint8_t *data, size_t len)
{
    ws.binaryAll(data, len);
    Serial.println("Binary data sent to all clients");
}

void WebSocketServer::onMessage(void (*callback)(const char *msg))
{
    messageCallback = callback;
}

#include <Arduino.h>
#include "WebSocketServer.h"

// Create WebSocketServer instance on port 80 and WebSocket path "/ws"
WebSocketServer wsServer(80, "/ws");

// Callback function to handle incoming messages from clients
void handleMessage(const char *msg)
{
  Serial.println("Message received in callback:");
  Serial.println(msg);
}

void setup()
{
  // Start Serial for debugging
  Serial.begin(115200);

  // Begin WebSocket server with Wi-Fi credentials
  wsServer.begin("Victus_Ext", "1732020@4G");

  // Set the callback function for receiving messages
  wsServer.onMessage(handleMessage);
}

void loop()
{
  // Send message every 5 seconds
  static unsigned long lastTime = 0;
  unsigned long now = millis();

  if (now - lastTime > 5000)
  {
    lastTime = now;
    wsServer.sendMsg("Hello from ESP32!");
  }
}

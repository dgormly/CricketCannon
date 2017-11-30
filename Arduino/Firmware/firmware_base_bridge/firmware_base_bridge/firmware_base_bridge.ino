#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <SPI.h> //Not sure if needed for serial printing.
const char* ssid     = "UQ Cannon"; //AP
const char* password = "uqcomposites$aeb";

IPAddress apIP(192, 168, 4, 1); //Node static IP
IPAddress gateway(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
const byte DNS_PORT = 53;
DNSServer dnsServer;
unsigned char reconnectCount = 0;

#define MAX_SRV_CLIENTS 5
WiFiServer server(21);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  server.begin();
  server.setNoDelay(true);
  Serial.print("I have booted (server)");
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("USP Server started");
}

void loop() {
    dnsServer.processNextRequest();
  //server.handleClient();

  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial1.print("New client: "); Serial1.print(i);
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if (serverClients[i].available()) {
        //get data from the telnet client and push it to the UART
        while (serverClients[i].available()) Serial.write(serverClients[i].read());
      }
    }
  }
  //check UART for data
  if (Serial.available()) {
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for (int i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        serverClients[i].write(sbuf, len);
        //delay(1);
      }
    }
  }
}



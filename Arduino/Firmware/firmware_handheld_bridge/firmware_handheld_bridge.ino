#include <ESP8266WiFi.h>
WiFiClient client;
#include <SPI.h> //Not sure if needed for serial printing.
const char* ssid     = "UQ Cannon"; //AP
const char* password = "uqcomposites$aeb"; //Left blank, open hotspot.

unsigned char reconnectCount = 0;
const char* host = "192.168.4.1";

#define MAX_SRV_CLIENTS 5
//WiFiServer server(21);
//WiFiClient serverClients[MAX_SRV_CLIENTS];

IPAddress ip(192, 168, 4, 2); //Node static IP
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  delay(10);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  delay(10);
  //server.begin();
  //server.setNoDelay(true);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (reconnectCount >= 120) {
      ESP.restart();
    }
    Serial.print(".");
    reconnectCount++;
  }
  reconnectCount = 0;

  while (client.connect(host, 21) != 1) {
    delay(500);
    if (reconnectCount >= 120) {
      ESP.restart();
    }
    Serial.print(",");
    reconnectCount++;
  }
  reconnectCount = 0;

}

void loop() {
  if (client.connect(host, 21)) {
    long rssi = WiFi.RSSI();
    Serial.print("RS");
    Serial.println(rssi);
    delay(250);
  } else {
    Serial.print("RS");
    Serial.println("0");
  }

  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  while (Serial.available()) {
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    client.write(sbuf, len);
    //delay(1);
  }

}








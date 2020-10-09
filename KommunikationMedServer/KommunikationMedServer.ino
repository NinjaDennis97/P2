#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Thread.h>
#include <ThreadController.h>

ThreadController controll = ThreadController();
Thread receiverThread = Thread();
char ssid[] = "NETGEAR61";               // SSID of your home WiFi
char pass[] = "12345678";               // password of your home WiFi
const char* host = "192.168.0.10";
const uint16_t port = 3500;
WiFiClient client;

IPAddress myIP(192, 168, 0, 11);            // IP address of the server

void setup() {
  Serial.begin(9600);                   // only for debug
  Serial.println("Setup");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.connect(host,port);
  if(client.connected()){
    Serial.print("Connection completed");
    Serial.print("Host: "); Serial.println(host);
  }else{
    Serial.println("Connection failed");
  }
}

void loop() {
    if(client.connected()){
      Serial.println("Writing to server");
      client.println("hej");
      client.flush();
  
   while(client.available()){
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }

    }
  if(!client.connected()){
    Serial.println();
    Serial.println("Closing connection");
    client.flush();
    client.stop();
    Serial.println("Connection Closed");
    client.connect(host,port);
    Serial.println("Reconnecting");
  }
  delay(4000);

}


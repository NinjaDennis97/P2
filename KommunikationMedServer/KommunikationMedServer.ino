#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Dessa två rader måste vara med.
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

ThreadController controll = ThreadController();
Thread receiverThread = Thread();

char ssid[] = "NETGEAR61";               // SSID of your home WiFi
char pass[] = "12345678";               // password of your home WiFi
const char* host = "192.168.0.28";
const uint16_t port = 3500;

WiFiClient client;
String up_neighbour = "4";
String down_neighbour = "8";
String left_neighbour = "10";
String right_neighbour = "12";


IPAddress myIP(192, 168, 0, 29);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 224);

void setup() {
  Serial.begin(9600);  // only for debug
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(500);
  Serial.println("Setup");
  WiFi.mode(WIFI_STA);
  WiFi.config(myIP, gateway, subnet);
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.connect(host, port);
  if (client.connected()) {
    Serial.print("Connection completed");
    Serial.print("Host: "); Serial.println(host);
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  if (client.connected()) {
    
    while (client.available()) {
      //char ch = static_cast<char>(client.read());
      String ch = client.readStringUntil('\r');
      
      if (isDigit(ch.charAt(0))) {
        myIP[3] = ch.toInt();
        WiFi.config(myIP, gateway, subnet);
        Serial.println(myIP);
      }
      if(ch == "korrekt"){
        Serial.println("vi är i korrekt delen");
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("Grattis ni har klarat spelet!");
        display.display();
        delay(2000);
      }else if (ch == "fel"){
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Ihopsatta ordet är fel. Testa igen!!");
        display.display();
        delay(2000);
      } else if (ch.length() > 1){
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(ch);
        display.display();
        client.println(ch);
        delay(2000);
      } else {
        Serial.println(ch);
        client.println(ch);
        WriteonOLED(ch.charAt(0));
      }

//      if (ch != "fel") {
//        Serial.println(ch);
//        client.println(ch);
//        WriteonOLED(ch.charAt(0));
//      } else if(ch = "fel") {
//        display.clearDisplay();
//        display.setTextColor(WHITE);
//        display.setCursor(0, 0);
//        display.println("Ihopsatta ordet är fel. Testa igen!!");
//        display.display();
//        delay(2000);
//      } else if(ch = "korrekt"){
//        display.clearDisplay();
//        display.setTextColor(WHITE);
//        display.setCursor(0,0);
//        display.println("Grattis ni har klarat spelet!");
//        display.display();
//        delay(2000);
//      }
    }
  }
  if (!client.connected()) {
    Serial.println();
    Serial.println("Closing connection");
    client.flush();
    client.stop();
    Serial.println("Connection Closed");
    client.connect(host, port);
    Serial.println("Reconnecting");
  }
  delay(1000);
}

void WriteonOLED (char ch) {
  display.clearDisplay();
  display.display();
  delay(1000);
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(20, 7);
  display.println(ch);
  delay(100);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("U:" + up_neighbour);
  display.setCursor(0, 41);
  display.println("D:" + down_neighbour);
  display.setCursor(40, 0);
  delay(100);
  display.println("L:" + left_neighbour);
  display.setCursor(40, 41);
  display.println("R:" + right_neighbour);
  display.setCursor(0, 20);
  display.println(myIP[3]);
  display.display();
  delay(1000);
}

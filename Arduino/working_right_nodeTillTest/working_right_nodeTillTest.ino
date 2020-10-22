#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Ö =148 Ä = 132 Å= 133


//Dessa två rader måste vara med.
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

ThreadController controll = ThreadController();
Thread receiverThread = Thread();


char ssid[] = "NETGEAR61";                // SSID of your home WiFi
char pass[] = "12345678";                 // password of your home WiFi
const char* host = "192.168.0.28";
const uint16_t port = 3500;
WiFiServer server(5000);
WiFiClient client;
WiFiClient javaClient;

int myNbr = 29;
int antalNoder;
String own_letter = "";
int pulsesToSendLeft = myNbr;
int pulsesToSendRight = myNbr;
int pulsesToSendUp = myNbr;
int pulsesToSendDown = myNbr;
bool sendLeft = false;
bool sendRight = false;
bool sendUp = false;
bool sendDown = false;
bool countPulsesLeft = false;
bool countPulsesRight = false;
bool countPulsesUp = false;
bool countPulsesDown = false;
bool timeToSendUpdateLeft, timeToSendUpdateRight, timeToSendUpdateUp, timeToSendUpdateDown;
int left_nbr = 0;
int right_nbr = 0;
int up_nbr = 0;
int down_nbr = 0;
int rightHighTime, leftHighTime, upHighTime, downHighTime, readTime;
int rightLowTime, leftLowTime, upLowTime, downLowTime = 0;
String left_str = "", right_str = "", up_str = "", down_str = "";
void ICACHE_RAM_ATTR readPinRIGHT();
void ICACHE_RAM_ATTR readPinLEFT();
void ICACHE_RAM_ATTR readPinLowRIGHT();
void ICACHE_RAM_ATTR readPinLowLEFT();

IPAddress myIP(192, 168, 0, myNbr);             // IP address of the server
IPAddress gateway(192, 168, 0, 1);          // gateway of your network
IPAddress subnet(255, 255, 255, 224);       // subnet mask of your network
IPAddress leftsIP (192, 168, 0, 0);
IPAddress rightsIP (192, 168, 0, 0);
IPAddress upsIP (192, 168, 0, 0);
IPAddress downsIP (192, 168, 0, 0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
//  display.setTextSize(5);
//  display.setTextColor(WHITE);
//  display.setCursor(20,7);
//  display.write(148);
  display.display();
  delay(500);

#define OUTPUT_LEFT D3
#define INPUT_LEFT D6
#define INPUT_RIGHT D5
#define OUTPUT_RIGHT D4
#define OUTPUT_DOWN D8
#define INPUT_DOWN D8
#define INPUT_UP D7
  attachInterrupt(digitalPinToInterrupt(INPUT_RIGHT), readPinRIGHT, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INPUT_LEFT), readPinLEFT, CHANGE);


  pinMode (OUTPUT_LEFT, OUTPUT);
  pinMode (OUTPUT_RIGHT, OUTPUT);
  pinMode (INPUT_LEFT, INPUT);
  pinMode (INPUT_RIGHT, INPUT);


  //  pinMode (OUTPUT_UP,OUTPUT);
  //  pinMode (INPUT_UP,INPUT);
  //  pinMode (INPUT_DOWN,INPUT);
  //  pinMode (OUTPUT_DOWN,OUTPUT);


  digitalWrite(OUTPUT_LEFT, HIGH);
  digitalWrite(OUTPUT_RIGHT, HIGH);
  //  digitalWrite(OUTPUT_UP, HIGH);
  //  digitalWrite(OUTPUT_DOWN, HIGH);

  Serial.println("Setup");
  WiFi.config(myIP, gateway, subnet);                       // forces to use the fix IP
  WiFi.begin(ssid, pass);                                   // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                                           // starts the server
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());

  // Java Server connection
  javaClient.connect(host, port);
  if (javaClient.connected()) {
    Serial.println("Connection completed");
    Serial.print("Host: "); Serial.println(host);
  } else {
    Serial.println("Connection failed");
  }

  String ch = javaClient.readStringUntil('\r');
  if (isDigit(ch.charAt(0))) {
    myNbr = ch.toInt();
    myIP[3] = myNbr;
    Serial.println(myIP);
    WiFi.config(myIP, gateway, subnet);
    Serial.println(myNbr);
    pulsesToSendLeft = myNbr;
    pulsesToSendRight = myNbr;
    pulsesToSendUp = myNbr;
    pulsesToSendDown = myNbr;
  }



  receiverThread.onRun(receiverThreadRun);
  receiverThread.setInterval(100);
  controll.add(&receiverThread);

}

void loop() {
  readLetter();
  controll.run();
  connection();
  resetNeighbours();


  if (digitalRead(INPUT_LEFT) == HIGH && millis() - leftHighTime > 400) {
    sendLeft = true;
  }

  if (sendLeft == true) {
    if (pulsesToSendLeft > 0) {
      digitalWrite(OUTPUT_LEFT, LOW);
      delay(5);
      digitalWrite(OUTPUT_LEFT, HIGH);
      Serial.print("send l");
      Serial.println(pulsesToSendLeft);
      pulsesToSendLeft--;
    }
  }

  if (digitalRead(INPUT_RIGHT) == HIGH && millis() - rightHighTime > 400) {
    sendRight = true;
  }
  if (sendRight) {
    if (pulsesToSendRight > 0) {
      digitalWrite(OUTPUT_RIGHT, LOW);
      delay(5);
      digitalWrite(OUTPUT_RIGHT, HIGH);

      Serial.println(pulsesToSendRight);
      pulsesToSendRight--;
    }
  }

  if (timeToSendUpdateLeft) {
    if (leftsIP[3] != 0) {
      if (client.connect(leftsIP, 5000)) {
        timeToSendUpdateLeft = false;
        String temp_str = "R" + own_letter + right_str + "\r";
        client.println(temp_str);
        client.flush();
        Serial.print("sendning: ");
        Serial.println(temp_str);
      }
    } else {
      timeToSendUpdateLeft = false;
    }
  }
  if (timeToSendUpdateRight) {
    if (rightsIP[3] != 0) {
      if (client.connect(rightsIP, 5000)) {
        timeToSendUpdateRight = false;
        String temp_str = "L" + left_str + own_letter + "\r";
        client.println(temp_str);
        client.flush();
        Serial.print("sendning: ");
        Serial.println(temp_str);
      }
    } else {
      timeToSendUpdateRight = false;
    }
  }
  if (timeToSendUpdateUp) {
    if (upsIP[3] != 0) {
      if (client.connect(upsIP, 5000)) {
        timeToSendUpdateUp = false;
        String temp_str = "D" + own_letter + down_str + "\r";
        client.println(temp_str);
        client.flush();
        Serial.print("sendning: ");
        Serial.println(temp_str);
      }
    } else {
      timeToSendUpdateUp = false;
    }
  }
  if (timeToSendUpdateDown) {
    if (downsIP[3] != 0) {
      if (client.connect(downsIP, 5000)) {
        timeToSendUpdateDown = false;
        String temp_str = "U" + up_str + own_letter + "\r";
        client.println(temp_str);
        client.flush();
        Serial.print("sendning: ");
        Serial.println(temp_str);
      }
    } else {
      timeToSendUpdateDown = false;
    }
  }
}

void readLetter() {
  if (javaClient.available()) {
    String ch = javaClient.readStringUntil('\r');
    if (isDigit(ch.charAt(0))) {
      antalNoder = ch.toInt();
      Serial.println(antalNoder);
    } else if (ch == "korrekt") {
      Serial.println("vi är i korrekt delen");
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(10, 20);
      display.println("Korrekt");
      display.display();
      delay(2000);
    } else if (ch == "fel") {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(18, 10);
      display.println("Fel");
      display.display();
      delay(2000);
      uppdateNeighborsOLED();
    }
    else {
      Serial.println(ch);
      javaClient.println(ch);
      own_letter = ch;
 //     own_letter = (ch.charAt(0));
      uppdateNeighborsOLED();
    }
  }
  if (!javaClient.connected()) {
    Serial.println("Closing connection");
    javaClient.flush();
    javaClient.stop();
    Serial.println("Connection Closed");
    javaClient.connect(host, port);
    Serial.println("Reconnecting");
  }
  delay(1000);
}

void uppdateNeighborsOLED() {
  display.clearDisplay();
  display.display();
  delay(1000);
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(20, 7);
  if(own_letter.equals("å")){
    display.write(133);
  }else if(own_letter.equals("ä")){
    display.write(132);
  }else if(own_letter.equals("ö")){
    display.write(148);
  }else{
  display.println(own_letter);
  }
  display.display();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("U:" + String(upsIP[3]));
  display.setCursor(0, 41);
  display.println("D:" + String(downsIP[3]));
  display.setCursor(40, 0);
  delay(100);
  display.println("L:" + String(leftsIP[3]));
  display.setCursor(40, 41);
  display.println("R:" + String(rightsIP[3]));
  display.setCursor(0, 20);
  display.println(myIP[3]);
  display.display();
}

void readPinLEFT() {
  int readedValue = digitalRead(INPUT_LEFT);
  if (readedValue == HIGH) {
    leftHighTime = millis();
  }
  if (readedValue == LOW) {
    leftLowTime = millis();
    if (millis() - leftHighTime > 100) {
      countPulsesLeft = true;
    }
    if (countPulsesLeft) {
      left_nbr++;
      Serial.print("L: ");
      Serial.println(left_nbr);
    }
  }
}

void readPinRIGHT() {
  int readedValue = digitalRead(INPUT_RIGHT);
  if (readedValue == HIGH) {
    rightHighTime = millis();
  }
  if (readedValue == LOW) {
    rightLowTime = millis();
    if (millis() - rightHighTime > 100) {
      countPulsesRight = true;
    }
    if (countPulsesRight) {
      right_nbr++;
      Serial.print("R: ");
      Serial.println(right_nbr);
    }
  }
}



void receiverThreadRun() {
  WiFiClient newClient = server.available();
  if (newClient) {
    Serial.println("newClient");
    Serial.println("reading");
    String msg = newClient.readStringUntil('\r');
    Serial.println(newClient.remoteIP());
    Serial.println(msg);
    if (msg.equals("left")) {
      if (newClient.remoteIP()[3] == left_nbr) {
        newClient.write("ACK\r");
        newClient.flush();
        Serial.println("minACK");
      } else {
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
      }
    }
    else if (msg.equals("right")) {
      if (newClient.remoteIP()[3] == right_nbr) {
        newClient.write("ACK\r");
        newClient.flush();
        Serial.println("minACK");
      } else {
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
      }
    }
    else if (msg.equals("up")) {
      if (newClient.remoteIP()[3] == up_nbr) {
        newClient.write("ACK\r");
        newClient.flush();
        Serial.println("minACK");
      } else {
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
      }
    }
    else if (msg.equals("down")) {
      if (newClient.remoteIP()[3] == down_nbr) {
        newClient.write("ACK\r");
        newClient.flush();
        Serial.println("minACK");
      } else {
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
      }
    } else {
      char first_char = msg.charAt(0);
      if (first_char == 'L') {
        left_str = msg.substring(1);
        Serial.print("L_str:");
        Serial.println(left_str);
        check();
        timeToSendUpdateRight = true;
      }
      if (first_char == 'R') {
        right_str = msg.substring(1);
        Serial.print("R_str:");
        Serial.println(right_str);
        check();
        timeToSendUpdateLeft = true;
      }
      if (first_char == 'U') {
        up_str = msg.substring(1);
        Serial.print("U_str:");
        Serial.println(up_str);
        check();
        timeToSendUpdateDown = true;
      }
      if (first_char == 'D') {
        down_str = msg.substring(1);
        Serial.print("D_str:");
        Serial.println(down_str);
        check();
      }
      timeToSendUpdateUp = true;
    }
    newClient.stop();
  }
}

void check () {
  if (left_str.length() + right_str.length() == antalNoder - 1) {
    if(leftsIP[3] == 0 && right_str.length() == antalNoder - 1){
      Serial.println(own_letter + right_str);
      javaClient.println(own_letter + right_str);
      javaClient.flush();
    }
  } else if (up_str.length() + down_str.length() == antalNoder - 1) {
    if(upsIP[3] == 0 && down_str.length() == antalNoder - 1){
      Serial.println(own_letter + down_str);
      javaClient.println(own_letter + down_str);
      javaClient.flush();
    }
  } else if (left_str.length() + right_str.length() + up_str.length() + down_str.length() == antalNoder - 1) {
    javaClient.println(up_str + own_letter + down_str + left_str + own_letter + right_str);
    javaClient.flush();
  }
}


void connection() {
  if (pulsesToSendLeft == 0 && leftsIP[3] == 0) {
    IPAddress tempIP(192, 168, 0, left_nbr);
    Serial.print("trying to connect to: ");
    Serial.println(left_nbr);
    if (client.connect(tempIP, 5000)) {
      Serial.println(client.write("right"));
      client.flush();
      String response1 = client.readStringUntil('\r');
      String response2 = client.readStringUntil('\r');
      if (response1.equals("ACK") || response2.equals("ACK")) {
        Serial.println("WOOOHOOO!!!");
        leftsIP = tempIP;
        uppdateNeighborsOLED();
        timeToSendUpdateLeft = true;
        countPulsesLeft = false;
      }
      Serial.println(response1 + "1");
      Serial.println(response2 + "2");
      Serial.println(leftsIP);
    }
  }
  /*-----------------------------------------------------------*/
  if (pulsesToSendRight == 0 && rightsIP[3] == 0) {
    IPAddress tempIP(192, 168, 0, right_nbr);
    Serial.print("trying to connect to: ");
    Serial.println(right_nbr);
    if (client.connect(tempIP, 5000)) {
      Serial.println(client.write("left\r"));
      client.flush();
      String response1 = client.readStringUntil('\r');
      String response2 = client.readStringUntil('\r');
      if (response1.equals("ACK") || response2.equals("ACK")) {
        Serial.println("WOOOHOOO!!!");
        rightsIP = tempIP;
        uppdateNeighborsOLED();
        timeToSendUpdateRight = true;
        countPulsesRight = false;
      }
      Serial.println(response1 + "1");
      Serial.println(response2 + "2");

      Serial.println(rightsIP);
    }
  }
  /*--------------------------------------------------------------*/
  if (pulsesToSendUp == 0 && upsIP[3] == 0) {
    IPAddress tempIP(192, 168, 0, up_nbr);
    Serial.print("trying to connect to: ");
    Serial.println(up_nbr);
    if (client.connect(tempIP, 5000)) {
      Serial.println(client.write("down"));
      client.flush();
      String response1 = client.readStringUntil('\r');
      String response2 = client.readStringUntil('\r');
      if (response1.equals("ACK") || response2.equals("ACK")) {
        Serial.println("WOOOHOOO!!!");
        upsIP = tempIP;
        uppdateNeighborsOLED();
        timeToSendUpdateUp = true;
        countPulsesUp = false;
      }
      Serial.println(response1 + "1");
      Serial.println(response2 + "2");
      Serial.println(upsIP);
    }
  }
  /*--------------------------------------------------------------*/
  if (pulsesToSendDown == 0 && downsIP[3] == 0) {
    IPAddress tempIP(192, 168, 0, down_nbr);
    Serial.print("trying to connect to: ");
    Serial.println(down_nbr);
    if (client.connect(tempIP, 5000)) {
      Serial.println(client.write("up"));
      client.flush();
      String response1 = client.readStringUntil('\r');
      String response2 = client.readStringUntil('\r');
      if (response1.equals("ACK") || response2.equals("ACK")) {
        Serial.println("WOOOHOOO!!!");
        downsIP = tempIP;
        uppdateNeighborsOLED();
        timeToSendUpdateDown = true;
        countPulsesDown = false;
      }
      Serial.println(response1 + "1");
      Serial.println(response2 + "2");
      Serial.println(downsIP);
    }
  }
}

void resetNeighbours() {
  if (digitalRead(INPUT_LEFT) == LOW) {
    if (leftLowTime != 0 && (millis() - leftLowTime) > 2000) {
      Serial.println("resetN2");
      sendLeft = false;
      pulsesToSendLeft = myNbr;
      countPulsesLeft = false;
      left_nbr = 0;
      leftsIP[3] = 0;
      leftLowTime = 0;
      left_str = "";
      timeToSendUpdateRight = true;
      uppdateNeighborsOLED();
    }
  }
  /*--------------------------------------------------------------*/

  if (digitalRead(INPUT_RIGHT) == LOW) {
    if (rightLowTime != 0 && (millis() - rightLowTime) > 2000) {
      Serial.println("resetN4");
      sendRight = false;
      pulsesToSendRight = myNbr;
      countPulsesRight = false;
      right_nbr = 0;
      rightsIP[3] = 0;
      rightLowTime = 0;
      right_str = "";
      timeToSendUpdateLeft = true;
      uppdateNeighborsOLED();
    }
  }
  /*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*/
}

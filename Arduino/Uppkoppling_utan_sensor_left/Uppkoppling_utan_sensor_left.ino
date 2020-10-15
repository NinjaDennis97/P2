#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Thread.h>
#include <ThreadController.h>

ThreadController controll = ThreadController();
Thread receiverThread = Thread();
int i = 0;
int y = 0;
char ssid[] = "NETGEAR61";                // SSID of your home WiFi
char pass[] = "12345678";                 // password of your home WiFi
WiFiServer server(5000);
WiFiClient client;

 int myID = 2;
 int lefts_ID = 0;
 int rights_ID = 0;
 int ups_ID;
 int downs_ID;
 bool sendPulsesLEFT = false;
 bool sendPulsesRIGHT = false;
 

IPAddress myIP(192, 168, 0, myID);             // IP address of the server
IPAddress gateway(192,168,0,1);             // gateway of your network
IPAddress subnet(255,255,255,224);          // subnet mask of your network
IPAddress rightsIP (192, 168, 0, 0);

 
void ICACHE_RAM_ATTR readPin_RIGHT();
void ICACHE_RAM_ATTR readPin_LEFT();

void setup() {
  Serial.begin(115200);                       // only for debug
  // D0 Kan inte användas för att läsa in pulser!!!!!!
  #define OUTPUT_TO_LEFT D5                   // DEFINE pins on the board
  #define INPUT_FROM_RIGHT D1
 // #define SENSOR_LEFT D7
 // #define SENSOR_RIGHT D4
  #define OUTPUT_TO_RIGHT D2
  #define INPUT_FROM_LEFT D6
  attachInterrupt(digitalPinToInterrupt(INPUT_FROM_RIGHT), readPin_RIGHT, FALLING);
  attachInterrupt(digitalPinToInterrupt(INPUT_FROM_LEFT), readPin_LEFT, FALLING); 

  pinMode (OUTPUT_TO_LEFT,OUTPUT);           //set the pins on the board
  digitalWrite(OUTPUT_TO_LEFT, HIGH);
  pinMode (INPUT_FROM_RIGHT,INPUT);
  //pinMode(SENSOR_LEFT, INPUT_PULLUP);
 // pinMode(SENSOR_RIGHT, INPUT_PULLUP);
  pinMode(OUTPUT_TO_RIGHT, OUTPUT);
  digitalWrite(OUTPUT_TO_RIGHT, HIGH);
  pinMode(INPUT_FROM_LEFT, INPUT);

  
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

  receiverThread.onRun(receiverThreadRun);
  receiverThread.setInterval(100);
  controll.add(&receiverThread);

}

void loop() {
  controll.run();
  if(myID == 0 && rightsIP[3] == 0) {
  IPAddress tempIP(192, 168, 0, rights_ID);
  Serial.print("trying to connect to: ");
  Serial.println(rights_ID);
  if (client.connect(tempIP, 5000)) {
    //client.write( "left");
    Serial.println(client.write("left"));
    String response1 = client.readStringUntil('\r');
    String response2 = client.readStringUntil('\r');
    if(response2.equals("ACK")){
      Serial.println("WOOOHOOO!!!");
      rightsIP = tempIP;
    }
    Serial.println(response1 + "1");
    Serial.println(response2 + "2");
    Serial.println(rightsIP);
   }
  }
if(digitalRead (INPUT_FROM_LEFT) == HIGH){  
  Serial.println("left high!!!");
 // sendPulsesLEFT = true;
}
if(digitalRead (INPUT_FROM_RIGHT) == HIGH){  
  Serial.println("right high!!!");
 // sendPulsesRIGHT = true;
}
if(sendPulsesLEFT && myID > 0){
    digitalWrite(OUTPUT_TO_LEFT, LOW);
    delay(1);
    digitalWrite(OUTPUT_TO_LEFT, HIGH);
    Serial.println(myID);
    myID--;
}

if(sendPulsesRIGHT && myID > 0){
    digitalWrite(OUTPUT_TO_RIGHT, LOW);
    delay(1);
    digitalWrite(OUTPUT_TO_RIGHT, HIGH);
    Serial.println(myID);
    myID--;
}

//if(digitalRead(SENSOR_LEFT) == HIGH){
////  Serial.println("Reset");
//  myID = 3;
//}

//if(digitalRead(SENSOR_RIGHT) ==LOW){
//  lefts_ID = 0;
//}
 if(y == 0){
  Serial.println(rightsIP);
  y++;
 }
}

void receiverThreadRun(){
  WiFiClient newClient = server.available();
  if (newClient) {
    Serial.println("newClient");
        Serial.println("reading");
        String msg = newClient.readStringUntil('\r');
        Serial.println(newClient.remoteIP());
        Serial.println(msg);
        if(msg.equals("right")){
           if(newClient.remoteIP()[3] == rights_ID){
            // client.write("ACK\r");
             newClient.write("ACK\r");
             Serial.println("minACK");
           }else{
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
           }
        }
    newClient.stop();
  }
  //Serial.println(rightsIP);
}

void readPin_RIGHT(){
    sendPulsesRIGHT = true;
    rights_ID++;
    Serial.println(rights_ID);
}
void readPin_LEFT(){
    sendPulsesLEFT = true;
    lefts_ID++;
    Serial.println(lefts_ID);
}
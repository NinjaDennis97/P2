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

 int myID = 29;
 int lefts_ID = 0;
 int rights_ID = 0;
 int ups_ID;
 int downs_ID;
 bool sendPulsesLEFT = false;
 bool sendPulsesRIGHT = false;
 unsigned long now = millis();
 unsigned long unPluggedFor = 0;
 unsigned long docked_right = 0;
 unsigned long docked_left = 0;
 bool is_docked_left;
 bool is_docked_right;
 

IPAddress myIP(192, 168, 0, myID);             // IP address of the server
IPAddress gateway(192,168,0,1);             // gateway of your network
IPAddress subnet(255,255,255,224);          // subnet mask of your network
IPAddress rightsIP (192, 168, 0, 0);
IPAddress leftsIP (192, 168, 0, 0);

 
void ICACHE_RAM_ATTR readPin_RIGHT();
void ICACHE_RAM_ATTR readPin_LEFT();
void ICACHE_RAM_ATTR sensor_Right();
void ICACHE_RAM_ATTR sensor_Left();

void setup() {
  Serial.begin(115200);                       // only for debug
  // D0 Kan inte användas för att läsa in pulser!!!!!!
  #define OUTPUT_TO_LEFT D5                   // DEFINE pins on the board
  #define INPUT_FROM_RIGHT D1
 // #define SENSOR_LEFT D7
 // #define SENSOR_RIGHT D4
  #define OUTPUT_TO_RIGHT D2
  #define INPUT_FROM_LEFT D6
  attachInterrupt(digitalPinToInterrupt(INPUT_FROM_RIGHT), sensor_Right, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_FROM_LEFT), sensor_Left, RISING); 

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
  now = millis();
  controll.run();
  if(myID == 0 && rightsIP[3] == 0) {
  IPAddress tempIP(192, 168, 0, lefts_ID);
  Serial.print("trying to connect to: ");
  Serial.println(lefts_ID);
  if (client.connect(tempIP, 5000)) {
    //client.write( "right");
    Serial.println(client.write("right"));
    String response1 = client.readStringUntil('\r');
    String response2 = client.readStringUntil('\r');
    if(response2.equals("ACK")){
      Serial.println("WOOOHOOO!!!");
      leftsIP = tempIP;
    }
    Serial.println(response1 + "1");
    Serial.println(response2 + "2");
    Serial.println(leftsIP);
   }
  }

if(digitalRead(INPUT_FROM_RIGHT) == HIGH){
  Serial.println("INPUT FROM RIGHT");
  if (now - docked_right > 100){
    attachInterrupt(digitalPinToInterrupt(INPUT_FROM_RIGHT), readPin_RIGHT, FALLING);
    is_docked_right = true;
    if(myID > 0){
     digitalWrite(OUTPUT_TO_RIGHT, LOW);
     delay(1);
     digitalWrite(OUTPUT_TO_RIGHT, HIGH);
     Serial.println(myID);
     myID--;
    }
  }  if (now - docked_right > 70){
    attachInterrupt(digitalPinToInterrupt(INPUT_FROM_RIGHT), readPin_RIGHT, FALLING);
    is_docked_right = true;
      }else{
    is_docked_right = false;
  }
}

if(digitalRead(INPUT_FROM_LEFT) == HIGH){
  Serial.println("INPUT FROM LEFT");
  if (now - docked_right > 100){
    attachInterrupt(digitalPinToInterrupt(INPUT_FROM_LEFT), readPin_LEFT, FALLING);
    is_docked_left = true;
    if(myID > 0){
     digitalWrite(OUTPUT_TO_LEFT, LOW);
     delay(1);
     digitalWrite(OUTPUT_TO_LEFT, HIGH);
     Serial.println(myID);
     myID--;
    }
  } if (now - docked_right > 80){
    attachInterrupt(digitalPinToInterrupt(INPUT_FROM_LEFT), readPin_LEFT, FALLING);
    is_docked_left = true;
      }else{
    is_docked_left = false;
    
  }
}

//  
//if(digitalRead (INPUT_FROM_LEFT) == HIGH){  
//  Serial.println("left high!!!");
// // sendPulsesLEFT = true;
//}
//if(digitalRead (INPUT_FROM_RIGHT) == HIGH){  
//  Serial.println("right high!!!");
// // sendPulsesRIGHT = true;
//}
}

void receiverThreadRun(){
  WiFiClient newClient = server.available();
  if (newClient) {
    Serial.println("newClient");
        Serial.println("reading");
        String msg = newClient.readStringUntil('\r');
        Serial.println(newClient.remoteIP());
        Serial.println(msg);
        if(msg.equals("left")){
           if(newClient.remoteIP()[3] == lefts_ID){
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
    attachInterrupt(digitalPinToInterrupt(INPUT_FROM_RIGHT), sensor_Right, RISING);
    sendPulsesRIGHT = true;
    rights_ID++;
    Serial.print("R_id: ");
    Serial.println(rights_ID);
}
void readPin_LEFT(){
    attachInterrupt(digitalPinToInterrupt(INPUT_FROM_LEFT), sensor_Left, RISING);
    sendPulsesLEFT = true;
    lefts_ID++;
    Serial.print("L_id: ");
    Serial.println(lefts_ID);
}
void sensor_Right(){
  attachInterrupt(digitalPinToInterrupt(INPUT_FROM_RIGHT), readPin_RIGHT, FALLING);
  docked_right = millis();
  Serial.println("docked_right");
}
void sensor_Left(){
  attachInterrupt(digitalPinToInterrupt(INPUT_FROM_LEFT), readPin_LEFT, FALLING);
  docked_left = millis();
  Serial.println("docked_left");
}

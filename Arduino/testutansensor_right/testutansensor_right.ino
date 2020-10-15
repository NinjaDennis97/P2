#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Thread.h>
#include <ThreadController.h>

ThreadController controll = ThreadController();
Thread receiverThread = Thread();


char ssid[] = "NETGEAR61";                // SSID of your home WiFi
char pass[] = "12345678";                 // password of your home WiFi
WiFiServer server(5000);
WiFiClient client;

 int myNbr = 17;
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
 int left_nbr = 0;
 int right_nbr = 0;
 int up_nbr = 0;
 int down_nbr = 0;
 int rightHighTime, leftHighTime,upHighTime, downHighTime;
 void ICACHE_RAM_ATTR readPinRIGHT();
 void ICACHE_RAM_ATTR readPinLEFT();
 void ICACHE_RAM_ATTR readPinLowRIGHT();
 void ICACHE_RAM_ATTR readPinLowLEFT();

IPAddress myIP(192, 168, 0, myNbr);             // IP address of the server
IPAddress gateway(192,168,0,1);             // gateway of your network
IPAddress subnet(255,255,255,224);          // subnet mask of your network
IPAddress leftsIP (192, 168, 0, 0);
IPAddress rightsIP (192, 168, 0, 0);
IPAddress upsIP (192, 168, 0, 0);
IPAddress downsIP (192, 168, 0, 0);
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  #define OUTPUT_LEFT D6
  #define INPUT_LEFT D5
  #define INPUT_RIGHT D1
  #define OUTPUT_RIGHT D2
  //#define OUTPUT_UP TX
  //#define INPUT_UP RX
  #define OUTPUT_DOWN D8
  #define INPUT_DOWN D7
  attachInterrupt(digitalPinToInterrupt(INPUT_RIGHT), readPinRIGHT, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INPUT_LEFT), readPinLEFT, CHANGE);


  pinMode (OUTPUT_LEFT,OUTPUT);
  pinMode (INPUT_LEFT,INPUT);
  pinMode (INPUT_RIGHT,INPUT);
  pinMode (OUTPUT_RIGHT,OUTPUT);
  
  //pinMode (OUTPUT_UP,OUTPUT);
  //pinMode (INPUT_UP,INPUT);
  pinMode (INPUT_DOWN,INPUT);
  pinMode (OUTPUT_DOWN,OUTPUT);


  digitalWrite(OUTPUT_LEFT, HIGH);
  digitalWrite(OUTPUT_RIGHT, HIGH);
//  digitalWrite(OUTPUT_UP, HIGH);
  digitalWrite(OUTPUT_DOWN, HIGH);


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
  connection();

  
  if(digitalRead(INPUT_LEFT) == HIGH && millis()-leftHighTime > 200){
    sendLeft = true;
  }
  
  if(sendLeft == true){
    if(pulsesToSendLeft > 0){
      digitalWrite(OUTPUT_LEFT, LOW);
      delay(1);
      digitalWrite(OUTPUT_LEFT, HIGH);
      Serial.print("send l");
      Serial.println(pulsesToSendLeft);
      pulsesToSendLeft--;
    }
  }


  if(digitalRead(INPUT_RIGHT) == HIGH && millis()-rightHighTime > 200){
    sendRight = true;
  }
  if(sendRight){
    if(pulsesToSendRight > 0){
      digitalWrite(OUTPUT_RIGHT, LOW);
      delay(1);
      digitalWrite(OUTPUT_RIGHT, HIGH);

      Serial.println(pulsesToSendRight);
      pulsesToSendRight--;
    }
  }
}

void readPinLEFT(){
  int readedValue = digitalRead(INPUT_LEFT);
  if(readedValue == HIGH){
    leftHighTime = millis();
  }
  if(readedValue == LOW){
      if(millis()-leftHighTime > 100){
      countPulsesLeft = true;
      }
    if(countPulsesLeft){
      left_nbr++;
      Serial.print("L: ");
      Serial.println(left_nbr);
    }
  }
}

void readPinRIGHT(){
  int readedValue = digitalRead(INPUT_RIGHT);
  if(readedValue == HIGH){
    rightHighTime = millis();
  }
  if(readedValue == LOW){
      if(millis()-rightHighTime > 100){
        countPulsesRight = true;
      }
    if(countPulsesRight){
      right_nbr++;
      Serial.print("R: ");
      Serial.println(right_nbr);    
    }
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
        if(msg.equals("left")){
           if(newClient.remoteIP()[3] == left_nbr){
             newClient.write("ACK\r");
             newClient.flush();
             Serial.println("minACK");
           }else{
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
           }
        }
        if(msg.equals("right")){
           if(newClient.remoteIP()[3] == right_nbr){
             newClient.write("ACK\r");
             newClient.flush();
             Serial.println("minACK");
           }else{
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
           }
        }
        if(msg.equals("up")){
           if(newClient.remoteIP()[3] == up_nbr){
             newClient.write("ACK\r");
             newClient.flush();
             Serial.println("minACK");
           }else{
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
           }
        }
        if(msg.equals("down")){
           if(newClient.remoteIP()[3] == down_nbr){
             newClient.write("ACK\r");
             newClient.flush();
             Serial.println("minACK");
           }else{
        newClient.write("NACK\r");
        newClient.flush();
        Serial.println("minNACK");
           }
        }
    newClient.stop();
    }
  }


void connection(){
  if(pulsesToSendLeft == 0 && leftsIP[3] == 0 && left_nbr != 0) {
  IPAddress tempIP(192, 168, 0, left_nbr);
  Serial.print("trying to connect to: ");
  Serial.println(left_nbr);
  if (client.connect(tempIP, 5000)) {
    Serial.println(client.write("right"));
    client.flush();
//    String response1 = client.readStringUntil('\r');
//    String response2 = client.readStringUntil('\r');
    String response;
    int i = 0;
    do{
      response = client.readStringUntil('\r');
      Serial.print(++i);
      delay(102);
    }while(response.equals(""));
    if(response.equals("ACK") || response.equals("ACK")){
      Serial.println("WOOOHOOO!!!");
      leftsIP = tempIP;
    }
    //Serial.println(response1 + "1");
   // Serial.println(response2 + "2");
    Serial.println(response);
    Serial.println(leftsIP);
   }
  }
/*-----------------------------------------------------------*/
   if(pulsesToSendRight == 0 && rightsIP[3] == 0) {
   IPAddress tempIP(192, 168, 0, right_nbr);
   Serial.print("trying to connect to: ");
   Serial.println(right_nbr);
    if (client.connect(tempIP, 5000)) {
     Serial.println(client.write("left"));
     client.flush();
    // String response1 = client.readStringUntil('\r');
    // String response2 = client.readStringUntil('\r');
    String response;
    do{
      response = client.readStringUntil('\r');
    }while(response.equals(""));
      if(response.equals("ACK") || response.equals("ACK")){
       Serial.println("WOOOHOOO!!!");
       rightsIP = tempIP;
     }
//     Serial.println(response1 + "1");
//     Serial.println(response2 + "2");
     Serial.println(response);
     Serial.println(rightsIP);
    }
   }
/*--------------------------------------------------------------*/
   if(pulsesToSendUp == 0 && upsIP[3] == 0) {
   IPAddress tempIP(192, 168, 0, up_nbr);
   Serial.print("trying to connect to: ");
   Serial.println(up_nbr);
    if (client.connect(tempIP, 5000)) {
     Serial.println(client.write("down"));
     client.flush();
     String response1 = client.readStringUntil('\r');
     String response2 = client.readStringUntil('\r');
      if(response1.equals("ACK") || response2.equals("ACK")){
       Serial.println("WOOOHOOO!!!");
       upsIP = tempIP;
     }
     Serial.println(response1 + "1");
     Serial.println(response2 + "2");
     Serial.println(upsIP);
    }
   }
/*--------------------------------------------------------------*/
   if(pulsesToSendDown == 0 && downsIP[3] == 0) {
   IPAddress tempIP(192, 168, 0, down_nbr);
   Serial.print("trying to connect to: ");
   Serial.println(down_nbr);
    if (client.connect(tempIP, 5000)) {
     Serial.println(client.write("up"));
     client.flush();
     String response1 = client.readStringUntil('\r');
     String response2 = client.readStringUntil('\r');
      if(response1.equals("ACK") || response2.equals("ACK")){
       Serial.println("WOOOHOOO!!!");
       downsIP = tempIP;
     }
     Serial.println(response1 + "1");
     Serial.println(response2 + "2");
     Serial.println(downsIP);
    }
   } 
}

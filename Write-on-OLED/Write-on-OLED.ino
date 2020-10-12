//Dessa fyra biblioteken måste inkluderas.
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Dessa två rader måste vara med.
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

String up_neighbour = "4";
String down_neighbour = "8";
String left_neighbour = "10";
String right_neighbour = "12";
String my_ID = "17";


void setup() {
  Serial.begin(9600);
  //Raden nedan är ett måste i setupen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  delay(1000);
  WriteonOLED();
  

}

void loop() {
  

}

void WriteonOLED () {
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(20,7);
  display.println("C");
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("U:" + up_neighbour);
  display.setCursor(0,41);
  display.println("D:" + down_neighbour);
  display.setCursor(40,0);
  display.println("L:" + left_neighbour);
  display.setCursor(40,41);
  display.println("R:" + right_neighbour);
  display.setCursor(0,20);
  display.println(my_ID);
  display.display();
 }

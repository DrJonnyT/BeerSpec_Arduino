#include <Wire.h>
#include "Adafruit_TCS34725.h"


/*********
  BeerSpec Arduino
  https://github.com/DrJonnyT/BeerSpec_Arduino
  This code is designed to run on an Arduino MEGA 2560 Board
  Also see https://github.com/DrJonnyT/BeerSpec_GUI for a windows control GUI
*********/
//

//LED pins
int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

//Serial input
String inputString = "";         // a String to hold incoming serial data
bool stringComplete = false;  // whether the string is complete


//Initial settings
int LEDR = 0;
int LEDG = 0;
int LEDB = 0;
int gainExtR = 1;
int gainExtG = 1;
int gainExtB = 1;
int gainScaR = 1;
int gainScaG = 1;
int gainScaB = 1;
float intTimeExtR = 24.;
float intTimeExtG = 24.;
float intTimeExtB = 24.;
float intTimeScaR = 24.;
float intTimeScaG = 24.;
float intTimeScaB = 24.;
int numSamples = 1;


void setup() {
  
// Begins serial communication 
Serial.begin(9600);

// reserve 200 bytes for the inputString:
inputString.reserve(200);

//Cycle LED to signify setup OK
set_LED(128,0,0);
delay(1000);
set_LED(0,128,0);
delay(1000);
set_LED(0,0,128);
delay(1000);
set_LED(0,0,0);


}
void loop() {

  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    //Set the LED
    if (inputString == "#SETLED") {
      // look for the next valid integer in the incoming serial stream:
      int red = Serial.parseInt();
      int green = Serial.parseInt();
      int blue = Serial.parseInt();
      set_LED(red,green,blue);
    }

    //Set the gains
    if (inputString == "#SETGAINS") {
      // look for the next valid integer in the incoming serial stream:
      gainExtR = Serial.parseInt();
      gainExtG = Serial.parseInt();
      gainExtB = Serial.parseInt();
      gainScaR = Serial.parseInt();
      gainScaG = Serial.parseInt();
      gainScaB = Serial.parseInt();      
    }

    if (inputString == "#READEXT") {
        read_ext();
      }

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}



void set_LED(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
  Serial.print("RGB = (" + String(red_light_value) + "," + String(green_light_value) + "," + String(blue_light_value) + ")\n");
}


void read_ext(){
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R = ");
  Serial.print(r);
  Serial.println();
  Serial.print("G = ");
  Serial.print(g);
  Serial.println();
  Serial.print("B = ");
  Serial.print(b);
  Serial.println();
  Serial.print("C = ");
  Serial.print(c);
  Serial.println();
}

void set_and_read(int red_light_value, int green_light_value, int blue_light_value){
  set_LED(red_light_value,green_light_value,blue_light_value);
  delay(4000);
  read_ext();
  delay(1000);
}

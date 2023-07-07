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
Adafruit_TCS34725 tcs_Ext = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);

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
LEDR = 128;
set_LED();
delay(1000);
LEDR = 0;
LEDG = 128;
set_LED();
delay(1000);
LEDG = 0;
LEDB = 128;
set_LED();
delay(1000);
LEDB=0;
set_LED();


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

    //Set the LED colours
    if (inputString == "#SETTINGSLEDRGB") {
      // look for the next valid integer in the incoming serial stream:
      LEDR = Serial.parseInt();
      LEDG = Serial.parseInt();
      LEDB = Serial.parseInt();
    }

    //Set the LED to the selected colours as a one-off
    if (inputString == "#SETLED") {
      set_LED();
    }

    //Set the gains
    if (inputString == "#SETTINGSGAINS") {
      // look for the next valid integer in the incoming serial stream:
      gainExtR = Serial.parseInt();
      gainExtG = Serial.parseInt();
      gainExtB = Serial.parseInt();
      gainScaR = Serial.parseInt();
      gainScaG = Serial.parseInt();
      gainScaB = Serial.parseInt();      
    }
    
    //Read Ext sensor in sequence, with relevant settings
    if (inputString == "#READEXTSEQ") {
        //readExtSeq();
      }

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


//Turn the LED on to the relevant settings
void set_LED()
 {
  analogWrite(red_light_pin, LEDR);
  analogWrite(green_light_pin, LEDG);
  analogWrite(blue_light_pin, LEDB);
  Serial.print("LEDRGB = (" + String(LEDR) + "," + String(LEDG) + "," + String(LEDB) + ")\n");
}

//Turn the Ext gain on to the relevant settings
void set_GainExt(Adafruit_TCS34725 tcs, int gain)
 {
     switch (gain) {
  case 1:
    tcs.setGain(TCS34725_GAIN_1X);
    Serial.print("Set Ext gain to 1");
    Serial.println();
    break;
  case 4:
    tcs.setGain(TCS34725_GAIN_4X);
    Serial.print("Set Ext gain to 4");
    Serial.println();
    break;
  case 16:
    tcs.setGain(TCS34725_GAIN_16X);
    Serial.print("Set Ext gain to 16");
    Serial.println();
    break;
  case 60:
    tcs.setGain(TCS34725_GAIN_60X);
    Serial.print("Set Ext gain to 60");
    Serial.println();
    break;
  default:
    // Code to execute when expression doesn't match any case
    tcs.setGain(TCS34725_GAIN_1X);
    Serial.print("Set Ext gain to 1 (default)");
    Serial.println();
    break; 
   }
 }

//Turn the Ext integration time to the relevant settings
void set_IntegTimeExt(Adafruit_TCS34725 tcs,int integTime)
 {
   switch (integTime) {
  case 24:
    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
    Serial.print("Set Integration time to 24ms");
    Serial.println();
    break;
  case 60:
    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_60MS);
    Serial.print("Set Integration time to 60ms");
    Serial.println();
    break;
  case 120:
    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_120MS);
    Serial.print("Set Integration time to 120ms");
    Serial.println();
    break;
  case 240:
    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_240MS);
    Serial.print("Set Integration time to 240ms");
    Serial.println();
    break;
  case 480:
    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_480MS);
    Serial.print("Set Integration time to 480ms");
    Serial.println();
    break;
  // Add more case statements as needed
  default:
    // Code to execute when expression doesn't match any case
    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
    Serial.print("Set Integration time to 24ms (default)");
    Serial.println();
    break;
    }
 }

   
   





// //Read extinction sensor, just one colour
// void read_ext_onecolour(char colour){
//   //First set the sensor to the relevant settings
//   setIntegrationTime(uint8_t it);
  

//   uint16_t r, g, b, c;
//   tcs.getRawData(&r, &g, &b, &c);

//   Serial.print("R = ");
//   Serial.print(r);
//   Serial.println();
//   Serial.print("G = ");
//   Serial.print(g);
//   Serial.println();
//   Serial.print("B = ");
//   Serial.print(b);
//   Serial.println();
//   Serial.print("C = ");
//   Serial.print(c);
//   Serial.println();
// }

// void set_and_read(int red_light_value, int green_light_value, int blue_light_value){
//   set_LED(red_light_value,green_light_value,blue_light_value);
//   delay(4000);
//   read_ext();
//   delay(1000);
// }

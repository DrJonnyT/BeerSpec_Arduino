#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <avr/wdt.h> // Include the Watchdog Timer library

/*********
  BruSpec Arduino
  https://github.com/DrJonnyT/BruSpec_Arduino
  This code is designed to run on an Arduino MEGA 2560 Board
  Also see https://github.com/DrJonnyT/BruSpec_GUI for a windows control GUI
*********/

/********PINS********/
//LED pins
int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

//Set the tcs LED pin
int tcsLEDPin = 2;
int tcsScaLEDPin = 3;


/********SENSOR SETUP********/
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();
/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_120MS, TCS34725_GAIN_16X);
//The I2C bus for each detector, used on the PCA9546A board
int i2CBusExt = 0;
int i2CBusSca = 1;


/********SERIAL INPUT********/
String inputString = "";         // a String to hold incoming serial data
bool stringComplete = false;  // whether the string is complete


/********INSTRUMENT SETTINGS********/
//LED colour
int LEDR = 0;
int LEDG = 0;
int LEDB = 0;
//Detector gains
int gainExt = 16;
int gainSca = 16;
//Text representation of the detector gains, to be used in tcs.setGain function
char str_GainExt[18];
char str_GainSca[18];
//Detector integration times (ms)
int intTimeExt = 120;
int intTimeSca = 120;
//Text representation of the integration times, to be used in tcs.setIntegrationTime function
char str_IntTimeExt[31];
char str_IntTimeSca[31];


/********FUNCTION DECLARATIONS********/
void LED_On(bool quiet=false);
void LED_Off();
void settings_LED(int red, int green, int blue);
void set_Gains(int ext, int sca);
void set_IntTimes(int ext, int sca);
void setGain(int gain, char* str_Gain);
void setIntTime(int gain, char* str_IntTime);
void read_Ext();
void read_Sca();
void selectI2CBus(uint8_t bus);
void softReset();


void setup() {

//Set the tcs LED pins to zero
pinMode(tcsLEDPin, OUTPUT);
digitalWrite(tcsLEDPin, LOW);
pinMode(tcsScaLEDPin, OUTPUT);
digitalWrite(tcsScaLEDPin, LOW);

// Begins serial communication 
Serial.begin(9600);
Serial.println("@SETUPSTARTING");

// reserve 200 bytes for the inputString:
inputString.reserve(200);

//Setup the I2C multiplexer
Wire.begin();
read_Ext();
read_Sca();

//Cycle LED to signify setup OK
settings_LED(128,0,0);
LED_On(true);
delay(1000);
settings_LED(0,128,0);
LED_On(true);
delay(1000);
settings_LED(0,0,128);
LED_On(true);
delay(1000);
LED_Off();
Serial.println("@SETUPCOMPLETE");
}

/*
  All the loop needs to do is clear the serial input so the same command
  doesn't get run twice. All the main work is done by serialEvent
*/
void loop() {  
  if (stringComplete) {
    //Serial.println(inputString);
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

    //Set the LED colours settings
    if (inputString == "#SETTINGSLEDRGB") {
      // look for the next valid integer in the incoming serial stream:
      LEDR = Serial.parseInt();
      LEDG = Serial.parseInt();
      LEDB = Serial.parseInt();
      Serial.println("@LEDRGB = " + String(LEDR) + " " + String(LEDG) + " " + String(LEDB));
    }

    //Set the LED to the selected colours as a one-off
    if (inputString == "#LEDON") {
      LED_On();
    }

    //Set the LED to off as a one-off
    if (inputString == "#LEDOFF") {
      LED_Off();
    }

    //Set the LED colours, gains and int times
    if (inputString == "#SETALL") {
      // look for the next valid integer in the incoming serial stream:
      int red = Serial.parseInt();
      int green = Serial.parseInt();
      int blue = Serial.parseInt();
      int gExt = Serial.parseInt();
      int gSca = Serial.parseInt();
      int itExt = Serial.parseInt();
      int itSca = Serial.parseInt();

      settings_LED(red, green, blue);
      LED_On();
      set_Gains(gExt, gSca);
      set_IntTimes(itExt, itSca);

    }
    
    //Read Extinction sensor
    if (inputString == "#READEXT") {
      read_Ext();
    }

    //Read Scattering sensor
    if (inputString == "#READSCA") {
      read_Sca();
    }

    //Check serial comms
    if (inputString == "#CHECKSERIAL") {
      Serial.println("@SERIALOK");
    }

    //Soft reset the Arduino board
    if (inputString == "#RESET") {
      softReset();
    }


    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


/********FUNCTION DEFINITIONS********/

//Turn the LED on to the relevant settings
void LED_On(bool quiet)
 {
  analogWrite(red_light_pin, LEDR);
  analogWrite(green_light_pin, LEDG);
  analogWrite(blue_light_pin, LEDB);
  if(quiet == false){
  Serial.println("@LEDON");
  }
}

//Turn the LED off
void LED_Off()
 {
  analogWrite(red_light_pin, 0);
  analogWrite(green_light_pin, 0);
  analogWrite(blue_light_pin, 0);
  Serial.println("@LEDOFF");
}

//Set the LED settings
void settings_LED(int red, int green, int blue)
 {
  LEDR = red;
  LEDG = green;
  LEDB = blue;
  Serial.println("@LEDSETTINGS = " + String(LEDR) + " " + String(LEDG) + " " + String(LEDB));
}

//Set the Gain settings
void set_Gains(int ext, int sca)
 {
  gainExt = ext;
  gainSca = sca;
  //Get the relevant string for the gain setting
  selectI2CBus(i2CBusExt);
  setGain(gainExt,str_GainExt);
  selectI2CBus(i2CBusSca);
  setGain(gainSca,str_GainSca);
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%s%s%d%s%s", "@GAINSETTINGS = ", gainExt," ",str_GainExt," ",gainSca," ",str_GainSca);
  Serial.println(serialOut);
}

//Set the Int Time settings
void set_IntTimes(int ext, int sca)
 {
  intTimeExt = ext;
  intTimeSca = sca;
  //Select Ext detector and set int time
  selectI2CBus(i2CBusExt);
  setIntTime(intTimeExt,str_IntTimeExt);
  //Select Sca detector and set int time
  selectI2CBus(i2CBusSca);
  setIntTime(intTimeSca,str_IntTimeSca);
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%s%s%d%s%s", "@INTTIMESETTINGS = ", intTimeExt," ",str_IntTimeExt," ",intTimeSca," ",str_IntTimeSca);
  Serial.println(serialOut);
}

// Set the detector gain, and gain string for traceability
//New version using str_Gain as argument
void setGain(int gain, char* str_Gain) {
  switch (gain) {
    case 1:
      strncpy(str_Gain, "TCS34725_GAIN_1X", sizeof(str_GainExt) - 1);
      tcs.setGain(TCS34725_GAIN_1X);
      break;
    case 4:
      strncpy(str_Gain, "TCS34725_GAIN_4X", sizeof(str_GainExt) - 1);
      tcs.setGain(TCS34725_GAIN_4X);
      break;
    case 16:
      strncpy(str_Gain, "TCS34725_GAIN_16X", sizeof(str_GainExt) - 1);
      tcs.setGain(TCS34725_GAIN_16X);
      break;
    case 60:
      strncpy(str_Gain, "TCS34725_GAIN_60X", sizeof(str_GainExt) - 1);
      tcs.setGain(TCS34725_GAIN_60X);
      break;
    default:
      Serial.println("@Invalid gain setting: " + String(gain));
      strncpy(str_Gain, "TCS34725_GAIN_1X", sizeof(str_GainExt) - 1);
      tcs.setGain(TCS34725_GAIN_1X);
      break;
  }

  str_Gain[sizeof(str_GainExt) - 1] = '\0'; // Null-terminate the character array
}

// Set the integration time, and intTime string for traceability
void setIntTime(int intTime, char* str_IntTime) {
  switch (intTime) {
    case 24:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_24MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
      break;
    case 50:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_50MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_50MS);
      break;
    case 60:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_60MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_60MS);
      break;
    case 120:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_120MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_120MS);
      break;
    case 240:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_240MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_240MS);
      break;
    case 480:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_480MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_480MS);
      break;
    case 600:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_600MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_600MS);
      break;
    default:
      Serial.println("@Invalid integ time setting: " + String(intTime));
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_24MS", sizeof(str_IntTimeExt) - 1);
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
      break;
  }

  str_IntTime[sizeof(str_IntTimeExt) - 1] = '\0'; // Null-terminate the character array
}

//Read extinction sensor
void read_Ext(){
  //Select Ext serial bus
  selectI2CBus(i2CBusExt);
  //Variables to take the read data
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  //Send it straight away
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%d%s%d", "@EXT = ", r," ",g," ",b);
  Serial.println(serialOut);
}

//Read scattering sensor
//Currently a dummy function
void read_Sca(){
  //Select Sca serial bus
  selectI2CBus(i2CBusSca);
  //Variables to take the read data
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  //Send it straight away
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%d%s%d", "@SCA = ", r," ",g," ",b);
  Serial.println(serialOut);
}

// Select I2C BUS
void selectI2CBus(uint8_t bus){
    Wire.beginTransmission(0x70);
    Wire.write(1 << (bus)); 
    Wire.endTransmission();
}

void softReset() {
  wdt_enable(WDTO_15MS); // Enable the WDT with a short timeout (15ms)
  while (1); // Wait for the WDT to trigger a reset
}
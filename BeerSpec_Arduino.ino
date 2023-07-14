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

//Set the tcsExt LED pin
int tcsExtLEDPin = 2;

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();


/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcsExt = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);


//Serial input
String inputString = "";         // a String to hold incoming serial data
bool stringComplete = false;  // whether the string is complete


//Initial settings
//LED colour
int LEDR = 0;
int LEDG = 0;
int LEDB = 0;
//Detector gains
int gainExt = 1;
int gainSca = 1;
//Text representation of the detector gains, to be used in tcs.setGain function
char str_GainExt[18];
char str_GainSca[18];
//Detector integration times (ms)
int intTimeExt = 24;
int intTimeSca = 24;
//Text representation of the integration times, to be used in tcs.setIntegrationTime function
char str_IntTimeExt[31];
char str_IntTimeSca[31];


void setup() {

//Set the tcs LED pin to zero
pinMode(tcsExtLEDPin, OUTPUT);    // sets the digital pin 13 as output
digitalWrite(tcsExtLEDPin, LOW);

// Begins serial communication 
Serial.begin(9600);

// reserve 200 bytes for the inputString:
inputString.reserve(200);

//Cycle LED to signify setup OK
settings_LED(128,0,0);
LED_On();
delay(1000);
settings_LED(0,128,0);
LED_On();
delay(1000);
settings_LED(0,0,128);
LED_On();
delay(1000);
LED_Off();
Serial.println("@SETUPCOMPLETE");

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

    //Set the LED colours settings
    if (inputString == "#SETTINGSLEDRGB") {
      // look for the next valid integer in the incoming serial stream:
      LEDR = Serial.parseInt();
      LEDG = Serial.parseInt();
      LEDB = Serial.parseInt();
      Serial.print("@LEDRGB = " + String(LEDR) + " " + String(LEDG) + " " + String(LEDB));
      Serial.println();
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
      settings_Gains(gExt, gSca);
      settings_IntTimes(itExt, itSca);

    }


    
    //Set the gain and int time and make a measurement
    if (inputString == "#SETREADEXT") {
      setReadExt();       
      }

    

    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


//Turn the LED on to the relevant settings
void LED_On()
 {
  analogWrite(red_light_pin, LEDR);
  analogWrite(green_light_pin, LEDG);
  analogWrite(blue_light_pin, LEDB);
  Serial.println("@LEDON");
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
void settings_Gains(int ext, int sca)
 {
  gainExt = ext;
  gainSca = sca;
  //Get the relevant string for the gain setting
  setGainString(gainExt,str_GainExt);
  setGainString(gainSca,str_GainSca);
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%s%s%d%s%s", "@GAINSETTINGS = ", gainExt," ",str_GainExt," ",gainSca," ",str_GainSca);
  Serial.println(serialOut);
}

//Set the Int Time settings
void settings_IntTimes(int ext, int sca)
 {
  intTimeExt = ext;
  intTimeSca = sca;
  //Get the relevant string for the int time settings
  setIntTimeString(intTimeExt,str_IntTimeExt);
  setIntTimeString(intTimeSca,str_IntTimeSca);
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%s%s%d%s%s", "@INTTIMESETTINGS = ", intTimeExt," ",str_IntTimeExt," ",intTimeSca," ",str_IntTimeSca);
  Serial.println(serialOut);
}


//Set the gain string to be used to set the gain
void setGainString(int gain, char* str_Gain) {
  switch (gain) {
    case 1:
      strncpy(str_Gain, "TCS34725_GAIN_1X", sizeof(str_GainExt) - 1);
      tcsExt.setGain(TCS34725_GAIN_1X);
      break;
    case 4:
      strncpy(str_Gain, "TCS34725_GAIN_4X", sizeof(str_GainExt) - 1);
      tcsExt.setGain(TCS34725_GAIN_4X);
      break;
    case 16:
      strncpy(str_Gain, "TCS34725_GAIN_16X", sizeof(str_GainExt) - 1);
      tcsExt.setGain(TCS34725_GAIN_16X);
      break;
    case 60:
      strncpy(str_Gain, "TCS34725_GAIN_60X", sizeof(str_GainExt) - 1);
      tcsExt.setGain(TCS34725_GAIN_60X);
      tcsExt.setGain(60);
      break;
    default:
      Serial.println("@Invalid gain setting: " + String(gain));
      strncpy(str_Gain, "TCS34725_GAIN_1X", sizeof(str_GainExt) - 1);
      tcsExt.setGain(TCS34725_GAIN_1X);
      break;
  }

  str_Gain[sizeof(str_GainExt) - 1] = '\0'; // Null-terminate the character array
}

//Set the integration time string to be used to set the gain
void setIntTimeString(int intTime, char* str_IntTime) {
  switch (intTime) {
    case 24:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_24MS", sizeof(str_IntTimeExt) - 1);
      tcsExt.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
      break;
    case 60:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_60MS", sizeof(str_IntTimeExt) - 1);
      tcsExt.setIntegrationTime(TCS34725_INTEGRATIONTIME_60MS);
      break;
    case 120:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_120MS", sizeof(str_IntTimeExt) - 1);
      tcsExt.setIntegrationTime(TCS34725_INTEGRATIONTIME_120MS);
      break;
    case 240:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_240MS", sizeof(str_IntTimeExt) - 1);
      tcsExt.setIntegrationTime(TCS34725_INTEGRATIONTIME_240MS);
      break;
    case 480:
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_480MS", sizeof(str_IntTimeExt) - 1);
      tcsExt.setIntegrationTime(TCS34725_INTEGRATIONTIME_480MS);
      break;
    default:
      Serial.println("@Invalid integ time setting: " + String(intTime));
      strncpy(str_IntTime, "TCS34725_INTEGRATIONTIME_24MS", sizeof(str_IntTimeExt) - 1);
      tcsExt.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
      break;
  }

  str_IntTime[sizeof(str_IntTimeExt) - 1] = '\0'; // Null-terminate the character array
}




//Read extinction sensor
void read_ext(){
  //Variables to take the read data
  uint16_t r, g, b, c;
  tcsExt.getRawData(&r, &g, &b, &c);
  //Send it straight away
  char serialOut[50];
  sprintf(serialOut, "%s%d%s%d%s%d%s%d", "@EXT = ", r," ",g," ",b," ",c);
  Serial.println(serialOut);
}

//Set tcsExt with gain and int time, and read RGB
void setReadExt(){
  //Set detector - now handled by setGainString and setIntTimeString
  //tcsExt.setGain(*str_GainExt);
  //tcsExt.setIntegrationTime(*str_IntTimeExt);
  
  //Set LED
  LED_On();
  
  delay(4000);
  
  read_ext();
  
}

/*
 * ATTiny85-based Thermostat
 * M. Bujold - May 2020
 * Version History
 * v0.1 - Original test build
 * v0.2 - Added sanity check on timeout
 */
#define VERSION "0.2"
// Constants for control
#define TIMEOUT 120  // Timeout to prevent compressor over-cycling
#define T_OFFSET 1  // Temperature offset for hysteresis
#define LOOPDELAY 1000 // How often to refresh (this affects timeout countdown too)
#define LOWVALUE 2 // Bottom of set scale value (-ive)
#define LIGHT_THRESH 900 // Value above which photocell detects 'dark'

#define SDA_PORT PORTB
#define SDA_PIN 0
#define SCL_PORT PORTB
#define SCL_PIN 2

// config for tinyprint lib
#define TP_PRINTLINES 1
#define TP_FLASHSTRINGHELPER 0
#define TP_NUMBERS 1
#define TP_FLOAT 1
#define TP_WINDOWSLINEENDS 0

// Includes for OLED 128x64 display
#include "SH1106Lib.h"
#include "glcdfont.h"
// Includes for reading temp sensor
#include "OneWire.h"

SH1106Lib display;

// Temp sensor on pin 3
OneWire ow(3);

// Program variables
int timeout = 0;
int compStatus = 0;
double temp;
int analogVal;
int rstVal;

double readTemp(void){
  byte data[12];
  int16_t raw;
  
  ow.reset();
  ow.skip();
  ow.write(0x44);
  delay(1000);
  ow.reset();
  ow.skip();
  ow.write(0xBE);
  byte i;
  for (i=0; i<9; i++){
    data[i] = ow.read();
  }

  raw = (data[1] << 8) | data[0];
  return (float)raw / 16.0;
 
}

double setTemp(int rawValue){
  // Take ADC value and convert to temperature (0.5 deg steps)
  if (rawValue < 175){
    return (LOWVALUE * (-1)) + ((rawValue / 42) * 0.5);
  }
  else{
    return ((rawValue / 42) * 0.5);
  }
}

void updateDisp(int curTmp, int setTmp, int timeout){
  
}

void setup(){
  pinMode(A0, INPUT); // Photocell
  pinMode(A2, INPUT); // Potentiometer
  pinMode(PB1, OUTPUT); // Relay
  digitalWrite(PB1, LOW);
  display.initialize();
  display.clearDisplay();
  display.setFont(font,5,7);
  display.setTextWrap(true);
  display.setTextColor(WHITE, TRANSPARENT);
  display.clearDisplay();
  // temp sensor
  //while(!ow.search(addr)){
  //  delay(750);
  //}
  display.setCursor(0,0);
  display.print("Thermostat v");
  display.println(VERSION);
}

void loop(){
  temp = readTemp(); // Read from DS18B20
  analogVal = analogRead(A2); // Setpoint value
  rstVal = analogRead(A0); // Photocell value
  // Draw box to cover old text
  display.fillRect(60,16,60,60,BLACK);
  // Write data
  display.setCursor(0,16);
  display.print("Current T: ");
  display.setCursor(60,16);
  display.println(temp, 1);
  display.setCursor(0,24);
  display.print("Set T: ");
  display.setCursor(60,24);
  display.println(setTemp(analogVal), 1);
  display.setCursor(0,32);

  display.print("Photocell: ");
  display.setCursor(60,32);
  display.println(rstVal, 10);
  /*
  if (rstVal > LIGHT_THRESH){
    // If rstVal is higher than LIGHT_THRESH, assume it is 'dark'
    display.println(rstVal);
    //display.println("Dark");
  }
  else{
    display.println(rstVal);
    //display.println("Light");  
  }
  */
  display.setCursor(0,40);
  display.print("Timeout: ");
  display.setCursor(60,40);
  display.println(timeout, 10);

  display.setCursor(0,48);
  display.print("Relay: ");
  display.setCursor(60,48);
  display.println(digitalRead(PB1), 10);
  //display.println(compStatus, 10);

  display.setCursor(0,56);
  if (temp + T_OFFSET >= setTemp(analogVal)){
    // Relay on
    if (timeout == 0){
      digitalWrite(PB1, HIGH);
      compStatus = 1;
    }
    
  }
  else{
    if (compStatus == 1){
      // Relay off
      digitalWrite(PB1, LOW);
      compStatus = 0;
      timeout = TIMEOUT;
    }
  }

  if (compStatus == 0){
    timeout--;
  }

  // Sanity check on timeout, testing revealed bug where value went out-of-bounds
  if (timeout > TIMEOUT || timeout < 0){
    timeout = TIMEOUT;
  }
  delay(LOOPDELAY);
  
}


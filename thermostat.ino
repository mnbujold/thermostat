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

// Constants for control
#define TIMEOUT 60  // Timeout to prevent compressor over-cycling
#define T_OFFSET 2  // Temperature overshoot for hysteresis

// Includes for OLED display
#include "SH1106Lib.h"
#include "glcdfont.h"

// Includes for reading temp sensor
#include "OneWire.h"

SH1106Lib display;

OneWire ow(3);

int timeout = 0;
int compStatus = 0;

void setup(){
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(PB1, OUTPUT);
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
  display.println("Thermostat v0.1");
}

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
  if (rawValue < 175){
    return (2 * (-1)) + ((rawValue / 42) * 0.5);
  }
  else{
    return ((rawValue / 42) * 0.5);
  }
}
void loop(){
  double temp;
  temp = readTemp();
  int analogVal = analogRead(A2);
  int rstVal = analogRead(A0);
  // Draw box to erase old text
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
  //display.println(rstVal, 10);
  if (rstVal > 1010){
    display.println("Dark");
  }
  else{
    display.println("Light");  
  }
  display.setCursor(0,40);
  display.print("Timeout: ");
  display.setCursor(60,40);
  display.println(timeout, 10);

  display.setCursor(0,48);
  display.print("Relay: ");
  display.setCursor(60,48);
  display.println(digitalRead(PB1), 10);
  //display.println(compStatus, 10);

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
  delay(1000);
  
}


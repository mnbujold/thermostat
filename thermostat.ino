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

// Includes for OLED display
#include "SH1106Lib.h"
#include "glcdfont.h"

// Includes for reading temp sensor
#include "OneWire.h"

SH1106Lib display;

OneWire ow(3);

void setup(){
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  
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
  display.println("Thermostat Test");
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
  if (rawValue < 200){
    return (2 * (-1)) + (rawValue * 0.01);
  }
  else{
    return (rawValue * 0.01);
  }
}
void loop(){
  double temp;
  temp = readTemp();
  int analogVal = analogRead(A2);
  int rstVal = analogRead(A0);
  // Draw box to erase old text
  display.fillRect(0,8,30,30,BLACK);
  // Write data
  display.setCursor(0,8);
  display.println(temp, 2);
  display.setCursor(0,16);
  display.println(setTemp(analogVal), 1);
  display.setCursor(0,24);
  display.println(rstVal, 10);

  delay(500);
  
  
}


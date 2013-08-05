/*
                               _                
 ___  _   _  _ __ ___    ___  | |_  ___   _   _ 
 / __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
 \__ \| |_| || | | | | || (_) || |_| (_) || |_| |
 |___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
 |___/ 
 
 --------------------------------------------------------------------------------
 A fast C-Library for several 'Intelligent Led Matrix Displays'
 that works with just 2/3 wires though 2 shift registers or 1 GPIO extender chip,
 for most Arduinos, Arduino 2 and Teensy (3 included).
 version 1 (12 March 2013)
 coded by Max MC Costa for s.u.m.o.t.o.y - sumotoy@gmail.com
 --------------------------------------------------------------------------------
 Demo 2 - demonstrate the ability of the library to easy show any type
 of value or alphanumeric in 1 to 8 display units easily (each unit = 4 char)
 This demo use a MCP23S17 GPIO extender so it works in SPI mode.
 */

#include <SPI.h>
#include <mcp23xxx.h>
#include <IntMatrixDisp.h>

#define MCP_CS_PIN 10
#define MCP_ADRS 0x20
#define DISPLAY_UNITS 3

IntMatrixDisp mdisp = IntMatrixDisp(MCP_CS_PIN,MCP_ADRS,HDLA2416);

void setup()
{
  //Serial.begin(38400);
  //Serial.println("start");
  mdisp.init(DISPLAY_UNITS,DIRECT);//necessary! will init SPI also

  mdisp.setBrightness(0,3);
  delay(100);
  mdisp.setAutoclean(true);

  mdisp.scroll("Intelligent Matrix-LED display library v1.0",80);
  for (byte i=1;i<4;i++){
    mdisp.printString("Intensity...",1);
    mdisp.setBrightness(4-i,0);
    delay(500);
  }
   mdisp.setBrightness(0,0);
}



void loop()
{
  mdisp.clearAll();
  mdisp.printString("Numbers",1);
  delay(100);
  mdisp.clearAll();
  mdisp.printNumber(12345678,0);
  delay(500);
  mdisp.clearAll();
  mdisp.printString("Centigrades",1);
  delay(100);
  mdisp.clearAll();
  mdisp.printFloat((float)22.5,1,C,0);
  delay(1000);
  mdisp.setCursor(0);
  mdisp.printString("Farenheight",1);
  delay(100);
  mdisp.clearAll();
  mdisp.printFloat((float)22.5,1,F,0);
  delay(1000);
  mdisp.setCursor(0);
  mdisp.printString("Decibels",1);
  delay(100);
  mdisp.clearAll();
  mdisp.printFloat((float)18.5,1,dB,0);
  delay(1000);
  mdisp.setCursor(0);
  mdisp.printString("Degrees",1);
  delay(100);
  mdisp.clearAll();
  mdisp.printFloat((float)160.5,1,DEG,0);
  delay(1000);
  mdisp.setCursor(0);
  mdisp.printString("Frequency (plus efx)",1);
  delay(100);
  mdisp.clearAll();
  mdisp.printFloat((float)16550.5,1,Hz,1);
  delay(1000);
}

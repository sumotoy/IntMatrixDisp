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

 coded by Max MC Costa for s.u.m.o.t.o.y - sumotoy@gmail.com
 --------------------------------------------------------------------------------
 Demo 3 - minimal setup.
 */

#include <SPI.h>
#include <IntMatrixDisp.h>

#define MCP_CS_PIN 10   //we use SPI so we need CS pin
#define MCP_ADRS 0x20   //we use SPI with HAEN features (8 chip with same CS shared) so we need address
#define DISPLAY_UNITS 2 //how many display connected?

IntMatrixDisp mdisp = IntMatrixDisp(MCP_CS_PIN,MCP_ADRS,HDLA2416);//configured as SPI with HAEN enabled and display type xxxx

void setup()
{

  mdisp.init(DISPLAY_UNITS,_DIRECT);//we specify the display addressing method and initialize the library
  mdisp.setBrightness(3);//has effects only with some display type
  delay(100);
  mdisp.scroll("Intelligent Matrix-LED display library v1.0",80);
}



void loop()
{
}

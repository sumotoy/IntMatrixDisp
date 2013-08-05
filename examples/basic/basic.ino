#include <SPI.h>
#include <IntMatrixDisp.h>
/*
DEMO 'BASIC'
This library has been written for easily control many old-style alphanumeric led-matrix
 intelligent displays from Avago, Siemens, Osram and so on. Those displays use quite a lot
 of pins so this library allow to control from 1 to 16 or more displays with 2/3 wire on SPI,WIRE
 on Arduino/Teensy2/Teensy3 with just 1 MCP23xx17+1 chip.
 The example uses 2 displays and one MCP23s17.
 Note that I'm driving MCP23s17 by using it's HAEN specialized option that allow you to use at list
 8 MCP23sxx chip compatible HAEN on same wires (include CS pin!)
 !!! Read documentation inside IntMatrixDisp for wiring and compatible displays !!!
 */
#define MCP_CS_PIN 10    // witch CS pin to use, this is the default one called SS
#define MCP_ADRS 0x20    // HAEN addressing! sounds strange use an hardware addressing in SPI but true
#define DISPLAY_UNITS 2  // how many units we are using?

// first, create an instance
// parameters:CS pin, mcp address, display type (see docs)
IntMatrixDisp mdisp = IntMatrixDisp(MCP_CS_PIN,MCP_ADRS,HDLA2416);

void setup()
{
  // we first init the display, the SPI and tell library witch addressing method we choosed
  // PAY ATTENTION! We init the spi here so do not add any spi init after this or before.
  // parameters:how many displays(numeric),driving method(_DIRECT or _74HC138)
  mdisp.init(DISPLAY_UNITS,_DIRECT);//necessary! will init SPI also
  delay(100);
  /*
   not all chip support brightness, if not it will be ignored
   values:0 to 7. May vary results in some display
   Parameters:witch display(0=all or display n),value(0..7)
   */
  mdisp.setBrightness(4);
  delay(100);
  /*
  autoclean will prevent text garbage by clearing
   text from the point selected to the end
   */
  mdisp.setAutoclean(false);
  //set cursor to a specified char position
  mdisp.setCursor(0);//doesn exat what you mean
  // scrolling text. On arduino will use a delay but Teensy 3 has the ability to use internal timer for this
  // parameters: text,speed
  mdisp.scroll("Universal 2/3 wire led matrix library for arduino and teensy 3",80);
}

void loop()
{
  //clean all display and set cursor to 0
  mdisp.clearAll();
  //simply print a string
  //parameters:text,use efx(0=no,1=yes)
  mdisp.printString("Numbers",1);
  delay(100);
  mdisp.clearAll();
  //print a number
  //parameters:number,use efx(0=no,1=yes)
  mdisp.printNumber(12345678,0);
  delay(500);
  mdisp.clearAll();
  mdisp.printString("Centigrades",1);
  delay(100);
  mdisp.clearAll();
  //print a float
  //parameters:float number,how many digits,preset (NONE,POL,C,F,Hz,dB,DEG,Hz(automatic Khz,Mhz,etc)),use efx(0=no,1=yes)
  //note that apart float number the other parameters are optional! In that case we print centigrades
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



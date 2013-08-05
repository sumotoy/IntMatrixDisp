#include <SPI.h>
#include <mcp23xxx.h>
#include <IntMatrixDisp.h>
/*
This library has been written for easily control many old-style alphanumeric led-matrix
 intelligent displays from Avago, Siemens, Osram and so on. Those displays use quite a lot
 of pins so this library allow to control from 1 to 8 displays with 2/3 wire on SPI,WIRE
 on Arduino or Teensy2 or Teensy3 with just 1 MCP23xx17+1 chip, for this reason it's needed
 library mcp23xxx I developed for use as standalone and included in other libraries.
 This is the SPI/Wire version and will be extented for use ShiftRegisters too.
 Actually, here's displays handled:
 DLR3416,DLO3416,DLG3416,DLR2416,DLO2416,DLG2416,
 HDLA2416,HDLS2416,HDLO2416,HDLG2416,
 HPDL1414,DL3416 plus all compatible models (should work but I cannot verify).
 !!! Read documentation about wiring !!!
 */
#define MCP_CS_PIN 10
#define MCP_ADRS 0x20
#define DISPLAY_UNITS 1
/* 
	Instantiate the library---------------------------
	cs pin, chip address, how many displays, chip type 
*/
IntMatrixDisp mdisp = IntMatrixDisp(MCP_CS_PIN,MCP_ADRS,HDLA2416);

void setup()
{
  //Serial.begin(38400);
  //Serial.println("start");
  mdisp.init(DISPLAY_UNITS);//necessary! will init SPI also
  /*
  This following it's necessary, tell library
  how to handle the phisical addressing of each display
  */
  mdisp.setAddressChip(TTL74HC191);
  delay(100);
  /*
   not all chip support brightness, if not
   will not create any problem to uncompatible chip
   values:0 to 7. May vary results in some display
   */
  mdisp.setBrightness(1,4);
  delay(100);
  /*
  autoclean will prevent text garbage by clearing
   text from the point selected to the end
   */
  mdisp.setAutoclean(false);
  mdisp.setCursor(0);//doesn exat what you mean
  /*
  Library does not use arduino text write and have specialized
   functions:
   xxx.printString(text,(optional)bool efx);
   xxx.printNumbers(numeric,(optional)bool efx);
   xxx.printFloat(float,(optional)byte digits,(optional)bool useSign,(optional)bool efx);
   digits are decimal numbers (default 2) and has correct rounding
   useSign put - or + in front number
   xxx.printDb(uint8_t val,(optional)byte digits,(optional)bool efx,(optional)float maxdb) 
   this is very useful in audio applications, it convert a digital number (0..255) to
   it's dB counterpart by using the maxdb parameter that is 6.50. This mean that 255 it's 6.50
   dB so changing this parameter will modify the convertion algorithm
   xxx.printTemp(float,(optional)byte digits,(optional)bool mode,(optional)bool efx)
   another specialized function for write temperature. mode control the sign (cent or far)
   Beside those functions there's some useful primitive ones:
   xxx.writeChar(char,digit,(optional)autoposition)
   This write a single char in the selected digit, the autoposition (normally false) will
   auto increment the global cursor position so next time you use it it will not cover the
   existing char.
   There's a variation xxx.writeChar(char,(optional)autoposition) that not contain digit, in that
   case use xxx.setCursor(pos);
   Last function is 
   xxx.scroll(text,(optional)speed);
   */
  mdisp.scroll("Brightness..."); 
  delay(100);
  mdisp.printString("wow!",false);

  mdisp.setBrightness(1,7);
  delay(300);

  mdisp.setBrightness(1,5);
  delay(100);
  mdisp.scroll("Numeric Values"); 
  delay(100);
  mdisp.printNumber(1234,1);
  delay(700);
  mdisp.scroll("Float Values"); 
  delay(100);
  mdisp.printFloat((float)8,1,1);
  delay(700);
  mdisp.scroll("dB Values"); 
  delay(100);
  mdisp.printDb(100,1);
  delay(700);
  mdisp.scroll("Any text...");  
  delay(100);
  mdisp.printString("CIAO",false);
  delay(700);
  mdisp.scroll("Effected...");  
  delay(100);
  mdisp.printString("CIAO",true);
  delay(700);
  mdisp.printTemp((float)10);
  delay(300);
}

void loop()
{
  //  mdisp.scroll("SUMOTOY STUDIOS",150); 
  //  delay(300);
  //  randomCharGenerator();
}


void randomCharGenerator(){
  for (int t=0;t<10;t++){
    mdisp.clearAll();
	delay(50);
    for (int i=0;i<(mdisp.getDigitsPerUnit()*DISPLAY_UNITS);i++){
      mdisp.writeChar(random(0,127),i);
      delay(80);
    }
  }
}



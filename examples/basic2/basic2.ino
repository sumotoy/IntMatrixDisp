#include <SPI.h>
#include <IntMatrixDisp.h>

#define MCP_CS_PIN 10
#define MCP_ADRS 0x20
#define DISPLAY_UNITS 2

IntMatrixDisp mdisp = IntMatrixDisp(MCP_CS_PIN,MCP_ADRS,HDLA2416);

void setup()
{
  // we first init the display, the SPI and tell library witch addressing method we choosed
  // PAY ATTENTION! We init the spi here so do not add any spi init after this or before.
  // parameters:how many displays(numeric),driving method(_DIRECT or _74HC138)
  mdisp.init(DISPLAY_UNITS,_DIRECT);//necessary! will init SPI also
  delay(100);

  //mdisp.setBrightness(3);
  //delay(100);
  mdisp.setAutoclean(true);

  mdisp.scroll("Intelligent Matrix-LED display library v1.0",80);
  for (int i=0;i<8;i++){
    mdisp.setBrightness(7-i);
    mdisp.setCursor(0);
    mdisp.printString("Int: ");
    mdisp.printNumber(7-i,0);
    if (i == 7 || i == 0) {
      delay(700);
    } else {
      delay(300);
    }
    
  }
  mdisp.setBrightness(3);
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

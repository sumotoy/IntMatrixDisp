/*
                               _                
 ___  _   _  _ __ ___    ___  | |_  ___   _   _ 
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/ 

--------------------------------------------------------------------------------
A library for several 'Intelligent Led Matrix Displays'
that works with just 2/3 wires though 2 shift registers or 1 GPIO extender chip,
for most Arduinos, Teensy's and Enegia IDE compatible MCU's.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VERSION 1b9 (11 october 2014) should work with Energia IDE
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
coded by Max MC Costa for s.u.m.o.t.o.y - sumotoy@gmail.com
note: if you want to use (even parts), inform to the author, thanks!
--------------------------------------------------------------------------------
Intelligent Led Matrix Displays from the past are beautiful and sexy, they was very
expensive and mostly usud in expensive stuff but they are very readable. Today it's
not easy to find but sometime you can find NOS for a resonable price. First problem,
they use a lot of pins, second dei consume quite a lot of current (of course, they are
maded of leds!). For consumption you can use an oscillator to the blank pin but some
have a kind of intensity control maded by an internal oscillator, for pins I used
a fantastic SPI/I2C GPIO extender chip that saves a lot of pin and can be used in
multiples (till 8) on same pinouts using a feature called HAEN (in SPI). This library
need just one GPIO (from microchip, called MCP23S17) for drive 4 units but adding an extra
addressing chip (that not use any pin of your processor) 16 units can be drived.
--------------------------------------------------------------------------------
version note:
this is the first working version but only partially! Actually only SPI supported and
many code needs fix/optimizations, you are warned.
--------------What is working:
all functions enabled
most or all displays
2 addressing methods (max 8 units)
--------------ToDo:
Optimizations
More addressing methods
use Teensy 3 timer for scrolling
I2C methods
Switch registers methods

*/
#ifndef IntMatrixDisp_h
#define IntMatrixDisp_h

#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#else
#include "Arduino.h"
#endif

#include <inttypes.h>
#include "Print.h"
//#include <utility/mcpB23xxx.h>

#define INVERTED_ADDRESSING		// location of char 0 pos will be at right, disable this will set at left
#define MAX_DISPLAYS 		16	// max number of displays for current hardware
#define DEFAULT_BRIGHTNESS 	0	// Only works with displays that allow extended functions	
#define USE_DIRECT_READ  		// will save ram and it's faster but not reallocate missed chars
//#define DDDEBUG 1

//------------ BIT and PIN definitions of MCP (see datasheet) ------------
//------------port A (MCP23xx17)---------------------------------------------------
/*
_dataBox format:(bit7)[CUE/D6/D5/D4/D3/D2/D1/D0](bit0)
display-----bit-----mcp-pin--nam-------------------------------note----------------------
MCP_DATA0 	[0]		pin 21	(data)					address char/extended functions*
MCP_DATA1 	[1]		pin 22	(data)					address char/extended functions*
MCP_DATA2 	[2]		pin 23	(data)					address char/extended functions*
MCP_DATA3 	[3]		pin 24	(data)					address char/extended functions*
MCP_DATA4 	[4]		pin 25	(data)					address char/extended functions*
MCP_DATA5 	[5]		pin 26	(data)					address char/extended functions*
MCP_DATA6 	[6]		pin 27	(data)					address char/extended functions*
MCP_CUE 	[7]		pin 28	(data) 					0:normal(ASCII)/1:active
//------------port B (MCP23xx17)--------------------------------------------------
_cntrBox format:(bit7)[WR/CU/CE4/CE3/CE2/CE1/A1/A0](bit0)
MCP_A0 		[0]		pin 1	(digit selector)		addresses the digit for single module
MCP_A1 		[1]		pin 2   (digit selector)		addresses the digit for single module
MCP_CE1 	[2]		pin 3   (display selector)		addresses module.(NOTE)*depends of chip addressing method
MCP_CE2 	[3]		pin 4	(display selector)		addresses module.(NOTE)*depends of chip addressing method
MCP_CE3 	[4]		pin 5	(display selector)		addresses module.(NOTE)*depends of chip addressing method
MCP_CE4 	[5]		pin 6	(display selector)		addresses module.(NOTE)*depends of chip addressing method
MCP_CU 		[6]		pin 7   (0:ON / 1:OFF)			access extended functions(*)
MCP_WR 		[7]		pin 8	(0:ON / 1:OFF)			write command
---------- other display pins -----------------------------------------------------
MCP_CLR 			tied to +5
MCP_BLK 			tied to +5 or 555 timer for pulse-blank (see datasheet)
+					tied to +5
-					tied to ground
CE1					depends of the choosed hardware addressing method(**)
CE2					depends of the choosed hardware addressing method(**)
(*)  not all chips uses extended functions
(**) not all chip has 2 addressing CE pin, some (very old ones) has none! 
If chip has none CE pin you should have extra hardware for addressing.

*** HDLX-2416 extended functions mode----------------------------------------------
*D0:		remove/store cursor at digit selected - 0:remove/1:store
*D1:		allow digit to be blanked			  - 0:allow blank /1:NOT allow
*D2:		disable display blanking			  - 0:allow blank /1:NOT allow
*D3:		intensity control (see brightness table)
*D4:		intensity control (see brightness table)
*D5:		intensity control (see brightness table)
*D6:		disable ALL extended functions
A0:			active;address D0 & D1 functions - select digit
A1:			active;address D0 & D1 functions - select digit
CU:			should be LOW for extended functions mode
WR:			should be LOW for extended functions mode
CE1:		enable the display when LOW
CE2:		enable the display when LOW
CLR:		should be HIGH for extended functions mode
CUE:		xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
brightness table -----------------------
_dataBox format:CUE/D6/D5/D4/D3/D2/D1/D0
	00000000 - 000 - 100% - 0	-	0x00
	00001000 - 001 - 60%  - 1	-	0x08
	00010000 - 010 - 40%  - 2	-	0x10
	00011000 - 011 - 27%  - 3	-	0x18
	00100000 - 100 - 17%  - 4	-	0x20
	00101000 - 101 - 10%  - 5	-	0x28
	00110000 - 110 - 7%   - 6	-	0x30
	00111000 - 111 - 3%   - 7	-	0x70
*/
/*
ADDRESSING CHIP WIRING
Since most of the pin of these displays are commoned you need an addressing method.
This library allow to choose from different methods depends of how many displays
you want to use.

_DIRECT wiring (max 4 displays)------------------------------
This use the internal addressing wired on GPIO chip and no extra hardware is needed.
Some display has 2 CE pin: one must be tied to ground, the other CE pin
should be connect as below. If only one CE pin is provided connect as below.
				mcp pin3  		->		  CE pin display 1	
				mcp pin4  		->		  CE pin display 2			
				mcp pin5  		->		  CE pin display 3	
				mcp pin6  		->		  CE pin display 4		
				
_74HC138 wiring (max 8 displays)-----------------------------
This extra chip hallow you connect as much as 8 displays.
Some display has 2 CE pin: one must be tied to ground, the other CE pin
should be connect as below. If only one CE pin is provided connect as below.
				mcp pin3  -> [|--U--|] <- +5v
				mcp pin4  -> [|     |] <- CE pin display 1
				mcp pin5  -> [|     |] <- CE pin display 2
				gnd       -> [|     |] <- CE pin display 3
				gnd       -> [|     |] <- CE pin display 4
				+5V       -> [|     |] <- CE pin display 5
		CE pin display 8  -> [|     |] <- CE pin display 6
				gnd       -> [|_____|] <- CE pin display 7
*/
#define MCP_CUE 	7// mcp port a

#define MCP_A0 		0// mcp port b
#define MCP_A1 		1// mcp port b
#define MCP_CE1 	2// mcp port b
#define MCP_CE2 	3// mcp port b
#define MCP_CE3 	4// mcp port b
#define MCP_CE4 	5// mcp port b
#define MCP_CU 		6// mcp port b
#define MCP_WR 		7// mcp port b

#define MCP_DC 		0

//-----------------------------------------------------
typedef enum MLD_type{
    DLR3416 	= 1,//siemens 5x7
	DLO3416 	= 2,
    DLG3416 	= 3,
		  
	DLR2416 	= 4,
	DLO2416 	= 5,
	DLG2416 	= 6,
		  
	HDLA2416	= 7,
	HDLS2416 	= 8,
	HDLO2416 	= 9,
	HDLG2416 	= 10,
	HPDL1414 	= 11,
	DL3416		= 12,
	DL2416		= 13 //siemens
};
	
typedef enum ADRS_CHIP{
    _DIRECT 	= 50,// (4 displays) OK, fully working!
	_74HC138 	= 52,// (8 displays) OK, fully working!
	/*
	_HEF4515 	= 53,// (16 displays) NO!
	_74HC4551	= 54,// (16 displays) NO!
	_74HC154	= 55 // (16 displays) NO!
	*/
};	

typedef enum PRES_VALS{
    NONE 	= 20,
	POL 	= 21,
	C 		= 22,
	F 		= 23,
	dB 		= 24,
	DEG 	= 25,
	Hz 		= 26
};	
class IntMatrixDisp : public Print {
  public:
	IntMatrixDisp		(const uint8_t cs_pin,const byte addr,uint8_t MLD_type);//SPI
	//IntMatrixDisp		(const byte addr,uint8_t MLD_type);//I2C
	//IntMatrixDisp		(const byte dta,const byte clk,const byte ltc,uint8_t MLD_type);//switch register
	
	void 				init(const uint8_t displays,uint8_t ADRS_CHIP=_DIRECT);
	/*
		_DIRECT 	=	MAX 4 displays
		_74HC138	=	MAX 8 displays
		_HEF4515	=	MAX 16 displays
		_74HC4551	=	MAX 16 displays
		_74HC154	=	MAX 16 displays
	*/
	void 				clear(byte display=1);
	void 				clearDigit(uint8_t digit);
	void 				clearAll();
	void 				home(void);
	uint8_t 			getCursor(void);
	void 				setCursor(uint8_t position);
	void 				setAutoclean(bool autoclean);
	void 				writeChar(char c,uint8_t digit,bool autoPosition);
	void 				writeChar(char c,bool autoPosition);
	
	void 				printString(char* stringToDisplay,bool useEfx=0);
	void 				printNumber(long number,bool useEfx=false);
	void 				printFloat(float number,byte digits=2,uint8_t PRES_VALS=1,bool useEfx=0);
	void 				scroll(char* testo,unsigned int speed=80);
	void 				setBrightness(uint8_t value=7,byte display=0);//0...7
	uint8_t 			getDigitsPerUnit(void);
	float 				byteToDb(uint8_t byteVal,float maxdbVal=6.5);
	void				setCue(int digit);
	virtual size_t 		write(uint8_t);
	using Print::write;
  protected:

    uint8_t 			_csPin;
	uint8_t 			_dtaPin;
	uint8_t 			_clkPin;
	uint8_t 			_ltcPin;
    byte 				_hw_addr;
    byte 				_read_cmd;
    byte 				_write_cmd;
	word				_gpioDirection;
	word				_gpioState;
	byte				_comType;
    void 				write_word(byte addr, word data);

	void 				startSend(bool mode);
	void 				endSend();

    uint16_t 			byte2uint16(byte high_byte, byte low_byte);
	
	const static byte		_IOCON = 0x0A;
	const static byte		_IODIR = 0x00;
	const static byte		_GPIO = 0x12;
  private:
	//--------------------------------------------- PRIVATE DATA
	byte 				_inited;
	uint8_t 			_displays;
	uint8_t 			_dispType;
	uint8_t    			_digitPerUnit;
	uint8_t    			_addressMethod;
	bool 				_autoclean;
	bool 				_hasExtendedCtrl;

	uint8_t				_maxDigits;
	uint8_t				_maxUnits;
	uint8_t   			_lowChar;
	uint8_t   			_hiChar;
	
	bool				_softClear;
	
	byte				_dataBox;
	byte				_cntrBox;
	uint8_t 			_cursorPos;
	uint8_t   			_brightness;
	//--------------------------------------------- PRIVATE FUNCTIONS
	void 				_initialize(const uint8_t dta_pin,const uint8_t clk_pin,const uint8_t ltc_pin,const uint8_t cs_pin,const byte addr,uint8_t MLD_type);
	void 				writeCharacter(char c, uint8_t digit, bool autoPosition);
	void 				_writeCharacter(char c, uint8_t digit, bool autoPosition,bool useEfx);
	void 				_setBrightness(byte display,uint8_t value=7);//0...7
	void 				_setEnableDisplay(byte display);
	uint8_t 			_selectDigit(uint8_t digit,bool unitSelect=false);
	byte 				_digitToUnit(uint8_t digit);
	void 				_scrollEngine(char* testo,uint8_t lenght,uint8_t advance);
	void 				_efx(uint8_t position,uint8_t dly=0,char stopChar=0);
	void 				_cleanGarbage(void);
	void 				_sendData(bool writeBit);
	//void				_setCUE(uint8_t digit,bool state);
	//void 				printByte(byte data);
	int					_cueDigit;
	
	void 			beginGPIO(const uint8_t cs_pin,const byte addr);
    void 			gpioPinMode(bool mode);
	void 			gpioPinMode(uint8_t pin, bool mode);
    void 			gpioPort(word value);
	void 			gpioPort(byte lowByte, byte highByte);
	void 			_sideDelay(uint8_t val);
	uint8_t  		_ssdelay;
};

#endif
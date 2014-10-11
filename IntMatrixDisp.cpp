#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#else
#include "Arduino.h"
#endif
#include <SPI.h>
#include "intMatrixDisp.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>


//---------- STATIC VARIABLES ----------------------------------------------------

const byte tempCharA[27] = {
0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x20};
const byte tempCharB[11] = {
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x20};
const byte brightTable[8] = {
0b00111000,0b00110000,0b00101000,0b00100000,0b00011000,0b00010000,0b00001000,0b00000000};


/*-----------------------CONSTRUCTOR----------------------------*/

//drive by SPI
IntMatrixDisp::IntMatrixDisp(const uint8_t cs_pin,const byte addr,uint8_t MLD_type){
	_initialize(0,0,0,cs_pin,addr,MLD_type);
}

//drive by I2C
/*
IntMatrixDisp::IntMatrixDisp(const byte addr,uint8_t MLD_type){
	_initialize(0,0,0,0,addr,MLD_type);
}
*/

//drive by 2 Switch Registers
/*
IntMatrixDisp::IntMatrixDisp(const byte dta,const byte clk,const byte ltc,uint8_t MLD_type){
	_initialize(dta,clk,ltc,0,0x00,MLD_type);
}
*/

//private - initialization (just set some vars)
void IntMatrixDisp::_initialize(const uint8_t dta_pin,const uint8_t clk_pin,const uint8_t ltc_pin,const uint8_t cs_pin,const byte addr,uint8_t MLD_type){
	//determine witch way to drive display
	if (cs_pin > 0){//SPI
		this->_csPin = cs_pin;
		this->_dtaPin = 0;
		this->_clkPin = 0;
		this->_ltcPin = 0;
		this->_read_cmd  = (addr << 1) | 1;
		this->_write_cmd = addr << 1;
		this->_hw_addr = addr;
		this->_comType = 1;
	/*	
	} else if (dta_pin > 0 && clk_pin > 0 && cs_pin == 0){// Switch register
		this->_csPin = 0;
		this->_dtaPin = dta_pin;
		this->_clkPin = clk_pin;
		this->_ltcPin = ltc_pin;
		this->_read_cmd  = 0;
		this->_write_cmd = 0;
		this->_hw_addr = 0;
		this->_comType = 3;
	} else if (dta_pin == 0 && clk_pin == 0 && cs_pin == 0){// I2C
		this->_csPin = 0;
		this->_dtaPin = 0;
		this->_clkPin = 0;
		this->_ltcPin = 0;
		this->_read_cmd  = (addr << 1) | 1;
		this->_write_cmd = addr << 1;
		this->_hw_addr = addr;
		this->_comType = 2;
	*/	
	}
	_hasExtendedCtrl = false;
	_softClear = false;
	//set some basic parameters
	_dispType = 0;
	_digitPerUnit = 0;
	_lowChar = 0;
	_hiChar = 0;
	_softClear = true;
	_hasExtendedCtrl = false;
	//now determine witch display we are using...
	if (MLD_type == DLR3416 || MLD_type == DLO3416 || MLD_type == DLG3416){
		_dispType = 1;
		_digitPerUnit = 4;
		_lowChar = 0;
		_hiChar = 127;
		_hasExtendedCtrl = false;
	} else if (MLD_type == DLR2416 || MLD_type == DLO2416 || MLD_type == DLG2416){
		_dispType = 2;
		_digitPerUnit = 4;
		_lowChar = 0;
		_hiChar = 127;
		_hasExtendedCtrl = false;
	} else if (MLD_type == HDLA2416 || MLD_type == HDLS2416 || MLD_type == HDLO2416 || MLD_type == HDLG2416){
		_dispType = 2;
		_digitPerUnit = 4;
		_lowChar = 0;
		_hiChar = 127;
		_hasExtendedCtrl = true;
	} else if (MLD_type == HPDL1414){	
		_dispType = 3;
		_digitPerUnit = 4;
		_lowChar = 32;
		_hiChar = 95;
		_softClear = true;
		_hasExtendedCtrl = false;
	} else if (MLD_type == DL3416){	
		_dispType = 4;
		_digitPerUnit = 4;
		_lowChar = 32;
		_hiChar = 95;
		_softClear = true;
		_hasExtendedCtrl = false;
	} else if (MLD_type == DL2416){	
		_dispType = 3;
		_digitPerUnit = 4;
		_lowChar = 32;
		_hiChar = 95;
		_softClear = true;
		_hasExtendedCtrl = false;
	}
}


//public - main initialization
void IntMatrixDisp::init(const uint8_t displays,uint8_t ADRS_CHIP){
	_ssdelay = 3;//just for start
	_addressMethod = 0;
	_maxUnits = 0;
	_ssdelay = 0;
	if (_dispType == 0 || this->_comType == 0){
		//something wrong in initialization, cannot continue
		_inited = 0;
#ifdef DDDEBUG
		Serial.println("error in INIT");
#endif
	} else {
		//if (this->_comType == 1){//SPI
#ifdef DDDEBUG
	Serial.println("spi init");
#endif
			SPI.begin();
#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI_CLOCK_DIV4;
#elif defined(__SAM3X8E__)	
			SPI_CLOCK_DIV4;
#else
			SPI_CLOCK_DIV4;
#endif
			delay(100);
			::pinMode(this->_csPin,OUTPUT);
			::digitalWrite(this->_csPin,HIGH);
/*			
		} else if (this->_comType == 3){//shift register
			::pinMode(this->_dtaPin,OUTPUT);
			::pinMode(this->_ltcPin,OUTPUT);
			::pinMode(this->_clkPin,OUTPUT);
			::digitalWrite(this->_ltcPin,HIGH);	
			::digitalWrite(this->_clkPin,LOW);
#ifdef DDDEBUG
			Serial.println("SR init");
#endif	
		} else {//I2C
#ifdef DDDEBUG
			Serial.println("I2C init");
#endif	
*/
		//}
		gpioPinMode(OUTPUT);//set all mcp pins as out
		_inited = 1;
		_cueDigit = -1;
		//identify the address method
		if (ADRS_CHIP == _DIRECT){
			_addressMethod = 1;
			_maxUnits = 4;
			_ssdelay = 0;
		} else if (ADRS_CHIP == _74HC138){
			_addressMethod = 3;
			_maxUnits = 8;
			_ssdelay = 2;
		}
		if (displays > _maxUnits) {
			_displays = _maxUnits;
		} else {
			_displays = displays;
		}
		_maxDigits = _displays*_digitPerUnit;//calculate once _maxDigits
		clearAll();//clear displays
		_brightness = DEFAULT_BRIGHTNESS;//set default brightness var
		_cursorPos = 0;//default cursor position
		_autoclean = false;//autoclean default
	}
}

//public:clear one display unit
void IntMatrixDisp::clear(byte display){
	if (_inited && display <= _displays){
		uint8_t temp;
		//calcola il range dei digit coinvolti
		if (display <= 1){
			display = 1;
			temp = 0;
		} else {
			temp = _digitPerUnit*(display-1);//(2)4*2-1=4 | (3)4*3-1=8
		}
		for (uint8_t i=temp;i<(_digitPerUnit+temp);i++){
			_dataBox = 0b00100000;//space
			_cntrBox = 0b01000000;//prepare WR enabled
			_selectDigit(i,true);//set address bits 0,1,2,3,4,5 accordly
			_sendData(true);//write on
			_sendData(false);//write off
		}
		_dataBox = 0b00100000;//space,CUE off
		_cntrBox = 0b11111100;//resetted
	}
}

//public:clear all displays
void IntMatrixDisp::clearAll(){
	for (byte i=1;i<=_displays;i++){
		clear(i);
	}
	_cursorPos = 0;
}

//public:only does something if display support brightness
void IntMatrixDisp::setBrightness(uint8_t value,byte display){
	if (_hasExtendedCtrl){
		if (display == 0){
			for (byte i=1;i<=_displays;i++){
				_setBrightness(i,value);
			}
			return;
		} else if (display <= _displays){
			_setBrightness(display,value);
		}
	}
}

//private:main brightness routine
void IntMatrixDisp::_setBrightness(byte display,uint8_t value){
	if (_inited){
		//_cntrBox format:(bit7)[WR/CU/CE4/CE3/CE2/CE1/A1/A0](bit0)
		_cntrBox = 0b00000000;
		_setEnableDisplay(display);//this set the address 2,3,4,5 to _cntrBox
		if (value > 7) value = 7;
		_brightness = value;
		//_dataBox format:(bit7)[CUE/D6/D5/D4/D3/D2/D1/D0](bit0)
		_dataBox = brightTable[_brightness];//should set bit 3,4,5
		gpioPort(_dataBox,_cntrBox);//send
		_cntrBox |= B10000000;// set MCP_WR
		gpioPort(_dataBox,_cntrBox);//send again
		_dataBox = 0b00100000;
		_cntrBox |= B01000000;// set MCP_CU
		gpioPort(_dataBox,_cntrBox);//send again
	}
}


//public:clear just one digit (with space)
void IntMatrixDisp::clearDigit(uint8_t digit){
	if (digit < _maxDigits){
		writeCharacter(' ',digit,false);
	}
}



//private:modify 4 bit of _cntrBox to enable display(bit 2/3/4/5)
void IntMatrixDisp::_setEnableDisplay(byte display){
	_cntrBox &= B11000011;
	if (_addressMethod == 1) {        // direct mode, no chip (uses 4 bit for 4 display)
		if (display == 0 || display == 1){
			_cntrBox |= B00111000;// set bit 3,4,5 as 1
			_cntrBox &= B11111011;// clear bits 2
		} else if (display == 2) {
			_cntrBox |= B00110100;// set bit 2,4,5 as 1
			_cntrBox &= B11110111;// clear bits 3
		} else if (display == 3) {
			_cntrBox |= B00101100;// set bit 2,3,5 as 1
			_cntrBox &= B11101111;// clear bits 4
		} else if (display == 4) {
			_cntrBox |= B00011100;// set bit 2,3,4 as 1
			_cntrBox &= B11011111;// clear bits 5
		}
	} else if (_addressMethod == 2) { // not now
	} else if (_addressMethod == 3) { // 74LS138 chip (uses 3 bit for 8 display)	
		if (display == 0 || display == 1){
			_cntrBox |= B00100000;// set bit 5 as 1
			_cntrBox &= B11100011;// clear
		} else if (display == 2) {
			_cntrBox |= B00100100;// set 
			_cntrBox &= B11100111;// clear
		} else if (display == 3) {
			_cntrBox |= B00101000;// set 
			_cntrBox &= B11101011;// clear
		} else if (display == 4) {
			_cntrBox |= B00101100;// set 
			_cntrBox &= B11101111;// clear
		} else if (display == 5) {
			_cntrBox |= B00110000;// set 
			_cntrBox &= B11110011;// clear
		} else if (display == 6) {
			_cntrBox |= B00110100;// set 
			_cntrBox &= B11110111;// clear
		} else if (display == 7) {
			_cntrBox |= B00111000;// set 
			_cntrBox &= B11111011;// clear
		} else if (display == 8) {
			_cntrBox |= B00111100;// set 
		}
	}
}

//private:digit --> witch display belong (to enable it)
//TODO - need optimizations
byte IntMatrixDisp::_digitToUnit(uint8_t digit){
	byte d = 0;
	byte t = 1;
	uint8_t i;
	for (i = 0;i < (_digitPerUnit * _displays);i++){
		if (digit == i) return t;
		if (d >= 3){
			d = 0;
			t++;
		} else {
			d++;
		}
	}
}

//private:modify bit 0/1 of _cntrBox to address digit
uint8_t IntMatrixDisp::_selectDigit(uint8_t digit,bool unitSelect){
	if (digit >= _maxDigits) digit = _maxDigits - 1;//3-7-etc.
#if defined(INVERTED_ADDRESSING)
	digit = (_maxDigits-1) - digit;
#endif
	uint8_t display = _digitToUnit(digit);//witch display for the digit?
	//int display = digit >> (_digitPerUnit/2);
	digit = digit - (_digitPerUnit*(display-1));
	if (digit == 0){
		_cntrBox &= B11111100;//clear out bits 0 e 1
	} else if (digit == 1){
		_cntrBox |= B00000001; // set bit 0 as 1
		_cntrBox &= B11111101;//  clear bit 1
	} else if (digit == 2){
		_cntrBox |= B00000010; // set bit 0 as 1
		_cntrBox &= B11111110;//  clear bit 1
	} else if (digit == 3){
		_cntrBox |= B00000011; // set bit 0,1 as 1
	} else {
	}
	if (unitSelect) _setEnableDisplay(display);
	return display;
}

//private:send data
//TODO - add options for other methods
void IntMatrixDisp::_sendData(bool writeBit){
	if (writeBit){
		_cntrBox &= B01111111;// clear MCP_WR
	} else {
		_cntrBox |= B10000000;// set MCP_WR
	}
	gpioPort(_dataBox,_cntrBox);//send 2 byte
	//delay(1);
}

//private:write char
void IntMatrixDisp::writeCharacter(char c, uint8_t digit, bool autoPosition) {
	c = constrain(c,_lowChar,_hiChar);
	
	uint8_t display = _selectDigit(digit,true);//got the display interested and set his digit

	_dataBox = (byte)c;
	_sendData(true);//write ON
	//_cntrBox |= B11000011;// set MCP_WR
	//------------------------------------------------------------------------------------------------ccheck
	_cntrBox = 0b11111100;//resetted
	_sendData(false);//write OFF
	if (autoPosition) _cursorPos++;
}

//private:other method
void IntMatrixDisp::_writeCharacter(char c, uint8_t digit, bool autoPosition,bool useEfx) {
	if (useEfx) _efx(digit,0);//want efx?
	writeCharacter(c,digit,autoPosition);
}

//public:
void IntMatrixDisp::writeChar(char c,uint8_t digit,bool autoPosition) {
	if (digit < _maxDigits){
		writeCharacter(c,digit,false);
		if (autoPosition) _cursorPos = digit;
	} 
}

//public:
void IntMatrixDisp::writeChar(char c,bool autoPosition) {
	if (_cursorPos < _maxDigits) writeCharacter(c,_cursorPos,autoPosition);
}

//public:print a string
void IntMatrixDisp::printString(char* stringToDisplay,bool useEfx){
	uint8_t stringLenght = strlen(stringToDisplay);
	uint8_t cpos,thisChar;
	if (stringLenght == 0) { // la stringa è vuota
	} else if (stringLenght > 0 && stringLenght <= _maxDigits){//la stringa rimane dentro il display
		for (cpos = 0; cpos <  stringLenght; cpos++) {
			if (_cursorPos < _maxDigits){
				#if defined(INVERTED_ADDRESSING)
				_writeCharacter(stringToDisplay[cpos],_cursorPos,true,useEfx);
				#else
				_writeCharacter(stringToDisplay[stringLenght-cpos-1],_cursorPos,true,useEfx);
				#endif
			}
		}
		if (_autoclean) _cleanGarbage();	
   } else { //stringa troppo lunga, usa lo scroll
		scroll(stringToDisplay,100);
   }
}

//public:print a number
void IntMatrixDisp::printNumber(long number,bool useEfx){
	char ascii[_maxDigits];
	if (number > 32768 || number < -32767){
		ltoa(number,ascii,10);
	} else {
		itoa(number,ascii,10);
	}
	printString(ascii,useEfx);
}

//public:print a float
//TODO - need optimizations
void IntMatrixDisp::printFloat(float number,byte digits,uint8_t PRES_VALS,bool useEfx){
	char ascii[_maxDigits];
	double tempnum;
	char preChar;
	byte prefix = 0;
	//sign
	if (PRES_VALS == NONE || PRES_VALS == POL || PRES_VALS == C || PRES_VALS == F || PRES_VALS == dB || PRES_VALS == DEG){
		tempnum = (double)number;
		if (tempnum < 0.0) {
			tempnum = -tempnum;
			preChar = '-';
		} else if (tempnum == 0){ 
			preChar = ' ';
		} else {
			preChar = '+';
		}
		_writeCharacter(preChar,_cursorPos,true,useEfx);
	} else if (PRES_VALS == Hz){
		if (number > 999.9 && number <= 9999.9){
			prefix = 1;
			number = number/1000;
			digits = 1;
		} else if (number > 9999.9 && number <= 99999.9){
			prefix = 2;
			number = number/10000;
			digits = 1;
		} else if (number > 99999.9 && number <= 999999.9){
			prefix = 3;
			number = number/100000;
			digits = 1;
		} else if (number > 999999.9){
			prefix = 4;
			number = number/1000000;
			digits = 1;
		}
		tempnum = (double)number;
	} else {	
		tempnum = (double)number;
	}
	// correctly roundling(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (byte i=0;i < digits;++i){
		rounding /= 10.0;
	}
	tempnum += rounding;
	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)tempnum;
	double remainder = tempnum - (double)int_part;
	printString(itoa(int_part,ascii,10),useEfx);
	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) writeCharacter('.',getCursor(),true);
	while (digits-- > 0){
		remainder *= 10.0;
		int toPrint = int(remainder);
		printString(itoa(toPrint,ascii,10),useEfx);
		remainder -= toPrint; 
	}
	if (PRES_VALS == NONE)       {
	} else if (PRES_VALS == C)   {
		_writeCharacter(0x1B,getCursor(),true,useEfx);
	} else if (PRES_VALS == F)   {
		_writeCharacter(0x1C,getCursor(),true,useEfx);
	} else if (PRES_VALS == dB)  {
		if (useEfx) _efx(getCursor());
		printString("dB",useEfx);
	} else if (PRES_VALS == DEG) {
		_writeCharacter(0x08,getCursor(),true,useEfx);
	} else if (PRES_VALS == Hz)  {
		if (prefix >= 0 && prefix <= 4) {
			if (prefix < 4){
				_writeCharacter('K',getCursor(),true,useEfx);
			} else {
				_writeCharacter('M',getCursor(),true,useEfx);
			}
			_writeCharacter('H',getCursor(),true,useEfx);
			_writeCharacter('z',getCursor(),true,useEfx);
		}
	}
	if (_autoclean) _cleanGarbage();
}

inline size_t IntMatrixDisp::write(uint8_t value) {
  writeChar(value,true);
  return 1; // assume sucess
}

//public:useful for print db correctly
float IntMatrixDisp::byteToDb(uint8_t byteVal,float maxdbVal){
	return (float)(maxdbVal-(255.0 - (float)byteVal) / 2.0);
}

//private:the efx routine
void IntMatrixDisp::_efx(uint8_t digit,uint8_t mdly,char stopChar){
	byte limit = 27;
	uint8_t dly = 2;
	uint8_t i;
	char tchar;
	if (_softClear){
		limit = 11;
		dly = 10;
	}
	if (mdly > 0) {
		limit = 11;
		dly = mdly;
	}
	for (i = 0; i < limit; i++) {
		if (_softClear){
			tchar = tempCharB[i];
		} else {
			tchar = tempCharA[i];
		}
		writeCharacter(tchar,digit,false);
		delay(dly);
	}
}

//private:to avoid garbage
void IntMatrixDisp::_cleanGarbage(void){
	uint8_t digit = getCursor()+1;
	if (digit < _maxDigits-1){
		for (uint8_t i=digit;i<_maxDigits;i++){
			writeCharacter(' ',i,false);
		}
	}
}

//public:
void IntMatrixDisp::home(void) {
	_cursorPos = 0;
}

//public:
uint8_t IntMatrixDisp::getCursor(void) {
	return _cursorPos;
}

//public:
void IntMatrixDisp::setCursor(uint8_t position){
	if (position < _maxDigits-1){
		_cursorPos = position;
	}
}

//public:
void IntMatrixDisp::setAutoclean(bool autoclean){
	_autoclean = autoclean;
}

//public:
uint8_t IntMatrixDisp::getDigitsPerUnit(void) {
	return _digitPerUnit;
}

//public:
void IntMatrixDisp::scroll(char* testo,unsigned int speed) {
	uint8_t testolen = strlen(testo);//15
	if (testolen > 0){
		uint8_t i;
		clearAll();
		for (i=0;i<=(_maxDigits+testolen);i++){//main loop
			_scrollEngine(testo,testolen,i);
			delay(speed);
		}
		setCursor(0);
	}
} 

//private:
void IntMatrixDisp::_scrollEngine(char* testo,uint8_t lenght,uint8_t advance){
	uint8_t disp;
	uint8_t tindex;
	uint8_t pos = 0;
	char tchar;
	for (disp=0;disp<_maxDigits;disp++){//display loop feed all digits at once
		tindex = disp+advance;
		if ((tindex < _maxDigits) || (tindex - _maxDigits) >= lenght){
			tchar = 0x20;//space
		} else {
			tchar = testo[tindex - _maxDigits];
		}
		#if defined(INVERTED_ADDRESSING)
		writeCharacter(tchar,pos,false);
		#else
		writeCharacter(tchar,(_maxDigits-1)-pos,false);
		#endif
		if (pos < (_maxDigits-1)){
			pos++;
		} else {
			pos = 0;
		}
	}
}

void IntMatrixDisp::startSend(bool mode){
	//if (this->_comType == 1){
#if defined(__MK20DX128__) || defined(__MK20DX256__)
		::digitalWriteFast(this->_csPin, LOW);
#else
		::digitalWrite(this->_csPin, LOW);
#endif
		_sideDelay(_ssdelay);
		if (mode){//IN
			SPI.transfer(this->_read_cmd);
		} else {//OUT
			SPI.transfer(this->_write_cmd);
		}
/*		
	} else if (this->_comType == 2){//I2C
	} else {
#if defined(__MK20DX128__)
		::digitalWriteFast(this->_ltcPin,LOW);
#else
		::digitalWrite(this->_ltcPin,LOW);
#endif
*/
	//}
}

void IntMatrixDisp::endSend(){
	//if (this->_comType == 1){
#if defined(__MK20DX128__) || defined(__MK20DX256__)
		::digitalWriteFast(this->_csPin, HIGH);
#else
		::digitalWrite(this->_csPin, HIGH);
#endif
		_sideDelay(_ssdelay);
/*	
	} else if (this->_comType == 2){//I2C
	} else {
#if defined(__MK20DX128__)
		::digitalWriteFast(this->_ltcPin,HIGH);
#else
		::digitalWrite(this->_ltcPin,HIGH);
#endif
*/
	//}
}


void IntMatrixDisp::write_word(byte addr, word data){
	startSend(0);
	//if (this->_comType == 1){
		SPI.transfer(addr);
		SPI.transfer((byte)(data & 0x00FF));
		SPI.transfer((byte)(data>>8));
	/*	
	} else if (this->_comType == 2){//I2C
	} else {
		_fastShiftOut((byte)(data & 0x00FF));
		_fastShiftOut((byte)(data>>8));
	*/	
	//}
	endSend();
}




uint16_t IntMatrixDisp::byte2uint16(byte high_byte,byte low_byte){
	return (word)high_byte<<8 | (word)low_byte;
}


void IntMatrixDisp::gpioPinMode(bool mode){
	if (mode == INPUT){
		_gpioDirection = 0xFFFF;
	} else {
		_gpioDirection = 0x0000;
		_gpioState = 0x0000;
	}
	write_word(this->_IODIR,_gpioDirection);
}


void IntMatrixDisp::gpioPort(word value){
	_gpioState = value;
	write_word(this->_GPIO,_gpioState);
}

void IntMatrixDisp::gpioPort(byte lowByte, byte highByte){
	_gpioState = byte2uint16(highByte,lowByte);
	write_word(this->_GPIO,_gpioState);
}

void IntMatrixDisp::gpioPinMode(uint8_t pin, bool mode){
	if (pin < 16){
		if (mode == INPUT){
			bitSet(_gpioDirection,pin);
		} else {
			bitClear(_gpioDirection,pin);
		}
		write_word(this->_IODIR,_gpioDirection);
	}
}

/*
void IntMatrixDisp::_fastShiftOut(uint8_t val){
	uint8_t i;
	for (i = 0; i < 2; i++)  {
		#if defined(__MK20DX128__)
		::digitalWriteFast(this->_dtaPin, !!(val & (1 << (7 - i))));
		::digitalWriteFast(this->_clkPin, HIGH);
		::digitalWriteFast(this->_clkPin, LOW);
		#else
		::digitalWrite(this->_dtaPin, !!(val & (1 << (7 - i))));
		::digitalWrite(this->_clkPin, HIGH);
		::digitalWrite(this->_clkPin, LOW);
		#endif
	}
}
*/

void IntMatrixDisp::_sideDelay(uint8_t val){
	if (val > 0){
		delayMicroseconds(val);
	}
}
/*  void IntMatrixDisp::printByte(byte data){
	Serial.println("-+-+-+-+-+-+-+-+-++-+-+-+-+-+");
  for (int i=7; i>=0; i--){
    if (bitRead(data,i)==1){
      Serial.print("1");
    } 
    else {
      Serial.print("0");
    }
  }
  Serial.print(" -> 0x");
  Serial.print(data,HEX);
  Serial.print("\n-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
} */
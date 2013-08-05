Intelligent Matrix Display Library
===================================

An 'universal' Arduino/Teensy3 library for all 'Intelligent Led Matrix Displays' that use just 2/3 wires.

Intelligent Led Matrix Displays was mostly used in expensive apparatus during 80/90's and I love to use in my stuff because they are beautiful and kinda 'sexy'.
They are/was expensive and even now you still can find some new model for a high price but there's a lot of NOS in ebay and it's no rare find them in dismantled stuff. They are 'intelligent' but still use a lot of pin, too much for tiny micros, so I build a library that use 2 or 3 pins max for 2 char to 32 char!
Library use a popular and cheap microchip GPIO expander called MCP23S17 (or I2C version MCP23017) that I choosed for his ability
to use a special feature called HAEN that permit use 8 chip on same SPI lines (included CS).
Good thing about these displays is that you have to connect pins of all units in parallel except one, the addressing pin (if you have 2 or more addressing pin one should be tied to ground). I'm using a proprietary addressing method in library that simplify a lot connections and wiring so follow documentation inside the .h file.
Some of these displays can have extended function that provide brightness control and some other thing, I've implemented
where it's possible but for the other units take care about consumption! They use tons of tiny leds and can such a lot of current!
Those chips provide a 'blank' pin that can turn off/on display but don't touch the unit memory or content, for fast processor like Teensy 3 it's possible apply a frequency to this pin to save current but if you want to save processing power justlook at any of these displays datasheet for a simple 555 oscillator applied to this pin.
At the moment I have tested with these displays:

	DLR3416
	DLO3416
	DLG3416
	DLR2416
	DLO2416
	DLG2416
	HDLA2416
	HDLS2416
	HDLO2416
	HDLG2416
	HPDL1414
	DL3416
	DL2416
	
For just 4 units (each unit normally has 4 char) it's possible drive directly from the MCP gpio but for more units you
will need an extra chip connected to the MCP that can be 74HC138 for 8 units till 74HC4551 or others for 16 units, in library I provided the code needed for some different ones.
If you look at those displays pdf they provide a way to enable 4 chip with 3 lines but if you want to use this library please ignore! To enable a chip those CE lines should be low so you have to put one of them low as default, the other is controlled by the library (see wiki page).

Now some wiring (look at wiki page for better view)

MCP23S17 

      [A0]      -> [|--U--|] <- [CUE]
      [A1]      -> [|     |] <- [DATA6]
      [CE1]     -> [|     |] <- [DATA5]
      [CE2]     -> [|     |] <- [DATA4]
      [CE3]     -> [|     |] <- [DATA3]
      [CE4]     -> [|     |] <- [DATA2]
      [CU]      -> [|     |] <- [DATA1]
      [WRITE]   -> [|     |] <- [DATA0]
      +5V       -> [|     |] <- nc
      ground    -> [|     |] <- nc
      cs (10)   -> [|     |] <- ----####(4k7k)---+5v (sometimes bypass resistor is better)
      sck (13)  -> [|     |] <- SPI address A2 (4k7 resistor to 5V == 1, gnd = 0)
      mosi (12) -> [|     |] <- SPI address A1 (4k7 resistor to 5V == 1, gnd = 0)
      miso (nc) -> [|_____|] <- SPI address A0 (4k7 resistor to 5V == 1, gnd = 0)
      
Displays have all pins connected together but not one of the chip address that have to be tied to one of the CEx of
MCP chip or in case you would use more than 4 units to the addressing chip, check below:

ADDRESSING CHIP WIRING

_DIRECT wiring (max 4 displays)------------------------------

    mcp pin3 [CE1]  		->		  display 1	
    mcp pin4 [CE2]  		->		  display 2			
    mcp pin5 [CE3]  		->		  display 3	
    mcp pin6 [CE4]  		->		  display 4		
				
_74HC138 wiring (max 8 displays)-----------------------------

    mcp pin3 [CE1]  -> [|--U--|] <- +5v
    mcp pin4 [CE2]  -> [|     |] <- display 1
    mcp pin5 [CE3]  -> [|     |] <- display 2
        gnd         -> [|     |] <- display 3
        gnd         -> [|     |] <- display 4
        +5V         -> [|     |] <- display 5
        display 8   -> [|     |] <- display 6
        gnd         -> [|_____|] <- display 7
        
Check the .h file for more documentation and don't forget to download the datasheet of your matrix display before all!
Note for Teensy3 users. All those displays works only at 5V so you will need a unidirectional level converter (I'm using 
only 3 wires for the GPIO).

Features:

 - Use just 2/3 microcontroller PIN
 - In SPI use HAEN that allow you share pins with other chips (max 8), CE included!
 - It use just one GPIO for 4 to 16 units (more than 4 will use an additional chip connected to GPIO)
 - Drive many types of displays.
 - It uses the extra features of some chip.
 - Can work with other library I made that uses GPIO (like NewLiquid one)

 
What is working?

Tested all displays with Arduino and Teensy3 with the MCP23s17 chip using HAEN (was on the same line of another MCP23s8 
that was using HAEN and both worked perfect).
Tested a 74HC138 for addressing 8 displays with a single GPIO and worked.

To Do?!?

I need to fix the I2C that it's working but I don't want to include all the time inside library.
Separate the low level calls to gpio to use just one library for all gpio chips without multiple includes.
Add more addressing chips for more addressable units.
Lots of optimizations


What I will not do

Use shift register! they are much like SPI but I cannot share the pins so it's a waste of time and pins.


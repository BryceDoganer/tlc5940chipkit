/******************************************************************************
LED Cube TLC5940 library made for Digilent chipKit microcontrollers.

	This library is made possible by "ColinHarrington" who has done the 
grunt work in making this library possible with the TLC5940 which is 
based on the TLC5940 library for Arduino.

	The architecture between the ATMega (Arduino) & PIC32 (chipKit) is very 
different and porting a library from one to the other is not an easy task.

*Websites where information regarding the chipKit TLC5940 library can be found:
http://www.heath-bar.com/blog/?p=128
https://github.com/ColinHarrington/tlc5940chipkit/

*TLC5940 Data Sheet: (Very Important)
http://www.ti.com/lit/ds/symlink/tlc5940.pdf   

*Extra Information:
http://playground.arduino.cc/learning/TLC5940
******************************************************************************/


// TODO finish hardware write up. 
// Add in MOSFETS and talk about external power supply and de-coupling capacitors

/****************************************************************************** 
   ~*~ This hardware setup diagram was modified from the "BasicUse" sketch
       for the TLC5940 Arduino Library ~*~
 
    Basic Pin setup:
         chipKit									   TLC5940
     ----------------                                  ---u----
               39  13|-> SCLK (pin 25)           OUT1 |1     28|OUT channel 0
               38  12|                           OUT2 |2     27|-> VPRG   (GND)
         XER <-37  11|-> SIN (pin 26)            OUT3 |3     26|-> SIN    (pin 11)
        VPRG <-36  10|-> BLANK (pin 23)          OUT4 |4     25|-> SCLK   (pin 13)
       DCPRG <-35   9|-> XLAT (pin 24)           OUT5 |5     24|-> XLAT   (pin 9)
               34   8|                           OUT6 |6     23|-> BLANK  (pin 10)
     Layer 7 <-33   7|                           OUT7 |7     22|-> GND
     Layer 6 <-32   6|                           OUT8 |8     21|-> VCC    (+3.3V)
     Layer 5 <-31   5|                           OUT9 |9     20|-> 2K Resistor -> GND
     Layer 4 <-30   4|                           OUT10|10    19|-> +3.3V  (DCPRG)
     Layer 3 <-29   3|-> GSCLK (pin 18)          OUT11|11    18|-> GSCLK  (pin 3)
     Layer 2 <-28   2|                           OUT12|12    17|-> SOUT 
     Layer 1 <-27   1|                           OUT13|13    16|-> XERR   ()
     Layer 0 <-26   0|                           OUT14|14    15|OUT channel 15
    -----------------                                  --------

    -  Put the longer leg (anode) of the LEDs in the +5V and the shorter leg
         (cathode) in OUT(0-15).
    -  +3.3V from chipKit -> TLC pin 21 and 19     (VCC and DCPRG)
    -  GND from Arduino   -> TLC pin 22 and 27     (GND and VPRG)
    -  digital 3          -> TLC pin 18            (GSCLK)
    -  digital 9          -> TLC pin 24            (XLAT)
    -  digital 10         -> TLC pin 23            (BLANK)
    -  digital 11         -> TLC pin 26            (SIN)
    -  digital 13         -> TLC pin 25            (SCLK)
    -  The 2K resistor between TLC pin 20 and GND will let ~20mA through each
       LED.  To be precise, it's I = 39.06 / R (in ohms).  This doesn't depend
       on the LED driving voltage.
    - (Optional): put a pull-up resistor (~10k) between +5V and BLANK so that
                  all the LEDs will turn off when the Arduino is reset.

    If you are daisy-chaining more than one TLC, connect the SOUT of the first
    TLC to the SIN of the next.  All the other pins should just be connected
    together:
        BLANK on chipKit -> BLANK of TLC1 -> BLANK of TLC2 -> ...
        XLAT on chipKit -> XLAT of TLC1  -> XLAT of TLC2  -> ...
    The one exception is that each TLC needs it's own resistor between pin 20
    and GND.
******************************************************************************/


// Size of LED cube. Defines the value for the X,Y and Z axis
// TODO: I believe this currently only works with multiples of 4
#ifndef CUBE_SIZE
	#define CUBE_SIZE	8
#endif

// Specify the number of TLC5940 chips that are connected
#ifndef NUM_TLCS
	#define NUM_TLCS	12
#endif

#define NUM_CHANNELS  NUM_TLCS * 16 

#define LAYER_GSDATA NUM_TLCS * 6

#define GSDATA_SIZE CUBE_SIZE * LAYER_GSDATA // 6 * 32bit / 12bit = 16 channels per TLC

// Is the cube made of RGB LEDs or Single Color?
// Use this to include/exclude the RGB helper functions
#ifndef RGB_LEDS
	#define RGB_LEDS   1
#endif

#if RGB_LEDS

    // Number of colors in each LED
	#define LED_SIZE   3

	// When setting RGB values this number is useful
    #define RGB_CHANNELS  NUM_CHANNELS / LED_SIZE

    // Turn this on or off to include to spectrum helper functions
    #define RGB_SPECTRUM 1 

    #if RGB_SPECTRUM
        // Creates a 2D Array to keep track of spectrum data
        // for animation objects
        #define SPECTRUM_DATA_ARRAYS 2
        #define SPECTRUM_DATA_VALUES 10
    #endif

    #if RGB_SPECTRUM
        #define NUM_COLORS 12288 
    #endif

#else
	// Single color LEDs are size of 1
	#define LED_SIZE    1

#endif

// Bit-bang using any two i/o pins
#define TLC_BITBANG			0

// Use the much faster hardware SPI module
#define TLC_SPI            1

#ifndef TLC_SPI_PRESCALER_FLAGS
	#define TLC_SPI_PRESCALER_FLAGS PRI_PRESCAL_4_1|SEC_PRESCAL_4_1
#endif

/** Determines how data should be transfered to the TLCs.  Bit-banging can use
    any two i/o pins, but the hardware SPI is faster.
    - Bit-Bang = TLC_BITBANG --> *NOT YET WORKING* 
    - Hardware SPI = TLC_SPI (default) */
#ifndef DATA_TRANSFER_MODE
	#define DATA_TRANSFER_MODE TLC_SPI
#endif

// Defines whether or not you will be able to set Dot Correction
// The TLC5940 defaults to all channels at 100% if you decide to not set Dot Correction
#ifndef VPRG_ENABLED
	#define VPRG_ENABLED  1
#endif

/** XERR is not yet implemented */
#ifndef XERR_ENABLED
	#define XERR_ENABLED 0
#endif
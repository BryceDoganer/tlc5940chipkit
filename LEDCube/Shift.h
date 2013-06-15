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

#ifndef SHIFT_H
#define SHIFT_H
#include "Draw.h"
#include "LEDCube.h"


class Shift
{
	public:
		// void shiftCubeX(int direction);
		// void shiftCubeY(int direction);
		// void shiftCubeZ(int direction);

	#if RGB_LEDS // RGB Functions

	#else // Mono LED Functions


	#endif



	private:


};

// for the preinstantiated Shift variable.
extern Shift ShiftCube;

#endif
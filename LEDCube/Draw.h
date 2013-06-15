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

#ifndef DRAW_H
#define DRAW_H
#include "LEDCube.h"
#include <math.h>


class Draw
{
	public:
		void clearAll(void);
		
		int channel(int x, int y);
		unsigned char coordOutOfRange(int x, int y, int z);
		unsigned char intensityOutOfRange(int intensity);

		// TODO Add in funcationality to work with RGB and Mono LEDs
		void clearPlaneX(int x);
		void clearPlaneY(int y);
		void clearPlaneZ(int z);
		//


		void shiftCubeX(int direction);
		void shiftCubeY(int direction);
		void shiftCubeZ(int direction);

	#if RGB_LEDS // RGB Functions
		void setRGBVoxel(int x, int y, int z, int red, int green, int blue);
		void clearRGBVoxel(int x, int y, int z);
		void setRGBVoxelSpectrum(int x, int y, int z, int spectrum);
		void setRGBSpectrumForLayer(int layer, int spectrum);
		void setRGBSpectrumAll(int spectrum);
		int getRGBVoxelSpectrum(int x, int y, int z);
		void increaseRGBSpectrum(int x, int y, int z, int amount);
		void increaseRGBSpectrumForLayer(int layer, int amount);
		void increaseRGBSpectrumAll(int amount);
		unsigned char spectrumOutOfRange(int spectrum);
		unsigned char outOfRGBSpectrum(int red, int green, int blue);
		int spectrumFromRGB(int red, int green, int blue);
		int reduceRGBToSpectrum(int red, int green, int blue);
		int RGBChannel(int x, int y);
		unsigned char RGBIntensityOutOfRange(int red, int green, int blue);
		void drawRGBLine(int x1, int y1, int z1, int x2, int y2, int z2, int red, int green, int blue);
		void drawLineRGBCube(int x, int y, int z, int orientation, int size, int red, int green, int blue);
		void drawFillRGBCube(int x, int y, int z, int orientation, int size, int red, int green, int blue);
		void drawLineRGBBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int red, int green, int blue);
		void drawFillRGBBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int red, int green, int blue);
		void setRGBPlaneX(int x, int red, int green, int blue);
		void setRGBPlaneY(int y, int red, int green, int blue);
		void setRGBPlaneZ(int z, int red, int green, int blue);
	#else // Mono LED Functions


	#endif



	private:


};

// for the preinstantiated Draw variable.
extern Draw DrawCube;

#endif
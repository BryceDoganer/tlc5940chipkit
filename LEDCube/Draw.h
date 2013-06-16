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
// #include <cmath>


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
		int RGBChannel(int x, int y);
		unsigned char RGBIntensityOutOfRange(int red, int green, int blue);
		void setRGBVoxel(int x, int y, int z, int red, int green, int blue);
		void clearRGBVoxel(int x, int y, int z);
		void drawRGBPlaneLineX(int y, int z, int red, int green, int blue);
		void drawRGBPlaneLineY(int x, int z, int red, int green, int blue);
		void drawRGBPlaneLineZ(int x, int y, int red, int green, int blue);
		void drawRGBCrosshair(int x, int y, int z, int red, int green, int blue);
		void setRGBPlaneX(int x, int red, int green, int blue);
		void setRGBPlaneY(int y, int red, int green, int blue);
		void setRGBPlaneZ(int z, int red, int green, int blue);
		void drawRGBLine(int x1, int y1, int z1, int x2, int y2, int z2, int red, int green, int blue);
		void drawRGBCube(int x, int y, int z, int size, int orientation, int fill, int red, int green, int blue);
		void drawRGBBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int fill, int red, int green, int blue);
		
		#if RGB_SPECTRUM // RGB Spectrum functions
			int getRedSpectrum(int spectrum);
			int getGreenSpectrum(int spectrum);
			int getBlueSpectrum(int spectrum);
			unsigned char spectrumOutOfRange(int spectrum);
			void setVoxelSpectrum(int x, int y, int z, int spectrum);
			void setSpectrumForLayer(int layer, int spectrum);
			void setSpectrumAll(int spectrum);
			void setSpectrumValue(int array, int value, int amount);
			void setSpectrumArray(int array, int amount);
			int getSpectrumValue(int array, int value);
			void increaseSpectrumValue(int array, int value, int amount);
			void increaseSpectrumArray(int layer, int amount);
			void increaseSpectrumAll(int amount);
			void drawSpectrumPlaneLineX(int y, int z, int spectrum);
			void drawSpectrumPlaneLineY(int x, int z, int spectrum);
			void drawSpectrumPlaneLineZ(int x, int y, int spectrum);
			void drawSpectrumCrosshair(int x, int y, int z, int spectrum);
			void setSpectrumPlaneX(int x, int spectrum);
			void setSpectrumPlaneY(int y, int spectrum);
			void setSpectrumPlaneZ(int z, int spectrum);
			void drawSpectrumLine(int x1, int y1, int z1, int x2, int y2, int z2, int spectrum);
			void drawSpectrumCube(int x, int y, int z, int size, int orientation, int fill, int spectrum);
			void drawSpectrumBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int fill, int spectrum);
		#endif
	#else // Mono LED Functions


	#endif



	private:


};

// for the preinstantiated Draw variable.
extern Draw DrawCube;

#endif
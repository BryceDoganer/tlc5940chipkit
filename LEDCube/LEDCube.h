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

#ifndef LEDCUBE_H
#define LEDCUBE_H
#include <LEDCube_config.h>
#include <stdint.h>
#include <math.h>

//extern unsigned int tlc_GSData[NUM_TLCS * 6];

class LEDCube
{
  public:
	void init(int initialValue = 0);
	void clearAll(void);
	int clearLayer(int layer);
	int update(void);
	int startUpdate(void);
	void finishUpdate(void);
	int getCurrentLayer(void);
	int getNextLayer(void);
	void stepLayer(void);
	void set(int layer, int channel, int value);
	void setAll(int value);
	int getNumTLCs();
	int get(int layer, int channel);
	int updateInProgress(void);

#if RGB_LEDS
	void setAllRGB(int red, int green, int blue);
	void setAllRGBOnLayer(int layer, int red, int green, int blue);
	void setRGB(int layer, int channel, int r, int g, int b);
	void setRGB2(int layer, int channel, int r, int g, int b);
	int getRed(int layer, int channel);
	int getGreen(int layer, int channel);
	int getBlue(int layer, int channel);
#endif
	
#if VPRG_ENABLED
	void setAllDC(int value);
	void setDC(int channel, int value);
	int getDC(int channel);
	int updateDC();
	uint8_t* getDCData();
    //void setAllDC(uint8_t r, uint8_t g, uint8_t b);
    //void setAllDCtest(uint8_t r, uint8_t g, uint8_t b);
#endif

#if XERR_ENABLED
    uint8_t readXERR(void);
#endif

  private:
	void request_xlat_pulse();

};

// for the preinstantiated Cube variable.
extern LEDCube Cube;

#endif
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

#include <LEDCube_config.h>
#include "LEDCube.h"
#include <plib.h>


/** Macros to work with pins */
#define pulse_pin(port, pin)	port |= pin; port &= ~pin
#define setLow(port, pin)		port &= ~pin
#define setHigh(port, pin)		port |= pin
#define outputState(port, pin)	port & pin

/** Chipkit Ports **/           // chipKitUno/uC32 Pins:
#define VPRG 0x40               // pin 36 
#define VPRG_PORT PORTD

#define DCPRG 0x800             // pin 35
#define DCPRG_PORT PORTD

#define XERR 0x80               // pin 37
#define XERR_PORT PORTD

#define SOUT 0x100              // pin 11 
#define SOUT_PORT PORTG

#define SCLK 0x40               // pin 13
#define SCLK_PORT PORTG
 
#define GSCLK 0x1               // pin 3
#define GSCLK_PORT PORTD

#define BLANK 0x20              // pin 10
#define BLANK_PORT PORTD

#define XLAT 0x8                // pin 9
#define XLAT_PORT PORTD

#define LAYER0 0x01             // pin 26
#define LAYER0_TRIS TRISE
#define LAYER0_PORT PORTE

#define LAYER1 0x02             // pin 27
#define LAYER1_TRIS TRISE
#define LAYER1_PORT PORTE

#define LAYER2 0x04             // pin 28
#define LAYER2_TRIS TRISE
#define LAYER2_PORT PORTE

#define LAYER3 0x08				// pin 29
#define LAYER3_TRIS TRISE
#define LAYER3_PORT PORTE

#define LAYER4 0x10             // pin 30        
#define LAYER4_TRIS TRISE
#define LAYER4_PORT PORTE

#define LAYER5 0x20             // pin 31
#define LAYER5_TRIS TRISE
#define LAYER5_PORT PORTE

#define LAYER6 0x40             // pin 32
#define LAYER6_TRIS TRISE
#define LAYER6_PORT PORTE

#define LAYER7 0x80             // pin 33
#define LAYER7_TRIS TRISE
#define LAYER7_PORT PORTE

#define HIGHEST_LAYER LAYER7
#define HIGHEST_LAYER_TRIS LAYER7_TRIS
#define HIGHEST_LAYER_PORT LAYER7_PORT

/** Packed grayscale data, 24 bytes (16 * 12 bits) per TLC.

    Format: Lets assume we have 2 TLCs, A and B, daisy-chained with the SOUT of
    A going into the SIN of B.
    - byte 0: upper 8 bits of B.15
    - byte 1: lower 4 bits of B.15 and upper 4 bits of B.14
    - byte 2: lower 8 bits of B.0
    - ...
    - byte 24: upper 8 bits of A.15
    - byte 25: lower 4 bits of A.15 and upper 4 bits of A.14
    - ...
    - byte 47: lower 8 bits of A.0

    \note Normally packing data like this is bad practice.  But in this
          situation, shifting the data out is really fast because the format of
          the array is the same as the format of the TLC's serial interface. */


// Creates a 2D Array which will hold all the required values of every LED 
// Each layer holds the required number of bits for the TLC5940's  
unsigned int cube_GSData[CUBE_SIZE][NUM_TLCS * 6]; // 6 * 32 = 192 bits = 16x 12bit values

#if RGB_LEDS
	unsigned int cube_colorData[576]; // 576 * 32 = 18,432 / 36 Bits per color = 512 colors 
#endif

#if VPRG_ENABLED
	/** Packed Dot Correction data. Packed similarly to GSData. Using an 8 bit uint because it's easier to pack. */
	uint8_t tlc_DCData[NUM_TLCS * 12];
#endif 

// This keeps track of the current layer the cube is displaying 
uint8_t currentLayer = 0;

/** This will be true (!= 0) if update was just called and the data has not
    been latched in yet. */
volatile uint8_t cube_needXLAT;

/** Some of the extened library will need to be called after a successful
    update. */
volatile void (*tlc_onUpdateFinished)(void);


/** Returns > 0 if an update is currently in progress; else 0 */
int LEDCube::updateInProgress() 
{
	return cube_needXLAT;
}

void LEDCube::init(int initialValue)
{
	//Setting Directionality of ports	
	TRISDCLR = VPRG;
	TRISDCLR = DCPRG;
	TRISDSET = XERR;
	TRISGCLR = SOUT;
	TRISGCLR = SCLK;
	TRISDCLR = GSCLK;
	TRISDCLR = BLANK;
	TRISDCLR = XLAT;

    PORTD &= ~(VPRG); // Pull VPRG Low
    PORTD |= DCPRG;   // Pull DCPRG High

    TRISE |= 0xFF;
    PORTE |= 0xFF;



	#if DATA_TRANSFER_MODE == TLC_BITBANG

	#elif DATA_TRANSFER_MODE == TLC_SPI
	//Setting up SPI
	OpenSPI2(
			SPI_MODE32_ON
			| MASTER_ENABLE_ON 
			| SPI_CKE_ON 
			| TLC_SPI_PRESCALER_FLAGS 
			| FRAME_ENABLE_OFF, 
		SPI_ENABLE);
	#endif

	// Start the timer for GSCLK
	OpenTimer2(T2_ON | T2_PS_1_4, 0x3);

	// Start the timer for BLANK/XLAT
	OpenTimer3(T3_ON | T3_PS_1_16, 0x1003);

	// Start the OC for BLANK
	OpenOC5(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0x3, 0x0);

	// Start the OC for XLAT (but set the pulse time out of range, so that it doesn't trigger the pin)
	OpenOC4(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0xFFFF, 0xFFFF);

	//Interrupt for XLAT
	ConfigIntOC4(OC_INT_ON | OC_INT_PRIOR_3 | OC_INT_SUB_PRI_3);

	setAll(initialValue);

	// Init the layer pins
	stepLayer(); 

	update();

	// Wait for the first update to be sent to the TLC before starting GSCLK
	// so that the board doesn't start with random values
	while(cube_needXLAT);

	// Start the OC for GSCLK
	OpenOC1(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0x1, 0x1);	

}

// Clears Data array. Call call upate() to clear on cube.
void LEDCube::clearAll(void)
{
	for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
		for(int _data = 0; _data < (NUM_TLCS * 6); _data++) {
			cube_GSData[_layer][_data] = 0x0;
		}
	}
}

int LEDCube::clearLayer(int layer)
{
    if((layer < 0) || (layer >= CUBE_SIZE)) return 0;

    for(int _data = 0; _data < (NUM_TLCS * 6); _data++) {
		cube_GSData[layer][_data] = 0x0;
	}

	return 1;
}

#if DATA_TRANSFER_MODE == TLC_BITBANG

// THIS FUNCTION IS CURRENTLY A BUCKET OF FAAAAAAAAAIL
int LEDCube::update(void)
{

	for(int i = 0; i < (NUM_TLCS * 6); i++) {
		for(int s = 31; s >= 0; s--)
		{
			if(cube_GSData[currentLayer][i] >> s & 0x1) {
				PORTGSET |= SOUT;
			} else {
				PORTGCLR |= SOUT;
			}

			pulse_pin(SCLK_PORT, SCLK);
		}
	}

	request_xlat_pulse();

	return 0;
}

#elif DATA_TRANSFER_MODE == TLC_SPI

int LEDCube::update(void)
{
	// We CANNOT use SOUT/SCLK while XLAT is high - tampering with the data while it's being latched is a BAD idea
	if (cube_needXLAT){
		return 1; 
	}
	pulse_pin(SCLK_PORT, SCLK);

	//TODO use Interrupt driven SPI for a non-blocking performance boost - this could get tricky when mixed with DC updates
	putsSPI2(6 * NUM_TLCS, cube_GSData[currentLayer]);

	// Wait for buffers to be emptied
	while(SpiChnIsBusy(SPI_CHANNEL2));

	request_xlat_pulse();

	return 0;
}

int LEDCube::startUpdate(void)
{
	// We CANNOT use SOUT/SCLK while XLAT is high - tampering with the data while it's being latched is a BAD idea
	if (cube_needXLAT){
		return 1; 
	}
	pulse_pin(SCLK_PORT, SCLK);

	//TODO use Interrupt driven SPI for a non-blocking performance boost - this could get tricky when mixed with DC updates
	putsSPI2(6 * NUM_TLCS, cube_GSData[currentLayer]);

	return 0;
}

void LEDCube::finishUpdate(void)
{
	// Wait for buffers to be emptied
	while(SpiChnIsBusy(SPI_CHANNEL2));

	//stepLayerFlag = 1;

	request_xlat_pulse();

	// Also added for simplified code
	stepLayer();
}

#endif
// End of Data XFER TLC_SPI


int LEDCube::getCurrentLayer(void) {
	return currentLayer;
}

int LEDCube::getNextLayer(void) {

	int nextLayer = currentLayer + 1;

	if(nextLayer == CUBE_SIZE) nextLayer = 0;

	return nextLayer;
}

void LEDCube::stepLayer(void)
{
   switch (currentLayer) {
      case 0:
      	HIGHEST_LAYER_TRIS |= HIGHEST_LAYER; // Turn Last layer to input
      	HIGHEST_LAYER_PORT |= HIGHEST_LAYER; // Pull last layer high

      	while(cube_needXLAT) { };

        LAYER0_TRIS &= ~(LAYER0); // Turn the first layer pin(26) to an output 
        LAYER0_PORT &= ~(LAYER0); // Pull first layer pin low
        break;
      case 1:
      	LAYER0_TRIS |= LAYER0;
      	LAYER0_PORT |= LAYER0;

      	while(cube_needXLAT) { };

        LAYER1_TRIS &= ~(LAYER1); 
        LAYER1_PORT &= ~(LAYER1);
        break;
      case 2:
        LAYER1_TRIS |= LAYER1;
      	LAYER1_PORT |= LAYER1;

      	while(cube_needXLAT) { };

        LAYER2_TRIS &= ~(LAYER2);
        LAYER2_PORT &= ~(LAYER2);
        break;
      case 3:
        LAYER2_TRIS |= LAYER2;
      	LAYER2_PORT |= LAYER2;

      	while(cube_needXLAT) { };

        LAYER3_TRIS &= ~(LAYER3);
        LAYER3_PORT &= ~(LAYER3); 
        break;
      case 4:
        LAYER3_TRIS |= LAYER3;
      	LAYER3_PORT |= LAYER3;

      	while(cube_needXLAT) { };

        LAYER4_TRIS &= ~(LAYER4);
        LAYER4_PORT &= ~(LAYER4);
        break;
      case 5:
        LAYER4_TRIS |= LAYER4;
      	LAYER4_PORT |= LAYER4;

      	while(cube_needXLAT) { };

        LAYER5_TRIS &= ~(LAYER5);
        LAYER5_PORT &= ~(LAYER5);
        break;
      case 6:
        LAYER5_TRIS |= LAYER5;
      	LAYER5_PORT |= LAYER5;

      	while(cube_needXLAT) { };

        LAYER6_TRIS &= ~(LAYER6);
        LAYER6_PORT &= ~(LAYER6);
        break;
      case 7:
        LAYER6_TRIS |= LAYER6;
      	LAYER6_PORT |= LAYER6;

      	while(cube_needXLAT) { };

        LAYER7_TRIS &= ~(LAYER7);
        LAYER7_PORT &= ~(LAYER7);
        break;
    }

    currentLayer++;
    if(currentLayer == CUBE_SIZE) currentLayer = 0;


  // TRISE |= 0xFF; // Turn layer pins to inputs
  // PORTE |= 0xFF; // Pull layer pins high

  // while(cube_needXLAT) { };
  
  //  switch (currentLayer) {
  //     case 0:
  //       TRISE &= ~(0x01); // Turn the first layer pin to an output 
  //       PORTE &= ~(0x01); // Pull first layer ping low
  //       break;
  //     case 1:
  //       TRISE &= ~(0x02); // Turn the second layer pin to an output 
  //       PORTE &= ~(0x02); // Pull second layer ping low
  //       break;
  //     case 2:
  //       TRISE &= ~(0x04); // Turn the third layer pin to an output 
  //       PORTE &= ~(0x04); // Pull third layer ping low
  //       break;
  //     case 3:
  //       TRISE &= ~(0x08); // Turn the fourth layer pin to an output 
  //       PORTE &= ~(0x08); // Pull fourth layer ping low
  //       break;
  //     case 4:
  //       TRISE &= ~(0x10); // Turn the fifth layer pin to an output 
  //       PORTE &= ~(0x10); // Pull fifth layer ping low
  //       break;
  //     case 5:
  //       TRISE &= ~(0x20); // Turn the sixth layer pin to an output 
  //       PORTE &= ~(0x20); // Pull sixth layer ping low
  //       break;
  //     case 6:
  //       TRISE &= ~(0x40); // Turn the seventh layer pin to an output 
  //       PORTE &= ~(0x40); // Pull seventh layer ping low
  //       break;
  //     case 7:
  //       TRISE &= ~(0x80); // Turn the eighth layer pin to an output 
  //       PORTE &= ~(0x80); // Pull eighth layer ping low
  //       break;
  //   }
    
  // currentLayer++;
  // if(currentLayer == CUBE_SIZE) currentLayer = 0;
 
} // End of stepLayer()



/** Sets channel to value in the grayscale data array, #cube_GSData.
    \param channel (0 to #NUM_TLCS * 16 - 1).  OUT0 of the first TLC is
           channel 0, OUT0 of the next TLC is channel 16, etc.
    \param value (0-4095).  The grayscale value, 4095 is maximum.
    \see get */
void LEDCube::set(int layer, int channel, int value)
{
	if ((layer < 0) || (layer >= CUBE_SIZE)) return;
	if ((channel < 0) || (channel >= NUM_TLCS*16)) return;
	if ((value < 0) || (value > 4095)) return;

	// Data is packed into cube_GSData as pictured below. 
	// Each letter represents 4 bits and the last channel is stored first. 
	// So |A|A|A| would be the 12-bit value of the last TLC channel
	// 
	// As picture below, there are 8 possible positions for the 12-bit value to be stored in the array (Cases A-H)
	//	               ________________________________________________
	//  Channel data: |A|A|A|B|B|B|C|C|C|D|D|D|E|E|E|F|F|F|G|G|G|H|H|H|
	// 32-bit borders |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|



	// Since the last channel is stored first in the array, index32 is used as the inverse of channel
	// i.e. assuming NUM_TLCs = 1, when channel = 15, then index32 = 0 and when channel = 0, then index32 = 15
	unsigned int index32 = (NUM_TLCS * 16 - 1) - channel;

	// index12p = index32 * 3 / 8 = the index into cube_GSData where a 32-bit elment exists that will hold our 12-bit value
	unsigned int *index12p = cube_GSData[layer] + ((index32 * 3) >> 3);

	// caseNum  = index32 mod 8 = which of the 8 possible cases we need to handle
	int caseNum = index32 % 8;

	switch(caseNum){
		case 0:	// A: 12 bits start at the beginning of the element

			// zero out the 12 bits and OR those bits with the value
			*index12p = (*index12p & 0x000FFFFF) | value << 20;
			break;
		case 1:	// B: 12 bits start 12 bits into the element

			// zero out the 12 bits and OR those bits with the value
			*index12p = (*index12p & 0xFFF000FF) | value << 8;
			break;
		case 2:	// C: 12 bits start 24 bits into the element and continue 4 bits into the next

			// zero out the 8 bits that are in this element and OR them with the 8 MSB bits of value
			*index12p = (*index12p & 0xFFFFFF00) | value >> 4;

			// move to the next element, and zero out the remaining 4 bits, then OR them with the remaining 4 bits of value
			index12p++;
			*index12p = (*index12p & 0x0FFFFFFF) | value << 28;
			break;
		case 3:	// D: 12 bits start 4 bits into the element

			// zero out the 12 bits and OR those bits with the value
			*index12p = (*index12p & 0xF000FFFF) | value << 16; 			
			break;
		case 4: // E: 12 bits start 16 bits into the element

			// zero out the 12 bits and OR those bits with the value
			*index12p = (*index12p & 0xFFFF000F) | value << 4;
			break;
		case 5:	// F: 12 bits start 28 bits into the element and continue 8 bits into the next

			// set the 4 bits that apply to this element
			*index12p = (*index12p & 0xFFFFFFF0) | value >> 8;

			// move to the next element, and set the appropriate bits
			index12p++;
			*index12p = (*index12p & 0x00FFFFFF) | value << 24;
			break;
		case 6:	// G: 12 bits start 8 bits into the element
			*index12p = (*index12p & 0xFF000FFF) | value << 12;
			break;
		case 7:	// H: 12 bits start 20 bits into the element
			*index12p = (*index12p & 0xFFFFF000) | value;
			break;
	}
}

// There is most certainly a faster way to do this, but this will work for now
void LEDCube::setAll(int value){
	for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
		for (int _data = 0; _data < NUM_TLCS * 6; _data++){
		       set(_layer, _data, value);
		}
	}
}

int LEDCube::getNumTLCs() {
	return NUM_TLCS;
}


/** The logic here is almost identical to the set function which is well documented */
int LEDCube::get(int layer, int channel) {

	unsigned int index32 = (NUM_TLCS * 16 - 1) - channel;
	unsigned int *index12p = cube_GSData[layer] + ((index32 * 3) >> 3);
	int caseNum = index32 % 8;
	int value = 0x000;

	switch(caseNum){
		case 0:
			value |= ((*index12p >> 20) & 0xFFF);
			break;
		case 1:
			value |= ((*index12p >> 8) & 0xFFF);
			break;
		case 2:
			value |= ((*index12p << 4) & 0xFF0);
			index12p++;
			value |= ((*index12p >> 28) & 0xF);
			break;
		case 3:
			value |= ((*index12p >> 16) & 0xFFF);
			break;
		case 4:
			value |= ((*index12p >> 4) & 0xFFF);
			break;
		case 5:
			value |= ((*index12p << 8) & 0xF00);
			index12p++;
			value |= ((*index12p >> 24) & 0xFF);
			break;
		case 6:
			value |= ((*index12p >> 12) & 0xFFF);
			break;
		case 7:
			value |= (*index12p & 0xFFF);
			break;
	}
	return value;
}



// RGB Helper functions
#if RGB_LEDS
void LEDCube::loadColorData() {
	
	
}

void LEDCube::setAllRGB(int red, int green, int blue){
	for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
		for (int _channel = 0; _channel < RGB_CHANNELS; _channel++){
		       setRGB(_layer, _channel, red, green, blue);
		}
	}
}

void LEDCube::setAllRGBOnLayer(int layer, int red, int green, int blue){
		for (int _channel = 0; _channel < RGB_CHANNELS; _channel++){
		       setRGB(layer, _channel, red, green, blue);
		}
}

// RGB LEDs are connected to the TLC sequentially
// i.e. ch 0 = B1, ch 1 = G1, ch 2 = R1, ch 3 = B2, ch 4 = G2, ch 5 = R2
void LEDCube::setRGB(int layer, int channel, int r, int g, int b){
	int tlc_channel = channel * 3;

	set(layer, tlc_channel++, b);
	set(layer, tlc_channel++, g);
	set(layer, tlc_channel, r);
}

// Each RGB LED is connected to multiple TLCs.
// i.e. ch 0 = R1, ch 1 = R2, ch 16 = G1, ch 17 = G2, ch 32 = B1, ch 33 = B2
void LEDCube::setRGB2(int layer, int channel, int r, int g, int b){

	int tlc_channel = channel / 16 * 48 + channel % 16;

	set(layer, tlc_channel, r);
	tlc_channel += 16;
	set(layer, tlc_channel, g);
	tlc_channel += 16;
	set(layer, tlc_channel, b);
}

int LEDCube::getRed(int layer, int channel) {
	int tlc_channel = ((channel * 3) + 2);
	return get(layer, tlc_channel);
}

int LEDCube::getGreen(int layer, int channel) {
	int tlc_channel = ((channel * 3) + 1);
	return get(layer, tlc_channel);
}

int LEDCube::getBlue(int layer, int channel) {
	int tlc_channel = (channel * 3);
	return get(layer, tlc_channel);
}
#endif 
// End of RGB helper functions




#if VPRG_ENABLED

void LEDCube::setDC(int channel, int value){

	uint8_t index8 = (NUM_TLCS * 16 -1) - channel;
	uint8_t *index6p = tlc_DCData + ((index8 * 3) >> 2);	// index * 3 / 4 = which array element the bits start it
	int caseNum = index8 % 4; 

	switch(caseNum){
		case 0:
			*index6p = (*index6p & 0x03) | value << 2;
			break;
		case 1:
			*index6p = (*index6p & 0xFC) | value >> 4;
			index6p++;
			*index6p = (*index6p & 0x0F) | value << 4;
			break;
		case 2:
			*index6p = (*index6p & 0xF0) | value >> 2;
			index6p++;
			*index6p = (*index6p & 0x3F) | value << 6;
			break;
		case 3:
			*index6p = (*index6p & 0xC0) | value;
			break;
	}
}

void LEDCube::setAllDC(int value){
	for (int i=0; i<NUM_TLCS; i++){
		setDC(i, value);
	}
}


int LEDCube::getDC(int channel){

	uint8_t index8 = (NUM_TLCS * 16 -1) - channel;
	uint8_t *index6p = tlc_DCData + ((index8 * 3) >> 2);	// index * 3 / 4 = which array element the bits start it
	int caseNum = index8 % 4; 
	int value = 0;

	switch (caseNum){
		case 0:
			value |= (*index6p >> 2) & 0x3F;
			break;
		case 1:
			value |= (*index6p << 4) & 0x30;
			index6p++;
			value |= (*index6p >> 4) & 0x0F;
			break;
		case 2:
			value |= (*index6p << 2) & 0x3C;
			index6p++;
			value |= (*index6p >> 6) & 0x03;
			break;
		case 3:
			value |= (*index6p & 0x3F);
			break;
	}
	return value;
}

/** Send the bits for Dot Correction to the TLC*/
int LEDCube::updateDC(){

	// if needXLAT, there is already an update in process
	if (cube_needXLAT){
		return 1;
	}


	// Set VPRG High to switch to DC programming mode
	setHigh(VPRG_PORT, VPRG);

	// SPI didn't work out so well since I'm using an array of uint8_t
	// So let's try bit banging
	for(int i = 0; i < (NUM_TLCS * 12); i++) {
		for(int s = 7; s >= 0; s--)
		{
			if(tlc_DCData[i] >> s & 0x1) {
				setHigh(SOUT_PORT, SOUT);
			} else {
				setLow(SOUT_PORT, SOUT);
			}

			pulse_pin(SCLK_PORT, SCLK);
		}
	}
	request_xlat_pulse();

	return 0;
}

uint8_t* LEDCube::getDCData(){
	return tlc_DCData;
}
#endif




/**
Triggering XLAT starts with enabling the Blank interrrupt
this interrupt will fire after BLANK is pulsed
the Blank interrupt handler sets the XLAT pulse time to occur the next time BLANK is high.
After XLAT is pulsed the XLAT Interrupt fires 
which resets the need_XLAT flag and causes the XLAT pulse time to be set out of range 
(so that neither the pin or the interrupt will be fired)

We may be wasting some time waiting for the first blank cycle, then waiting for the second,
but at least this way we have an entire cycle to setup the XLAT pulse. If we decided to just
setup XLAT immediately, we would run the risk of setting XLAT right in the middle of when
it was supposed to be checked and we could run into some nasty race conditions. 
**/
void LEDCube::request_xlat_pulse(){

	// Set this flag so that we can keep track of whether or not we are waiting for an XLAT pulse
	cube_needXLAT = 1;

	// Enable the interrupt on BLANK to fire
	ConfigIntOC5(OC_INT_ON | OC_INT_PRIOR_3 | OC_INT_SUB_PRI_3);
}

#ifdef __cplusplus
extern "C"	// So c++ doesn't mangle the function names
{
#endif

	// Handle the interrupt triggered by BLANK
	void __ISR(_OUTPUT_COMPARE_5_VECTOR, ipl3) IntOC5Handler(void)	
	{
		// Stop BLANK from firing any more interrupts
		ConfigIntOC5(OC_INT_OFF);

		// Set the XLAT pulse to occur during the next time BLANK is high
		SetPulseOC4(0x1, 0x2);
	}

	// Handle the interrupt triggered by XLAT
	void __ISR(_OUTPUT_COMPARE_4_VECTOR, ipl3) IntOC4Handler(void)
	{
		// Rather than turning off the interrupt for XLAT, just set the pulse time to a value that will never be matched
		SetPulseOC4(0xFFFF, 0xFFFF);

		// Reset our flag
		cube_needXLAT = 0;

		mOC4ClearIntFlag();

		// If VPRG is High, then we just programmed DC
		// if (outputState(VPRG_PORT, VPRG)){
		// 	setLow(VPRG_PORT, VPRG);
		// 	pulse_pin(SCLK_PORT, SCLK);
		// }


		if (tlc_onUpdateFinished) {
		    tlc_onUpdateFinished();
		}
		//OpenTimer2(T2_ON | T2_PS_1_4, 0x3);
	}

#ifdef __cplusplus
}
#endif


/** Preinstantiated Cube variable. */
LEDCube Cube;
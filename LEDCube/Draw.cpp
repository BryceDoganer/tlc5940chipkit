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

#include "Draw.h"

// Found this, plus a many more cool macros on:
// http://www-graphics.stanford.edu/~seander/bithacks.html
// Swaps two variables
#define SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))


void Draw::clearAll(void) {
	 Cube.clearAll();
}

unsigned char Draw::intensityOutOfRange(int intensity) {
  if (intensity >= 0 && intensity < 4096) 
    return 0;
  else 
    return 1;
}

// Returns the channel number given X and Y coordinates
int Draw::channel(int x, int y) {
    return (x * CUBE_SIZE * LED_SIZE) + y;
}

// Returns True if out of range of CUBE_SIZE
unsigned char Draw::coordOutOfRange(int x, int y, int z) {
  if ((x >= 0 && x < CUBE_SIZE) &&
      (y >= 0 && y < CUBE_SIZE) &&
      (z >= 0 && z < CUBE_SIZE))
    return 0;
  else  // One of the coordinates was outside the cube.
    return 1;
}

void Draw::clearPlaneX(int x) {
  if ((x >= 0) && (x < CUBE_SIZE)) {
    for(int _z = 0; _z < CUBE_SIZE; _z++) {
          for (int _channel = (x * CUBE_SIZE); _channel < ((x * CUBE_SIZE) + CUBE_SIZE); _channel++) { 
            #if RGB_LEDS
              Cube.setVoxelSpectrumColor(_channel, _z, 0);
            #else
              Cube.set(_channel, _z, 0);
            #endif
          }
     }
  }
}

// Clears all voxels along a X/Z plane at a given point on axis Y
void Draw::clearPlaneY(int y) {
  if ((y >= 0) && (y < CUBE_SIZE)) {
    for(int _z = 0; _z < CUBE_SIZE; _z++) {
          for (int _channel = y; _channel < RGB_CHANNELS; _channel += CUBE_SIZE) {
            #if RGB_LEDS
              Cube.setVoxelSpectrumColor(_channel, _z, 0);
            #else
              Cube.set(_channel, _z, 0);
            #endif
          }
     }
  }
}

// Clears all voxels along a X/Y plane at a given point on axis Z
void Draw::clearPlaneZ(int z) {
  if (z >= 0 && z < CUBE_SIZE) {
      for (int _channel = 0; _channel < RGB_CHANNELS; _channel++) {
            #if RGB_LEDS
              Cube.setVoxelSpectrumColor(_channel, z, 0);
            #else
              Cube.set(_channel, z, 0);
            #endif 
      }
  }
}

// Shifts entire contents of cube over X axis
void Draw::shiftCubeX(int direction) {
  if(direction == 0) return;

  // Move in positive direction
  if(direction > 0) {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
        for(int _channel = (NUM_CHANNELS - 1); _channel >= 0; _channel--) {
          if(_channel > ((CUBE_SIZE * LED_SIZE) - 1)) {
            Cube.set(_channel, _layer, Cube.get((_channel - (CUBE_SIZE * LED_SIZE)), _layer));
          } else {
            // Clear the final X Plane
            Cube.set(_channel, _layer, 0);
          } 
        }
      }
  // Move in negative direction
  } else {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
        for(int _channel = 0; _channel < NUM_CHANNELS; _channel++) {
          if(_channel < (NUM_CHANNELS - (CUBE_SIZE * LED_SIZE))) {
            Cube.set(_channel, _layer, Cube.get((_channel + (CUBE_SIZE * LED_SIZE)), _layer));
          } else {
            // Clear the final X Plane
            Cube.set(_channel, _layer,  0);
          }
        }
      }
  }
}

// Shifts entire contents of cube over Y axis
void Draw::shiftCubeY(int direction) {
  if(direction == 0) return;

  // Move in positive direction
  if(direction > 0) {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
        for(int _channel = NUM_CHANNELS - 1; _channel >= 0; _channel --) {
          if(((_channel - (LED_SIZE - 1)) % (CUBE_SIZE * LED_SIZE)) == 0) {
            for(int i = 0; i < LED_SIZE; i++) {
              Cube.set(_channel, _layer, 0); // Clear final Y Layer
              _channel--;
            } 
            _channel++;
          } else {
            Cube.set(_channel, _layer, Cube.get(_channel - (1 * LED_SIZE), _layer));
          } 
        }
      }
  // Move in negative direction
  } else {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
        for(int _channel = 0; _channel < NUM_CHANNELS; _channel ++) {
          if(((_channel + LED_SIZE) % (CUBE_SIZE * LED_SIZE)) == 0) {
            for(int i = 0; i < LED_SIZE; i++) {
              Cube.set(_channel, _layer, 0); // Clear final Y Layer
              _channel++;
            } 
            _channel--;
          } else {
            Cube.set(_channel, _layer, Cube.get(_channel + (1 * LED_SIZE), _layer));
          }
        }
      }
  }
}

// Shifts entire contents of cube over Z axis
void Draw::shiftCubeZ(int direction) {
  if(direction == 0) return;

  // Move in positive direction
  if(direction > 0) {
      for(int _layer = CUBE_SIZE - 1; _layer >= 0; _layer--) {
        for(int _channel = 0; _channel < NUM_CHANNELS; _channel ++) {
          if(_layer == 0) {
            Cube.set(_channel, _layer, 0); // Clear final Z Layer 
          } else {
            Cube.set(_channel, _layer, Cube.get(_channel, (_layer - 1)));
          }
        }
      }
  // Move in negative direction
  } else {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
        for(int _channel = 0; _channel < NUM_CHANNELS; _channel ++) {
          if(_layer == (CUBE_SIZE - 1)) {
            Cube.set(_channel, _layer, 0); // Clear final Z Layer 
          } else {
            Cube.set(_channel, _layer, Cube.get(_channel, (_layer + 1)));
          }
        }
      }
  }
}


/*****************************************************************************/
// RGB LED Functions:
#if RGB_LEDS

// Returns the RGB channel number given X and Y coordinates
int Draw::RGBChannel(int x, int y) {
    return (x * CUBE_SIZE) + y;
}

// Returns True if out of range of color size
unsigned char Draw::RGBIntensityOutOfRange(int red, int green, int blue) {
  if ((  red >= 0 &&   red < 4096) &&
      (green >= 0 && green < 4096) &&
      ( blue >= 0 &&  blue < 4096))
    return 0;
  else  // One of the coordinates was outside the cube.
    return 1;
}

// Set a single voxel to ON
void Draw::setRGBVoxel(int x, int y, int z, int red, int green, int blue) {
  if (coordOutOfRange(x,y,z)) return;
  if (RGBIntensityOutOfRange(red, green, blue)) return;
  Cube.setRGB(RGBChannel(x,y), z, red, green, blue);  
}

// Set a single voxel to OFF
void Draw::clearRGBVoxel(int x, int y, int z) {
  if (coordOutOfRange(x,y,z)) return;
  Cube.setRGB(RGBChannel(x,y), z, 0, 0, 0);   
}

// Draws a line through the X plane at Y & Z coordinates
void Draw::drawRGBPlaneLineX(int y, int z, int red, int green, int blue) {
      if(coordOutOfRange(0,y,z))return;
      for(int _x = 0; _x < CUBE_SIZE; _x++) {
          setRGBVoxel(_x, y, z, red, green, blue);
      }
}

// Draws a line through the Y plane at X & Z coordinates
void Draw::drawRGBPlaneLineY(int x, int z, int red, int green, int blue) {
      if(coordOutOfRange(x,0,z))return;
      for(int _y = 0; _y < CUBE_SIZE; _y++) {
          setRGBVoxel(x, _y, z, red, green, blue);
      }
}

// Draws a line through the Z plane at X & Y coordinates
void Draw::drawRGBPlaneLineZ(int x, int y, int red, int green, int blue) {
      if(coordOutOfRange(x,y,0))return;
      for(int _z = 0; _z < CUBE_SIZE; _z++) {
          setRGBVoxel(x, y, _z, red, green, blue);
      }
}

// Draws a line on each plane which interset at a single point
void Draw::drawRGBCrosshair(int x, int y, int z, int red, int green, int blue) {
    drawRGBPlaneLineX(y, z, red, green, blue);
    drawRGBPlaneLineY(x, z, red, green, blue);
    drawRGBPlaneLineZ(x, y, red, green, blue);
}

// Sets all voxels along a Y/Z plane at a given point on axis X
void Draw::setRGBPlaneX(int x, int red, int green, int blue) {
  if (RGBIntensityOutOfRange(red, green, blue)) return;
  if ((x >= 0) && (x < CUBE_SIZE)) {
    for(int z = 0; z < CUBE_SIZE; z++) {
          for (int c = (x * CUBE_SIZE); c < ((x * CUBE_SIZE) + CUBE_SIZE); c++) {
            Cube.setRGB(c, z, red, green, blue); 
          }
     }
  }
}

// Sets all voxels along a X/Z plane at a given point on axis Y
void Draw::setRGBPlaneY(int y, int red, int green, int blue) {
  if (RGBIntensityOutOfRange(red, green, blue)) return;
  if ((y >= 0) && (y < CUBE_SIZE)) {
    for(int z = 0; z < CUBE_SIZE; z++) {
          for (int c = y; c < RGB_CHANNELS; c += CUBE_SIZE) {
            Cube.setRGB(c, z, red, green, blue); 
          }
     }
  }
}

// Sets all voxels along a X/Y plane at a given point on axis Z
void Draw::setRGBPlaneZ(int z, int red, int green, int blue) {
  if (RGBIntensityOutOfRange(red, green, blue)) return;
  if ((z >= 0) && (z < CUBE_SIZE)) {
      for (int c = 0; c < RGB_CHANNELS; c++) {
        Cube.setRGB(c, z, red, green, blue); 
      }
  }
}

// Draw a line between any two coordinates in 3d space.
void Draw::drawRGBLine(int x1, int y1, int z1, int x2, int y2, int z2, int red, int green, int blue) {
  int dx,dy,dz;
  int x,y,z;

  if( x1 >= x2 ) { dx = x1 - x2; } else { dx = x2 - x1; }
  if( y1 >= y2 ) { dy = y1 - y2; } else { dy = y2 - y1; }
  if( z1 >= z2 ) { dz = z1 - z2; } else { dz = z2 - z1; }

  // Change in X is greatest
  if((dx >= dy) && (dx >= dz)) {
        float xy;  // how many voxels do we move on the y axis for each step on the x axis
        float xz;  // how many voxels do we move on the y axis for each step on the x axis 

      // We always want to draw the line from the lowes value to highest
      // If x1 is bigger than x2, we need to flip all the values.
      if (x1 > x2) { SWAP(x1,x2); SWAP(y1,y2); SWAP(z1,z2); }
    
      xy = (float)(y2-y1)/(float)(x2-x1);
      xz = (float)(z2-z1)/(float)(x2-x1);

      for (x = x1; x <= x2; x++) {
        y = (int)(xy * (x - x1)) + y1;
        z = (int)(xz * (x - x1)) + z1;
        setRGBVoxel(x, y, z, red, green, blue);
      }

  // Change in Y is greatest
  } else if ((dy >= dx) && (dy >= dz)) {
        float yx;
        float yz;

      if (y1 > y2) { SWAP(x1,x2); SWAP(y1,y2); SWAP(z1,z2); }

      yx = (float)(x2-x1)/(float)(y2-y1);
      yz = (float)(z2-z1)/(float)(y2-y1);

      for (y = y1; y <= y2; y++) {
        x = (int)(yx * (y - y1)) + x1;
        z = (int)(yz * (y - y1)) + z1;
        setRGBVoxel(x, y, z, red, green, blue);
      }

  // Change in Z is greatest
  } else {
      float zx;
      float zy;

      if (z1 > z2) { SWAP(x1,x2); SWAP(y1,y2); SWAP(z1,z2); }

      zx = (float)(x2-x1)/(float)(z2-z1);
      zy = (float)(y2-y1)/(float)(z2-z1);
     
      for (z = z1; z <= z2; z++) {
        x = (int)(zx * (z - z1)) + x1;
        y = (int)(zy * (z - z1)) + y1;
        setRGBVoxel(x, y, z, red, green, blue);
      }
  }
} // End of drawRGBLine()

// Draws an outline of a cube starting from the x,y,z coordinatie moving out based on the orientation and size
void Draw::drawRGBCube(int x, int y, int z, int size, int orientation, int fill, int red, int green, int blue)
{
	if ((orientation > 8) || (orientation < 1)) return;

	// Orientation: (Corner which x,y,z are drawn from)
	// 1: Forward Bottom Right
	// 2: Forward Bottom Left
	// 3: Back Bottom Left
	// 4: Back Bottom Right
	// 5: Forward Top Right
	// 6: Forward Top Left
	// 7: Back Top Left
	// 8: Back Top Right

	int x2, y2, z2, tmp;

    switch (orientation) {
    	case 1:
    		x2 = x + size;
    		y2 = y + size;
    		z2 = z + size;
    		break;
		case 2:
  		  x = (CUBE_SIZE - 1) - x; // Flip to opposite side

    	 	x2 = x - size;
    		y2 = y + size;
    		z2 = z + size;
    		break;
    	case 3:
    		x = (CUBE_SIZE - 1) - x;
    		y = (CUBE_SIZE - 1) - y;

    		x2 = x - size;
    		y2 = y - size;
    		z2 = z + size;
    		break;
		case 4:
  			y = (CUBE_SIZE - 1) - y;

    		x2 = x + size;
    		y2 = y - size;
    		z2 = z + size;
    		break;
    	case 5:
    		z = (CUBE_SIZE - 1) - z;

    		x2 = x + size;
    		y2 = y + size;
    		z2 = z - size;
    		break;
    	case 6:
  			x = (CUBE_SIZE - 1) - x;
  			z = (CUBE_SIZE - 1) - z;

    		x2 = x - size;
    		y2 = y + size;
    		z2 = z - size;
    		break;
    	case 7:
    		x = (CUBE_SIZE - 1) - x;
    		y = (CUBE_SIZE - 1) - y;
			  z = (CUBE_SIZE - 1) - z;

    		x2 = x - size;
    		y2 = y - size;
    		z2 = z - size;
    		break;
        case 8:
      	y = (CUBE_SIZE - 1) - y;
		    z = (CUBE_SIZE - 1) - z;

    		x2 = x + size;
    		y2 = y - size;
    		z2 = z - size;
    		break;
    }

    if(x > x2) SWAP(x,x2); if(y > y2) SWAP(y,y2); if(z > z2) SWAP(z,z2);

    // If fill is 1, draw a soild cube
    if (fill) {
        for (int _z = z; _z <= z2; _z++) {
          for (int _x = x; _x <= x2; _x++) {
            for (int _y = y; _y <= y2; _y++) {
                setRGBVoxel(_x,_y,_z,  red, green, blue);
            }
          }
        }
    // If fill is 0, draw the outline 
    } else {
      for (int _z = z; _z <= z2; _z++) {
        for (int _x = x; _x <= x2; _x++) {
          for (int _y = y; _y <= y2; _y++) {
              // Draw the top and bottom squares
              if ((_z == z) || (_z == z2)) {
                  if((_x == x) || (_x == x2)) {
                    setRGBVoxel(_x, _y, _z, red, green, blue);
                  } else if ((_y == y) || (_y == y2)) {
                    setRGBVoxel(_x, _y, _z, red, green, blue);
                  }
              // Draw the vertical lines
              } else if( ((_x == x) && (_y == y)) || ((_x == x2) && (_y == y)) || 
                        ((_x == x) && (_y == y2)) || ((_x == x2) && (_y == y2)) ) {
                    setRGBVoxel(_x, _y, _z, red, green, blue);
              }
          }
        }
      }
    }
}

// Draws a box from corner to corner based on orientation
void Draw::drawRGBBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int fill, int red, int green, int blue)
{
	if ((orientation > 8) || (orientation < 1)) return;

	// Orientation: (Corner which x,y,z are drawn from)
	// 1: Forward Bottom Right
	// 2: Forward Bottom Left
	// 3: Back Bottom Left
	// 4: Back Bottom Right
	// 5: Forward Top Right
	// 6: Forward Top Left
	// 7: Back Top Left
	// 8: Back Top Right

    switch (orientation) {
    	case 1:

    		break;
		  case 2:
			   x = (CUBE_SIZE - 1) - x; // Flip to opposite side
			  x2 = (CUBE_SIZE - 1) - x2;
    		break;
    	case 3:
    		 x = (CUBE_SIZE - 1) - x;
    		 y = (CUBE_SIZE - 1) - y;
    		x2 = (CUBE_SIZE - 1) - x2;
    		y2 = (CUBE_SIZE - 1) - y2;
    		break;
		  case 4:
			   y = (CUBE_SIZE - 1) - y;
    		y2 = (CUBE_SIZE - 1) - y2;
    		break;
    	case 5:
    		z = (CUBE_SIZE - 1) - z;
    		z2 = (CUBE_SIZE - 1) - z2;
    		break;
    	case 6:
			   x = (CUBE_SIZE - 1) - x;
			   z = (CUBE_SIZE - 1) - z;
    		x2 = (CUBE_SIZE - 1) - x2;
    		z2 = (CUBE_SIZE - 1) - z2;
    		break;
    	case 7:
    		 x = (CUBE_SIZE - 1) - x;
    		 y = (CUBE_SIZE - 1) - y;
			   z = (CUBE_SIZE - 1) - z;
    		x2 = (CUBE_SIZE - 1) - x2;
    		y2 = (CUBE_SIZE - 1) - y2;
    		z2 = (CUBE_SIZE - 1) - z2;
    		break;
      case 8:
         y = (CUBE_SIZE - 1) - y;
			   z = (CUBE_SIZE - 1) - z;
    		y2 = (CUBE_SIZE - 1) - y2;
    		z2 = (CUBE_SIZE - 1) - z2;
    	break;
    }

    if(x > x2) SWAP(x,x2); if(y > y2) SWAP(y,y2); if(z > z2) SWAP(z,z2);

    // If fill is 1 draw a solid box
    if (fill) {
        for (int _z = z; _z <= z2; _z++) {
          for (int _x = x; _x <= x2; _x++) {
            for (int _y = y; _y <= y2; _y++) {
                setRGBVoxel(_x,_y,_z,  red, green, blue);
            }
          }
        }
    // If fill is 0 draw the outline
    } else {
        for (int _z = z; _z <= z2; _z++) {
          for (int _x = x; _x <= x2; _x++) {
            for (int _y = y; _y <= y2; _y++) {
                // Draw the top and bottom squares
                if ((_z == z) || (_z == z2)) {
                    if((_x == x) || (_x == x2)) {
                      setRGBVoxel(_x, _y, _z, red, green, blue);
                    } else if ((_y == y) || (_y == y2)) {
                      setRGBVoxel(_x, _y, _z, red, green, blue);
                    }
                // Draw the vertical lines
                } else if( ((_x == x) && (_y == y)) || ((_x == x2) && (_y == y)) || 
                          ((_x == x) && (_y == y2)) || ((_x == x2) && (_y == y2)) ) {
                      setRGBVoxel(_x, _y, _z, red, green, blue);
                }
            }
          }
        }
    }
}

#if RGB_SPECTRUM
///////////////////////////////////////////////////////////////////////////////
// The functions below set RGB LEDs through spectrum color which will 
// Keep total brightness level constant throughout the 
// entire spectrum. i.e. R+G+B = 4095 (unless off of course)
// This is helpful to limit current.
///////////////////////////////////////////////////////////////////////////////
int spectrumData[SPECTRUM_DATA_ARRAYS][SPECTRUM_DATA_VALUES];

int Draw::getRedSpectrum(int spectrum) {

  if ((spectrum < 1) || (spectrum > 12287)) {
      return 0;                            // spectrum 0 will turn LED off
  } else if (spectrum <= 4095) {
      return (4095 - spectrum);            // red goes from on to off
  } else if (spectrum <= 8191) {
      return 0;                            // red is always off
  } else { //(spectrum <= 12287) 
      return (spectrum - 8192);            // red off to on
  }
}

int Draw::getGreenSpectrum(int spectrum) {
    if ((spectrum < 1) || (spectrum > 12287)) {
      return 0;
    } else if (spectrum <= 4095) {
      return spectrum;                     // green goes from off to on
    } else if (spectrum <= 8191) {
      return (4095 - (spectrum - 4096));   // green on to off
    } else { //(spectrum <= 12287) 
      return 0;                            // green is always off
    }

}

int Draw::getBlueSpectrum (int spectrum) {
    if ((spectrum < 1) || (spectrum > 12287)) {
      return 0;
    } else if (spectrum <= 4095) {
      return 0;                           // blue is always off
    } else if (spectrum <= 8191) {
      return (spectrum - 4096);           // blue off to on
    } else { //(spectrum <= 12287) 
      return  (4095 - (spectrum - 8192));  // blue on to off
    }
}

unsigned char Draw::spectrumOutOfRange(int spectrum) {
    if ((spectrum >= NUM_COLORS) || (spectrum < 0))
      return 1;
    else 
      return 0;
}

void Draw::setVoxelSpectrum(int x, int y, int z, int spectrum) {
    if (spectrumOutOfRange(spectrum)) return;
    if (coordOutOfRange(x,y,z)) return;
    Cube.setVoxelSpectrumColor(RGBChannel(x,y), z, spectrum);
}

void Draw::setSpectrumForLayer(int layer, int spectrum) {
    for (int _channel = 0; _channel < RGB_CHANNELS; _channel++) {
        Cube.setVoxelSpectrumColor(_channel, layer, spectrum);
    }
}

void Draw::setSpectrumAll(int spectrum) {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
          setSpectrumForLayer(_layer, spectrum);
      }  
}

void Draw::setSpectrumValue(int array, int value, int amount) {
    if((array < 0) || (array > SPECTRUM_DATA_ARRAYS - 1)) return;
    if((value < 0) || (value > SPECTRUM_DATA_VALUES - 1)) return;
    if(spectrumOutOfRange(amount)) return;

    spectrumData[array][value] = amount;
}

void Draw::setSpectrumArray(int array, int amount) {
    if((array < 0) || (array > SPECTRUM_DATA_ARRAYS - 1)) return;
    if(spectrumOutOfRange(amount)) return;

    for(int value = 0; value < SPECTRUM_DATA_VALUES; value++) {
      spectrumData[array][value] = amount;
    }
}

int Draw::getSpectrumValue(int array, int value) {
    if((array < 0) || (array > SPECTRUM_DATA_ARRAYS - 1)) return 0;
    if((value < 0) || (value > SPECTRUM_DATA_VALUES - 1)) return 0;
    return spectrumData[array][value];
}

// Increases a value in one of the spectrum arrays
void Draw::increaseSpectrumValue(int array, int value, int amount) {
    if((array < 0) || (array > SPECTRUM_DATA_ARRAYS - 1)) return;
    if((value < 0) || (value > SPECTRUM_DATA_VALUES - 1)) return;

    int spectrum = spectrumData[array][value];
    spectrum += amount;
    if (spectrumOutOfRange(spectrum)) spectrum -= NUM_COLORS;
    if (spectrum == 0) spectrum++;
  
    spectrumData[array][value] = spectrum;
}

// Increases all the values in one of the spectrum arrays
void Draw::increaseSpectrumArray(int array, int amount) {
    if((array < 0) || (array > SPECTRUM_DATA_ARRAYS - 1)) return;

    int spectrum;
    for(int value = 0; value < SPECTRUM_DATA_VALUES; value++) {
        int spectrum = spectrumData[array][value];
        spectrum += amount;

        if (spectrumOutOfRange(spectrum)) spectrum -= NUM_COLORS;
        if (spectrum == 0) spectrum++;
  
        spectrumData[array][value] = spectrum;
    }
}

// Increases all the values all of the spectrum arrays
void Draw::increaseSpectrumAll(int amount) {
    int spectrum;
    for(int array = 0; array < SPECTRUM_DATA_ARRAYS; array++) {
      for(int value = 0; value < SPECTRUM_DATA_VALUES; value++) {
          int spectrum = spectrumData[array][value];
          spectrum += amount;

          if (spectrumOutOfRange(spectrum)) spectrum -= NUM_COLORS;
          if (spectrum == 0) spectrum++;
    
          spectrumData[array][value] = spectrum;
      }
    }
}

void Draw::drawSpectrumPlaneLineX(int y, int z, int spectrum) {
    if(spectrumOutOfRange(spectrum)) return;
    drawRGBPlaneLineX(y,z, getRedSpectrum(spectrum), 
                         getGreenSpectrum(spectrum), 
                          getBlueSpectrum(spectrum));

}
void Draw::drawSpectrumPlaneLineY(int x, int z, int spectrum) {
    if(spectrumOutOfRange(spectrum)) return;
    drawRGBPlaneLineY(x,z, getRedSpectrum(spectrum), 
                         getGreenSpectrum(spectrum), 
                          getBlueSpectrum(spectrum));
}
void Draw::drawSpectrumPlaneLineZ(int x, int y, int spectrum) {
    if(spectrumOutOfRange(spectrum)) return;
    drawRGBPlaneLineZ(x,y, getRedSpectrum(spectrum), 
                         getGreenSpectrum(spectrum), 
                          getBlueSpectrum(spectrum));
}
void Draw::drawSpectrumCrosshair(int x, int y, int z, int spectrum) {
    if(spectrumOutOfRange(spectrum)) return;
    drawRGBCrosshair(x,y,z, getRedSpectrum(spectrum), 
                          getGreenSpectrum(spectrum), 
                           getBlueSpectrum(spectrum));
}

// Sets all voxels along a Y/Z plane at a given point on axis X
void Draw::setSpectrumPlaneX(int x, int spectrum) {
  if (spectrumOutOfRange(spectrum)) return;
  if ((x >= 0) && (x < CUBE_SIZE)) {
    for(int z = 0; z < CUBE_SIZE; z++) {
          for (int c = (x * CUBE_SIZE); c < ((x * CUBE_SIZE) + CUBE_SIZE); c++) {
            Cube.setVoxelSpectrumColor(c, z, spectrum); 
          }
     }
  }
}

// Sets all voxels along a X/Z plane at a given point on axis Y
void Draw::setSpectrumPlaneY(int y, int spectrum) {
  if (spectrumOutOfRange(spectrum)) return;
  if ((y >= 0) && (y < CUBE_SIZE)) {
    for(int z = 0; z < CUBE_SIZE; z++) {
          for (int c = y; c < RGB_CHANNELS; c += CUBE_SIZE) {
            Cube.setVoxelSpectrumColor(c, z, spectrum);  
          }
     }
  }
}

// Sets all voxels along a X/Y plane at a given point on axis Z
void Draw::setSpectrumPlaneZ(int z, int spectrum) {
  if (spectrumOutOfRange(spectrum)) return;
  for (int _channel = 0; _channel < RGB_CHANNELS; _channel++) {
        Cube.setVoxelSpectrumColor(_channel, z, spectrum);
    }
}

void Draw::drawSpectrumLine(int x1, int y1, int z1, int x2, int y2, int z2, int spectrum) {
      if (spectrumOutOfRange(spectrum)) return;
      drawRGBLine(x1, y1, z1, x2, y2, z2, getRedSpectrum(spectrum), 
                                        getGreenSpectrum(spectrum), 
                                         getBlueSpectrum(spectrum));
}

void Draw::drawSpectrumCube(int x, int y, int z, int size, int orientation, int fill, int spectrum) {
      if (spectrumOutOfRange(spectrum)) return;
      drawRGBCube(x, y, z, size, orientation, fill, getRedSpectrum(spectrum), 
                                                  getGreenSpectrum(spectrum), 
                                                   getBlueSpectrum(spectrum));
}

void Draw::drawSpectrumBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int fill, int spectrum) {
      if (spectrumOutOfRange(spectrum)) return;
      drawRGBBox(x, y, z, x2, y2, z2, orientation, fill, getRedSpectrum(spectrum), 
                                                       getGreenSpectrum(spectrum), 
                                                        getBlueSpectrum(spectrum));
}


// END OF RGB Spectrum Functions
#endif
// END OF RGB LED Functions
/*****************************************************************************/
// MONO Color LED Functions:
#else 


#endif
// END OF MONO Color LED Functions
/*****************************************************************************/

/** Preinstantiated Draw variable. */
Draw DrawCube;

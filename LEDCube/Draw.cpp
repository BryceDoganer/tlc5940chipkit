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

// Clears all voxels along a Y/Z plane at a given point on axis X
void Draw::clearPlaneX(int x) {
  if (x >= 0 && x < CUBE_SIZE) {
    for(int z = 0; z < CUBE_SIZE; z++) {
          for (int c = (x * CUBE_SIZE); c < ((x * CUBE_SIZE) + CUBE_SIZE); c++) {
            Cube.setRGB(c, z, 0, 0, 0); 
          }
     }
  }
}

// Clears all voxels along a X/Z plane at a given point on axis Y
void Draw::clearPlaneY(int y) {
  if (y >= 0 && y < CUBE_SIZE) {
    for(int z = 0; z < CUBE_SIZE; z++) {
          for (int c = y; c < RGB_CHANNELS; c += CUBE_SIZE) {
            Cube.setRGB(c, z, 0, 0, 0); 
          }
     }
  }
}

// Clears all voxels along a X/Y plane at a given point on axis Z
void Draw::clearPlaneZ(int z) {
  if (z >= 0 && z < CUBE_SIZE) {
      for (int c = 0; c < RGB_CHANNELS; c++) {
        Cube.setRGB(c, z, 0, 0, 0); 
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

int spectrumStepCount = 0;

// Set a single voxel to ON
void Draw::setRGBVoxel(int x, int y, int z, int red, int green, int blue) {
  if (coordOutOfRange(x,y,z)) return;
  if (RGBIntensityOutOfRange(red, green, blue)) return;
  Cube.setRGB(RGBChannel(x,y), z, red, green, blue);  
}

// TODO FIX THIS:
// Set a single voxel to OFF
void Draw::clearRGBVoxel(int x, int y, int z) {
  if (coordOutOfRange(x,y,z)) return;
  Cube.setVoxelSpectrumColor(RGBChannel(x,y), z, 0);  
}

void Draw::setRGBVoxelSpectrum(int x, int y, int z, int spectrum) {
    if (spectrumOutOfRange(spectrum)) return;
    if (coordOutOfRange(x,y,z)) return;
    Cube.setVoxelSpectrumColor(RGBChannel(x,y), z, spectrum);
}

void Draw::setRGBSpectrumForLayer(int layer, int spectrum) {
      for (int _x = 0; _x < CUBE_SIZE; _x++) {
        for(int _y = 0; _y < CUBE_SIZE; _y++) {
          Cube.setVoxelSpectrumColor(RGBChannel(_x,_y), layer, spectrum);
        }
      }
}

void Draw::setRGBSpectrumAll(int spectrum) {
      for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
              setRGBSpectrumForLayer(_layer, spectrum);
      }  
}

void Draw::increaseRGBSpectrum(int x, int y, int z, int amount) {
    if(coordOutOfRange(x,y,z)) return;

    spectrumStepCount += amount;
    if (spectrumOutOfRange(spectrumStepCount)) spectrumStepCount -= NUM_COLORS;
    if (spectrumStepCount == 0) spectrumStepCount++;
  
    Cube.setVoxelSpectrumColor(RGBChannel(x,y), z, spectrumStepCount);
}

//TODO Make this faster:
void Draw::increaseRGBSpectrumForLayer(int layer, int amount) {
    if(coordOutOfRange(0,0,layer)) return;

    spectrumStepCount += amount;
    if (spectrumOutOfRange(spectrumStepCount)) spectrumStepCount -= NUM_COLORS;
    if (spectrumStepCount == 0) spectrumStepCount++;

    for (int _x = 0; _x < CUBE_SIZE; _x++) {
       for(int _y = 0; _y < CUBE_SIZE; _y++) {
            Cube.setVoxelSpectrumColor(RGBChannel(_x,_y), layer, spectrumStepCount);
       }
    }
}

//TODO Make this faster:
void Draw::increaseRGBSpectrumAll(int amount) {

    spectrumStepCount += amount;
    if (spectrumOutOfRange(spectrumStepCount)) spectrumStepCount -= NUM_COLORS;
    if (spectrumStepCount == 0) spectrumStepCount++;

    for(int _z = 0; _z < CUBE_SIZE; _z++) {
        for (int _x = 0; _x < CUBE_SIZE; _x++) {
           for(int _y = 0; _y < CUBE_SIZE; _y++) {
                Cube.setVoxelSpectrumColor(RGBChannel(_x,_y), _z, spectrumStepCount);
           }
        }
    }
}

unsigned char Draw::spectrumOutOfRange(int spectrum) {
    if ((spectrum >= NUM_COLORS) || (spectrum < 0))
      return 1;
    else 
      return 0;
}

//TODO Fix this:
unsigned char Draw::outOfRGBSpectrum(int red, int green, int blue) {
  // The spectrum only keeps a combined total of 4095 between Red, Green and Blue
    if ((red + green + blue) < 4096)
      return 0;
    else
      return 1;
}

// TODO Fix this...
int Draw::spectrumFromRGB(int red, int green, int blue) {
    int _spectrum = 0;

    if ((red == green) && (green == blue)) return 0; // LED is most likely off

    if ((blue == 0) && (red != 0)) {
             _spectrum = green; // spectrum from 0 to 4095

    } else if ((red == 0) && (green != 0)) {
      _spectrum = blue + 4096; // spectrum from 4096 to 8191

    } else {
      _spectrum = red + 8192; // spectrum from 8192 to 12287
    }
  
    return _spectrum;
}


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

// Draw a line between any coordinates in 3d space.
void Draw::drawRGBLine(int x1, int y1, int z1, int x2, int y2, int z2, int red, int green, int blue) {
  int dx,dy,dz;

  if( x1 >= x2 ) { dx = x1 - x2; } else { dx = x2 - x1; }
  if( y1 >= y2 ) { dy = y1 - y2; } else { dy = y2 - y1; }
  if( z1 >= z2 ) { dz = z1 - z2; } else { dz = z2 - z1; }

  // Change in X is greatest
  if((dx >= dy) && (dx >= dz)) {
  		  float xy;  // how many voxels do we move on the y axis for each step on the x axis
  		  float xz;  // how many voxels do we move on the y axis for each step on the x axis 
  		  int x,y,z,lowesty,lowestz;

  		  // We always want to draw the line from x=0 to x=7.
		  // If x1 is bigger than x2, we need to flip all the values.
		  if (x1 > x2) {
		    int tmp;
		    tmp = x2; x2 = x1; x1 = tmp;
		    tmp = y2; y2 = y1; y1 = tmp;
		    tmp = z2; z2 = z1; z1 = tmp;
		  }
		
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
  		  int x,y,z;

		  if (y1 > y2) {
		    int tmp;
		    tmp = x2; x2 = x1; x1 = tmp;
		    tmp = y2; y2 = y1; y1 = tmp;
		    tmp = z2; z2 = z1; z1 = tmp;
		  }

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
  		  int x,y,z;

		  if (z1 > z2) {
		    int tmp;
		    tmp = x2; x2 = x1; x1 = tmp;
		    tmp = y2; y2 = y1; y1 = tmp;
		    tmp = z2; z2 = z1; z1 = tmp;
		  }

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
void Draw::drawLineRGBCube(int x, int y, int z, int orientation, int size, int red, int green, int blue)
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

	int x2, y2, z2;

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

    // for (int _z = z; _z <= z2; _z++) {
    //   for (int _x = x; _x <= x2; _x++) {
    //     for (int _y = y; _y <= y2; _y++) {

    //         if( ((_x == y)  && (_x == z)) || ((_x == y) && (_x == z2)) || 
    //             ((_x == y2) && (_x == z)) || ((_x == y2) && (_x == z2)) ) {
    //                 setRGBVoxel(_x,_y,_z,  red, green, blue);
    //          } else if ( ((_y == x)  && (_y == z)) || ((_y == x) && (_y == z2)) || 
    //                      ((_y == x2) && (_y == z)) || ((_y == x2) && (_y == z2)) ) {
    //                 setRGBVoxel(_x,_y,_z,  red, green, blue);
    //          } else if ( ((_z == x)  && (_z == y)) || ((_z == x) && (_z == y2)) || 
    //                      ((_z == x2) && (_z == y)) || ((_z == x2) && (_z == y2)) ) {
    //                 setRGBVoxel(_x,_y,_z,  red, green, blue);
    //          }
    //     }
    //   }
    // }

  // Bottom Square
  drawRGBLine(x, y, z, x2, y, z, red, green, blue); // Forward Horizontal Bottom
	drawRGBLine(x, y2, z, x2, y2, z, red, green, blue); // Back Horizontal Bottom
	drawRGBLine(x, y, z, x, y2, z, red, green, blue); // Right Horizontal Bottom
	drawRGBLine(x2, y, z, x2, y2, z, red, green, blue); // Left Horizontal Bottom

	// // Vertical Lines
	drawRGBLine(x, y, z, x, y, z2, red, green, blue); // Forward Right Vertical
	drawRGBLine(x2, y, z, x2, y, z2, red, green, blue); // Forward Left Vertical
	drawRGBLine(x2, y2, z, x2, y2, z2, red, green, blue); // Back Left Vertical
	drawRGBLine(x, y2, z, x, y2, z2, red, green, blue); // Back Right Vertical

	// // Top Square
	drawRGBLine(x, y, z2, x2, y, z2, red, green, blue); // Forward Horizontal Top
	drawRGBLine(x, y2, z2, x2, y2, z2, red, green, blue); // Back Horizontal Top
	drawRGBLine(x, y, z2, x, y2, z2, red, green, blue); // Right Horizontal Top
	drawRGBLine(x2, y, z2, x2, y2, z2, red, green, blue); // Left Horizontal Top
}

// Draws a filled in cube starting from the x,y,z coordinatie moving out based on the orientation and size
void Draw::drawFillRGBCube(int x, int y, int z, int orientation, int size, int red, int green, int blue)
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

    if (x > x2) { tmp = x; x = x2; x2 = tmp; }
    if (y > y2) { tmp = y; y = y2; y2 = tmp; }
    if (z > z2) { tmp = z; z = z2; z2 = tmp; }

    for (int _z = z; _z <= z2; _z++) {
    	for (int _x = x; _x <= x2; _x++) {
    		for (int _y = y; _y <= y2; _y++) {
    				setRGBVoxel(_x,_y,_z,  red, green, blue);
    		}
    	}
    }
}

// Draws a box from corner to corner based on orientation
void Draw::drawLineRGBBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int red, int green, int blue)
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

  // Bottom Square
  drawRGBLine(x, y, z, x2, y, z, red, green, blue); // Forward Horizontal Bottom
	drawRGBLine(x, y2, z, x2, y2, z, red, green, blue); // Back Horizontal Bottom
	drawRGBLine(x, y, z, x, y2, z, red, green, blue); // Right Horizontal Bottom
	drawRGBLine(x2, y, z, x2, y2, z, red, green, blue); // Left Horizontal Bottom

	// Vertical Lines
	drawRGBLine(x, y, z, x, y, z2, red, green, blue); // Forward Right Vertical
	drawRGBLine(x2, y, z, x2, y, z2, red, green, blue); // Forward Left Vertical
	drawRGBLine(x2, y2, z, x2, y2, z2, red, green, blue); // Back Left Vertical
	drawRGBLine(x, y2, z, x, y2, z2, red, green, blue); // Back Right Vertical

	// Top Square
	drawRGBLine(x, y, z2, x2, y, z2, red, green, blue); // Forward Horizontal Top
	drawRGBLine(x, y2, z2, x2, y2, z2, red, green, blue); // Back Horizontal Top
	drawRGBLine(x, y, z2, x, y2, z2, red, green, blue); // Right Horizontal Top
	drawRGBLine(x2, y, z2, x2, y2, z2, red, green, blue); // Left Horizontal Top
}

// Draws a box from corner to corner filled in based on orientation
void Draw::drawFillRGBBox(int x, int y, int z, int x2, int y2, int z2, int orientation, int red, int green, int blue)
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

	int tmp;

    switch (orientation) {
    	case 1:
        // Do nothing 
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

    if (x > x2) { tmp = x; x = x2; x2 = tmp; }
    if (y > y2) { tmp = y; y = y2; y2 = tmp; }
    if (z > z2) { tmp = z; z = z2; z2 = tmp; }

    for (int _z = z; _z <= z2; _z++) {
    	for (int _x = x; _x <= x2; _x++) {
    		for (int _y = y; _y <= y2; _y++) {
    				setRGBVoxel(_x,_y,_z,  red, green, blue);
    		}
    	}
    }

}

// Sets all voxels along a Y/Z plane at a given point on axis X
void Draw::setRGBPlaneX(int x, int red, int green, int blue) {
  if (RGBIntensityOutOfRange(red, green, blue)) return;
  if (x >= 0 && x < CUBE_SIZE) {
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
  if (y >= 0 && y < CUBE_SIZE) {
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
  if (z >= 0 && z < CUBE_SIZE) {
      for (int c = 0; c < RGB_CHANNELS; c++) {
        Cube.setRGB(c, z, red, green, blue); 
      }
  }
}

// END OF RGB LED Functions
/*****************************************************************************/
// MONO Color LED Functions:
#else 


#endif
// END OF MONO Color LED Functions
/*****************************************************************************/

/** Preinstantiated Draw variable. */
Draw DrawCube;

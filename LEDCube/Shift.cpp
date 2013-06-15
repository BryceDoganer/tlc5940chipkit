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

#include "Shift.h"


// // Shifts entire contents of cube over X axis
// void Shift::shiftCubeX(int direction) {
//   if(direction == 0) return;

//   // Move in positive direction
//   if(direction > 0) {
//       for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
//         for(int _channel = (NUM_CHANNELS - 1); _channel >= 0; _channel--) {
//           if(_channel > ((CUBE_SIZE * LED_SIZE) - 1)) {
//             Cube.set(_layer, _channel, Cube.get(_layer, (_channel - (CUBE_SIZE * LED_SIZE))));
//           } else {
//             // Clear the final X Plane
//             Cube.set(_layer, _channel, 0);
//           } 
//         }
//       }
//   // Move in negative direction
//   } else {
//       for(int _layer = 0; _layer < CUBE_SIZE; _layer++) {
//         for(int _channel = 0; _channel < NUM_CHANNELS; _channel++) {
//           if(_channel < (NUM_CHANNELS - (CUBE_SIZE * LED_SIZE))) {
//             Cube.set(_layer, _channel, Cube.get(_layer, (_channel + (CUBE_SIZE * LED_SIZE))));
//           } else {
//             // Clear the final X Plane
//             Cube.set(_layer, _channel, 0);
//           }
//         }
//       }
//   }
// }
// 		void shiftCubeY(int direction);
// 		void shiftCubeZ(int direction);
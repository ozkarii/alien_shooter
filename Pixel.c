/*
 * Pixel.c
 *
 *  Created on: -----
 *      Author: -----
 */

#include "Pixel.h"



//Table for pixel dots.
//				 dots[X][Y][COLOR]
volatile uint8_t dots[8][8][3]={0};

uint8_t* controls = (uint8_t*)0x41220008;
uint8_t* channels = (uint8_t*)0x41220000;


// Here the setup operations for the LED matrix will be performed
void setup(){


	// Resetting the screen at start is a MUST to operation (Set RSTn to 1).

	// set all bits in control signals to 0
	*controls = 0;

	// set all channel bits to 0
	*channels = 0;

	// set RSTn to 1
	*controls |= 0b00001;

	usleep(500);

	//Write code that sets 6-bit values in register of DM163 chip. Recommended that every bit in that register is set to 1. 6-bits and 24 "bytes", so some kind of loop structure could be nice.
	//24*6 bits needs to be transmitted

	// set SDA to 1
	*controls |= 0b10000;

	for (int i = 0; i < 144; i++) {
		toggle_clock();
	}


	//Final thing in this function is to set SB-bit to 1 to enable transmission to 8-bit register.

	// set SB to 1
	*controls |= 0b100;

}

//Change value of one pixel at led matrix. This function is only used for changing values of dots array
void SetPixel(uint8_t x,uint8_t y, uint8_t r, uint8_t g, uint8_t b){

	//Hint: you can invert Y-axis quite easily with 7-y
	dots[x][y][0]=b;
	//Write rest of two lines of code required to make this function work properly (green and red colors to array).
	dots[x][y][1]=g;
	dots[x][y][2]=r;


}


//Put new data to led matrix. Hint: This function is supposed to send 24-bytes and parameter x is for channel x-coordinate.
void run(uint8_t x){

	// Set latch to 0
	*controls &= 0b11111101;

    int bit;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 8; k++) {
				// Extract correct bit to LSB of "bit"
				bit = dots[x][i][j] >> k;
				bit &= 0b00000001;

				// Set SDA to correct bit value
				if (bit == 1) {
					*controls |= 0b10000;
				}
				else {
					*controls &= 0b11101111;
				}


				// Toggle clock from 0 to 1 to 0
				toggle_clock();
			}
		}
	}

	latch();

	//Write code that writes data to led matrix driver (8-bit data). Use values from dots array
	//Hint: use nested loops (loops inside loops)
	//Hint2: loop iterations are 8,3,8 (pixels,color,8-bitdata)


}

//Latch signal. See colorsshield.pdf or DM163.pdf in project folder on how latching works
void latch(){

	*controls |= 0b10;
	*controls &= 0b11111101;

}


//Set one line (channel) as active, one at a time.
void open_line(uint8_t x){
	if ( x == 8 ) {
		*channels = 0;
	}
	*channels = 0b00000001 << x;

}

// Set clock control signal first to 0 then 1 and 0 again
void toggle_clock() {
	*controls &= 0b11110111;
	*controls |= 0b1000;
	*controls &= 0b11110111;
}




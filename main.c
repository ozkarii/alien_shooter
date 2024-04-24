/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 *
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

// Main program for exercise

//****************************************************
//By default, every output used in this exercise is 0
//****************************************************
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "Pixel.h"
#include "Interrupt_setup.h"

//********************************************************************
//***************TRY TO READ COMMENTS*********************************
//********************************************************************

//***Hint: Use sleep(x)  or usleep(x) if you want some delays.****
//***To call assembler code found in blinker.S, call it using: blinker();***


//Comment this if you want to disable all interrupts
#define enable_interrupts




/***************************************************************************************
Name: Oskari Heinonen
Student number: ?

Name: ?
Student number: ?

Tick boxes that you have coded

Led-matrix driver		Game		    Assembler
	[x]					[x]					[x]

Brief description: Alien shooter game

*****************************************************************************************/

uint8_t current_open_line = 0;
int increment = 1;
uint8_t current_alien_line = 0;
uint8_t current_alien_row = 0;
uint8_t current_ship_left_edge = 2;
uint8_t current_projectile_row = 5;
bool shoot = false;
uint8_t current_projectile_column = 0;
uint8_t hit_score = 0;
uint8_t miss_score = 0;
uint8_t remaining_ammo = 8;
bool game_paused = false;
bool game_over = false;
uint8_t game_over_timer = 0;
uint8_t* led_blinker_ptr = (uint8_t*)0x41200000;



int main()
{
	//**DO NOT REMOVE THIS****
	    init_platform();
	//************************


#ifdef	enable_interrupts
	    init_interrupts();
#endif


	    //setup screen
	    setup();

	    //*led_blinker_ptr = 1;

	    // draw ship
	    SetPixel(2, 7, 0, 0, 255);
	    SetPixel(3, 7, 0, 0, 255);
	    SetPixel(4, 7, 0, 0, 255);
	    SetPixel(3, 6, 0, 0, 255);

	    Xil_ExceptionEnable();



	    //Try to avoid writing any code in the main loop.
		while(1){


		}


		cleanup_platform();
		return 0;
}


void clear_row(uint8_t row) {
	for (uint8_t i = 0; i < 8; ++i) {
		SetPixel(i, row, 0, 0, 0);
	}
}

void clear_all_rows() {
	for (int i = 0; i < 8; i++) {
		clear_row(i);
	}
}

void draw_ship() {
	SetPixel(current_ship_left_edge,     7, 0, 0, 255);
    SetPixel(current_ship_left_edge + 1, 7, 0, 0, 255);
    SetPixel(current_ship_left_edge + 2, 7, 0, 0, 255);
    SetPixel(current_ship_left_edge + 1, 6, 0, 0, 255);
}

uint8_t get_projectile_position(){
	return current_ship_left_edge+1;
}



void move_projectile() {

	if (current_projectile_row > 0 && (shoot == true)) {
		if (current_projectile_row == 5) {
			clear_row(current_projectile_row);
			SetPixel(current_projectile_column, current_projectile_row, 255, 69, 72);
			current_projectile_row--;
		}
		else {
			clear_row(5);
			clear_row(current_projectile_row);
			current_projectile_row--;
			SetPixel(current_projectile_column, current_projectile_row, 255, 69, 72);
		}

	}
	else if (current_projectile_row == 0){
		if (current_alien_line == current_projectile_column) {
			hit_score++;
		}
		else {
			miss_score++;
		}
		remaining_ammo--;
		current_projectile_row = 5;
		shoot = false;
	}

}


bool hit() {

	if (current_alien_line == current_projectile_column && current_projectile_row == 0) {
		return true;
	}
	return false;
}

void draw_miss_score() {

	SetPixel(0, 7, 255, 0, 0);
	SetPixel(2, 7, 255, 0, 0);
	SetPixel(1, 6, 255, 0, 0);
	SetPixel(0, 5, 255, 0, 0);
	SetPixel(2, 5, 255, 0, 0);

	for (int i = 0; i < miss_score; ++i) {
		if (i < 4) {
			SetPixel(i + 4, 6, 255, 0, 0);
		}
		else {
			SetPixel(i, 7, 255, 0, 0);
		}
	}
}

void draw_hit_score() {
	SetPixel(0, 1, 0, 255, 0);
	SetPixel(1, 2, 0, 255, 0);
	SetPixel(2, 1, 0, 255, 0);
	SetPixel(3, 0, 0, 255, 0);

	for (int i = 0; i < hit_score; ++i) {
		if (i < 4) {
			SetPixel(i + 4, 1, 0, 255, 0);
		}
		else {
			SetPixel(i, 2, 0, 255, 0);
		}
	}

}

void draw_remaining_ammo() {
	for (int i = 0; i < remaining_ammo; ++i) {
		if (i < 4) {
			SetPixel(i + 4, 3, 255, 69, 72);
		}
		else {
			SetPixel(i, 4, 255, 69, 72);
		}
	}

}

void draw_L() {
	for (int j = 1; j < 6; ++j) {
		SetPixel(2, j, 255, 0, 0);
		SetPixel(3, j, 255, 0, 0);
		if (j > 3) {
			SetPixel(4, j, 255, 0, 0);
			SetPixel(5, j, 255, 0, 0);
		}
	}
}

void draw_W() {
	SetPixel(1, 2, 0, 255, 0);
	SetPixel(1, 3, 0, 255, 0);
	SetPixel(2, 4, 0, 255, 0);
	SetPixel(3, 2, 0, 255, 0);
	SetPixel(3, 3, 0, 255, 0);
	SetPixel(4, 4, 0, 255, 0);
	SetPixel(5, 2, 0, 255, 0);
	SetPixel(5, 3, 0, 255, 0);
}


//Timer interrupt handler for led matrix update. Frequency is 800 Hz
void TickHandler(void *CallBackRef){
	//Don't remove this
	uint32_t StatusEvent;

	// Exceptions must be disabled when updating screen
	Xil_ExceptionDisable();



	//****Write code here ****

	// show all pixels
	open_line(8);
	run(current_open_line);
	open_line(current_open_line);
	++current_open_line;
	if (current_open_line > 7) {
		current_open_line = 0;
	}


	//****END OF OWN CODE*****************

	//*********clear timer interrupt status. DO NOT REMOVE********
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
	//*************************************************************
	//enable exceptions
	Xil_ExceptionEnable();
}

// handle when game over
void game_over_handler() {
	if (game_over_timer == 0) {
		clear_all_rows();
	}
	game_over_timer++;
	if (miss_score > hit_score) {
		draw_L();
	}
	else {
		draw_W();
	}

	if (game_over_timer >= 30) {
		clear_all_rows();
		draw_miss_score();
		draw_hit_score();
		if (game_over_timer >= 60) {
			remaining_ammo = 8;
			hit_score = 0;
			miss_score = 0;
			game_over_timer = 0;
			clear_all_rows();
			game_over = false;
			draw_ship();
		}
	}
}


// handle when game paused
void pause_handler() {
	draw_miss_score();
	draw_hit_score();
	draw_remaining_ammo();
}

//Timer interrupt for moving alien, shooting... Frequency is 10 Hz by default
void TickHandler1(void *CallBackRef){

	//Don't remove this
	uint32_t StatusEvent;

	//****Write code here ****

	if (!game_paused && !game_over) {
		blinker();
		// Moving the alien
		clear_row(0);
		current_alien_line += increment;
		SetPixel(current_alien_line, current_alien_row, 0, 255, 0);

		if (current_alien_line >= 7) {
			increment = -1;
		}
		else if (current_alien_line <= 0) {
			increment = 1;
		}

		// projectile
		move_projectile();

		if (remaining_ammo == 0) {
			game_over = true;
		}
	}
	else if (game_paused) {
		pause_handler();
	}
	else if (game_over) {
		game_over_handler();
	}


	//****END OF OWN CODE*****************
	//clear timer interrupt status. DO NOT REMOVE
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);

}


//Interrupt handler for switches and buttons.
//Reading Status will tell which button or switch was used
//Bank information is useless in this exercise
void ButtonHandler(void *CallBackRef, u32 Bank, u32 Status){
	//****Write code here ****

	//Hint: Status==0x01 ->btn0, Status==0x02->btn1, Status==0x04->btn2, Status==0x08-> btn3, Status==0x10->SW0, Status==0x20 -> SW1

	//If true, btn1 was used to trigger interrupt
	// move right
	if(Status==0x02 && !(current_ship_left_edge >= 5) && !game_paused && !game_over){
		clear_row(7);
	    clear_row(6);
	    ++current_ship_left_edge;
	    draw_ship();
	}
	//If true, btn2 was used to trigger interrupt
	// move left
	else if (Status==0x04 && !(current_ship_left_edge <= 0) && !game_paused && !game_over) {
	    clear_row(7);
	    clear_row(6);
	    --current_ship_left_edge;
	    draw_ship();

	}
	// If true, btn0 was used to trigger interrupt
	// Pause switch
	else if(Status==0x01 && !game_paused && !game_over) {
		current_projectile_column = get_projectile_position();
		shoot = true;
	}
	else if (Status==0x10) {
		game_paused = !game_paused;
		clear_all_rows();
		if (!game_paused) {
			draw_ship();
		}
	}

	//If true, btn3 was used to trigger interrupt
	// Reset button
	else if(Status==0x08){
		clear_all_rows();
		draw_ship();
		hit_score = 0;
		miss_score = 0;
		remaining_ammo = 8;
		game_paused = false;
		game_over = false;
		game_over_timer = 0;
		usleep(300000);
	}


	//****END OF OWN CODE*****************
}



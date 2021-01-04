/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    MKL43Z256xxx4_Project_FinalProject.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL43Z4.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 *
 */

void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 8000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    printf("Hello World\n");

    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //turn on clocks for ports needed
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    //PORT CONTROL REGISTER n(PORTx_PCRn)
    PORTE -> PCR[30] = 0x100; //PORT E for IR Sensors
	PORTE -> PCR[1] = 0x100;
	PORTD -> PCR[2] = 0x100; //EN1
	PORTD -> PCR[4] = 0x100; //EN2
	PORTA -> PCR[12]= 0x100;			//LEFT MOTOR
	PORTA -> PCR [4]= 0x100;			//LEFT
	PORTA -> PCR[1] = 0x100;			//right motor
	PORTA -> PCR[13] = 0x100;			//right
	PORTE -> PCR[22]=0x100;				//light sensor

	//PORT DADA DIRECTION REGISTER 0 means input, 1 means output
	PTD->PDDR |= 0b00000000000000000000000000010100; //1 is output ... ENABLES
    PTA->PDDR |= 0b00000000000000000011000000010010; //output pins for motors

    uint32_t result = 0;
    uint32_t light = 0;

    //setting up ADC
    	//enabling clock for port B
    	SIM -> SCGC5 |= SIM_SCGC5_PORTB_MASK;
        PORTB -> PCR[0] = 0x000; //ensuring INPUT is selected from gpio
        SIM -> SCGC6 |= SIM_SCGC6_ADC0_MASK;  //Controls the clock gate to the ADC0 module.
        //ADC Configuration Register for ADCO
        ADC0 -> CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
        //Status and Control Register 2
        ADC0 -> SC2 &= ~0x40; //makes sure that its software trigger
        ADC0 -> SC2 |= 0b00000001;

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {

    			ADC0->SC1[0] = 0b00011; 		//Read Ch2 of ADC0 //ADC0 connections/channel assignment
    			while(!(ADC0->SC1[0]&0x80)){} //wait for CoCo

    	        light = ADC0->R[0];
    			//printf("result=%d\n", light); //debugging purposes

    			delay();

    	 	 	result = PTE -> PDIR; //copy
    	        if (result == 1073741824) {


    	        	turnLeft();

    	        }

    	        else if (light < 4000){

    	        	stop();
    	        }

    	        else if (result == 2) {


    	    	   turnRight();
    	       }

    	        else if ((((result & 0b000000000000000000000000000000010)==0) && ((result & 0b01000000000000000000000000000000)==0))){

    	           	forward();}


    	        else {

    	        	stop();
    	        }

    	        delay();

    }
    return 0 ;
}

void turnRight(void){


	    PTD -> PSOR |= 0b00000000000000000000000000010000; //ONE Enable OFF
	    PTA -> PCOR |= 0b00000000000000000010000000000000;

}

void turnLeft(void){

		PTD -> PSOR |= 0b00000000000000000000000000000100; //ONE Enable OFF
	   	PTA -> PCOR |= 0b00000000000000000000000000010000;
}

void forward(void){

		PTD -> PSOR |= 0b00000000000000000000000000010100; //Enables for motor set on
      	PTA->PSOR |= 0b00000000000000000010000000010000; //Run Motors
}

void stop(void){


  	   	PTD -> PSOR |= 0b00000000000000000000000000000000; //Enables for motor set on
  	    PTA->PSOR |= 0b00000000000000000000000000000000; //off

}

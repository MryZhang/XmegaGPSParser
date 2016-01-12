/*
 * GPSParsing.cpp
 *
 * Created: 3/12/2015 12:28:02 PM
 * Author : Leo Febey
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "USART.h"
#include "USART_Debug.h"
#include "GPS_Parser.h"

USART_Data uc0;
USART C0;
USART_Data gdata;
GPS_Parser gps;

void initClocks(){
	OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm;  /* Enable the internal 32MHz & 32KHz oscillators */
	while(!(OSC.STATUS & OSC_RC32KRDY_bm));       /* Wait for 32Khz oscillator to stabilize */
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));       /* Wait for 32MHz oscillator to stabilize */
	DFLLRC32M.CTRL = DFLL_ENABLE_bm ;             /* Enable DFLL - defaults to calibrate against internal 32Khz clock */
	CCP = CCP_IOREG_gc;                           /* Disable register security for clock update */
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;              /* Switch to 32MHz clock */
	OSC.CTRL &= ~OSC_RC2MEN_bm;                   /* Disable 2Mhz oscillator */
}

void restartInterrupts(){
	cli();
	PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm;
	sei();
}

void setupUSARTDevices(){
	//uc0 = {};
	uc0.port = &PORTC;
	uc0.usart_port = &USARTC0;
	uc0.txPin = PIN3_bm;
	uc0.rxPin = PIN2_bm;
	uc0.baudRate = 9600;
	
	C0 = USART(&uc0, false);

	gdata.port = &PORTE;
	gdata.usart_port = &USARTE0;
	gdata.txPin = PIN3_bm;
	gdata.rxPin = PIN2_bm;
	gdata.baudRate = 9600;

	gps = GPS_Parser(&gdata);
}

int main(void)
{
	initClocks();
	restartInterrupts();
	setupUSARTDevices();
	setDebugOutputPort(&USARTC0);
	
	printf("GPS Parser Program Starting\n");
	
    while (1) 
    {
    	gps.readNMEA();
    	if(gps.fixAvailable()){
    		if(gps.newPositionAvailable()){
    			float latitude = gps.getLatitude();
    			float longitude = gps.getLongitude();
    			printf("Position: %f, %f\n", latitude, longitude);
    		}

    	} else {
    		printf("Waiting for a fix..\n");
    	}
    }
    return 0;
}

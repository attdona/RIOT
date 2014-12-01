/*
 * dummy_main.c
 *
 *  Created on: 27/nov/2014
 *      Author: SO000112
 */
#include "board.h"
#include "gpioint.h"

uint8_t _FRAM_AREA_ out;

void greenButtonHandler(void) {
	out ^= BIT0;
	P1OUT &= ~BIT0;
	P1OUT |= BIT0 & out;
}

void redButtonHandler(void) {
	out ^= BIT6;
	P4OUT &= ~BIT6;
	P4OUT |= BIT6 & out;
}

int main(void) {

	P1OUT |= (BIT0 & out);
	P4OUT |= (BIT6 & out);

	gpioint_init();

	// set P1.1 as interrupt enabled on RISING EDGE
	gpioint_set(1, BV(1), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, greenButtonHandler);

	gpioint_set(4, BV(5), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, redButtonHandler);

	return 1;
}


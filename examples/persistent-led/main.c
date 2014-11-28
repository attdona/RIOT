/*
 * dummy_main.c
 *
 *  Created on: 27/nov/2014
 *      Author: SO000112
 */
#include "board.h"
#include "gpioint.h"

uint8_t _FRAM_AREA_ out;

void buttonHandler(void) {
	out ^= BIT0;
	P1OUT &= ~BIT0;
	P1OUT |= out;
}

int main(void) {

	P1OUT |= out;

	gpioint_init();
	// set P1.1 as interrupt enabled on RISING EDGE
	gpioint_set(1, BV(1), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, buttonHandler);

	return 1;
}


/*
 * fram-launchpad.c
 *
 *  Created on: 25/nov/2014
 *      Author: Attilio Dona'
 */
#include "greatest.h"
#include "board.h"

#ifdef CC110L_RADIO
  #include "cc110x-interface.h"
  #include "cc110x-internal.h"
#endif

#include "gpioint.h"
#include "posix_io.h"
#include "board_uart0.h"
#include "hwtimer.h"

#define CLOCKS_PER_SEC 32768

clock_t mytime = 0;

clock_t clock(void) {
	return hwtimer_now();
}

char buttonPressed = 0;

GREATEST_MAIN_DEFS();

void asino(void) {
	puts("sei un Asino!: press the other button!\n");
}

void buttonHandler(void) {
	P4OUT ^= BV(6);
	buttonPressed = 1;
}

TEST gpio() {
	ASSERT(P1OUT & BV(1));
	ASSERT_FALSE(P1DIR & BV(1));
	ASSERT(P1REN & BV(1));
	ASSERT_FALSE(P1IES & BV(1));
	ASSERT(P1IE & BV(1));

	// set P1.3 as interrupt enabled on FALLING EDGE
	gpioint_set(1, BV(3), GPIOINT_FALLING_EDGE, NULL);
	ASSERT_FALSE(P1OUT & BV(3));
	ASSERT_FALSE(P1DIR & BV(3));
	ASSERT_FALSE(P1REN & BV(3));
	ASSERT(P1IES & BV(3));
	ASSERT(P1IE & BV(3));

	// set P4.7 as interrupt disabled
	gpioint_set(4, BV(7), GPIOINT_DISABLE, NULL);
	ASSERT_FALSE(P4DIR & BV(7));
	ASSERT_FALSE(P4REN & BV(7));
	ASSERT_FALSE(P4IE & BV(7));

	PASS();
}

TEST example() {
	PASS();
}

#ifdef CC110L_RADIO
TEST checkRegisters() {
	uint8_t val;
	for(uint8_t i = 0; i < CC1100_CONF_SIZE-4; i++) {
		val = cc110x_read_reg(i);
		ASSERT_EQ(cc110x_conf[i], val);
		//printf("%x: %x EXP %x\n", i, val, cc110x_conf[i]);
	}
	PASS();
}
#endif

GREATEST_SUITE(example_suite) {
	RUN_TEST(example);
#ifdef CC110L_RADIO
	RUN_TEST(checkRegisters);
#endif
	RUN_TEST(gpio);
}


uint8_t run() {

	RESET_TEST_COUNTERS();
	//GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
	RUN_SUITE(example_suite);   /* run a suite */
	GREATEST_MAIN_END();        /* display results */

}

int main(void) {
	uint8_t result;

#ifdef CC110L_RADIO
	cc110x_init(5);
#endif

	gpioint_init();

	// set P1.1 as interrupt enabled on RISING EDGE
	gpioint_set(1, BV(1), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, buttonHandler);
	gpioint_set(4, BV(5), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, asino);

	puts("\n\nPress the P1.1 button\n");

	while(1) {
		if (buttonPressed) {
			buttonPressed = 0;
			result = run();
			if (result == EXIT_SUCCESS) {
				P1OUT |= BIT0;
			} else {
				P4OUT |= BIT6;
			}

		}
	}

}


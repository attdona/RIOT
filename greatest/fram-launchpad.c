/*
 * fram-launchpad.c
 *
 *  Created on: 25/nov/2014
 *      Author: Attilio Dona'
 */
#include "greatest.h"
#include "cc110x-interface.h"
#include "cc110x-internal.h"
#include "board.h"
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

TEST checkRegisters() {
	uint8_t val;
	for(uint8_t i = 0; i < CC1100_CONF_SIZE-4; i++) {
		val = cc110x_read_reg(i);
		ASSERT_EQ(cc110x_conf[i], val);
		//printf("%x: %x EXP %x\n", i, val, cc110x_conf[i]);
	}
	PASS();
}

TEST checkRadio() {
	uint8_t val = cc110x_read_reg(CC1100_IOCFG0);
	ASSERT_EQ(0x0E, val);

	val = cc110x_read_reg(CC1100_SYNC0);
	printf("SYNC0: %x\n", val);


	val = cc110x_read_reg(CC1100_FREND0);
	printf("FREND0: %x\n", val);
	//ASSERT_EQ(0x10, val);

	val = cc110x_read_reg(CC1100_FSCAL3);
	printf("FSCAL3: %x\n", val);

	PASS();
}


GREATEST_SUITE(example_suite) {
	RUN_TEST(example);
	RUN_TEST(checkRegisters);
	RUN_TEST(gpio);
}


uint8_t run() {

	greatest_info.assertions = 0;
	greatest_info.tests_run = 0;
	greatest_info.passed = 0;
	greatest_info.failed = 0;
	greatest_info.skipped = 0;
	greatest_info.suite.passed = 0;
	greatest_info.suite.failed = 0;
	greatest_info.suite.skipped = 0;

	//GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
	RUN_SUITE(example_suite);   /* run a suite */
	GREATEST_MAIN_END();        /* display results */

}

int main(void) {
	uint8_t result;
	char c;

    (void) posix_open(uart0_handler_pid, 0);

	cc110x_init(5);

	gpioint_init();
	// set P1.1 as interrupt enabled on RISING EDGE
	gpioint_set(1, BV(1), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, buttonHandler);
	gpioint_set(4, BV(5), GPIOINT_RISING_EDGE | GPIO_PULLUP_ENABLED, asino);

	puts("\n\nPress the P1.1 button\n");

	//posix_read(uart0_handler_pid, &c, 1);

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


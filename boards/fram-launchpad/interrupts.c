/*
 * interrupts.c
 *
 *  Created on: 26/nov/2014
 *      Author: Attilio Dona'
 */
#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "gpioint.h"
#include "hwtimer.h"
#include "cc110x_legacy.h"

extern uint8_t debounce_flags[];
extern uint16_t debounce_time[INT_PORTS][BITMASK_SIZE];

/** interrupt callbacks */
extern fp_irqcb cb[INT_PORTS][BITMASK_SIZE];

/*
 * CC110x receive interrupt
 */
ISRV(PORT1_VECTOR, cc110x_gdo0)
{
	uint8_t int_enable, ifg_num, p1ifg;
	uint16_t p1iv;
	uint16_t diff;

	__enter_isr();

	/* Debounce
	 * Disable PORT1 IRQ
	 */
	p1ifg = P1IFG;
	p1iv = P1IV;
	int_enable = P1IE;
	P1IE = 0x00;

	ifg_num = (p1iv >> 1) - 1;

	/*
	 * Guard condition
	 * Writing to PxOUT, PxDIR, or PxREN can result in setting the corresponding PxIFG flags:
	 * if this is the case P1IV can be 0x00 when the interrupt fires.
	 */
	if (ifg_num == 0xFF) {
		goto exit_point;
	}


	/* Check IFG */
	if (p1ifg & BV(CC110L_GDO0_PIN)) {
		CC110L_GDO0_PORT(IFG) &= ~BV(CC110L_GDO0_PIN);
		cc110x_gdo0_irq();
	} else {

		/* check interrupt source */
		if (debounce_flags[0] & p1ifg) {
			/* check if bouncing */
			diff = hwtimer_now() - debounce_time[0][ifg_num];

			if (diff > DEBOUNCE_TIMEOUT) {
				debounce_time[0][ifg_num] = hwtimer_now();

				if (cb[0][ifg_num] != NULL) {
					cb[0][ifg_num]();
				}
			}
			else {
				/* TODO: check for long duration irq */
				asm volatile(" nop ");
			}
		}
		else {
			if (cb[0][ifg_num] != NULL) {
				cb[0][ifg_num]();
			}
		}
	}


exit_point:
    P1IFG = 0x00;
	P1IE  = int_enable;

	__exit_isr();
}


ISRV(PORT4_VECTOR, cc110x_gdo2)
{

#if 0
    __enter_isr();

    /* Check IFG */
    if ((CC110L_GDO2_PORT(IFG) & BV(CC110L_GDO2_PIN)) != 0) {
    	CC110L_GDO2_PORT(IFG) &= ~BV(CC110L_GDO2_PIN);
        cc110x_gdo2_irq();
    }
    else {
        puts("cc110x_gdo2(): unexpected IFG!");
        /* Should not occur - only GDO1 and GDO2 interrupts are enabled */
    }

    __exit_isr();
#endif

	uint8_t int_enable, ifg_num, pifg;
	uint16_t piv;
	uint16_t diff;

	__enter_isr();

	/* Debounce
	 * Disable PORT1 IRQ
	 */
	pifg = P4IFG;
	piv = P4IV;
	int_enable = P4IE;
	P4IE = 0x00;

	ifg_num = (piv >> 1) - 1;

	/*
	 * Guard condition
	 * Writing to PxOUT, PxDIR, or PxREN can result in setting the corresponding PxIFG flags:
	 * if this is the case P1IV can be 0x00 when the interrupt fires.
	 */
	if (ifg_num == 0xFF) {
		goto exit_point;
	}


	/* Check IFG */
	if (pifg & BV(CC110L_GDO2_PIN)) {
		CC110L_GDO2_PORT(IFG) &= ~BV(CC110L_GDO2_PIN);
		cc110x_gdo2_irq();
	} else {

		/* check interrupt source */
		if (debounce_flags[0] & pifg) {
			/* check if bouncing */
			diff = hwtimer_now() - debounce_time[0][ifg_num];

			if (diff > DEBOUNCE_TIMEOUT) {
				debounce_time[0][ifg_num] = hwtimer_now();

				if (cb[3][ifg_num] != NULL) {
					cb[3][ifg_num]();
				}
			}
			else {
				/* TODO: check for long duration irq */
				asm volatile(" nop ");
			}
		}
		else {
			if (cb[3][ifg_num] != NULL) {
				cb[3][ifg_num]();
			}
		}
	}

exit_point:
    P4IFG = 0x00;
	P4IE  = int_enable;

	__exit_isr();
}



/*
 * Copyright 2010, Freie Universit√§t Berlin (FUB).
 * Copyright 2013, INRIA.
 * Copyright 2014, piccinoLab
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     msp430fram
 * @file        gpioint.c
 * @brief       MSP430FR5969 GPIO Interrupt Multiplexer implementation
 * @author      Attilio Don‡
 */

#include <stdlib.h>
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
 #include <legacymsp430.h>
#endif
#include "gpioint.h"
#include "cpu.h"
#include "irq.h"
#include "hwtimer.h"
#include "bitarithm.h"


/** interrupt callbacks */
fp_irqcb cb[INT_PORTS][BITMASK_SIZE];

/** debounce interrupt flags */
uint8_t debounce_flags[INT_PORTS];

/** debounce interrupt times */
uint16_t debounce_time[INT_PORTS][BITMASK_SIZE];

uint16_t c1 = 0, c2 = 0;


void gpio_config_port(volatile unsigned char *portaddr, int flags, uint8_t bitmask) {

	portaddr += 2;
	if (flags & GPIO_PULLUP_ENABLED) {
		*portaddr |= bitmask;  // PxOUT pullup selected
	} else {
		*portaddr &= ~bitmask;  // PxOUT
	}

	/* set port to input */
    portaddr += 2;
    *portaddr &= ~bitmask;  // PxDIR

    /* enable internal pull-down */
    portaddr += 2;
    if(flags & (GPIO_PULLUP_ENABLED | GPIO_PULLDOWN_ENABLED)) {
    	*portaddr |= bitmask; // PxREN pullup or pulldown enabled
    } else {
    	*portaddr &= ~bitmask;
    }

    /* trigger on rising... */
    portaddr += 18;
    if (flags & GPIOINT_RISING_EDGE) {
    	*portaddr &= ~bitmask; // PxIES
    }

    /* ...or falling edge */
    if (flags & GPIOINT_FALLING_EDGE) {
    	*portaddr |= bitmask; // PxIES
    }

    portaddr += 2;
    if (flags == GPIOINT_DISABLE) {
    	/*  disable interrupt */
    	*portaddr &= ~bitmask; //PxIE
    } else {
    	/* enable interrupt */
    	*portaddr |= bitmask;  //PxIE
    }

    /* reset IRQ flag */
    portaddr += 2;
    *portaddr &= ~bitmask;  // PxIFG

}


void gpioint_init(void)
{
    uint8_t i, j;

    for (i = 0; i < INT_PORTS; i++) {
        for (j = 0; j < BITMASK_SIZE; j++) {
            cb[i][j] = NULL;
            debounce_time[i][j] = 0;
        }
    }
}

bool gpioint_set(int port, uint32_t bitmask, int flags, fp_irqcb callback)
{
    if ((port >= PORTINT_MIN) && (port <= PORTINT_MAX)) {
        /* set the callback function */
        int8_t base = bitarithm_msb(bitmask);

        if (base >= 0) {
            cb[port - PORTINT_MIN][base] = callback;
        }
        else {
            return false;
        }

        if (flags & GPIOINT_DEBOUNCE) {
            debounce_flags[port - PORTINT_MIN] |= bitmask;
        }
        else {
            debounce_flags[port - PORTINT_MIN] &= ~bitmask;
        }
    }

    switch(port) {
        case 1:
        	gpio_config_port((unsigned char*)&P1IN, flags, bitmask);
        	break;
        case 2:
        	gpio_config_port((unsigned char*)&P2IN, flags, bitmask);
        	break;
        case 3:
        	gpio_config_port((unsigned char*)&P3IN, flags, bitmask);
        	break;
        case 4:
        	gpio_config_port((unsigned char*)&P4IN, flags, bitmask);
        	break;
        default:
        	return false;
    }

#if 0
    switch(port) {
        case 1:
            /* set port to input */
            P1DIR &= ~bitmask;
            /* enable internal pull-down */
            P1OUT &= ~bitmask;
            P1REN |= bitmask;

            /* reset IRQ flag */
            P1IFG &= ~bitmask;

            /* trigger on rising... */
            if (flags & GPIOINT_RISING_EDGE) {
                P1IES &= bitmask;
            }

            /* ...or falling edge */
            if (flags & GPIOINT_FALLING_EDGE) {
                P1IES |= bitmask;
            }

            /*  disable interrupt */
            if (flags == GPIOINT_DISABLE) {
                P1IE &= ~bitmask;
            }

            /* enable interrupt */
            P1IE |= bitmask;
            break;

        case 2:
            /* set port to input */
            P2DIR &= ~bitmask;
            /* enable internal pull-down */
            P2OUT &= ~bitmask;
            P2REN |= bitmask;

            /* reset IRQ flag */
            P2IFG &= ~bitmask;

            /* trigger on rising... */
            if (flags == GPIOINT_RISING_EDGE) {
                P2IES &= bitmask;
            }
            /* ...or falling edge */
            else if (flags == GPIOINT_FALLING_EDGE) {
                P2IES |= bitmask;
            }
            /* or disable interrupt */
            else {
                P2IE &= ~bitmask;
            }

            /* enable interrupt */
            P2IE |= bitmask;
            break;

        default:
            return false;
    }
#endif

    return 1;
}

#if 0
ISRV(PORT1_VECTOR, port1_isr)
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

    P1IFG = 0x00;
    P1IE  = int_enable;
    __exit_isr();
}


//interrupt(PORT2_VECTOR) __attribute__((naked)) port2_isr(void)
ISRV(PORT2_VECTOR, port2_isr)
{
    uint8_t int_enable, ifg_num, p2ifg;
    uint16_t p2iv;
    uint16_t diff;
    __enter_isr();

    /* Debounce
     * Disable PORT2 IRQ
     */
    p2ifg = P2IFG;
    p2iv = P2IV;
    int_enable = P2IE;
    P2IE = 0x00;

    ifg_num = (p2iv >> 1) - 1;

    /* check interrupt source */
    if (debounce_flags[1] & p2ifg) {
        /* check if bouncing */
        diff = hwtimer_now() - debounce_time[1][ifg_num];

        if (diff > DEBOUNCE_TIMEOUT) {
            debounce_time[1][ifg_num] = hwtimer_now();
            c1++;

            if (cb[1][ifg_num] != NULL) {
                cb[1][ifg_num]();
            }
        }
        else {
            c2++;
            /* TODO: check for long duration irq */
            asm volatile(" nop ");
        }
    }
    else {
        if (cb[1][ifg_num] != NULL) {
            cb[1][ifg_num]();
        }
    }


    P2IFG = 0x00;
    P2IE  = int_enable;
    __exit_isr();
}
#endif

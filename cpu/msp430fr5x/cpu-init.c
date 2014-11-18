/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Modified by Kaspar Schleiser
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file        msp430-main.c
 * @brief       MSP430 CPU initialization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "cpu.h"

/*---------------------------------------------------------------------------*/
static void init_ports(void)
{
    /* Turn everything off, device drivers enable what is needed. */

    /* All configured for digital I/O */
    P1SEL0 = 0;
    P1SEL1 = 0;

    P2SEL0 = 0;
    P2SEL1 = 0;

    P3SEL0 = 0;
    P3SEL1 = 0;

    P4SEL0 = 0;
    P4SEL1 = 0;

    PJSEL0 = 0;
    PJSEL1 = 0;

    /* All available inputs */

    PADIR = 0x0000;
    PAREN = 0xFFFF;
    PAOUT = 0x0000;

    PBDIR = 0x0000;
    PBREN = 0xFFFF;
    PBOUT = 0x0000;

    PJDIR = 0x00;
    PJREN = 0xFF;
    PJOUT =  0x00;
    PJSEL0 |= 0x10;     /* PJ.4 Configured for ext clock function on these pins */

    P1IE = 0;
    P2IE = 0;
    P3IE = 0;
    P4IE = 0;

}

/*---------------------------------------------------------------------------*/
/* msp430-ld may align _end incorrectly. Workaround in cpu_init. */
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
#define end _end
#endif

extern int end; /* Not in sys/unistd.h */

static char *cur_break = (char *) &end;

void msp430_cpu_init(void)
{

    dint();
    init_ports();
    //  lpm_init();

    WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog

    // Configure GPIO
    P2SEL1 |= BIT0 | BIT1;                    // USCI_A0 UART operation
    P2SEL0 &= ~(BIT0 | BIT1);

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    eint();

    if ((uintptr_t) cur_break & 1) { /* Workaround for msp430-ld bug!*/
        cur_break++;
    }
}

/*---------------------------------------------------------------------------*/
#define asmv(arg) __asm__ __volatile__(arg)

#define STACK_EXTRA 32

/*
 * Allocate memory from the heap. Check that we don't collide with the
 * stack right now (some other routine might later). A watchdog might
 * be used to check if cur_break and the stack pointer meet during
 * runtime.
 */
void *sbrk(int incr)
{
    char *stack_pointer;

asmv("mov r1, %0" : "=r"(stack_pointer));
    stack_pointer -= STACK_EXTRA;

    if (incr > (stack_pointer - cur_break)) {
        return (void *) - 1; /* ENOMEM */
    }

    void *old_break = cur_break;
    cur_break += incr;
    /*
     * If the stack was never here then [old_break .. cur_break] should
     * be filled with zeros.
     */
    return old_break;
}
/*---------------------------------------------------------------------------*/
/*
 * Mask all interrupts that can be masked.
 */
int splhigh_(void)
{
    /* Clear the GIE (General Interrupt Enable) flag. */
    int sr;
asmv("mov r2, %0" : "=r"(sr));
asmv("bic %0, r2" : : "i"(GIE));
    return sr & GIE; /* Ignore other sr bits. */
}
/*---------------------------------------------------------------------------*/
/*
 * Restore previous interrupt mask.
 */
void splx_(int sr)
{
    /* If GIE was set, restore it. */
asmv("bis %0, r2" : : "r"(sr));
}
/*---------------------------------------------------------------------------*/

extern void board_init(void);

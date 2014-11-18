/*
 * Copyright (C) 2014, Freie Universitaet Berlin (FUB) & INRIA.
 * All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _CPU_H
#define _CPU_H

#define eint __eint
#define  dint __dint

/**
 * @defgroup    msp430 TI MSP430
 * @ingroup     cpu
 * @brief       Texas Instruments MSP430 specific code

<h2>First steps</h2>
\li See the <a href="../manual/index.html">manual</a> for toolchain and ide setup

 * @{
 */

//#include <stdio.h>

#include <msp430.h>
#include "board.h"

#include "sched.h"
#include "msp430_types.h"
#include "cpu-conf.h"

#define WORDSIZE 16

extern volatile int __inISR;
extern char __isr_stack[MSP430_ISR_STACK_SIZE];

inline void __save_context_isr(void)
{
    __asm__("pushm.w #12,R15");
    __asm__("mov.w r1,%0" : "=r"(sched_active_thread->sp));
}

inline void __restore_context_isr(void)
{
    __asm__("mov.w %0,r1" : : "m"(sched_active_thread->sp));
    __asm__("popm.w #12,R15");
}


inline void __enter_isr(void)
{
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
    __save_context_isr();
#else
    __asm__("mov.w r1,%0" : "=r"(sched_active_thread->sp));
#endif

    __asm__("mov.w %0,r1" : : "i"(__isr_stack+MSP430_ISR_STACK_SIZE));
    __inISR = 1;
}

inline void __exit_isr(void)
{
    __inISR = 0;

    if (sched_context_switch_request) {
        sched_run();
    }

#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
    __restore_context_isr();
    __asm__("reti");
#else
    __asm__("mov.w %0,r1" : : "m"(sched_active_thread->sp));
#endif


}

inline void __save_context(void)
{
    __asm__("push r2"); /* save SR */
    __save_context_isr();
}

inline void __restore_context(unsigned int irqen)
{
    __restore_context_isr();

    /*
     * we want to enable appropriate IRQs *just after*
     * quitting the interrupt handler; to that end,
     * we change the GIE bit in the value to be restored
     * in R2 (a.k.a. SR) by the next RETI instruction
     */
    if (irqen) {
        __asm__("bis.w #8, 0(r1)");
    }
    else {
        __asm__("bic.w #8, 0(r1)");
    }

    __asm__("reti");
}

inline void eINT(void)
{
    //    puts("+");
    eint();
}

inline void dINT(void)
{
    //    puts("-");
    dint();
}

int inISR(void);

void msp430_cpu_init(void);

/** @} */
#endif // _CPU_H

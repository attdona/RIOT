/*
 * uart0.c - Implementation of the uart.
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "board.h"

#define   UART0_TX                  UCA0TXBUF
#define   UART0_WAIT_TXDONE()       while(!(UCA0IFG&UCTXIFG))

#include "kernel.h"

#include "board_uart0.h"

#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
 #define even_in_range(A,RANGE) A
#else
 #define even_in_range(A,RANGE) __even_in_range(A, RANGE)
#endif

int putchar(int c)
{
    UART0_TX = c;
    UART0_WAIT_TXDONE();

    if (c == 10) {
        UART0_TX = 13;
        UART0_WAIT_TXDONE();
    }

    return c;
}

void usart0irq(void);
/**
 * \brief the interrupt function
 */
//interrupt(USART0RX_VECTOR) usart0irq(void) {
ISRV(USCI_A0_VECTOR, usart0irq) {

	__enter_isr();

    int dummy = 0;

    switch(even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
      case USCI_NONE: break;
      case USCI_UART_UCRXIFG:
        while(!(UCA0IFG&UCTXIFG));
        dummy = UCA0RXBUF;
        __no_operation();
        break;
      case USCI_UART_UCTXIFG: break;
      case USCI_UART_UCSTTIFG: break;
      case USCI_UART_UCTXCPTIFG: break;
    }
    // TODO:  Clear error flags by forcing a dummy read ?
    // dummy = UCA0RXBUF;

    if (uart0_handler_pid != KERNEL_PID_UNDEF) {
    	uart0_handle_incoming(dummy);
    	uart0_notify_thread();
    }

    __exit_isr();
}

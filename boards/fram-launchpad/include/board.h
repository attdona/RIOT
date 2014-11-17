/*
 * Copyright (C) 2014 piccino Lab <piccino.lab@gmail.com>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 */

/**
 * @defgroup    MSP-EXP430FR5969 LaunchPad
 * @ingroup     boards
 * @brief       Support for the MSP-EXP430FR5969 LaunchPad board
 *
 * <h2>Compontents</h2>
 * \li MSP430FR5969
 * \li CC110L
 *
 * @{
 *
 * @file        board.h
 * @brief       Basic definitions for the Senslab WSN430 v1.3b board
 *
 * @author      Attilio Dona' <attilio.dona@gmail.com>
 */

#ifndef _WSN_BOARD_H
#define _WSN_BOARD_H

#include <stdint.h>
#include <msp430.h>

#ifndef BV
#define BV(x) (1 << x)
#endif

#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
 #define ISRV(a,b) void __attribute__((interrupt (a))) __attribute__((naked)) b(void)
#else
 #define ISRV(a,b) void __attribute__((interrupt(a))) b(void)
 #define __bic_status_register   _bic_SR_register
 #define __bis_status_register   _bis_SR_register
 #define __read_status_register  _get_SR_register
#endif

//numbers of A Timers
#define __MSP430_HAS_T0A5__

// used in msp430-common/hwtimer_cpu.c
#define TIMER_CCTL0    TB0CCTL0
#define TIMER_CCR0     TB0CCR0
#define TIMER_COUNTER  TBR


// for correct inclusion of <msp430.h>
//#ifndef __MSP430FR5969__
//#define __MSP430FR5969__
//#endif

//MSB430 core
#define MSP430_INITIAL_CPU_SPEED    800000uL
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

/* LEDs ports MSB430 */
#define LEDS_PxDIR P5DIR
#define LEDS_PxOUT P5OUT
#define LEDS_CONF_RED       0x04
#define LEDS_CONF_GREEN     0x05
#define LEDS_CONF_BLUE  0x06

#define LED_RED_ON          LEDS_PxOUT &=~LEDS_CONF_RED
#define LED_RED_OFF         LEDS_PxOUT |= LEDS_CONF_RED
#define LED_RED_TOGGLE      LEDS_PxOUT ^= LEDS_CONF_RED

#define LED_GREEN_ON        LEDS_PxOUT &=~LEDS_CONF_GREEN
#define LED_GREEN_OFF       LEDS_PxOUT |= LEDS_CONF_GREEN
#define LED_GREEN_TOGGLE    LEDS_PxOUT ^= LEDS_CONF_GREEN

#define LED_BLUE_ON         LEDS_PxOUT &=~LEDS_CONF_BLUE
#define LED_BLUE_OFF        LEDS_PxOUT |= LEDS_CONF_BLUE
#define LED_BLUE_TOGGLE     LEDS_PxOUT ^= LEDS_CONF_BLUE



typedef uint8_t radio_packet_length_t;


/*
 * SPI bus configuration
 */
#define CC110L_GDO0_PORT(type) P1##type
#define CC110L_GDO0_PIN        2

// shared with MISO
#define CC110L_GDO1_PORT(type) P1##type
#define CC110L_GDO1_PIN        7


#define CC110L_GDO2_PORT(type) P4##type
#define CC110L_GDO2_PIN        2

#define CC110L_SPI_CSN_PORT(type)  P3##type
#define CC110L_SPI_CSN_PIN     0
#define CC110L_SPI_MOSI_PORT(type)  P1##type
#define CC110L_SPI_MOSI_PIN    6
#define CC110L_SPI_MISO_PORT(type)  P1##type
#define CC110L_SPI_MISO_PIN    7
#define CC110L_SPI_SCLK_PORT(type)  P2##type
#define CC110L_SPI_SCLK_PIN    2


/* SPI input/output registers. */
#define SPI_TXBUF UCB0TXBUF
#define SPI_RXBUF UCB0RXBUF


/** @} */
#endif // _MSP430FR_BOARD_H

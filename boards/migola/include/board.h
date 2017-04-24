/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_migola Migola board
 * @ingroup     boards
 * @brief       Board specific files for the Migola
 * @{
 *
 * @file
 * @brief       Board configuration for the Migola board
 *
 * @author      Attilio Dona'
 *
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the nominal CPU core clock in this board
 */
#define F_CPU               (16000000UL)

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)

#define XTIMER_WIDTH                16

#define XTIMER_SHIFT_ON_COMPARE     (2)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    Define the boards stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define ONBOARD_LED         1
#define LED_RED_PIN         18 //(1 << 18)
#define LED_GREEN_PIN       19 //(1 << 19)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_BIT         (1 << 18)
#define LED_GREEN_BIT       (1 << 19)

#define LED_RED_ON          (NRF_GPIO->OUTSET = LED_RED_BIT)
#define LED_RED_OFF         (NRF_GPIO->OUTCLR = LED_RED_BIT)
#define LED_RED_TOGGLE      (NRF_GPIO->OUT ^= LED_RED_BIT)
#define LED_GREEN_ON        (NRF_GPIO->OUTSET = LED_GREEN_BIT)
#define LED_GREEN_OFF       (NRF_GPIO->OUTCLR = LED_GREEN_BIT)
#define LED_GREEN_TOGGLE    (NRF_GPIO->OUT ^= LED_GREEN_BIT)
/** @} */

/**
 * @brief   Initialize the board, including triggering the CPU initialization
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */

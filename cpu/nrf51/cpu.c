/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"


#ifdef SOFTDEVICE_PRESENT
#include "softdevice_handler.h"
uint8_t _ble_evt_buffer[BLE_STACK_EVT_MSG_BUF_SIZE];
#endif


/** @brief Function starting the internal LFCLK XTAL oscillator.
 */
static void lfclk_config(void) {
	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;

	NRF_CLOCK->TASKS_HFCLKSTOP = 1;

	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
		//Do nothing.
	}
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}


/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{

	/* initialize the Cortex-M core */
    cortexm_init();
    /* set the correct clock source for HFCLK */
#if CLOCK_CRYSTAL == 32
    NRF_CLOCK->XTALFREQ = CLOCK_XTALFREQ_XTALFREQ_32MHz;
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
#elif CLOCK_CRYSTAL == 16
    NRF_CLOCK->XTALFREQ = CLOCK_XTALFREQ_XTALFREQ_16MHz;
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {}
#endif

    /* softdevice needs to be enabled from ISR context */
#ifdef SOFTDEVICE_PRESENT
#ifndef DEVELHELP
	lfclk_config();
#endif

    softdevice_handler_init(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, &_ble_evt_buffer,
            BLE_STACK_EVT_MSG_BUF_SIZE, NULL);
#endif

}

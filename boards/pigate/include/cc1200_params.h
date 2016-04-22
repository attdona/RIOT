/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_pigate
 * @{
 *
 * @file
 * @brief     cc1200 board specific configuration
 *
 * @author    Attilio Dona'
 */

#ifndef CC1200_PARAMS_H
#define CC1200_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#define GDO0_PIN 19

#define GDO2_PIN 2

/**
 * @name CC1200 configuration
 * @brief pin mapping for CC110L Anaren Boosterpack
 * @{
 */
const cc1200_params_t cc1200_params[] = {
    {
        .spi  = SPI_0,          //<! spi module instance
        .cs   = SPI_0_PIN_CS,   //<! chip select
        .gdo0 = GDO0_PIN,       //<! GDO0 pin
        .gdo1 = SPI_0_PIN_MISO, //<! GDO1 pin is shared with MISO
        .gdo2 = GDO2_PIN        //<! GDO2 pin
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC1200_PARAMS_H */
/** @} */

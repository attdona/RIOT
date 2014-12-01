/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_cc110x_legacy
 * @{
 *
 * @file
 * @brief   TI Chipcon CC110x default settings
 *
 * @author    Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author    Heiko Will <hwill@inf.fu-berlin.de>
 * @author    Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "cc110x_legacy.h"
#include "board.h"

// Default PA table index (output power)
#define PATABLE                 (11)

/**
 * Usable, non overlapping channels and corresponding frequencies
 * for use with CC1100. CHANNR is the register for selecting a channel.
 *
 * channel number | CHANNR | frequency [MHz]
 * -----------------------------------------
 *              0 |      0 | 869.525
 *              1 |     10 | 871.61
 *              2 |     20 | 873.58     ~ seems to be bad (hang-ups with this channel)
 *              3 |     30 | 875.61
 *              4 |     40 | 877.58
 *              5 |     50 | 879.61
 *              6 |     60 | 881.58
 *              7 |     70 | 883.61
 *              8 |     80 | 885.58
 *              9 |     90 | 887.61
 *             10 |    100 | 889.58
 *             11 |    110 | 891.57
 *             12 |    120 | 893.58
 *             13 |    130 | 895.61
 *             14 |    140 | 897.58
 *             15 |    150 | 899.57
 *             16 |    160 | 901.57
 *             17 |    170 | 903.61
 *             18 |    180 | 905.57
 *             19 |    190 | 907.57
 *             20 |    200 | 909.57
 *             21 |    210 | 911.57
 *             22 |    220 | 913.57
 *             23 |    230 | 915.61
 *             24 |    240 | 917.61
 */
#ifdef CC110L_RADIO
const char cc110x_conf[] = {
    0x06,  // IOCFG2 Asserts when sync word has been sent/received, de-asserts at the end of the packet
    0x2E,  // IOCFG1 High impedance (3-state)
    0x0E,  // IOCFG0 Carrier sense. High if RSSI level is above threshold. Cleared when entering IDLE mode
    0x47,   // FIFOTHR            RX FIFO and TX FIFO Thresholds
    0xD3,   // SYNC1              Sync Word, High Byte
    0x91,   // SYNC0              Sync Word, Low Byte
    0xFF,   // PKTLEN             Packet Length
    0x04,   // PKTCTRL1           Packet Automation Control
    0x05,   // PKTCTRL0           Packet Automation Control
    0x00,   // ADDR               Device Address
    0x00,   // CHANNR             Channel number
    0x06,   // FSCTRL1            Frequency Synthesizer Control
    0x00,   // FSCTRL0            Frequency Synthesizer Control
    0x20,   // FREQ2              Frequency Control Word, High Byte
    0x25,   // FREQ1              Frequency Control Word, Middle Byte
    0xED,   // FREQ0              Frequency Control Word, Low Byte
    0xF5,   // MDMCFG4            Modem Configuration
    0x75,   // MDMCFG3            Modem Configuration
    0x03,   // MDMCFG2            Modem Configuration
    0x22,   // MDMCFG1            Modem Configuration
    0xE5,   // MDMCFG0            Modem Configuration
    0x14,   // DEVIATN            Modem Deviation Setting
    0x07,   // MCSM2              Main Radio Control State Machine Configuration
    0x30,   // MCSM1              Main Radio Control State Machine Configuration
    0x18,   // MCSM0              Main Radio Control State Machine Configuration
    0x16,   // FOCCFG             Frequency Offset Compensation Configuration
    0x6C,   // BSCFG              Bit Synchronization Configuration
    0x03,   // AGCCTRL2           AGC Control
    0x40,   // AGCCTRL1           AGC Control
    0x91,   // AGCCTRL0           AGC Control
    0x00,
    0x00,
    0xFB,   // RESERVED_0X20      Use setting from SmartRF Studio
    0x56,   // FREND1             Front End RX Configuration
    0x10,   // FREND0             Front End TX Configuration
    0xE9,   // FSCAL3             Frequency Synthesizer Calibration
    0x2A,   // FSCAL2             Frequency Synthesizer Calibration
    0x00,   // FSCAL1             Frequency Synthesizer Calibration
    0x1F,   // FSCAL0             Frequency Synthesizer Calibration
};

#if 0
// 1.2 kbps, 2-FSK, X-tal: 27 MHz
const char cc110x_conf[] = {
    0x06,  // IOCFG2 Asserts when sync word has been sent/received, de-asserts at the end of the packet
    0x2E,  // IOCFG1 High impedance (3-state)
    0x0E,  // IOCFG0 Carrier sense. High if RSSI level is above threshold. Cleared when entering IDLE mode
    0x4F,  // FIFOTHR
    0xD3,  // SYNC1
    0x91,  // SYNC0
    0x3D,  // PKTLEN
    0x06,  // PKTCTRL1
    0x45,  // PKTCTRL0
    0xFF,  // ADDR
    0x00,  // CHANNR
    0x06,  // FSCTRL1
    0x00,  // FSCTRL0
    0x20,  // FREQ2
    0x25,  // FREQ1
    0xED,  // FREQ0
    0xF5,  // MDMCFG4
    0x75,  // MDMCFG3
    0x03,  // MDMCFG2
    0x22,  // MDMCFG1
    0xE5,  // MDMCFG0
    0x14,  // DEVIATN
    0x07,  // MCSM2
    0x32,  // 0x30 MCSM1
    0x18,  // MCSM0
    0x16,  // FOCCFG
    0x6C,  // BSCFG
    0x03,  // AGCCTRL2
    0x40,  // AGCCTRL1
    0x91,  // AGCCTRL0
    0xFB,  // RESERVED_0X20
    0x56,  // FREND1
    0x10,  // FREND0
    0xE9,  // FSCAL3
    0x2A,  // FSCAL2
    0x00,  // FSCAL1
    0x1F  // FSCAL0
};
#endif
#else
// 400 kbps, MSK, X-tal: 26 MHz (Chip Revision F)
const char cc110x_conf[] = {
    0x06, // IOCFG2
    0x2E, // IOCFG1
    0x0E, // IOCFG0
    0x0F, // FIFOTHR
    0x9B, // SYNC1
    0xAD, // SYNC0
    0x3D, // PKTLEN     (maximum value of packet length byte = 61)
    0x06, // PKTCTRL1
    0x45, // PKTCTRL0   (variable packet length)
    0xFF, // ADDR
    CC1100_DEFAULT_CHANNR * 10, // CHANNR
    0x0B, // FSCTRL1
    0x00, // FSCTRL0
    0x21, // FREQ2
    0x71, // FREQ1
    0x7A, // FREQ0
    0x2D, // MDMCFG4
    0xF8, // MDMCFG3
    0x73, // MDMCFG2
    0x42, // MDMCFG1
    0xF8, // MDMCFG0
    0x00, // DEVIATN
    0x07, // MCSM2
    0x03, // MCSM1
    0x18, // MCSM0
    0x1D, // FOCCFG
    0x1C, // BSCFG
    0xC0, // AGCCTRL2
    0x49, // AGCCTRL1, (old value was 0x49 -> made carrier sense less sensitive!)
    //            0x47 - 7 dB above MAGN_TARGET setting
    0xB2, // AGCCTRL0
    0x87, // WOREVT1
    0x6B, // WOREVT0
    0xF8, // WORCTRL
    0xB6, // FREND1
    0x10, // FREND0
    0xEA, // FSCAL3
    0x2A, // FSCAL2
    0x00, // FSCAL1
    0x1F, // FSCAL0
    0x00  // padding to 4 bytes
};
#endif

uint8_t pa_table_index = PATABLE; ///< Current PATABLE Index
uint8_t const pa_table[] = {        ///< PATABLE with available output powers
    0x00,         ///< -52 dBm
    0x03,         ///< -30 dBm
    0x0D,         ///< -20 dBm
    0x1C,         ///< -15 dBm
    0x34,         ///< -10 dBm
    0x57,         ///< - 5 dBm
    0x3F,         ///< - 1 dBm
    0x8E,         ///<   0 dBm
    0x85,         ///< + 5 dBm
    0xCC,         ///< + 7 dBm
    0xC6,         ///< + 9 dBm
    0xC3          ///< +10 dBm
}; // If PATABLE is changed in size, adjust MAX_OUTPUT_POWER definition in CC1100 interface!

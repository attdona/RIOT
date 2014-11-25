/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        bitarithm.h
 * @brief       Helper functions for bit arithmetic
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef BITARITHM_H_
#define BITARITHM_H_

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @def SETBIT
 * @brief Sets a bitmask for a bitfield
 *
 * @param[in] val   The bitfield
 * @param[in] bit   Specifies the bits to be set
 *
 * @return The modified bitfield
 */
#define SETBIT(val, bit)    val |= (bit)

/**
 * @def CLRBIT
 * @brief Clears bitmask for a bitfield
 *
 * @param[in] val   The bitfield
 * @param[in] bit   Specifies the bits to be cleared
 *
 * @return The modified bitfield
 */
#define CLRBIT(val, bit)    val &= (~(bit))

/**
 * @name    Single Bit Defines
 * @{
 */
#ifndef BIT0
#define BIT0  0x00000001
#endif
#ifndef BIT1
#define BIT1  0x00000002
#endif
#ifndef BIT2
#define BIT2  0x00000004
#endif
#ifndef BIT3
#define BIT3  0x00000008
#endif
#ifndef BIT4
#define BIT4  0x00000010
#endif
#ifndef BIT5
#define BIT5  0x00000020
#endif
#ifndef BIT6
#define BIT6  0x00000040
#endif
#ifndef BIT7
#define BIT7  0x00000080
#endif
#ifndef BIT8
#define BIT8  0x00000100
#endif
#ifndef BIT9
#define BIT9  0x00000200
#endif
#ifndef BIT10
#define BIT10 0x00000400
#endif
#ifndef BIT11
#define BIT11 0x00000800
#endif
#ifndef BIT12
#define BIT12 0x00001000
#endif
#ifndef BIT13
#define BIT13 0x00002000
#endif
#ifndef BIT14
#define BIT14 0x00004000
#endif
#ifndef BIT15
#define BIT15 0x00008000
#endif
#ifndef BIT16
#define BIT16 0x00010000
#endif
#ifndef BIT17
#define BIT17 0x00020000
#endif
#ifndef BIT18
#define BIT18 0x00040000
#endif
#ifndef BIT19
#define BIT19 0x00080000
#endif
#ifndef BIT20
#define BIT20 0x00100000
#endif
#ifndef BIT21
#define BIT21 0x00200000
#endif
#ifndef BIT22
#define BIT22 0x00400000
#endif
#ifndef BIT23
#define BIT23 0x00800000
#endif
#ifndef BIT24
#define BIT24 0x01000000
#endif
#ifndef BIT25
#define BIT25 0x02000000
#endif
#ifndef BIT26
#define BIT26 0x04000000
#endif
#ifndef BIT27
#define BIT27 0x08000000
#endif
#ifndef BIT28
#define BIT28 0x10000000
#endif
#ifndef BIT29
#define BIT29 0x20000000
#endif
#ifndef BIT30
#define BIT30 0x40000000
#endif
#ifndef BIT31
#define BIT31 0x80000000
#endif

/** @} */

#define ARCH_32_BIT   (__INT_MAX__ == 2147483647) /**< 1 for 32 bit architectures, 0 otherwise */

/**
 * @brief   Returns the number of the highest '1' bit in a value
 * @param[in]   v   Input value
 * @return          Bit Number
 *
 * Source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
 */
unsigned bitarithm_msb(unsigned v);

/**
 * @brief   Returns the number of the lowest '1' bit in a value
 * @param[in]   v   Input value - must be unequal to '0', otherwise the
 *                  function will produce an infinite loop
 * @return          Bit Number
 *
 * Source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
 */
unsigned bitarithm_lsb(register unsigned v);

/**
 * @brief   Returns the number of bits set in a value
 * @param[in]   v   Input value
 * @return          Number of set bits
 *
 * Source: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
 */
unsigned bitarithm_bits_set(unsigned v);

#ifdef __cplusplus
}
#endif

#endif /* BITARITHM_H_ */
/** @} */

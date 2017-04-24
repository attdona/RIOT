/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_nrf51
 * @{
 *
 * @file
 * @brief           Memory definitions for the NRF51822XXAA
 *
 * @author          Attilio Dona'
 *
 * @}
 */

MEMORY
{
  rom (rx) : ORIGIN = 0x16000, LENGTH = 0x2a000
  ram (rwx) :  ORIGIN = 0x20002000, LENGTH = 0x2000
}

SECTIONS
{
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > ram
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(.pwr_mgmt_data))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > ram
} INSERT AFTER .relocate;

INCLUDE cortexm_base.ld

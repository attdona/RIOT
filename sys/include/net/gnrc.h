/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc    Generic (gnrc) network stack.
 * @ingroup     net
 * @brief       RIOT's modular default IP network stack.
 * @{
 *
 * @file
 * @brief       Pseudo header that includes all gnrc network stack base modules
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_NETBASE_H_
#define NG_NETBASE_H_

#include "net/netopt.h"
#include "net/ng_netdev.h"
#include "net/ng_netapi.h"
#include "net/ng_netreg.h"
#include "net/ng_nettype.h"
#include "net/ng_netif.h"
#include "net/ng_netif/hdr.h"
#include "net/ng_pktbuf.h"
#include "net/ng_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* this file does not provide anything on it's own */

#ifdef __cplusplus
}
#endif

#endif /* NG_NETBASE_H_ */
/** @} */
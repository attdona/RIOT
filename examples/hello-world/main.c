/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

void * memset(void * buff, int val, size_t len) {
	while(len--) {
		((char *)buff)[len] = val;
	}
	return buff;
}

void * memcpy(void* target, const void *source, size_t len) {
	return target;
}

int main(void)
{
	//startup();

    puts("Hello World!");

    //printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    //printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    return 0;
}

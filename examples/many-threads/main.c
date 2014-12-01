/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "board.h"

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "msg.h"
#include "vtimer.h"

// TODO insert if GCC ...
//#undef putchar

#define ENABLE_DEBUG (1)
#include "debug.h"

#define TWEET_SIZE 32

#define JUMBO_STACKSIZE 4096

char _FRAM_AREA_ thread_1_stack[JUMBO_STACKSIZE];
char _FRAM_AREA_ thread_2_stack[JUMBO_STACKSIZE];

char shell_stack[KERNEL_CONF_STACKSIZE_DEFAULT];


void *thread_1(void *arg)
{
    (void) arg;
    char verso[TWEET_SIZE] = "o mia bella mora";

    printf("2nd thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;

    while (1) {
        msg_receive(&m);
        printf("1st: Got msg from %" PRIkernel_pid "\n", m.sender_pid);
        m.content.ptr = verso;
        msg_reply(&m, &m);
    }

    return NULL;
}

void *thread_2(void *arg)
{
    (void) arg;
    char verso[TWEET_SIZE] = "ti voglio al più presto sposar";

    printf("2nd thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;

    while (1) {
        msg_receive(&m);
        printf("2nd: Got msg from %" PRIkernel_pid "\n", m.sender_pid);
        m.content.ptr = verso;
        msg_reply(&m, &m);
    }

    return NULL;
}



static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

void *start_shell(void* arg) {
	(void) arg;
    shell_t shell;

    (void) posix_open(uart0_handler_pid, 0);

    shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);

    shell_run(&shell);

}

int main(void)
{
	timex_t sleep_time;
	sleep_time.microseconds = 0;
	sleep_time.seconds = 1;

	msg_t m;

    //(void) posix_open(uart0_handler_pid, 0);

    kernel_pid_t shell = thread_create(shell_stack, sizeof(shell_stack),
                            PRIORITY_MAIN + 1, CREATE_STACKTEST,
                            start_shell, NULL, "shell");

    DEBUG("shell pid: %u\n", shell);

    kernel_pid_t pid1 = thread_create(thread_1_stack, sizeof(thread_1_stack),
                            PRIORITY_MAIN + 1, CREATE_STACKTEST,
                            thread_1, NULL, "thread 1");

    DEBUG("thread_1_stack pid: %u\n", pid1);

    kernel_pid_t pid2 = thread_create(thread_2_stack, sizeof(thread_2_stack),
                            PRIORITY_MAIN + 1, CREATE_STACKTEST,
                            thread_2, NULL, "thread 2");


    (void) puts("Welcome to RIOT!\n");

    msg_send_receive(&m, &m, pid1);

        //printf("1st: Got msg with content %u\n", (unsigned int)m.content.value);
        printf("%s\n", m.content.ptr);
        msg_send_receive(&m, &m, pid2);
        printf("%s\n", m.content.ptr);

        while(1) {
        	vtimer_sleep(sleep_time);
        }

    return 0;
}

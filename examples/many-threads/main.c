
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

#define ENABLE_DEBUG (0)
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

    DEBUG("2nd thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;

    while (1) {
        msg_receive(&m);
        DEBUG("1st: Got msg from %" PRIkernel_pid "\n", m.sender_pid);
        m.content.ptr = verso;
        msg_reply(&m, &m);
    }

    return NULL;
}

void *thread_2(void *arg)
{
    (void) arg;
    char verso[TWEET_SIZE] = "ti voglio al più presto sposar";

    DEBUG("2nd thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;

    while (1) {
        msg_receive(&m);
        DEBUG("2nd: Got msg from %" PRIkernel_pid "\n", m.sender_pid);
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

    printf("%s\n", m.content.ptr);
    msg_send_receive(&m, &m, pid2);
    printf("%s\n", m.content.ptr);

    while(1) {
    	vtimer_sleep(sleep_time);
    }

    return 0;
}

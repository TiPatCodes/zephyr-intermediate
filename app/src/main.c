#include "zephyr/toolchain.h"
#include <math.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l2_assignment, LOG_LEVEL_DBG);


#define STACK_SIZE 1024
#define INCREMENT    10
#define PRIO   3

static volatile uint32_t cntr = 0;

K_MUTEX_DEFINE(mutex_1);

void incr_cntr(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (cntr < INCREMENT) {
        LOG_INF("[ %s ] Incrementing at [ %lld ]", k_thread_name_get(k_current_get()), k_uptime_get());
        cntr ++;
        LOG_INF("Counter at %d",cntr);
        k_msleep(30);
    }
}

K_THREAD_DEFINE(thread_a, STACK_SIZE, incr_cntr,NULL, NULL, NULL, PRIO, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, incr_cntr,NULL,NULL, NULL, PRIO, 0, 0);           


int main(void)
{
    LOG_INF("START test running over...........");
    return 0;
}


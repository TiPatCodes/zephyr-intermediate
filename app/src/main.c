#include <math.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l1_task1, LOG_LEVEL_DBG);


#define STACK_SIZE 1024
#define INCREMENT    7
#define PRIO   3

static uint32_t cntr = 0;

void incr_cntr(void *p1, void *p2, void *p3)
{
    while (1) {
        // k_msleep(300);
        LOG_DBG("running");
    }
}

K_MUTEX_DEFINE(mutex_1);

K_THREAD_DEFINE(thread_a, STACK_SIZE, incr_cntr,NULL, NULL, NULL, PRIO, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, incr_cntr,NULL,NULL, NULL, PRIO, 0, 0);           


int main(void)
{
    LOG_INF("START test running over...........");
    k_msleep(300);
    return 0;
}


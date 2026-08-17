#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);


#define STACK_SIZE 1024

#define PRIO_LOW  7
#define PRIO_MED  5
#define PRIO_HIGH 3

void t_low_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        k_msleep(300);
        LOG_DBG("T_LOW running\n");
    }
}

void t_med_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        k_msleep(200);
        LOG_DBG("T_MED running\n");
    }
}

void t_high_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        k_msleep(100);
        LOG_DBG("T_HIGHrunning\n");
    }
}

K_THREAD_DEFINE(thread_a, STACK_SIZE, t_low_fn,
                NULL, NULL, NULL, PRIO_LOW, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, t_med_fn,
                NULL, NULL, NULL, PRIO_MED, 0, 0);
K_THREAD_DEFINE(thread_c, STACK_SIZE, t_high_fn,
                NULL, NULL, NULL, PRIO_MED, 0, 0);           

int main(void)
{
    return 0;
}


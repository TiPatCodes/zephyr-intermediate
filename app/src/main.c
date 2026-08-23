#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l1_task1, LOG_LEVEL_DBG);


#define STACK_SIZE 1024

#define PRIO_LOW    7
#define PRIO_MED    5
#define PRIO_HIGH   3
#define PRIO_COOP   -1

void t_low_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        k_msleep(300);
        LOG_DBG("T_LOW running");
    }
}

void t_med_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        k_msleep(200);
        LOG_DBG("T_MED running");
    }
}

void t_high_fn(void *p1, void *p2, void *p3)
{
    while (1) {
        k_msleep(100);
        LOG_DBG("T_HIGH running");
    }
}

void t_coop_fn(void *p1, void *p2, void *p3)
{
    // while (1) {
        for (int i = 0; i < 5 ; i ++)
        {
            LOG_DBG("T_Coop_iteration %d",i);
        }
        // LOG_DBG("T_Coop_iteration");
        k_yield();
    // }
    
}

K_THREAD_DEFINE(thread_a, STACK_SIZE, t_low_fn,NULL, NULL, NULL, PRIO_LOW, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, t_med_fn,NULL, NULL, NULL, PRIO_MED, 0, 0);
K_THREAD_DEFINE(thread_c, STACK_SIZE, t_high_fn,NULL, NULL, NULL, PRIO_MED, 0, 0);           
K_THREAD_DEFINE(thread_coop, STACK_SIZE, t_coop_fn,NULL,NULL, NULL, PRIO_COOP, 0, 0);           


int main(void)
{
    LOG_INF("Total running over...........");
    return 0;
}


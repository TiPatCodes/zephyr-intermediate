#include "tracing_ctf.h"
#include <stdint.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l2_assignment, LOG_LEVEL_DBG);


#define STACK_SIZE      1024
#define PRIO            5
#define INCREMENTS      1000000   /* each thread increments this many times */

/* Shared state - intentionally unprotected */
static volatile uint32_t counter;

static struct k_sem done_sem;

static K_MUTEX_DEFINE(counter_mutex);

void worker_fn(void *p1, void *p2, void *p3)
{
    const char *name = k_thread_name_get(k_current_get());

    // without the mutex protection there will be missed increments done ( LOAD - MODIFY - STORE won't happen without interrupt)
    for (int i = 0; i < INCREMENTS; i++) {
        k_mutex_lock(&counter_mutex, K_FOREVER);
        counter++;
        k_mutex_unlock(&counter_mutex);
    }

    // while(counter < INCREMENTS) { // -- This while loop instead of for loop only gives  Counter = INCREMETNT at the end.
    //     k_mutex_lock(&counter_mutex, K_FOREVER);
    //     counter++;
    //     k_mutex_unlock(&counter_mutex);
    //     k_yield(); // -- If not used this in while loop then the entire thread will keep executing till Counter = INCREMETNT.
    //     LOG_DBG("[%s] finished at [%d]", name,counter);
    // }

    LOG_INF("[%s] finished ", name);
    k_sem_give(&done_sem);
}

/* ------------------------------------------------------------------ */
/*  polling_thread - checks flag every 10ms                          */
/*                                                                     */
/*  TASK 2: Replace this entire function + thread with a k_work       */
/*  handler. The handler body is the same as what's inside the        */
/*  if (sensor_flag) block below.                                      */
/* ------------------------------------------------------------------ */
#ifdef TASK1
static void polling_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    while (total_processed < EVENT_COUNT) {
        k_msleep(POLL_MS);
        total_wakeups++;

        if (sensor_flag) {
            sensor_flag = false;
            total_processed++;

            /*
             * This is the "real work". In Task 2 this goes into
             * the k_work handler body.
             */
            LOG_INF("[CONSUMER] processed event %d  wakeups_so_far=%d  tick=%u",
                    total_processed, total_wakeups,
                    k_uptime_get_32());
        }
    }

    /* Summary after all events processed */
    LOG_INF("[TASK 1 SUMMARY] events=%d  total_wakeups=%d  wasted=%d",
            total_processed,
            total_wakeups,
            total_wakeups - total_processed);
    LOG_INF("[TASK 1 SUMMARY] wasted wakeups = %d%% of all wakeups",
            (total_wakeups - total_processed) * 100 /
            total_wakeups);
}
#endif

K_THREAD_DEFINE(thread_a, STACK_SIZE, t_low_fn,NULL, NULL, NULL, PRIO_LOW, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, t_med_fn,NULL, NULL, NULL, PRIO_MED, 0, 0);
K_THREAD_DEFINE(thread_c, STACK_SIZE, t_high_fn,NULL, NULL, NULL, PRIO_HIGH, 0, 0);           
K_THREAD_DEFINE(thread_coop, STACK_SIZE, t_coop_fn,NULL,NULL, NULL, PRIO_COOP, 0, 0);           


int main(void)
{
    LOG_INF("=== L6 Homework: Runtime Investigation ===");
    LOG_INF("Control work must start within 10 ms");
    LOG_INF("Inspect, measure, trace, explain, and correct the delay");

    k_timer_start(&event_timer, K_MSEC(500), K_MSEC(EVENT_PERIOD_MS));

    return 0;
}

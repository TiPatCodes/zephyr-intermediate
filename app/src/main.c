#include "zephyr/toolchain.h"
#include <math.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l2_assignment, LOG_LEVEL_DBG);


#define STACK_SIZE      1024
#define PRIO            5
#define INCREMENTS      18000   /* each thread increments this many times */

/* Shared state - intentionally unprotected */
static volatile uint32_t counter;

static struct k_sem done_sem;

static K_MUTEX_DEFINE(counter_mutex);

void worker_fn(void *p1, void *p2, void *p3)
{
    const char *name = k_thread_name_get(k_current_get());

    for (int i = 0; i < INCREMENTS; i++) { // Using "FOR" terminates the thread itself and continues unless break , no need of yielding
        // k_mutex_lock(&counter_mutex, K_FOREVER);
        counter++;
        // LOG_DBG("[%s] finished at [%d]", name,counter);
        // k_mutex_unlock(&counter_mutex);
        // break;
    }   
    
    // while(counter < INCREMENTS) { // -- This while loop instead of for loop only gives  Counter = INCREMETNT at the end.
    //     k_mutex_lock(&counter_mutex, K_FOREVER);
    //     counter++;
    //     k_mutex_unlock(&counter_mutex);
    //     // LOG_DBG("[%s] finished at [%d]", name,counter);
    //     k_yield(); // -- If not used this in while loop then the entire thread will keep executing till Counter = INCREMETNT.
    // }

    LOG_INF("[%s] finished ", name);
    k_sem_give(&done_sem);
}

K_THREAD_DEFINE(worker_a, STACK_SIZE, worker_fn, NULL, NULL, NULL,
                PRIO, 0, 0);
K_THREAD_DEFINE(worker_b, STACK_SIZE, worker_fn, NULL, NULL, NULL,
                PRIO, 0, 0);

int main(void)
{
    k_sem_init(&done_sem, 0, 2);
    int64_t time = k_uptime_get();
    
    LOG_INF("Execution start time: %lld ms", time);

    LOG_INF("=== L2 Demo 2: Mutex Protection ===");
    LOG_INF("Expected final value: %d", INCREMENTS * 2);
    
    /* Wait for both workers to complete */
    k_sem_take(&done_sem, K_FOREVER);
    k_sem_take(&done_sem, K_FOREVER);
    
    LOG_INF("Actual  final value: %u", counter);

    if (counter == INCREMENTS * 2) {
        LOG_WRN("No race this run");
    } 
    else
    {
        if (counter == INCREMENTS )
        {
            LOG_WRN("Used WHILE instead of FOR , thus both thread incremented the counter till INCRMENT only:  %d ",
                    counter);
        }
        else{
            LOG_ERR("Race condition confirmed: lost %d updates",
                (INCREMENTS * 2) - counter);
        }
    }


    LOG_INF("Execution time: %lld ms", k_uptime_delta(&time));

    return 0;
}


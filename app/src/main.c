#include "zephyr/toolchain.h"
#include <math.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdbool.h>

LOG_MODULE_REGISTER(homework, LOG_LEVEL_DBG);

#define STACK_SIZE    1024
#define SENSOR_MS     100    /* sensor fires every 100ms */
#define POLL_MS       10     /* polling consumer checks every 10ms */
#define EVENT_COUNT   10     /* total sensor events to produce */

/* ================================================================
 * STARTER CODE -- inefficient polling version
 * Run this first, then replace with workqueue in Task 2.
 * ================================================================ */

/* Shared flag between sensor_sim and polling_thread */
static volatile bool sensor_flag;

/* Statistics */
// static int total_events;
// static int total_wakeups;
static int total_processed;

/* ------------------------------------------------------------------ */
/*  sensor_sim - fires EVENT_COUNT events, 100ms apart               */
/* ------------------------------------------------------------------ */

static void sensor_handler(struct k_work *work)
{
      ARG_UNUSED(work);
      total_processed++;
      LOG_INF("[HANDLER] processed event %d  tick=%u", \
        total_processed, k_uptime_get_32());
}

K_WORK_DEFINE(sensor_work, sensor_handler);
// K_WORK_DELAYABLE_DEFINE(debounce_work, sensor_handler);

static void sensor_sim_fn(void *p1, void *p2, void *p3)
{
    for (int i = 0; i < EVENT_COUNT; i++) {
        // k_msleep(SENSOR_MS);
        LOG_INF("[SENSOR] event %d  tick=%u", i, k_uptime_get_32());

        // TASK 2: Replace these two lines with:
        int ret = k_work_submit(&sensor_work);
        // int ret = k_work_reschedule(&debounce_work, K_MSEC(30));
        if (ret < 0) { 
            LOG_ERR("rescheduling after 30ms failed: %d", ret); 
        }
    }

    LOG_INF("[SENSOR] all events produced");
}

K_THREAD_DEFINE(sensor_thread,  STACK_SIZE, sensor_sim_fn, NULL, NULL, NULL, 5, 0, 0);


int main(void)
{
    LOG_INF("=== L3 Homework: Polling to Workqueue ===");
    LOG_INF("Starter:  sensor fires every %dms and submit work to handler", SENSOR_MS);
    /* Wait long enough for all events to complete */
    k_msleep((EVENT_COUNT + 2) * SENSOR_MS + 500);

    LOG_INF(" Verify [SENSOR] and [HANDLER] ticks processed by  workqueue.");
    return 0;
}


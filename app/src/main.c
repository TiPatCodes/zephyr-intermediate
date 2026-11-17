#include "tracing_ctf.h"
#include <stdint.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/tracing/tracing.h>

LOG_MODULE_REGISTER(homework, LOG_LEVEL_INF);

#define STACK_SIZE            2048
#define CONTROL_PRIORITY         7
#define MAINTENANCE_PRIORITY     4
#define EVENT_PERIOD_MS        250
#define MAINTENANCE_LOAD_US  45000
#define RESPONSE_TIME_GUARANTEE 10

struct control_event {
    uint32_t seq;
    uint32_t ready_ms;
};

K_MSGQ_DEFINE(control_queue, sizeof(struct control_event), 4, 4);
K_SEM_DEFINE(maintenance_start, 0, 1);

/* ================================================================== */
/*  Timer expiry: creates one control event                           */
/* ================================================================== */

static void event_timer_expiry(struct k_timer *timer)
{
    ARG_UNUSED(timer);

    static uint32_t seq;
    struct control_event event = {
        .seq = seq++,
        .ready_ms = k_uptime_get_32(),
    };

    /* Timer expiry runs in interrupt context, so never wait here. */
    int ret = k_msgq_put(&control_queue, &event, K_NO_WAIT);

    if (ret != 0) {
        return;
    }

    /* Both threads become ready when the timer interrupt returns. */
    k_sem_give(&maintenance_start);

    /* TODO: Add an application trace event for this sequence. */
    sys_trace_named_event("event messg put", event.seq , event.ready_ms);
}

K_TIMER_DEFINE(event_timer, event_timer_expiry, NULL);

/* ================================================================== */
/*  Control thread                                                   */
/* ================================================================== */

static void control_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    uint32_t count = 0;
    while (true) {
        struct control_event event;
        int ret = k_msgq_get(&control_queue, &event, K_FOREVER);

        if (ret != 0) {
            LOG_ERR("[CONTROL] receive failed: %d", ret);
            continue;
        }

        LOG_INF("[CONTROL] processed seq=%u", event.seq);

        /* TODO: Define a response-time guarantee. */
        uint32_t latency =  k_uptime_get_32() -  event.ready_ms ;
        /* TODO: Measure latency and count every deadline miss. */
        if ( latency > RESPONSE_TIME_GUARANTEE){
            count ++;
        /* TODO: Rate-limit repeated warning messages. */
            LOG_WRN_RATELIMIT("deadline misses count = %u seq = %u latency_ms %u", count, event.seq, latency);
        }
        // LOG_DBG("T_Coop_iteration");
        k_yield();
    // }
    
}

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

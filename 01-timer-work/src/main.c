#include "zephyr/sys/util.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo1, LOG_LEVEL_INF);

/* Helper: who is running right now? */
static const char *ctx(void)
{
    return k_is_in_isr() ? "ISR" : k_thread_name_get(k_current_get());
}

// struct senosr
struct sensor {
    const char *name;
    struct k_work work;
};

static struct sensor temp = {.name = "temp"};
static struct sensor hum = {.name = "hum"};

// work handler
static void work_handler(struct k_work *w)
{
    // struct sensor *s = (struct sensor *)(w);
    struct sensor *s = CONTAINER_OF(w, struct sensor, work);
    LOG_INF("handles in %s", ctx());
    LOG_INF("sensor name: %s", s->name);
}

// temperature senor timer
static void temp_expiry(struct k_timer *tim)
{
    LOG_INF("expiry from %s", ctx());
    k_work_submit(&temp.work);
}

K_TIMER_DEFINE(temp_timer, temp_expiry, NULL);

// humidity senor timer
static void hum_expiry(struct k_timer *tim)
{
    LOG_INF("expiry from %s", ctx());
    // k_work_submit(&hum.work);
    k_work_submit_to_queue(&hum.work);
}

K_TIMER_DEFINE(hum_timer, hum_expiry, NULL);

int main(void)
{
    LOG_INF("demo 1 start, ctx=%s", ctx());

    k_timer_start(&temp_timer, K_MSEC(1000), K_MSEC(1000));
    k_timer_start(&hum_timer, K_MSEC(2500), K_MSEC(2500));

    k_work_init(&temp.work, work_handler);
    k_work_init(&hum.work, work_handler);

    return 0;
}

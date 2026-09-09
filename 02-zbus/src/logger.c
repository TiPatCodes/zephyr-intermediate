/*
 * Observer #2 - a SUBSCRIBER (contrast with display.c, a listener).
 *
 * A subscriber gets a notification in its own queue and reads the channel
 * later, from its own thread. That means it is allowed to be slow: the
 * k_sleep() below would be unthinkable in a listener, which runs inside
 * the publisher's call to zbus_chan_pub().
 */
#include "sensor_iface.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(logger, LOG_LEVEL_INF);

ZBUS_SUBSCRIBER_DEFINE(logger_sub, 2);
ZBUS_CHAN_ADD_OBS(sensor_chan, logger_sub, 4);

static void logger_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);

    const struct zbus_channel *chan;

    while (!zbus_sub_wait(&logger_sub, &chan, K_FOREVER)) {
        struct sensor_data d;

        if (chan != &sensor_chan) {
            continue;
        }

        zbus_chan_read(&sensor_chan, &d, K_MSEC(200));

        LOG_INF("  logger: seq=%u temp=%d ctx=%s", d.seq, d.temp_c,
                k_thread_name_get(k_current_get()));
    }
}

K_THREAD_DEFINE(logger_tid, 1024, logger_thread, NULL, NULL, NULL, 6, 0, 0);

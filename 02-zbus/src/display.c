/*
 * Observer #1 - a listener.
 * Runs synchronously, in the publisher's context. Note the thread name.
 */
#include "sensor_iface.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(display, LOG_LEVEL_INF);

static void display_cb(const struct zbus_channel *chan)
{
    const struct sensor_data *d = zbus_chan_const_msg(chan);

    LOG_INF("  display: %d C (seq=%u) ctx=%s", d->temp_c, d->seq,
            k_thread_name_get(k_current_get()));
}

ZBUS_LISTENER_DEFINE(display_lis, display_cb);
ZBUS_CHAN_ADD_OBS(sensor_chan, display_lis, 3);

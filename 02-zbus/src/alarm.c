/*
 * Observer #2 - added in STEP 3.
 *
 * Adding this file to CMakeLists.txt is the ENTIRE change.
 * sensor.c is never opened, and never even recompiled.
 */
#include "sensor_iface.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(alarm, LOG_LEVEL_INF);

#define ALARM_THRESHOLD_C 30

static void alarm_cb(const struct zbus_channel *chan)
{
    const struct sensor_data *d = zbus_chan_const_msg(chan);

    if (d->temp_c > ALARM_THRESHOLD_C) {
        LOG_WRN("  alarm: %d C is over %d C!", d->temp_c, ALARM_THRESHOLD_C);
    }
}

ZBUS_LISTENER_DEFINE(alarm_lis, alarm_cb);
ZBUS_CHAN_ADD_OBS(sensor_chan, alarm_lis, 5);

/*
 * The producer.
 *
 * Look at the includes: it knows sensor_iface.h and nothing else.
 * No display.h. No alarm.h. It does not know who is listening.
 */
#include "sensor_iface.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor, LOG_LEVEL_INF);

/*
 * The channel lives with the interface it implements.
 *
 * ZBUS_OBSERVERS_EMPTY: this producer names NOBODY. Observers attach
 * themselves from their own files. This list never grows.
 */
ZBUS_CHAN_DEFINE(sensor_chan,
		 struct sensor_data,
		 NULL,			/* validator */
		 NULL,			/* user data */
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

static void sensor_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);

    struct sensor_data d = { .temp_c = 20, .seq = 0 };

    while (1) {
        d.seq++;
        // Drift upward so the alarm threshold is crossed live.
        d.temp_c += 2;

        LOG_INF("publish seq=%u temp=%d", d.seq, d.temp_c);
        zbus_chan_pub(&sensor_chan, &d, K_MSEC(100));

        k_sleep(K_MSEC(700));
    }
}

K_THREAD_DEFINE(sensor_tid, 1024, sensor_thread, NULL, NULL, NULL, 5, 0, 0);

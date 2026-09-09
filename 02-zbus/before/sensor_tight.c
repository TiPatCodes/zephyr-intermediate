/*
 * NOT BUILT - illustration only. The includes below reference headers
 * that intentionally do not exist; this file exists to be *read*, not
 * compiled. It is excluded from CMakeLists.txt on purpose.
 *
 * STEP 1 - the "before" picture. Tight coupling.
 *
 * The producer names every consumer. Read the includes out loud:
 * that list IS the coupling. Adding an alarm means editing this file.
 */
#include "display.h" // <-- knows about display
#include "logger.h" // <-- knows about logger
// #include "alarm.h" // yet another conusmer!!!

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor, LOG_LEVEL_INF);

static void sensor_thread(void *a, void *b, void *c)
{
    int temp_c = 20;

    while (1) {
        temp_c += 2;

        display_update(temp_c); // one call per consumer
        logger_record(temp_c);
        // alarm_check(temp_c); //    edit the producer. again. */

        k_sleep(K_MSEC(700));
    }
}

K_THREAD_DEFINE(sensor_tid, 1024, sensor_thread, NULL, NULL, NULL, 5, 0, 0);

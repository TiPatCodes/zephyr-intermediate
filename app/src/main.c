#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>



LOG_MODULE_REGISTER(homework, LOG_LEVEL_INF);

#define STACK_SIZE       2048
#define SENSOR_COUNT       18
#define SENSOR_PERIOD_MS  100
#define LOGGER_PERIOD_MS  (SENSOR_PERIOD_MS * 2)
#define TASK_TIMEPERIOD   (LOGGER_PERIOD_MS / 2 )
#define MSG_Q_DEPTH       (SENSOR_COUNT/2) 
/* ================================================================== */
/*  Shared  message                                            */
/* ================================================================== */
struct sensor_data {
    int32_t temperature_mc;
    uint32_t timestamp_ms;
    uint8_t seq;
};

K_MSGQ_DEFINE(msg_q,sizeof(struct sensor_data), MSG_Q_DEPTH, 4);
K_SEM_DEFINE(sem_done,0,3);

// task callback function 
static void task_callback_fn( int channel_id, void * user_data)
{
    LOG_WRN("[TASK WDT] DOESNOT FEED %d channel, thread %s", channel_id,k_thread_name_get((k_tid_t)user_data));
}

/* ================================================================== */
/*  Publisher                                                         */
/* ================================================================== */

static void sensor_thread_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    k_thread_name_set(k_current_get(), "sensor");

    for (int i = 0; i < SENSOR_COUNT; i++) {
        struct sensor_data data = {
            .temperature_mc = 24000 + (i * 350),
            .timestamp_ms = k_uptime_get_32(),
            .seq = (uint8_t)i,
        };

        LOG_INF("[SENSOR] data in msgq seq=%u temp=%d mC",
                data.seq,
                data.temperature_mc);

        // int ret = zbus_chan_pub(&sensor_chan, &data, K_MSEC(100));
        int ret = k_msgq_put(&msg_q, &data, K_NO_WAIT);
        if (ret != 0) {
            LOG_WRN("[SENSOR] publish failed ret=%d", ret);
        }

        k_msleep(SENSOR_PERIOD_MS);
    }

    LOG_INF("[SENSOR] done");
    k_sem_give(&sem_done);
}

/* ================================================================== */
/*  consumer - logger                                                 */
/* ================================================================== */

static void logger_thread_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    k_thread_name_set(k_current_get(), "logger");
    int received = 0;
    struct sensor_data msg;
    int task_wdtch;
    while (received < SENSOR_COUNT) {
        /*
         * Message subscribers receive a copy of the published message.
         * The slow logger will not reread the latest channel value.
         */
        // int ret = zbus_sub_wait_msg(&logger_sub, &chan, &msg, K_MSEC(1500));
        int ret = k_msgq_get(&msg_q, &msg, K_MSEC(20));
        if (ret != 0) {
            LOG_WRN("[LOGGER-MSG] timeout ret=%d", ret);
            break;
        }

        received++;

        task_wdtch = task_wdt_add(TASK_TIMEPERIOD, task_callback_fn, (void*)k_current_get());

        LOG_INF("[LOGGER-MSG] thread=%s seq=%u temp=%d latency=%ums",
                k_thread_name_get(k_current_get()),
                msg.seq,
                msg.temperature_mc,
                k_uptime_get_32() - msg.timestamp_ms);

        /*
         * Slow logger.
         * Message copies let it process old samples safely.
        */
        task_wdt_feed(task_wdtch);

        k_msleep(LOGGER_PERIOD_MS);
    }

    LOG_INF("[LOGGER-MSG] done received=%d", received);
    k_sem_give(&sem_done);
}


static void health_thread_fn(void *p1, void *p2, void *p3 )
{
    for (int i=0; i < SENSOR_COUNT ; i++)
    {
        int used  =  k_msgq_num_used_get(&msg_q);

        if(used > (MSG_Q_DEPTH * 3/4))
        {
            LOG_WRN("[HEALTH] msg_q has used 3/4 ");
        }
        LOG_INF("[HEALTH] msg_q has used %d / %d", used, MSG_Q_DEPTH);
        k_msleep(150);
    }
    k_sem_give(&sem_done);
}
/* ================================================================== */
/*  Threads                                                           */
/* ================================================================== */

K_THREAD_DEFINE(sensor_thread, STACK_SIZE, sensor_thread_fn,
                NULL, NULL, NULL, 5, 0, 0);

K_THREAD_DEFINE(logger_thread, STACK_SIZE, logger_thread_fn,
                NULL, NULL, NULL, 5, 0, 0);

K_THREAD_DEFINE(health_thread, STACK_SIZE, health_thread_fn,
                NULL, NULL, NULL, 6, 0, 0);

/* ================================================================== */
/*  Main                                                              */
/* ================================================================== */

int main(void)
{
    LOG_INF("=== L5 task 1: Message q  Producer - Consumer ===");
    LOG_INF("sensor publishes every %dms", SENSOR_PERIOD_MS);
    LOG_INF("logger uses message to read message copies");
    // Intializing the task watch dog
    if(task_wdt_init(NULL) != 0)
    {
        LOG_ERR("NO TASK WATCH DOG CREATED");
    }
   
    k_sem_take(&sem_done,K_FOREVER);
    k_sem_take(&sem_done,K_FOREVER);
    k_sem_take(&sem_done,K_FOREVER);

    LOG_INF("=== L5 task 1: END ===");
}



/*
 * THE INTERFACE.
 *
 * This header is the contract. The producer depends on it, every observer
 * depends on it, and nobody depends on anybody else.
 */
#pragma once

#include <zephyr/zbus/zbus.h>

struct sensor_data {
    int temp_c;
    uint32_t seq;
};

ZBUS_CHAN_DECLARE(sensor_chan);

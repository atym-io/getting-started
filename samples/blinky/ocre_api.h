/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */

#ifndef OCRE_API_H
#define OCRE_API_H

#include "stdbool.h"

// Sleep Function
int ocre_sleep(int milliseconds);

// Sensor API
typedef int ocre_sensor_handle_t;
int ocre_sensors_init();
int ocre_sensors_discover();
int ocre_sensors_open(ocre_sensor_handle_t handle);
int ocre_sensors_get_handle(int sensor_id);
int ocre_sensors_get_channel_count(int sensor_id);
int ocre_sensors_get_channel_type(int sensor_id, int channel_index);
int ocre_sensors_read(int sensor_id, int channel_type);

// Timer API
int ocre_timer_create(int id);
int ocre_timer_delete(int id);
int ocre_timer_start(int id, int interval, int is_periodic);
int ocre_timer_stop(int id);
int ocre_timer_get_remaining(int id);
#endif // OCRE_API_H
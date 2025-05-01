/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */

#ifndef OCRE_API_H
#define OCRE_API_H

#include "stdbool.h"

// For exported functions
#define OCRE_EXPORT(name) __attribute__((export_name(name)))

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

// --- GPIO Direction and State Constants ---
#define OCRE_GPIO_DIR_INPUT  0
#define OCRE_GPIO_DIR_OUTPUT 1

#define OCRE_GPIO_PIN_RESET  0
#define OCRE_GPIO_PIN_SET    1

// --- GPIO API ---
int ocre_gpio_init(void);
int ocre_gpio_configure(int port, int pin, int direction);
int ocre_gpio_pin_set(int port, int pin, int state);
int ocre_gpio_pin_get(int port, int pin);
int ocre_gpio_pin_toggle(int port, int pin);
int ocre_gpio_register_callback(int port, int pin);
int ocre_gpio_unregister_callback(int port, int pin);

// typedef void (*ocre_gpio_callback_t)(int pin, ocre_gpio_pin_state_t state);

#endif // OCRE_API_H
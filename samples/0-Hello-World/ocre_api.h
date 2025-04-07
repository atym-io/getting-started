#ifndef OCRE_API_H
#define OCRE_API_H

#include "stdbool.h"

// Sleep Function
int ocre_sleep(int milliseconds);
// Sensor API
int ocre_sensors_init();
int ocre_sensors_discover();
int ocre_sensors_open(const char *sensor_name);
int ocre_sensors_get_handle(const char *sensor_name);
int ocre_sensors_get_channel_count(const char *sensor_name);
int ocre_sensors_get_channel_type(const char *sensor_name, int channel_index);
int ocre_sensors_read(const char *sensor_name, int channel_type);
// Timer API
int ocre_timer_create(int id);
int ocre_timer_delete(int id);
int ocre_timer_start(int id, int interval, int is_periodic);
int ocre_timer_stop(int id);
int ocre_timer_get_remaining(int id);
// timer_callback - export function

// GPIO API
typedef enum
{
    OCRE_GPIO_PIN_RESET = 0,
    OCRE_GPIO_PIN_SET = 1
} ocre_gpio_pin_state_t;
typedef enum
{
    OCRE_GPIO_DIR_INPUT = 0,
    OCRE_GPIO_DIR_OUTPUT = 1
} ocre_gpio_direction_t;
typedef enum
{
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIOH
} ocre_gpio_stm32_port_t;
int ocre_gpio_init();
int ocre_gpio_configure(int port, int P_pin, int direction);
int ocre_gpio_set(int port, int P_pin, int state);
int ocre_gpio_get(int port, int P_pin);
int ocre_gpio_toggle(int port, int P_pin);
int ocre_gpio_register_callback(int port, int P_pin);
int ocre_gpio_unregister_callback(int port, int P_pin);
// gpio_callback - export function

int ocre_msg_system_init();
int ocre_publish_message(char *topic, char *content_type, void *payload, int payload_len);
int ocre_subscribe_message(char *topic, char *handler_name);
int ocre_unsubscribe_message(char *topic);

#define ocre_pause() ocre_sleep(9999999);

#endif // OCRE_API_H

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

int ocre_timer_create(int id);
int ocre_timer_delete(int id);
int ocre_timer_start(int id, int interval, int is_periodic);
int ocre_timer_stop(int id);
int ocre_timer_get_remaining(int id);
int ocre_sleep(int milliseconds);

#define ocre_pause() ocre_sleep(9999999);

// Typedefs
typedef enum
{
    OCRE_GPIO_DIR_INPUT,
    OCRE_GPIO_DIR_OUTPUT
} ocre_gpio_direction_t;

typedef enum
{
    OCRE_GPIO_PIN_RESET = 0,
    OCRE_GPIO_PIN_SET = 1
} ocre_gpio_pin_state_t;

typedef struct
{
    int pin;
    ocre_gpio_direction_t direction;
} ocre_gpio_config_t;

typedef void (*ocre_gpio_callback_t)(int pin, ocre_gpio_pin_state_t state);

/**
 * @brief Initialize the GPIO module.
 *
 * This function sets up the necessary hardware and software resources
 * for managing GPIO pins. It should be called before any other GPIO
 * functions are used.
 *
 * @return 0 on successful initialization, negative error code on failure.
 */
int ocre_gpio_init(void);

/**
 * @brief Configure a GPIO pin.
 **
 * @param config Pointer to an ocre_gpio_config_t structure containing the pin and direction.
 * @return 0 on success, negative error code on failure.
 */
int ocre_gpio_configure(int port, int P_pin, int direction);

/**
 * @brief Set the state of a GPIO pin.
 *
 * @param pin The GPIO pin number.
 * @param state The state to set (OCRE_GPIO_PIN_RESET or OCRE_GPIO_PIN_SET).
 * @return 0 on success, negative error code on failure.
 */
int ocre_gpio_pin_set(int port, int pin, ocre_gpio_pin_state_t state);

/**
 * @brief Get the state of a GPIO pin.
 *
 * @param pin The GPIO pin number.
 * @return The state of the pin (OCRE_GPIO_PIN_RESET or OCRE_GPIO_PIN_SET), or negative error code on failure.
 */
ocre_gpio_pin_state_t ocre_gpio_pin_get(int port, int pin);

/**
 * @brief Toggle the state of a GPIO pin.
 *
 * @param pin The GPIO pin number.
 * @return 0 on success, negative error code on failure.
 */
int ocre_gpio_pin_toggle(int port, int pin);

/**
 * @brief Register a callback for a GPIO input pin.
 *
 * @param pin The GPIO pin number.
 * @param callback The callback function to register.
 * @return 0 on success, negative error code on failure.
 */
int ocre_gpio_register_callback(int port, int pin);

/**
 * @brief Unregister a callback for a GPIO input pin.
 *
 * @param pin The GPIO pin number.
 * @return 0 on success, negative error code on failure.
 */
int ocre_gpio_unregister_callback(int port, int pin);

#endif // OCRE_API_H

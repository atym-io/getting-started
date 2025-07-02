/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#ifndef OCRE_API_H
#define OCRE_API_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Forward declarations for WASM types
struct WASMModuleInstance;
typedef struct WASMModuleInstance *wasm_module_inst_t;

#ifdef __cplusplus
extern "C"
{
#endif

// For exported callback functions (optional - only needed for WASM callbacks)
#define OCRE_EXPORT(name) __attribute__((export_name(name)))

// OCRE SDK Version Information
#define OCRE_SDK_VERSION_MAJOR 1
#define OCRE_SDK_VERSION_MINOR 0
#define OCRE_SDK_VERSION_PATCH 0
#define OCRE_SDK_VERSION "1.0.0"

// Common Return Codes
#define OCRE_SUCCESS 0
#define OCRE_ERROR_INVALID -1
#define OCRE_ERROR_TIMEOUT -2
#define OCRE_ERROR_NOT_FOUND -3
#define OCRE_ERROR_BUSY -4
#define OCRE_ERROR_NO_MEMORY -5

// Configuration
#define OCRE_MAX_TIMERS 16
#define OCRE_MAX_SENSORS 32
#define OCRE_MAX_CALLBACKS 64
#define OCRE_MAX_TOPIC_LEN 128
#define OCRE_MAX_PAYLOAD_LEN 1024
#define CONFIG_MAX_SENSOR_NAME_LENGTH 125
#define OCRE_API_POSIX_BUF_SIZE 65

// GPIO Configuration
#ifndef CONFIG_OCRE_GPIO_MAX_PINS
#define CONFIG_OCRE_GPIO_MAX_PINS 32
#endif
#ifndef CONFIG_OCRE_GPIO_MAX_PORTS
#define CONFIG_OCRE_GPIO_MAX_PORTS 8
#endif
#ifndef CONFIG_OCRE_GPIO_PINS_PER_PORT
#define CONFIG_OCRE_GPIO_PINS_PER_PORT 16
#endif

    // Structure for event data
    typedef struct
    {
        int32_t type;                          /**< Resource type (e.g., OCRE_RESOURCE_TYPE_*) */
        int32_t id;                            /**< Resource ID */
        int32_t port;                          /**< Port number (for GPIO) */
        int32_t state;                         /**< State (e.g., pin state for GPIO) */
        char topic[OCRE_MAX_TOPIC_LEN];        /**< Message topic (for message events) */
        char content_type[OCRE_MAX_TOPIC_LEN]; /**< Message content type (for message events) */
        uint8_t payload[OCRE_MAX_PAYLOAD_LEN]; /**< Message payload (for message events) */
        uint32_t payload_len;                  /**< Payload length (for message events) */
    } event_data_t;

    // Resource Types
    typedef enum
    {
        OCRE_RESOURCE_TYPE_TIMER,
        OCRE_RESOURCE_TYPE_GPIO,
        OCRE_RESOURCE_TYPE_SENSOR,
        OCRE_RESOURCE_TYPE_MESSAGE,
        OCRE_RESOURCE_TYPE_COUNT
    } ocre_resource_type_t;

    // Timer API
    int ocre_timer_create(int id);
    int ocre_timer_delete(int id);
    int ocre_timer_start(int id, int interval, int is_periodic);
    int ocre_timer_stop(int id);
    int ocre_timer_get_remaining(int id);

    // GPIO API
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

    int ocre_gpio_init(void);
    int ocre_gpio_configure(int port, int pin, int direction);
    int ocre_gpio_pin_set(int port, int pin, ocre_gpio_pin_state_t state);
    ocre_gpio_pin_state_t ocre_gpio_pin_get(int port, int pin);
    int ocre_gpio_pin_toggle(int port, int pin);
    int ocre_gpio_register_callback(int port, int pin);
    int ocre_gpio_unregister_callback(int port, int pin);

    // Event API
    typedef void (*timer_callback_func_t)(void);
    typedef void (*gpio_callback_func_t)(void);
    typedef void (*message_callback_func_t)(const char *topic, const char *content_type, const void *payload, uint32_t payload_len);

    int ocre_get_event(uint32_t type_offset, uint32_t id_offset, uint32_t port_offset,
                       uint32_t state_offset, uint32_t extra_offset, uint32_t payload_len_offset);
    void ocre_poll_events(void);
    int ocre_register_timer_callback(int timer_id, timer_callback_func_t callback);
    int ocre_register_gpio_callback(int pin, int port, gpio_callback_func_t callback);
    int ocre_register_message_callback(const char *topic, message_callback_func_t callback);
    int ocre_unregister_timer_callback(int timer_id);
    int ocre_unregister_gpio_callback(int pin, int port);
    int ocre_unregister_message_callback(const char *topic);

    // Messaging API
    int ocre_msg_system_init(void);
    int ocre_publish_message(const char *topic, const char *content_type, const void *payload, uint32_t payload_len);
    int ocre_subscribe_message(const char *topic, const char *handler_name);

    // Utility API
    int ocre_sleep(int milliseconds);
#define ocre_pause() ocre_sleep(9999999)

    // Sensor API
    typedef int ocre_sensor_handle_t;
    int ocre_sensors_init(void);
    int ocre_sensors_discover(void);
    int ocre_sensors_open(ocre_sensor_handle_t handle);
    int ocre_sensors_get_handle(int sensor_id);
    int ocre_sensors_get_channel_count(int sensor_id);
    int ocre_sensors_get_channel_type(int sensor_id, int channel_index);
    int ocre_sensors_read(int sensor_id, int channel_type);
    int ocre_sensors_get_handle_by_name(const char *name, ocre_sensor_handle_t *handle);
    int ocre_sensors_open_by_name(const char *name, ocre_sensor_handle_t *handle);

    int ocre_register_dispatcher(ocre_resource_type_t type, const char *function_name);

    // POSIX API
    struct _ocre_posix_utsname
    {
        char sysname[OCRE_API_POSIX_BUF_SIZE];
        char nodename[OCRE_API_POSIX_BUF_SIZE];
        char release[OCRE_API_POSIX_BUF_SIZE];
        char version[OCRE_API_POSIX_BUF_SIZE];
        char machine[OCRE_API_POSIX_BUF_SIZE];
        char domainname[OCRE_API_POSIX_BUF_SIZE];
    };
    int uname(struct _ocre_posix_utsname *name);
#ifdef __cplusplus
}
#endif
#endif
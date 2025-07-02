/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include "ocre_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Callback storage
static void (*timer_callbacks[OCRE_MAX_CALLBACKS])(void) = {0};
static void (*gpio_callbacks[OCRE_MAX_CALLBACKS])(void) = {0};
static message_callback_func_t message_callbacks[OCRE_MAX_CALLBACKS] = {0};
static char message_callback_topics[OCRE_MAX_CALLBACKS][OCRE_MAX_TOPIC_LEN] = {{0}};
static int gpio_callback_pins[OCRE_MAX_CALLBACKS] = {-1};
static int gpio_callback_ports[OCRE_MAX_CALLBACKS] = {-1};

// Initialize callback arrays
static void init_callback_system(void)
{
    static bool initialized = false;
    if (!initialized)
    {
        for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
        {
            timer_callbacks[i] = NULL;
            gpio_callbacks[i] = NULL;
            message_callbacks[i] = NULL;
            message_callback_topics[i][0] = '\0';
            gpio_callback_pins[i] = -1;
            gpio_callback_ports[i] = -1;
        }
        initialized = true;
    }
}

// =============================================================================
// INTERNAL CALLBACK DISPATCHERS
// =============================================================================

void OCRE_EXPORT("timer_callback") timer_callback(int timer_id)
{
    init_callback_system();
    if (timer_id >= 0 && timer_id < OCRE_MAX_CALLBACKS && timer_callbacks[timer_id])
    {
        printf("Executing timer callback for ID: %d\n", timer_id);
        timer_callbacks[timer_id]();
    }
    else
    {
        printf("No timer callback registered for ID: %d\n", timer_id);
    }
}

void OCRE_EXPORT("gpio_callback") gpio_callback(int pin, int state, int port)
{
    init_callback_system();
    printf("GPIO event triggered: pin=%d, port=%d, state=%d\n", pin, port, state);
    for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
    {
        if (gpio_callback_pins[i] == pin && gpio_callback_ports[i] == port && gpio_callbacks[i])
        {
            printf("Executing GPIO callback for pin: %d, port: %d\n", pin, port);
            gpio_callbacks[i]();
            return;
        }
    }
    printf("No GPIO callback registered for pin: %d, port: %d\n", pin, port);
}

void OCRE_EXPORT("message_callback") message_callback(int message_id, int extra_offset, int payload_len)
{
    init_callback_system();
    char *extra = (char *)extra_offset;
    uint32_t payload_len_val = *(uint32_t *)payload_len;

    // Assume extra_offset format: topic (null-terminated), content_type (null-terminated), payload
    char *topic = extra;
    char *content_type = extra + strlen(topic) + 1;
    char *payload = content_type + strlen(content_type) + 1;

    // Validate lengths to prevent buffer overflows
    if (strlen(topic) >= OCRE_MAX_TOPIC_LEN || strlen(content_type) >= OCRE_MAX_TOPIC_LEN || payload_len_val > OCRE_MAX_PAYLOAD_LEN)
    {
        printf("Error: Invalid message data lengths: topic_len=%zu, content_type_len=%zu, payload_len=%u\n",
               strlen(topic), strlen(content_type), payload_len_val);
        return;
    }

    printf("Message event triggered: topic=%s, content_type=%s, payload_len=%u\n", topic, content_type, payload_len_val);
    for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
    {
        if (message_callbacks[i] && strcmp(message_callback_topics[i], topic) == 0)
        {
            printf("Executing message callback for topic: %s\n", topic);
            message_callbacks[i](topic, content_type, payload, payload_len_val);
            return;
        }
    }
    printf("No message callback registered for topic: %s\n", topic);
}

void OCRE_EXPORT("poll_events") ocre_poll_events(void)
{
    event_data_t event_data;
    int event_count = 0;
    const int max_events_per_loop = 5;
    uint32_t extra_buffer[OCRE_MAX_TOPIC_LEN * 2 + OCRE_MAX_PAYLOAD_LEN];

    while (event_count < max_events_per_loop)
    {
        uint32_t payload_len = 0;
        int ret = ocre_get_event(
            (uint32_t)&event_data.type,
            (uint32_t)&event_data.id,
            (uint32_t)&event_data.port,
            (uint32_t)&event_data.state,
            (uint32_t)extra_buffer,
            (uint32_t)&payload_len);
        if (ret != OCRE_SUCCESS)
        {
            break;
        }

        switch (event_data.type)
        {
        case OCRE_RESOURCE_TYPE_TIMER:
            timer_callback(event_data.id);
            break;
        case OCRE_RESOURCE_TYPE_GPIO:
            gpio_callback(event_data.id, event_data.state, event_data.port);
            break;
        case OCRE_RESOURCE_TYPE_MESSAGE:
            message_callback(event_data.id, (int)extra_buffer, (int)&payload_len);
            break;
        default:
            printf("Unknown event: type=%d, id=%d, port=%d, state=%d\n",
                   event_data.type, event_data.id, event_data.port, event_data.state);
        }
        event_count++;
    }

    if (event_count == 0)
    {
        ocre_sleep(10);
    }
}

// =============================================================================
// PUBLIC API FUNCTIONS
// =============================================================================

int ocre_register_timer_callback(int timer_id, timer_callback_func_t callback)
{
    init_callback_system();
    if (timer_id < 0 || timer_id >= OCRE_MAX_CALLBACKS)
    {
        printf("Error: Timer ID %d out of range (0-%d)\n", timer_id, OCRE_MAX_CALLBACKS - 1);
        return OCRE_ERROR_INVALID;
    }
    if (callback == NULL)
    {
        printf("Error: Timer callback is NULL for ID %d\n", timer_id);
        return OCRE_ERROR_INVALID;
    }
    if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_TIMER, "timer_callback") != OCRE_SUCCESS)
    {
        printf("Failed to register timer dispatcher\n");
        return OCRE_ERROR_INVALID;
    }
    timer_callbacks[timer_id] = callback;
    printf("Timer callback registered for ID: %d\n", timer_id);
    return OCRE_SUCCESS;
}

int ocre_register_gpio_callback(int pin, int port, gpio_callback_func_t callback)
{
    init_callback_system();
    if (callback == NULL)
    {
        printf("Error: GPIO callback is NULL for pin %d, port %d\n", pin, port);
        return OCRE_ERROR_INVALID;
    }
    if (pin < 0 || pin >= CONFIG_OCRE_GPIO_PINS_PER_PORT || port < 0 || port >= CONFIG_OCRE_GPIO_MAX_PORTS)
    {
        printf("Error: Invalid pin %d or port %d\n", pin, port);
        return OCRE_ERROR_INVALID;
    }
    int slot = -1;
    for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
    {
        if (gpio_callback_pins[i] == pin && gpio_callback_ports[i] == port)
        {
            slot = i;
            break;
        }
        if (slot == -1 && gpio_callback_pins[i] == -1)
        {
            slot = i;
        }
    }
    if (slot == -1)
    {
        printf("Error: No available slots for GPIO callbacks\n");
        return OCRE_ERROR_NO_MEMORY;
    }
    if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_GPIO, "gpio_callback") != OCRE_SUCCESS)
    {
        printf("Failed to register GPIO dispatcher\n");
        return OCRE_ERROR_INVALID;
    }
    gpio_callback_pins[slot] = pin;
    gpio_callback_ports[slot] = port;
    gpio_callbacks[slot] = callback;
    printf("GPIO callback registered for pin: %d, port: %d (slot %d)\n", pin, port, slot);
    return ocre_gpio_register_callback(port, pin);
}

int ocre_register_message_callback(const char *topic, message_callback_func_t callback)
{
    init_callback_system();
    if (!topic || topic[0] == '\0')
    {
        printf("Error: Topic is NULL or empty\n");
        return OCRE_ERROR_INVALID;
    }
    if (callback == NULL)
    {
        printf("Error: Message callback is NULL for topic %s\n", topic);
        return OCRE_ERROR_INVALID;
    }
    int slot = -1;
    for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
    {
        if (message_callbacks[i] && strcmp(message_callback_topics[i], topic) == 0)
        {
            slot = i;
            break;
        }
        if (slot == -1 && message_callback_topics[i][0] == '\0')
        {
            slot = i;
        }
    }
    if (slot == -1)
    {
        printf("Error: No available slots for message callbacks\n");
        return OCRE_ERROR_NO_MEMORY;
    }
    if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_MESSAGE, "message_callback") != OCRE_SUCCESS)
    {
        printf("Failed to register message dispatcher\n");
        return OCRE_ERROR_INVALID;
    }
    strncpy(message_callback_topics[slot], topic, OCRE_MAX_TOPIC_LEN - 1);
    message_callback_topics[slot][OCRE_MAX_TOPIC_LEN - 1] = '\0';
    message_callbacks[slot] = callback;
    printf("Message callback registered for topic: %s (slot %d)\n", topic, slot);
    int ret = ocre_subscribe_message((char *)topic, "message_callback");
    if (ret != OCRE_SUCCESS)
    {
        printf("Error: Failed to subscribe to topic %s\n", topic);
        message_callback_topics[slot][0] = '\0';
        message_callbacks[slot] = NULL;
        return ret;
    }
    return OCRE_SUCCESS;
}

int ocre_unregister_timer_callback(int timer_id)
{
    init_callback_system();
    if (timer_id < 0 || timer_id >= OCRE_MAX_CALLBACKS)
    {
        printf("Error: Timer ID %d out of range (0-%d)\n", timer_id, OCRE_MAX_CALLBACKS - 1);
        return OCRE_ERROR_INVALID;
    }
    if (timer_callbacks[timer_id] == NULL)
    {
        printf("Error: No timer callback registered for ID %d\n", timer_id);
        return OCRE_ERROR_NOT_FOUND;
    }
    timer_callbacks[timer_id] = NULL;
    printf("Timer callback unregistered for ID: %d\n", timer_id);
    return OCRE_SUCCESS;
}

int ocre_unregister_gpio_callback(int pin, int port)
{
    init_callback_system();
    int slot = -1;
    for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
    {
        if (gpio_callback_pins[i] == pin && gpio_callback_ports[i] == port)
        {
            slot = i;
            break;
        }
    }
    if (slot == -1 || gpio_callbacks[slot] == NULL)
    {
        printf("Error: No GPIO callback registered for pin %d, port %d\n", pin, port);
        return OCRE_ERROR_NOT_FOUND;
    }
    gpio_callback_pins[slot] = -1;
    gpio_callback_ports[slot] = -1;
    gpio_callbacks[slot] = NULL;
    printf("GPIO callback unregistered for pin: %d, port: %d\n", pin, port);
    return ocre_gpio_unregister_callback(port, pin);
}

int ocre_unregister_message_callback(const char *topic)
{
    init_callback_system();
    if (!topic || topic[0] == '\0')
    {
        printf("Error: Topic is NULL or empty\n");
        return OCRE_ERROR_INVALID;
    }
    int slot = -1;
    for (int i = 0; i < OCRE_MAX_CALLBACKS; i++)
    {
        if (message_callbacks[i] && strcmp(message_callback_topics[i], topic) == 0)
        {
            slot = i;
            break;
        }
    }
    if (slot == -1 || message_callbacks[slot] == NULL)
    {
        printf("Error: No message callback registered for topic %s\n", topic);
        return OCRE_ERROR_NOT_FOUND;
    }
    message_callback_topics[slot][0] = '\0';
    message_callbacks[slot] = NULL;
    printf("Message callback unregistered for topic: %s\n", topic);
    return OCRE_SUCCESS;
}
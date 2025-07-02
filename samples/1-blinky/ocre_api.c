/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include "ocre_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CALLBACKS 16
#define BUTTON_PORT 2

static void (*timer_callbacks[MAX_CALLBACKS])(void) = {0};
static void (*gpio_callbacks[MAX_CALLBACKS])(void) = {0};
static message_callback_func_t message_callbacks[MAX_CALLBACKS] = {0};
static char message_callback_topics[MAX_CALLBACKS][OCRE_MAX_TOPIC_LEN] = {{0}};
static int gpio_callback_pins[MAX_CALLBACKS] = {-1};
static int gpio_callback_ports[MAX_CALLBACKS] = {-1};

// Initialize callback arrays to invalid values
static void init_callback_system(void)
{
    static bool initialized = false;
    if (!initialized)
    {
        for (int i = 0; i < MAX_CALLBACKS; i++)
        {
            gpio_callback_pins[i] = -1;
            gpio_callback_ports[i] = -1;
            message_callback_topics[i][0] = '\0';
        }
        initialized = true;
    }
}

// =============================================================================
// INTERNAL CALLBACK DISPATCHERS
// =============================================================================

__attribute__((export_name("timer_callback"))) void timer_callback(int timer_id)
{
    init_callback_system();

    if (timer_id >= 0 && timer_id < MAX_CALLBACKS && timer_callbacks[timer_id])
    {
        printf("Executing timer callback for ID: %d\n", timer_id);
        timer_callbacks[timer_id]();
    }
    else
    {
        printf("No timer callback registered for ID: %d\n", timer_id);
    }
}

__attribute__((export_name("gpio_callback"))) void gpio_callback(int pin, int state, int port)
{
    init_callback_system();

    printf("GPIO event triggered: pin=%d, port=%d, state=%d\n", pin, port, state);

    for (int i = 0; i < MAX_CALLBACKS; i++)
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

// Message callback dispatcher
__attribute__((export_name("message_callback"))) void message_callback(const char *topic, const char *content_type, const void *payload, uint32_t payload_len)
{
    init_callback_system();

    printf("Message event triggered: topic=%s, content_type=%s, payload_len=%u\n", topic, content_type, payload_len);

    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        if (message_callbacks[i] && strcmp(message_callback_topics[i], topic) == 0)
        {
            printf("Executing message callback for topic: %s\n", topic);
            message_callbacks[i](topic, content_type, payload, payload_len);
            return;
        }
    }

    printf("No message callback registered for topic: %s\n", topic);
}

__attribute__((export_name("poll_events"))) void poll_events(void)
{
    ocre_poll_events();
}

// =============================================================================
// PUBLIC API FUNCTIONS
// =============================================================================

int ocre_register_timer_callback(int timer_id, timer_callback_func_t callback)
{
    init_callback_system();

    if (timer_id < 0 || timer_id >= MAX_CALLBACKS)
    {
        printf("Error: Timer ID %d out of range (0-%d)\n", timer_id, MAX_CALLBACKS - 1);
        return -1;
    }

    if (callback == NULL)
    {
        printf("Error: Timer callback is NULL for ID %d\n", timer_id);
        return -1;
    }

    timer_callbacks[timer_id] = callback;
    printf("Timer callback registered for ID: %d\n", timer_id);
    return 0;
}

int ocre_register_gpio_callback(int pin, int port, gpio_callback_func_t callback)
{
    init_callback_system();

    if (callback == NULL)
    {
        printf("Error: GPIO callback is NULL for pin %d, port %d\n", pin, port);
        return -1;
    }

    int slot = -1;
    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        if (gpio_callback_pins[i] == pin && gpio_callback_ports[i] == port)
        {
            slot = i; // Update existing
            break;
        }
        if (slot == -1 && gpio_callback_pins[i] == -1)
        {
            slot = i; // Found empty slot
        }
    }

    if (slot == -1)
    {
        printf("Error: No available slots for GPIO callbacks\n");
        return -1;
    }

    gpio_callback_pins[slot] = pin;
    gpio_callback_ports[slot] = port;
    gpio_callbacks[slot] = callback;
    printf("GPIO callback registered for pin: %d, port: %d (slot %d)\n", pin, port, slot);
    return ocre_gpio_register_callback(port, pin);
}

// Register message callback
int ocre_register_message_callback(const char *topic, message_callback_func_t callback)
{
    init_callback_system();

    if (!topic || topic[0] == '\0')
    {
        printf("Error: Topic is NULL or empty\n");
        return -1;
    }

    if (callback == NULL)
    {
        printf("Error: Message callback is NULL for topic %s\n", topic);
        return -1;
    }

    int slot = -1;
    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        if (message_callbacks[i] && strcmp(message_callback_topics[i], topic) == 0)
        {
            slot = i; // Update existing
            break;
        }
        if (slot == -1 && message_callback_topics[i][0] == '\0')
        {
            slot = i; // Found empty slot
        }
    }

    if (slot == -1)
    {
        printf("Error: No available slots for message callbacks\n");
        return -1;
    }

    strncpy(message_callback_topics[slot], topic, OCRE_MAX_TOPIC_LEN - 1);
    message_callback_topics[slot][OCRE_MAX_TOPIC_LEN - 1] = '\0';
    message_callbacks[slot] = callback;
    printf("Message callback registered for topic: %s (slot %d)\n", topic, slot);

    int ret = ocre_subscribe_message(topic, "message_callback");
    if (ret != 0)
    {
        printf("Error: Failed to subscribe to topic %s\n", topic);
        message_callback_topics[slot][0] = '\0';
        message_callbacks[slot] = NULL;
        return ret;
    }

    return 0;
}

int ocre_unregister_timer_callback(int timer_id)
{
    init_callback_system();

    if (timer_id < 0 || timer_id >= MAX_CALLBACKS)
    {
        printf("Error: Timer ID %d out of range (0-%d)\n", timer_id, MAX_CALLBACKS - 1);
        return -1;
    }

    if (timer_callbacks[timer_id] == NULL)
    {
        printf("Error: No timer callback registered for ID %d\n", timer_id);
        return -1;
    }

    timer_callbacks[timer_id] = NULL;
    printf("Timer callback unregistered for ID: %d\n", timer_id);
    return 0;
}

int ocre_unregister_gpio_callback(int pin, int port)
{
    init_callback_system();

    int slot = -1;
    for (int i = 0; i < MAX_CALLBACKS; i++)
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
        return -1;
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
        return -1;
    }

    int slot = -1;
    for (int i = 0; i < MAX_CALLBACKS; i++)
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
        return -1;
    }

    message_callback_topics[slot][0] = '\0';
    message_callbacks[slot] = NULL;
    printf("Message callback unregistered for topic: %s\n", topic);

    // Unsubscribe from the messaging system
    int ret = ocre_unsubscribe_message(topic);
    if (ret != 0)
    {
        printf("Error: Failed to unsubscribe from topic %s\n", topic);
        return ret;
    }

    return 0;
}

// Update ocre_poll_events to handle message events
void ocre_poll_events(void)
{
    event_data_t event_data;
    int event_count = 0;
    const int max_events_per_loop = 5;

    // Buffers for message event data
    char topic[OCRE_MAX_TOPIC_LEN] = {0};
    char content_type[OCRE_MAX_TOPIC_LEN] = {0};
    uint8_t payload[OCRE_MAX_PAYLOAD_LEN] = {0};
    uint32_t payload_len = 0;

    while (event_count < max_events_per_loop)
    {
        int ret = ocre_get_event(
            (uint32_t)&event_data.type,
            (uint32_t)&event_data.id,
            (uint32_t)&event_data.port,
            (uint32_t)&event_data.state,
            (uint32_t)topic,
            (uint32_t)content_type,
            (uint32_t)payload,
            (uint32_t)&payload_len);
        if (ret != 0)
        {
            break;
        }

        // Dispatch events
        switch (event_data.type)
        {
        case OCRE_RESOURCE_TYPE_TIMER:
            timer_callback(event_data.id);
            break;
        case OCRE_RESOURCE_TYPE_GPIO:
            gpio_callback(event_data.id, event_data.state, event_data.port);
            break;
        case OCRE_RESOURCE_TYPE_MESSAGE:
            message_callback(topic, content_type, payload, payload_len);
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
/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../atym-sdk/ocre_api.h"

#define TIMER_ID        1
#define TIMER_INTERVAL  500

typedef struct {
    int port;
    int pin;
    ocre_gpio_direction_t direction;
    ocre_gpio_pin_state_t state;
} gpio_config_t;

static const gpio_config_t led_config = {
    .port = 7,
    .pin = 7,
    .direction = OCRE_GPIO_DIR_OUTPUT,
    .state = OCRE_GPIO_PIN_SET,
};

void blink_led(void) {
    static bool led_state = false;
    led_state = !led_state;
    ocre_gpio_pin_toggle(led_config.port, led_config.pin);
    printf("%s\n", led_state ? "+" : ".");
}

__attribute__((export_name("timer_callback"))) 
void timer_callback(int timer_id) {
    blink_led();
}

__attribute__((export_name("poll_events"))) 
void poll_events(void) {
    event_data_t event_data;
    int event_count = 0;
    const int max_events_per_loop = 5;

    uint32_t base_offset = (uint32_t)&event_data;
    uint32_t type_offset = base_offset + offsetof(event_data_t, type);
    uint32_t id_offset = base_offset + offsetof(event_data_t, id);
    uint32_t port_offset = base_offset + offsetof(event_data_t, port);
    uint32_t state_offset = base_offset + offsetof(event_data_t, state);

    while (event_count < max_events_per_loop) {
        int ret = ocre_get_event(type_offset, id_offset, port_offset, state_offset);
        if (ret != 0) {
            break;
        }

        int32_t type = event_data.type;
        int32_t id = event_data.id;
        int32_t port = event_data.port;
        int32_t state = event_data.state;
        
        if (type == OCRE_RESOURCE_TYPE_TIMER && port == 0) {
            timer_callback(id);
        }
        event_count++;
    }

    if (event_count == 0) {
        ocre_sleep(10);
    }
}

int main(void) {
    int ret;

    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Blinky app initializing...\n");

    ret = ocre_gpio_init();
    if (ret != 0) {
        printf("Failed to initialize GPIO: %d\n", ret);
        return ret;
    }

    ret = ocre_gpio_configure(led_config.port, led_config.pin, led_config.direction);
    if (ret != 0) {
        printf("Failed to configure GPIO: port=%d, pin=%d, direction=%d ret=%d\n", 
               led_config.port, led_config.pin, led_config.direction, ret);
        return ret;
    }

    ret = ocre_gpio_pin_set(led_config.port, led_config.pin, led_config.state);
    if (ret != 0) {
        printf("Failed to set GPIO pin state: port=%d, pin=%d, state=%d, ret=%d\n", 
               led_config.port, led_config.pin, led_config.state, ret);
        return ret;
    }

    ret = ocre_register_dispatcher(OCRE_RESOURCE_TYPE_TIMER, "timer_callback");
    if (ret != 0) {
        printf("Failed to register timer dispatcher: %d\n", ret);
        return ret;
    }

    ret = ocre_timer_create(TIMER_ID);
    if (ret != 0) {
        printf("Failed to create timer: %d\n", ret);
        return ret;
    }

    ret = ocre_timer_start(TIMER_ID, TIMER_INTERVAL, true);
    if (ret != 0) {
        printf("Failed to start timer: %d\n", ret);
        return ret;
    }
    
    while (1) {
        poll_events();
        ocre_sleep(10);
    }

    return 0;
}

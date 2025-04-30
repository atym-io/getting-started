/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include <stdio.h>
#include <stdbool.h>
#include "ocre_api.h"

// Define the timer ID and interval for the periodic timer
#define TIMER_ID        1
#define TIMER_INTERVAL  500

// Define the GPIO pin, port, state, and direction for the LED
typedef struct {
    int pin;
    int port;
    char direction;
    char state;
} gpio_config_t;

static const gpio_config_t led_config = {
    .port = 7,
    .pin = 7, // Red LED is on pin 6, Green LED is on pin 7
    .direction = OCRE_GPIO_DIR_OUTPUT,
    .state = OCRE_GPIO_PIN_SET,
};

// Manages the LED state, and called by the timer callback function
void blink_led(void) {
    static bool led_state = false;
    led_state = !led_state;
    ocre_gpio_pin_set(led_config.port, led_config.pin, led_state ? OCRE_GPIO_PIN_RESET : OCRE_GPIO_PIN_SET); // LEDS active low
    printf("%s\n", led_state ? "+" : ".");
}

// Timer callback function exposed to Zephyr
__attribute__((export_name("timer_callback"))) 
void exported_timer_callback(int timer_id) {
    blink_led();
}

int main(void) {
    int ret;

    // Initialize app
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Blinky app initializing...\n");

    // Initialize the GPIO subsystem
    ret = ocre_gpio_init();
    if (ret != 0) {
        printf("Failed to initialize GPIO: %d\n", ret);
        return ret;
    }

    // Configure the LED pin
    ret = ocre_gpio_configure(led_config.port, led_config.pin, led_config.direction);
    if (ret != 0) {
        printf("Failed to configure GPIO: port=%d, pin=%d, direction=%d ret=%d\n", 
               led_config.port, led_config.pin, led_config.direction, ret);
        return ret;
    }

    // Set initial PIN State
    ret = ocre_gpio_pin_set(led_config.port, led_config.pin, led_config.state);
    if (ret != 0) {
        printf("Failed to set GPIO pin state: port=%d, pin=%d, state=%d, ret=%d\n", 
               led_config.port, led_config.pin, led_config.state, ret);
        return ret;
    }

    // Create and start the periodic timer using the ocre timer API
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
    
    // Keep the application running and prevent busy waiting
    while (1) {
        ocre_sleep(100);
    }

    return 0;
}
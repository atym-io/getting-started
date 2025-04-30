/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include <stdio.h>
#include <stdbool.h>
#include "ocre_api.h"

// Define the timer ID and interval for the periodic timer
#define TIMER_ID        1
#define TIMER_INTERVAL  500

// Define LED port (both RED and GREEN LEDs are on port 7)
#define LED_PORT 7

// Define LED config
static const ocre_gpio_config_t led_config = {
    // RED LED
    .pin = 6,
    // GREEN LED
    // .pin = 7,
    .direction = OCRE_GPIO_DIR_OUTPUT
};

// Track LED state (setting it to 1 initially, as LEDS are active low and we want to turn it off) 
static ocre_gpio_pin_state_t led_state = OCRE_GPIO_PIN_SET;

// Manages the LED state, and called by the timer callback function
void blink_led(void) {
    static bool led_state = false;
    ocre_gpio_pin_toggle(LED_PORT, led_config.pin);
    led_state = !led_state;
    printf("%s\r", led_state ? "+" : ".");
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
    ret = ocre_gpio_configure(LED_PORT, led_config.pin, led_config.direction);
    if (ret != 0) {
        printf("Failed to configure GPIO: port=%d, pin=%d, direction=%d ret=%d\n", 
               LED_PORT, led_config.pin, led_config.direction, ret);
        return ret;
    }

    // Set initial PIN State
    ret = ocre_gpio_pin_set(LED_PORT, led_config.pin, led_state);
    if (ret != 0) {
        printf("Failed to set GPIO pin state: port=%d, pin=%d, state=%d, ret=%d\n", 
               LED_PORT, led_config.pin, led_state, ret);
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
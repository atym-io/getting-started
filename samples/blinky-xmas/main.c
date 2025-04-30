/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include <stdio.h>
#include <stdbool.h>
#include "ocre_api.h"

// Define the timer ID and interval for the periodic timer
#define TIMER_ID        1
#define TIMER_INTERVAL  500

// Define the GPIO port and pins for both LEDs
#define LED_PORT        7
#define RED_LED_PIN     6
#define GREEN_LED_PIN   7
#define OCRE_GPIO_DIR_OUTPUT 1
#define OCRE_GPIO_PIN_SET    1
#define OCRE_GPIO_PIN_RESET  0

// Manages the LED state, and called by the timer callback function
void toggle_leds(void) {
    static bool red_active = true;
    
    if (red_active) {
        // Turn on red LED (active low), turn off green LED
        ocre_gpio_pin_set(LED_PORT, RED_LED_PIN, OCRE_GPIO_PIN_RESET);
        ocre_gpio_pin_set(LED_PORT, GREEN_LED_PIN, OCRE_GPIO_PIN_SET);
        printf("LED is: RED");  // No newline character
    } else {
        // Turn on green LED (active low), turn off red LED
        ocre_gpio_pin_set(LED_PORT, RED_LED_PIN, OCRE_GPIO_PIN_SET);
        ocre_gpio_pin_set(LED_PORT, GREEN_LED_PIN, OCRE_GPIO_PIN_RESET);
        printf("LED is: GREEN");  // No newline character
    }
    
    // Add carriage return to overwrite the same line
    printf("\r");
    fflush(stdout);
    
    // Toggle state for next time
    red_active = !red_active;
}

// Timer callback function exposed to Zephyr
__attribute__((export_name("timer_callback"))) 
void exported_timer_callback(int timer_id) {
    toggle_leds();
}

int main(void) {
    int ret;
    
    // Initialize app
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Blinky-xmas app initializing...\n");
    
    // Initialize the GPIO subsystem
    ret = ocre_gpio_init();
    if (ret != 0) {
        printf("Failed to initialize GPIO: %d\n", ret);
        return ret;
    }
    
    // Configure the red LED pin
    ret = ocre_gpio_configure(LED_PORT, RED_LED_PIN, OCRE_GPIO_DIR_OUTPUT);
    if (ret != 0) {
        printf("Failed to configure red LED GPIO: pin=%d, ret=%d\n", RED_LED_PIN, ret);
        return ret;
    }
    
    // Configure the green LED pin
    ret = ocre_gpio_configure(LED_PORT, GREEN_LED_PIN, OCRE_GPIO_DIR_OUTPUT);
    if (ret != 0) {
        printf("Failed to configure green LED GPIO: pin=%d, ret=%d\n", GREEN_LED_PIN, ret);
        return ret;
    }
    
    // Set initial LED states - start with red LED on, green LED off
    ret = ocre_gpio_pin_set(LED_PORT, RED_LED_PIN, OCRE_GPIO_PIN_RESET);
    if (ret != 0) {
        printf("Failed to set red LED initial state: pin=%d, ret=%d\n", RED_LED_PIN, ret);
        return ret;
    }
    
    ret = ocre_gpio_pin_set(LED_PORT, GREEN_LED_PIN, OCRE_GPIO_PIN_SET);
    if (ret != 0) {
        printf("Failed to set green LED initial state: pin=%d, ret=%d\n", GREEN_LED_PIN, ret);
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
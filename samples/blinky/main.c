/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */

#include <stdio.h>
#include <stdbool.h>
#include "ocre_api.h"

#define TIMER_ID        1
#define TIMER_INTERVAL  500

// Manages the LED state, and called by the timer callback function
void blink_led(void) {
    static bool led_state = false;
    led_state = !led_state;
    printf("%s\r", led_state ? "+" : ".");
    fflush(stdout);
}

// Timer callback function exposed to Zephyr
__attribute__((export_name("timer_callback"))) 
void exported_timer_callback(int timer_id) {
    blink_led();
}

// Application entry point
int main(void) {
    int ret;

    printf("Blinky app initializing...\n");
   
    // Create the periodic timer using the ocre timer API
    ret = ocre_timer_create(TIMER_ID);
    if (ret != 0) {
        printf("Failed to create timer: %d\n", ret);
        return ret;
    }

    // Start the periodic timer using the ocre timer API
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
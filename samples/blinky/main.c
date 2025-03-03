/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */

#include <stdio.h>
#include <stdbool.h>
#include "ocre_api.h"

void blink_led() {
    static bool led_state = false;
    led_state = !led_state;
    printf("%s\r", led_state ? "+" : ".");
    fflush(stdout);
}

// Export the function so that the host (Zephyr) can register it
__attribute__((export_name("timer_callback"))) void exported_timer_callback(int timer_id) {
    blink_led();
}

int main() {
    const int timer_id = 1;
    int interval_ms = 500;
    bool is_periodic = true;

    printf("Blinky app initializing...\n");
    
    ocre_timer_create(timer_id);
    ocre_timer_start(timer_id, interval_ms, is_periodic);
   
    // Sleep for 60s to prevent main from exiting
    ocre_sleep(60000);
   
    printf("Blinky app exiting...\n");
    return 0;
}
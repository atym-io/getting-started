/*
 * Copyright (C) 2024 Atym Incorporated. All rights reserved.
 */

#include <stdio.h>
#include "wa-inc/timer_wasm_app.h"

void update_led(user_timer_t timer) {
  printf(".\n");
}

// Atym container entrypoint
void on_init() {
  user_timer_t timer;
  printf("Blinky app initializing.\n");

  timer = api_timer_create(500, true, false, update_led);
  api_timer_restart(timer, 500);
}
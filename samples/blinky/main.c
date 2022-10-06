/*
 * Copyright (C) 2022 Nubix Incorporated.  All rights reserved.
 */

#include <stdio.h>

#include "bi-inc/nubix_api.h"

void update_led(user_timer_t timer) {
  nbx_toggle_led();
  printf(".");
}

// Nubix container entrypoint
void nubix_init() {
  user_timer_t timer;
  printf("Blinky app initializing.\n");

  timer = nbx_timer_create(500, true, false, update_led);
  nbx_timer_restart(timer, 500);
}

/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include "../ocre_api/ocre_api.h"
#include <stdio.h>
#include <string.h>

#define TIMER_ID 1
#define TOPIC "test/topic"
#define CONTENT_TYPE "text/plain"

void timer_handler(void);

// WASM entry point
int main(void)
{
  ocre_msg_system_init();
  if (ocre_timer_create(TIMER_ID) != OCRE_SUCCESS)
  {
    printf("Failed to create timer %d\n", TIMER_ID);
  }
  if (ocre_register_timer_callback(TIMER_ID, timer_handler) != OCRE_SUCCESS)
  {
    printf("Failed to register timer callback\n");
  }
  if (ocre_timer_start(TIMER_ID, 1000, 1) != OCRE_SUCCESS)
  {
    printf("Failed to start timer %d\n", TIMER_ID);
  }
  printf("Publisher initialized: timer %d started, publishing to %s\n", TIMER_ID, TOPIC);
  while (1)
  {
    ocre_poll_events();
  }
  return 0;
}

void timer_handler(void)
{
  static int message_count = 0;
  char payload[32];
  snprintf(payload, sizeof(payload), "Message %d", message_count++);
  if (ocre_publish_message(TOPIC, CONTENT_TYPE, payload, strlen(payload) + 1) == OCRE_SUCCESS)
  {
    printf("Published: %s to topic %s\n", payload, TOPIC);
  }
  else
  {
    printf("Failed to publish message %d\n", message_count);
  }
}
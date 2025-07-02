/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include "ocre_api.h"
#include <stdio.h>
#include <string.h>

#define MESSAGE_TOPIC "test/topic"
#define MESSAGE_CONTENT_TYPE "text/plain"
#define MESSAGE_PAYLOAD "Hello, OCRE!"
#define TIMER_ID 1

static void timer_callback_handler(void)
{
  const char *payload = MESSAGE_PAYLOAD;
  uint32_t payload_len = strlen(payload) + 1;

  printf("Sender: Publishing message to topic '%s'\n", MESSAGE_TOPIC);
  int ret = ocre_publish_message(MESSAGE_TOPIC, MESSAGE_CONTENT_TYPE, payload, payload_len);
  if (ret != 0)
  {
    printf("Sender: Failed to publish message: %d\n", ret);
  }
}

int main(void)
{
  printf("Sender: Initializing messaging system\n");
  int ret = ocre_msg_system_init();
  if (ret != 0)
  {
    printf("Sender: Failed to initialize messaging system: %d\n", ret);
    return -1;
  }

  printf("Sender: Creating timer %d\n", TIMER_ID);
  ret = ocre_timer_create(TIMER_ID);
  if (ret != 0)
  {
    printf("Sender: Failed to create timer: %d\n", ret);
    return -1;
  }

  printf("Sender: Registering timer callback\n");
  ret = ocre_register_timer_callback(TIMER_ID, timer_callback_handler);
  if (ret != 0)
  {
    printf("Sender: Failed to register timer callback: %d\n", ret);
    return -1;
  }

  printf("Sender: Starting timer %d (periodic, 1000ms)\n", TIMER_ID);
  ret = ocre_timer_start(TIMER_ID, 1000, 1); // Periodic, 1-second interval
  if (ret != 0)
  {
    printf("Sender: Failed to start timer: %d\n", ret);
    return -1;
  }

  printf("Sender: Entering event loop\n");
  while (1)
  {
    ocre_poll_events();
    ocre_sleep(100);
  }

  return 0;
}
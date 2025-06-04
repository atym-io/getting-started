/*
 * Copyright (C) 2024 Atym Incorporated. All rights reserved.
 */
#include <stdio.h>
#include "ocre_api.h"

#define LED0_PORT 7
#define LED0 7

int main()
{
   int ret;
   printf("\
            ██   ██ ███████ ██      ██       ██████      \n\
            ██   ██ ██      ██      ██      ██    ██     \n\
            ███████ █████   ██      ██      ██    ██     \n\
            ██   ██ ██      ██      ██      ██    ██     \n\
            ██   ██ ███████ ███████ ███████  ██████      \n\
                                                         \n\
                                                         \n\
            ██     ██  ██████  ██████  ██      ██████    \n\
            ██     ██ ██    ██ ██   ██ ██      ██   ██   \n\
            ██  █  ██ ██    ██ ██████  ██      ██   ██   \n\
            ██ ███ ██ ██    ██ ██   ██ ██      ██   ██   \n\
             ███ ███   ██████  ██   ██ ███████ ██████    \n\n");

   if (ocre_gpio_configure(LED0_PORT, LED0, OCRE_GPIO_DIR_OUTPUT) != 0)
   {
      printf("LED config failed\n");
      return -1;
   }
   for (int i = 0; i < 5; i++)
   {
      printf("Testing LED: Setting ON\n");
      if (ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET) != 0) // Active-low ON
         printf("Failed to set LED ON\n");
      ocre_sleep(1000);
      printf("Testing LED: Setting OFF\n");
      if (ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) != 0) // Active-low OFF
         printf("Failed to set LED OFF\n");
      ocre_sleep(1000);
   }

   return 0;
}
#include <stdio.h>
#include <unistd.h>
#include "ocre_api.h"

#define LED0_PORT 7
#define LED0 7

#define BUTTON_PORT 2
#define BUTTON_0 13

void blink_led()
{
  static bool led_state = false;

  // Toggle the LED state
  // ocre_gpio_pin_toggle(GPIO_PORT, LED0);

  led_state ? ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET) : ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET);

  led_state = !led_state;
  printf("%s\n", led_state ? "+" : ".");
  fflush(stdout);
}

// Export the function so that the runtime can register it
__attribute__((export_name("timer_callback"))) void exported_timer_callback(int timer_id)
{
  blink_led();
}

__attribute__((export_name("gpio_callback"))) void gpio_callback(int pin, int state)
{
  printf("**>> GPIO changed state. Pin: %d, State: %d\n", pin, state);
  fflush(stdout);
}

int main()
{
  printf("\
    ██████  ██      ██ ███    ██ ██   ██ ██    ██ \n\
    ██   ██ ██      ██ ████   ██ ██  ██   ██  ██  \n\
    ██████  ██      ██ ██ ██  ██ █████     ████   \n\
    ██   ██ ██      ██ ██  ██ ██ ██  ██     ██    \n\
    ██████  ███████ ██ ██   ████ ██   ██    ██    \n");
  printf("Blinky Test\n");
  /*===========================================================================================*/
  fflush(stdout);

  const int timer_id = 1;
  int interval_ms = 3000;
  bool is_periodic = true;

  // Setup GPIO
  ocre_gpio_init();

  ocre_gpio_configure(LED0_PORT, LED0, OCRE_GPIO_DIR_OUTPUT);
  ocre_gpio_configure(BUTTON_PORT, BUTTON_0, OCRE_GPIO_DIR_INPUT);
  ocre_gpio_register_callback(BUTTON_PORT, BUTTON_0);

  ocre_timer_create(timer_id);
  printf("Timer created. ID: %d, Interval: %dms\n\n", timer_id, interval_ms);
  fflush(stdout);

  ocre_timer_start(timer_id, interval_ms, is_periodic);

  // Wait for exit
  ocre_pause();

  printf("Blinky exiting.\n");
  return 0;
}
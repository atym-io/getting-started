#include <stdio.h>
#include <stdint.h>
#include "ocre_api.h"

// Ensure variables are aligned for WASM memory
typedef struct
{
  int32_t type;
  int32_t id;
  int32_t port;
  int32_t state;
} event_data_t;

#define LED0_PORT 7
#define LED0 7
#define BUTTON_PORT 2
#define BUTTON_0 13

static void blink_led(void)
{
  static bool led_state = false;
  static int blink_count = 0;
  // Active-low: RESET (low) = ON, SET (high) = OFF
  int ret = led_state ? ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) // ON
                      : ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET);  // OFF
  if (ret != 0)
  {
    printf("Failed to set LED: %d\n", ret);
  }
  else
  {
    printf("LED state set to %s (logical %d, count %d)\n", led_state ? "ON" : "OFF", led_state, ++blink_count);
  }
  led_state = !led_state;
}

__attribute__((export_name("timer_callback"))) void timer_callback(int timer_id)
{
  if (timer_id != 1)
  {
    printf("Invalid timer ID: %d\n", timer_id);
    return;
  }
  printf("Timer callback: ID=%d\n", timer_id);
  blink_led();
}

__attribute__((export_name("gpio_callback"))) void gpio_callback(int pin, int state)
{
  if (pin != BUTTON_0 || (state != OCRE_GPIO_PIN_SET && state != OCRE_GPIO_PIN_RESET))
  {
    printf("Invalid GPIO event: pin=%d, state=%d\n", pin, state);
    return;
  }
  printf("GPIO callback: pin=%d, state=%d\n", pin, state);
  blink_led();
}

__attribute__((export_name("poll_events"))) void poll_events(void)
{
  event_data_t event_data;
  int event_count = 0;
  const int max_events_per_loop = 5;

  // Get the base address of event_data as an offset in WASM memory
  uint32_t base_offset = (uint32_t)&event_data;
  uint32_t type_offset = base_offset + offsetof(event_data_t, type);
  uint32_t id_offset = base_offset + offsetof(event_data_t, id);
  uint32_t port_offset = base_offset + offsetof(event_data_t, port);
  uint32_t state_offset = base_offset + offsetof(event_data_t, state);

  while (event_count < max_events_per_loop)
  {
    int ret = ocre_get_event(type_offset, id_offset, port_offset, state_offset);
    if (ret != 0)
    {
      if (ret != -1) // -1 likely means no events
        printf("Failed to get event: %d\n", ret);
      break;
    }

    // Access event data
    int32_t type = event_data.type;
    int32_t id = event_data.id;
    int32_t port = event_data.port;
    int32_t state = event_data.state;

    if (type < 0 || type >= OCRE_RESOURCE_TYPE_COUNT || id < 0 || port < 0 ||
        (type == OCRE_RESOURCE_TYPE_GPIO && state != OCRE_GPIO_PIN_SET && state != OCRE_GPIO_PIN_RESET))
    {
      printf("Invalid event: type=%d, id=%d, port=%d, state=%d\n", type, id, port, state);
      continue;
    }

    printf("Retrieved event: type=%d, id=%d, port=%d, state=%d\n", type, id, port, state);
    if (type == OCRE_RESOURCE_TYPE_TIMER && port == 0)
    {
      timer_callback(id);
    }
    else if (type == OCRE_RESOURCE_TYPE_GPIO && port == BUTTON_PORT)
    {
      gpio_callback(id, state);
    }
    else
    {
      printf("Unknown event: type=%d, id=%d, port=%d, state=%d\n", type, id, port, state);
    }
    event_count++;
  }

  if (event_count == 0)
  {
    ocre_sleep(10);
  }
}

int main(void)
{
  const int timer_id = 1;
  int interval_ms = 3000;
  bool is_periodic = true;

  printf("\
    ██████  ██      ██ ███    ██ ██   ██ ██    ██ \n\
    ██   ██ ██      ██ ████   ██ ██  ██   ██  ██  \n\
    ██████  ██      ██ ██ ██  ██ █████     ████   \n\
    ██   ██ ██      ██ ██  ██ ██ ██  ██     ██    \n\
    ██████  ███████ ██ ██   ████ ██   ██    ██    \n");
  printf("Blinky Test\n");

  // Initialize GPIO
  if (ocre_gpio_init() != 0)
  {
    printf("GPIO init failed\n");
    return -1;
  }

  // Configure LED
  if (ocre_gpio_configure(LED0_PORT, LED0, OCRE_GPIO_DIR_OUTPUT) != 0)
  {
    printf("LED config failed\n");
    return -1;
  }

  // Configure Button
  if (ocre_gpio_configure(BUTTON_PORT, BUTTON_0, OCRE_GPIO_DIR_INPUT) != 0)
  {
    printf("Button config failed\n");
    return -1;
  }
  if (ocre_gpio_register_callback(BUTTON_PORT, BUTTON_0) != 0)
  {
    printf("Button callback registration failed\n");
    return -1;
  }

  // Manual GPIO test
  printf("Testing LED: Setting ON\n");
  if (ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_RESET) != 0) // Active-low ON
    printf("Failed to set LED ON\n");
  ocre_sleep(1000);
  printf("Testing LED: Setting OFF\n");
  if (ocre_gpio_pin_set(LED0_PORT, LED0, OCRE_GPIO_PIN_SET) != 0) // Active-low OFF
    printf("Failed to set LED OFF\n");
  ocre_sleep(1000);

  // Register dispatchers
  if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_TIMER, "timer_callback") != 0)
  {
    printf("Failed to register timer dispatcher\n");
    return -1;
  }
  if (ocre_register_dispatcher(OCRE_RESOURCE_TYPE_GPIO, "gpio_callback") != 0)
  {
    printf("Failed to register GPIO dispatcher\n");
    return -1;
  }

  // Create and start timer
  if (ocre_timer_create(timer_id) != 0)
  {
    printf("Timer creation failed\n");
    return -1;
  }
  printf("Timer created. ID: %d, Interval: %dms\n", timer_id, interval_ms);

  if (ocre_timer_start(timer_id, interval_ms, is_periodic) != 0)
  {
    printf("Timer start failed\n");
    return -1;
  }

  while (1)
  {
    poll_events();
    ocre_sleep(10);
  }

  printf("Blinky exiting.\n");
  return 0;
}
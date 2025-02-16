#include <stdio.h>
#include "ocre_api.h"

void timer_callback(int timer_id)
{
  printf("Timer expired! Timer ID: %d\n", timer_id);
}

// Export the function so that the host (Zephyr) can register it
__attribute__((export_name("timer_callback"))) void exported_timer_callback(int timer_id)
{
  timer_callback(timer_id);
}

int main()
{
  printf("\
    ██     ██ ███████ ██████   █████  ███████ ███████ ███████ ███    ███ ██████  ██   ██    ██ \n\
    ██     ██ ██      ██   ██ ██   ██ ██      ██      ██      ████  ████ ██   ██ ██    ██  ██  \n\
    ██  █  ██ █████   ██████  ███████ ███████ ███████ █████   ██ ████ ██ ██████  ██     ████   \n\
    ██ ███ ██ ██      ██   ██ ██   ██      ██      ██ ██      ██  ██  ██ ██   ██ ██      ██    \n\
     ███ ███  ███████ ██████  ██   ██ ███████ ███████ ███████ ██      ██ ██████  ███████ ██    \n\
                                                                                               \n\
                                                                             powered by Atym   \n");

  printf("\n TEST TIMER API\n");

  const int timer_id = 1;
  int interval_ms = 500;
  bool is_periodic = true;
  int status = 0;

  // ocre_timer_init();

  status = ocre_timer_create(timer_id);
  if (status == 0)
  {
    printf("\n TIMER CREATED. ID: %d \n", timer_id);
  }
  else
  {
    printf("\n TIMER NOT CREATED. ID: %d \n", timer_id);
  }

  status = ocre_timer_start(timer_id, interval_ms, is_periodic);
  if (status == 0)
  {
    printf("\n TIMER STARTED. ID: %d, Interval: %d ms \n",
           timer_id, interval_ms);
  }
  else
  {
    printf("\n TIMER NOT STARTED. ID: %d", timer_id);
  }

  int remaining_time = ocre_timer_get_remaining(timer_id);
  printf("\n Remaining Time = %d \n", remaining_time);

  printf("\n Debug - Cleaning up timer\n");

  status = ocre_timer_stop(timer_id);
  if (status == 0)
  {
    printf("\n TIMER STOPPED. ID: %d \n", timer_id);
  }
  else
  {
    printf("\n TIMER NOT STOPPED. ID: %d \n", timer_id);
  }

  status = ocre_timer_delete(timer_id);
  if (status == 0)
  {
    printf("\n TIMER DELETED. ID: %d \n", timer_id);
  }
  else
  {
    printf("\n TIMER NOT DELETED. ID: %d \n", timer_id);
  }

  return 0;
}

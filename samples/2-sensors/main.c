#include <stdio.h>
#include <unistd.h>
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
    ███████ ███████ ███    ██ ███████  ██████  ██████  ███████ \n\
    ██      ██      ████   ██ ██      ██    ██ ██   ██ ██      \n\
    ███████ █████   ██ ██  ██ ███████ ██    ██ ██████  ███████ \n\
         ██ ██      ██  ██ ██      ██ ██    ██ ██   ██      ██ \n\
    ███████ ███████ ██   ████ ███████  ██████  ██   ██ ███████ \n");
  printf("Sensors Test\n");
  /*===========================================================================================*/
  fflush(stdout);
  int ret = ocre_sensors_init();
  if (ret)
  {
    printf("Sensors not initialized\n");
  }

  int nr_of_sensors = ocre_sensors_discover();
  printf("Sensors found:%d\n", nr_of_sensors);

  int channel_count = 0;
  int channel_type[5] = {0};

  for (int i = 0; i < nr_of_sensors; i++)
  {
    int handle = ocre_sensors_get_handle(i);
    if (handle == -1)
    {
      printf("Sensor NOK\n");
    }
    else
    {
      int ret = ocre_sensors_open(handle);
      if (!ret)
      {
        printf("Opened sensor with id: %d, with handle: %d\n", i, ret);
        int channel_count = ocre_sensors_get_channel_count(i);
        for (int j = 0; j < channel_count; j++)
        {
          channel_type[j] = ocre_sensors_get_channel_type(i, j);
          if (channel_type[j] == -1)
          {
            printf("Sensor channel type NOK\n");
          }
          else
          {
            int value_read = ocre_sensors_read(i, channel_type[j]);
            printf("Read value: %d, form sensor with id: %d, with handle: %d\n", value_read, i, ret);
          }
        }
      }
      else
      {
        printf("Could not open sensor with id: %d, with handle: %d\n", i, ret);
      }
    }
  }
  // Wait for exit
  ocre_pause();

  printf("Sensors exiting.\n");
  return 0;
}
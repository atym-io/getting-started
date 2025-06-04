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

  // Initialize the sensor API
  int ret = ocre_sensors_init();
  if (ret != 0)
  {
    printf("Error: Sensors not initialized (code: %d)\n", ret);
    return -1;
  }
  printf("Sensors initialized successfully\n");

  // Discover available sensors
  int nr_of_sensors = ocre_sensors_discover();
  printf("Sensors found: %d\n", nr_of_sensors);
  if (nr_of_sensors <= 0)
  {
    printf("Error: No sensors discovered\n");
    return -1;
  }

  printf("\n=== Testing String-Based API ===\n");

  // Test accessing sensors by name
  ocre_sensor_handle_t rng_handle;
  printf("Trying to open 'RNG Sensor'...\n");
  if (ocre_sensors_open_by_name("RNG Sensor") != 0)
  {
    printf("Could not open 'RNG Sensor'\n");
  }
  else
  {
    printf("Successfully opened 'RNG Sensor', handle: %d\n", rng_handle);

    // Get channel count
    int channel_count = ocre_sensors_get_channel_count_by_name("RNG Sensor");
    printf("'RNG Sensor' has %d channels\n", channel_count);

    // Get first channel type
    int channel_type = -1;
    if (channel_count > 0)
    {
      channel_type = ocre_sensors_get_channel_type_by_name("RNG Sensor", 0);
      printf("'RNG Sensor' first channel type: %d\n", channel_type);
    }

    // Read sensor data
    if (channel_type >= 0)
    {
      int value = ocre_sensors_read_by_name("RNG Sensor", channel_type);
      printf("Read 'RNG Sensor' value = %d\n", value);
    }
  }

  // Try another sensor
  ocre_sensor_handle_t accel_handle;
  printf("\nTrying to open 'imu'...\n");
  if (ocre_sensors_open_by_name("imu") != 0)
  {
    printf("Could not open 'imu'\n");
  }
  else
  {
    printf("Successfully opened 'imu', handle: %d\n", accel_handle);

    // Get channel count
    int channel_count = ocre_sensors_get_channel_count_by_name("imu");
    printf("'imu' has %d channels\n", channel_count);

    // Try to read from each channel
    for (int j = 0; j < channel_count; j++)
    {
      int channel_type = ocre_sensors_get_channel_type_by_name("imu", j);
      if (channel_type >= 0)
      {
        int value = ocre_sensors_read_by_name("imu", channel_type);
        printf("Channel %d (type %d) value = %d\n", j, channel_type, value);
      }
    }
  }

  printf("\n=== Testing Handle-Based API ===\n");

  printf("Sensor tests completed.\n");

  // Wait for exit (using your existing pause function)
  ocre_pause();

  printf("Sensors exiting.\n");
  return 0;
}
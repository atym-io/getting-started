/*
 * Copyright (C) 2022 Nubix Incorporated.  All rights reserved.
 */

#include <stddef.h>
#include <stdio.h>

#include "bi-inc/nubix_api.h"

#define ENV_VAR_SENSOR_POLLING_INTERVAL "sensorPollingInterval"

#define ENV_VAR_DEFAULT_VALUE_SENSOR_POLLING_INTERVAL 5000

static nbx_sensor_t temperature_sensor = NULL;

static int sensorPollingInterval =
    ENV_VAR_DEFAULT_VALUE_SENSOR_POLLING_INTERVAL;

/* temperature sensor event callback*/
void temp_sensor_event_handler(nbx_sensor_t sensor, nbx_attr_container_t *value,
                               void *user_data) {
  // printf("### app temp sensor event\n");

  nbx_publish_event("sensor/temperature", value,
                    attr_container_get_serialize_length(value));

  // attr_container_dump(value);

  attr_container_destroy(value);
}

void nubix_init() {
  char *user_data = NULL;

  printf("getting env variable '%s'\n", ENV_VAR_SENSOR_POLLING_INTERVAL);
  if (nbx_getenv_int(ENV_VAR_SENSOR_POLLING_INTERVAL, &sensorPollingInterval,
                     sensorPollingInterval)) {
    printf("'%s' value = %d\n", ENV_VAR_SENSOR_POLLING_INTERVAL,
           sensorPollingInterval);
  } else {
    printf("'%s' value not found, using default value of %d\n",
           ENV_VAR_SENSOR_POLLING_INTERVAL, sensorPollingInterval);
  }

  /* open temperature sensor */
  temperature_sensor =
      nbx_sensor_open("temperature", 0, temp_sensor_event_handler, user_data);

  /* config the temperature sensor */
  nbx_sensor_config(temperature_sensor, sensorPollingInterval, 0, 0);
}

void nubix_destroy() {
  if (temperature_sensor) {
    nbx_sensor_config(temperature_sensor, 0, 0, 0);
  }
}

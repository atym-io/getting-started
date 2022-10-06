/*
 * Copyright (C) 2022 Nubix Incorporated.  All rights reserved.
 */

#include "bi-inc/nubix_api.h"

#define SINGLE_FLOAT_CHAR_SIZE 16

#define ENV_VAR_UNITS "units"
#define ENV_VAR_INTERVALS "intervals"
#define ENV_VAR_LOWER_THRESHOLD "lowerThreshold"
#define ENV_VAR_UPPER_THRESHOLD "upperThreshold"
#define ENV_VAR_THRESHOLD_EXCEEDED_COUNT "thresholdExceededCount"
#define ENV_VAR_RETURN_TO_NORMAL_COUNT "returnToNormalCount"

#define ENV_VAR_DEFAULT_VALUE_UNITS "C"
#define ENV_VAR_DEFAULT_VALUE_INTERVALS 12
#define ENV_VAR_DEFAULT_VALUE_LOWER_THRESHOLD 40
#define ENV_VAR_DEFAULT_VALUE_UPPER_THRESHOLD 46
#define ENV_VAR_DEFAULT_VALUE_THRESHOLD_EXCEEDED_COUNT 3
#define ENV_VAR_DEFAULT_VALUE_RETURN_TO_NORMAL_COUNT 3

#define UNITS_CELCIUS "C"
#define UNITS_FAHRENHEIT "F"

static char *units = ENV_VAR_DEFAULT_VALUE_UNITS;
static int intervals = ENV_VAR_DEFAULT_VALUE_INTERVALS;
static float upperThreshold = ENV_VAR_DEFAULT_VALUE_UPPER_THRESHOLD;
static float lowerThreshold = ENV_VAR_DEFAULT_VALUE_LOWER_THRESHOLD;
static int thresholdExceededCount =
    ENV_VAR_DEFAULT_VALUE_THRESHOLD_EXCEEDED_COUNT;
static int returnToNormalCount = ENV_VAR_DEFAULT_VALUE_RETURN_TO_NORMAL_COUNT;

static float *dataPoints = NULL;
static int dataPointsCount = 0;
static float sumDataPoints = 0;

static bool overUpperThreshold = false, underLowerThreshold = false;
static int overUpperThresholdCount = 0;
static int underLowerThresholdCount = 0;
static int normalCount = 0;

static char alertBuffer[64];

char tempChar[SINGLE_FLOAT_CHAR_SIZE + 1];

void temperature_event_handler(nbx_request_t *request) {
  printf("### temperature event handler called\n");

  if (request->payload != NULL) {
    float value = attr_container_get_as_float(request->payload, "temperature");

    // convert default celcius sensor value to fahrenheit
    if ((strcmp(units, UNITS_FAHRENHEIT)) == 0) {
      value = value * 1.8 + 32;
    }

    printf("temperature reading = %f%s\n", value, units);

    dataPoints[dataPointsCount] = value;
    ++dataPointsCount;
    sumDataPoints += value;

    if (value <= lowerThreshold || value >= upperThreshold) {
      normalCount = 0;
      if (value <= lowerThreshold) {
        ++underLowerThresholdCount;
        if (!underLowerThreshold &&
            underLowerThresholdCount >= thresholdExceededCount) {
          underLowerThreshold = true;
          if (underLowerThresholdCount == thresholdExceededCount) {
            sprintf(alertBuffer,
                    "temperature of %f%s exceeds lower threshold of %f%s",
                    value, units, lowerThreshold, units);

            attr_container_t *temperatureAlertContainer =
                attr_container_create("temperature_alert");
            attr_container_set_string(&temperatureAlertContainer, "alert",
                                      alertBuffer);
            nbx_publish_event(
                "alerts/temperature", temperatureAlertContainer,
                attr_container_get_serialize_length(temperatureAlertContainer));

            attr_container_destroy(temperatureAlertContainer);
          }
        }
      } else {
        ++overUpperThresholdCount;
        if (!overUpperThreshold &&
            overUpperThresholdCount >= thresholdExceededCount) {
          overUpperThreshold = true;
          if (overUpperThresholdCount == thresholdExceededCount) {
            sprintf(alertBuffer,
                    "temperature of %f%s exceeds upper threshold of %f%s",
                    value, units, upperThreshold, units);

            attr_container_t *temperatureAlertContainer =
                attr_container_create("temperature_alert");
            attr_container_set_string(&temperatureAlertContainer, "alert",
                                      alertBuffer);
            nbx_publish_event(
                "alerts/temperature", temperatureAlertContainer,
                attr_container_get_serialize_length(temperatureAlertContainer));

            attr_container_destroy(temperatureAlertContainer);
          }
        }
      }
    } else {
      ++normalCount;
      underLowerThresholdCount = 0;
      overUpperThresholdCount = 0;
      if (underLowerThreshold || overUpperThreshold) {
        if (underLowerThreshold) {
          if (normalCount == returnToNormalCount) {
            underLowerThreshold = false;

            sprintf(alertBuffer, "temperature of %f%s returned to normal range",
                    value, units);

            attr_container_t *temperatureAlertContainer =
                attr_container_create("temperature_alert");
            attr_container_set_string(&temperatureAlertContainer, "alert",
                                      alertBuffer);
            nbx_publish_event(
                "alerts/temperature", temperatureAlertContainer,
                attr_container_get_serialize_length(temperatureAlertContainer));

            attr_container_destroy(temperatureAlertContainer);
          }
        } else {
          if (normalCount == returnToNormalCount) {
            overUpperThreshold = false;

            sprintf(alertBuffer, "temperature of %f%s returned to normal range",
                    value, units);

            attr_container_t *temperatureAlertContainer =
                attr_container_create("temperature_alert");
            attr_container_set_string(&temperatureAlertContainer, "alert",
                                      alertBuffer);
            nbx_publish_event(
                "alerts/temperature", temperatureAlertContainer,
                attr_container_get_serialize_length(temperatureAlertContainer));

            attr_container_destroy(temperatureAlertContainer);
          }
        }
      }
    }

    if (dataPointsCount == intervals) {
      float avgDataPoints = sumDataPoints / intervals;

      sprintf(tempChar, "%f%s", avgDataPoints, units);

      attr_container_t *dataPointsContainer =
          attr_container_create("temperarture_container");
      attr_container_set_string(&dataPointsContainer, "average_temperature",
                                tempChar);

      nbx_publish_event(
          "data/temperature", dataPointsContainer,
          attr_container_get_serialize_length(dataPointsContainer));

      attr_container_destroy(dataPointsContainer);

      dataPointsCount = 0;

      sumDataPoints = 0;
    }

    attr_container_destroy(request->payload);
  }
}

void nubix_init() {
  printf("getting env variable '%s'\n", ENV_VAR_UNITS);
  if (nbx_getenv_string(ENV_VAR_UNITS, &units, units)) {
    printf("'%s' value = %s\n", ENV_VAR_UNITS, units);
  } else {
    printf("'%s' value not found, using default value of %s\n", ENV_VAR_UNITS,
           units);
  }

  printf("getting env variable '%s'\n", ENV_VAR_INTERVALS);
  if (nbx_getenv_int(ENV_VAR_INTERVALS, &intervals, intervals)) {
    printf("'%s' value = %d\n", ENV_VAR_INTERVALS, intervals);
  } else {
    printf("'%s' value not found, using default value of %d\n",
           ENV_VAR_INTERVALS, intervals);
  }

  printf("getting env variable '%s'\n", ENV_VAR_UPPER_THRESHOLD);
  if (nbx_getenv_float(ENV_VAR_UPPER_THRESHOLD, &upperThreshold,
                       upperThreshold)) {
    printf("'%s' value = %f\n", ENV_VAR_UPPER_THRESHOLD, upperThreshold);
  } else {
    printf("'%s' value not found, using default value of %f\n",
           ENV_VAR_UPPER_THRESHOLD, upperThreshold);
  }

  printf("getting env variable '%s'\n", ENV_VAR_LOWER_THRESHOLD);
  if (nbx_getenv_float(ENV_VAR_LOWER_THRESHOLD, &lowerThreshold,
                       lowerThreshold)) {
    printf("'%s' value = %f\n", ENV_VAR_LOWER_THRESHOLD, lowerThreshold);
  } else {
    printf("'%s' value not found, using default value of %f\n",
           ENV_VAR_LOWER_THRESHOLD, lowerThreshold);
  }

  printf("getting env variable '%s'\n", ENV_VAR_THRESHOLD_EXCEEDED_COUNT);
  if (nbx_getenv_int(ENV_VAR_THRESHOLD_EXCEEDED_COUNT, &thresholdExceededCount,
                     thresholdExceededCount)) {
    printf("'%s' value = %d\n", ENV_VAR_THRESHOLD_EXCEEDED_COUNT,
           thresholdExceededCount);
  } else {
    printf("'%s' value not found, using default value of %d\n",
           ENV_VAR_THRESHOLD_EXCEEDED_COUNT, thresholdExceededCount);
  }

  printf("getting env variable '%s'\n", ENV_VAR_RETURN_TO_NORMAL_COUNT);
  if (nbx_getenv_int(ENV_VAR_RETURN_TO_NORMAL_COUNT, &returnToNormalCount,
                     returnToNormalCount)) {
    printf("'%s' value = %d\n", ENV_VAR_RETURN_TO_NORMAL_COUNT,
           returnToNormalCount);
  } else {
    printf("'%s' value not found, using default value of %d\n",
           ENV_VAR_RETURN_TO_NORMAL_COUNT, returnToNormalCount);
  }

  dataPoints = (float *)malloc(sizeof(float) * intervals);
  if (dataPoints == NULL) {
    printf("error: unable to allocate memory for data points");
    return;
  }

  nbx_subscribe_event("sensor/temperature", temperature_event_handler);
}

void nubix_destroy() {
  if (dataPoints != NULL) {
    free(dataPoints);
  }
}

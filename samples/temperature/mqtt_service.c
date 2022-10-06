/*
 * Copyright (C) 2022 Nubix Incorporated.  All rights reserved.
 */

#include "bi-inc/nubix_api.h"

#define ENV_VAR_AWS_IOT_MQTT "awsIotMqtt"
#define ENV_VAR_MQTT_HOST_NAME "mqttHostName"
#define ENV_VAR_MQTT_PORT "mqttPort"
#define ENV_VAR_CLIENT_IDENTIFIER "clientIdentifier"
#define ENV_VAR_USER_NAME "userName"
#define ENV_VAR_PASSWORD "password"
#define ENV_VAR_ROOT_CA "rootCA"
#define ENV_VAR_DATA_TOPIC "dataTopic"
#define ENV_VAR_ALERTS_TOPIC "alertsTopic"

#define ENV_VAR_DEFAULT_VALUE_AWS_IOT_MQTT 0
#define ENV_VAR_DEFAULT_VALUE_MQTT_HOST_NAME ""
#define ENV_VAR_DEFAULT_VALUE_MQTT_PORT 0
#define ENV_VAR_DEFAULT_VALUE_CLIENT_IDEFNTIFIER ""
#define ENV_VAR_DEFAULT_VALUE_USER_NAME ""
#define ENV_VAR_DEFAULT_VALUE_PASSWORD ""
#define ENV_VAR_DEFAULT_VALUE_ROOT_CA ""
#define ENV_VAR_DEFAULT_VALUE_DATA_TOPIC ""
#define ENV_VAR_DEFAULT_VALUE_ALERTS_TOPIC ""

nbx_mqtt_params_t mqttParams;
nbx_mqtt_message_t mqttMessage;

int awsIotMqtt = ENV_VAR_DEFAULT_VALUE_AWS_IOT_MQTT;
char *mqttHostName = ENV_VAR_DEFAULT_VALUE_MQTT_HOST_NAME;
int mqttPort = ENV_VAR_DEFAULT_VALUE_MQTT_PORT;
char *clientIdentifier = ENV_VAR_DEFAULT_VALUE_CLIENT_IDEFNTIFIER;
char *userName = ENV_VAR_DEFAULT_VALUE_USER_NAME;
char *password = ENV_VAR_DEFAULT_VALUE_PASSWORD;
char *rootCA = ENV_VAR_DEFAULT_VALUE_ROOT_CA;
char *dataTopic = ENV_VAR_DEFAULT_VALUE_DATA_TOPIC;
char *alertsTopic = ENV_VAR_DEFAULT_VALUE_ALERTS_TOPIC;

char *deviceName = NULL;

int connectionId = -1;

static int send_mqtt_message(char *topic, uint8 *payload, int payloadLength);

void temperature_data_handler(nbx_request_t *request) {
  printf("### temperature data handler called\n");

  if (request->payload != NULL) {
    char *avgTemp = attr_container_get_as_string(
        (attr_container_t *)request->payload, "average_temperature");

    printf("average temperature value: %s\n", avgTemp);

    int mqttStatus;
    do {
      mqttStatus =
          send_mqtt_message(dataTopic, (uint8_t *)avgTemp, strlen(avgTemp));
    } while (mqttStatus < 0);

    attr_container_destroy(request->payload);
  }
}

void temperature_alerts_handler(nbx_request_t *request) {
  printf("### temperature alerts handler called\n");

  if (request->payload != NULL) {
    char *alert = attr_container_get_as_string(
        (attr_container_t *)request->payload, "alert");

    printf("alert: %s\n", alert);

    int mqttStatus;
    do {
      mqttStatus =
          send_mqtt_message(alertsTopic, (uint8_t *)alert, strlen(alert));
    } while (mqttStatus < 0);

    attr_container_destroy(request->payload);
  }
}

void nubix_init() {
  printf("getting env variable '%s''\n", ENV_VAR_AWS_IOT_MQTT);
  if (nbx_getenv_int(ENV_VAR_AWS_IOT_MQTT, &awsIotMqtt, awsIotMqtt)) {
    printf("'%s' value = %d\n", ENV_VAR_AWS_IOT_MQTT, awsIotMqtt);
  } else {
    printf("'%s' value not found, using default value of %d\n",
           ENV_VAR_AWS_IOT_MQTT, awsIotMqtt);
  }

  printf("getting env variable '%s''\n", ENV_VAR_MQTT_HOST_NAME);
  if (nbx_getenv_string(ENV_VAR_MQTT_HOST_NAME, &mqttHostName, mqttHostName)) {
    printf("'%s'' value = %s\n", ENV_VAR_MQTT_HOST_NAME, mqttHostName);
  } else {
    printf("'%s' value not found, using default value of %s\n",
           ENV_VAR_AWS_IOT_MQTT, mqttHostName);
  }

  printf("getting env variable '%s'\n", ENV_VAR_MQTT_PORT);
  if (nbx_getenv_int("mqttPort", &mqttPort, mqttPort)) {
    printf("'%s' value = %d\n", ENV_VAR_MQTT_PORT, mqttPort);
  } else {
    printf("'%s' value not found, using default value of %d\n",
           ENV_VAR_MQTT_PORT, mqttPort);
  }

  printf("getting env variable '%s'\n", ENV_VAR_CLIENT_IDENTIFIER);
  if (nbx_getenv_string(ENV_VAR_CLIENT_IDENTIFIER, &clientIdentifier,
                        clientIdentifier)) {
    printf("'%s' value = %s\n", ENV_VAR_CLIENT_IDENTIFIER, clientIdentifier);
  } else {
    printf("'%s' value not found, using default value of %s\n",
           ENV_VAR_MQTT_PORT, clientIdentifier);
  }

  printf("getting env variable '%s''\n", ENV_VAR_USER_NAME);
  if (nbx_getenv_string(ENV_VAR_USER_NAME, &userName, userName)) {
    printf("'%s' value = %s\n", ENV_VAR_USER_NAME, userName);
  } else {
    printf("'%s' value not found, using default value of %s\n",
           ENV_VAR_USER_NAME, userName);
  }

  printf("getting env variable '%s'\n", ENV_VAR_PASSWORD);
  if (nbx_getenv_string(ENV_VAR_PASSWORD, &password, password)) {
    // printf("'%s' value = %s\n", ENV_VAR_PASSWORD, password);
  } else {
    // printf("'%s' value not found, using default value of %s\n",
    // ENV_VAR_PASSWORD, password);
  }

  printf("getting env variable '%s'\n", ENV_VAR_ROOT_CA);
  if (nbx_getenv_string(ENV_VAR_ROOT_CA, &rootCA, rootCA)) {
    printf("'%s' = %s\n", ENV_VAR_ROOT_CA, rootCA);
  } else {
    printf("'%s' value not found, using default value of %s\n", ENV_VAR_ROOT_CA,
           rootCA);
  }

  printf("getting env variable '%s'\n", ENV_VAR_DATA_TOPIC);
  if (nbx_getenv_string(ENV_VAR_DATA_TOPIC, &dataTopic, dataTopic)) {
    printf("'%s' value = %s\n", ENV_VAR_DATA_TOPIC, dataTopic);
  } else {
    printf("'%s' value not found, using default value of %s\n",
           ENV_VAR_DATA_TOPIC, dataTopic);
  }

  printf("getting env variable '%s'\n", ENV_VAR_ALERTS_TOPIC);
  if (nbx_getenv_string(ENV_VAR_ALERTS_TOPIC, &alertsTopic, alertsTopic)) {
    printf("'%s' value = %s\n", ENV_VAR_ALERTS_TOPIC, alertsTopic);
  } else {
    printf("'%s' value not found, using default value of %s\n",
           ENV_VAR_ALERTS_TOPIC, alertsTopic);
  }

  printf("getting device name\n");
  if (nbx_get_device_name(&deviceName)) {
    printf("device name = %s\n", deviceName);
  } else {
    printf("failed to get device name\n");
  }

  int topicSize = strlen(dataTopic) + strlen("/") + strlen(deviceName) + 1;
  char *newDataTopic = malloc(topicSize);
  if (newDataTopic == NULL) {
    printf("unable to allocate memory for data topic\n");
    return;
  }

  memset(newDataTopic, '\0', topicSize);
  sprintf(newDataTopic, "%s/%s", dataTopic, deviceName);
  free(dataTopic);
  dataTopic = newDataTopic;

  topicSize = strlen(alertsTopic) + strlen("/") + strlen(deviceName) + 1;
  char *newAlertsTopic = malloc(topicSize);
  if (newAlertsTopic == NULL) {
    printf("unable to allocate memory for alerts topic\n");
    return;
  }

  memset(newAlertsTopic, '\0', topicSize);
  sprintf(newAlertsTopic, "%s/%s", alertsTopic, deviceName);
  free(alertsTopic);
  alertsTopic = newAlertsTopic;

  nbx_subscribe_event("data/temperature", temperature_data_handler);
  nbx_subscribe_event("alerts/temperature", temperature_alerts_handler);
}

void nubix_destroy() {
  if (deviceName != NULL) {
    free(deviceName);
  }
  if (mqttHostName != NULL) {
    free(mqttHostName);
  }
  if (clientIdentifier != NULL) {
    free(clientIdentifier);
  }
  if (userName != NULL) {
    free(userName);
  }
  if (password != NULL) {
    free(password);
  }
  if (rootCA != NULL) {
    free(rootCA);
  }
  if (dataTopic != NULL) {
    free(dataTopic);
  }
  if (alertsTopic != NULL) {
    free(alertsTopic);
  }
}

static int send_mqtt_message(char *topic, uint8 *payload, int payloadLength) {
  mqttParams.awsIotMqttMode = awsIotMqtt;
  mqttParams.pHost = mqttHostName;
  mqttParams.port = mqttPort;
  mqttParams.pRootCACertificate = (uint8_t *)rootCA;
  mqttParams.rootCACertificateLength = strlen(rootCA) + 1;
  mqttParams.pClientIdentifier = clientIdentifier;
  mqttParams.pUserName = userName;
  mqttParams.pPassword = password;

  connectionId = -1;
  connectionId = nbx_mqtt_connection_open(&mqttParams);
  if (connectionId <= 0) {
    printf("unable to establish mqtt connection");
    return -1;
  }
  printf("mqtt connectionn successfully established, connectionId = %d\n",
         connectionId);

  mqttMessage.pTopic = topic;
  mqttMessage.pPayload = payload;
  mqttMessage.payloadLength = payloadLength;

  int mqttStatus = nbx_mqtt_connection_publish(connectionId, &mqttMessage);
  printf("nbx_mqtt_connection_publish completed, status = %d\n", mqttStatus);

  printf("closing mqtt connection\n");

  nbx_mqtt_connection_close(connectionId);

  return mqttStatus;
}

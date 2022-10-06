Temperature Application Sample
=============

## Building the Temperature sample
```
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```

This will create a three container images, one for each service as follows:
- `sensor_service.nbx` - temperature sensor service
- `analytics_service.nbx` - analysis service for alert monitoring
- `mqtt_service.nbx` - cloud communications service



## Deploying the temperature sample
The three services need to be pushed to the Nubix Orchestation Hub prior to use.  This is done using the following commands:
```
nubix container service push sensor_service:latest ./build/sensor_service.nbx -c sensor_service.env
nubix container service push analytics_service:latest ./build/analytics_service.nbx -c analytics_service.env
nubix container service push mqtt_service:latest ./build/mqtt_service.nbx -c mqtt_service.env

```

The application is defined in `manifest.yaml`.  This can be pushed to the Hub as follows:
```
nubix container application push temperature-app:latest manifest.yaml
```

Finally, this can be deployed to your device as follows:
```
nubix container application assign temperature-app:latest [DEVICE-ID]
```




## MQTT Configuration
This sample sends telemetry and alerts to the cloud via an MQTT connection.  This can be any MQTT service that supports username/password authentication.  It has been tested with HiveMQ and Eclipse Mosquitto.  Configuration parameters need to be set prior to pushing the service to the Nubix Hub.  The following values must be set in the `mqtt_service.env` file:
```
# MQTT Host Name
mqttHostName=[HOSTNAME]
# MQTT Client Identifier
clientIdentifier=[MQTT-CLIENT-ID]
# MQTT User Name
userName=[MQTT-USERNAME]
# MQTT Password
password=[MQTT-PASSWORD]
# Root CA used for validating MQTT TLS Server certificate
rootCA=[SERVER-ROOT-CERTIFICATE]
```

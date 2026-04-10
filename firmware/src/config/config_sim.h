#pragma once
#include <Arduino.h>

// WIFI Conection
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// MQTT Mosquitto Broker
#define MQTT_HOST "broker.emqx.io"
#define MQTT_PORT 1883

// MQTT Suscribe topics
static const char* MQTT_SUB_WATERPUMP = "sipa/edge-pipeline/sim/esp32-00/waterpump/set";

// MQTT publish topics
static const char* MQTT_PUB_DHT_TEMP  = "sipa/edge-pipeline/sim/esp32-00/dht/temp";
static const char* MQTT_PUB_DHT_HUM   = "sipa/edge-pipeline/sim/esp32-00/dht/hum";
static const char* MQTT_PUB_SOIL_HUM   = "sipa/edge-pipeline/sim/esp32-00/soil/moisture";
static const char* MQTT_PUB_LIGHT_LEVEL   = "sipa/edge-pipeline/sim/esp32-00/light/illuminance";
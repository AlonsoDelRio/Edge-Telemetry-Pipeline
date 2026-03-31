#pragma once
#include <Arduino.h>

// WIFI Conection
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// MQTT Mosquitto Broker
#define MQTT_HOST "broker.emqx.io"
#define MQTT_PORT 1883

// MQTT Suscribe topics
static const char* MQTT_SUB_WATERPUMP = "sipa/edge-pipeline/sym/waterpump";

// MQTT publish topics
static const char* MQTT_PUB_DHT_TEMP  = "sipa/edge-pipeline/sym/dht/temp";
static const char* MQTT_PUB_DHT_HUM   = "sipa/edge-pipeline/sym/dht/hum";
static const char* MQTT_PUB_SOIL_HUM   = "sipa/edge-pipeline/sym/soil/moisture";
static const char* MQTT_PUB_LIGHT_LEVEL   = "sipa/edge-pipeline/sym/light/illuminance";
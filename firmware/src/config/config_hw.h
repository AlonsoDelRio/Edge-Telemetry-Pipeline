#pragma once
#include <Arduino.h>
#include "secrets.h"

// MQTT Suscribe topics
static const char* MQTT_SUB_WATERPUMP = "edge-pipeline/hw/esp32-01/waterpump/set";

// MQTT publish topics
static const char* MQTT_PUB_DHT_TEMP  = "edge-pipeline/hw/esp32-01/dht/temp";
static const char* MQTT_PUB_DHT_HUM   = "edge-pipeline/hw/esp32-01/dht/hum";
static const char* MQTT_PUB_SOIL_HUM   = "edge-pipeline/hw/esp32-01/soil/moisture";
static const char* MQTT_PUB_LIGHT_LEVEL   = "edge-pipeline/hw/esp32-01/light/illuminance";
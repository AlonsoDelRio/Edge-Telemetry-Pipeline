#pragma once
#include <Arduino.h>
#include "secrets.h"

// MQTT Suscribe topics
static const char* MQTT_SUB_WATERPUMP = "edge-pipeline/ESP32/waterpump";

// MQTT publish topics
static const char* MQTT_PUB_DHT_TEMP  = "edge-pipeline/ESP32/dht/temp";
static const char* MQTT_PUB_DHT_HUM   = "edge-pipeline/ESP32/dht/hum";
static const char* MQTT_PUB_SOIL_HUM   = "edge-pipeline/ESP32/soil/hum";
static const char* MQTT_PUB_LIGHT_LEVEL   = "edge-pipeline/ESP32/light";
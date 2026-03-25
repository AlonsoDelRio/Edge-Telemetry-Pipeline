#pragma once
#include <Arduino.h>

#if defined(DEVICE_SIM)
    #include "config_sim.h"
#elif defined(DEVICE_HW)
    #include "config_hw.h"
#else
    #error "No build environment defined. Use -D DEVICE_SIM or -D DEVICE_HW."
#endif

// Sensors
static constexpr uint8_t DHT_PIN  = 18;  
#define DHTTYPE DHT22
static constexpr uint8_t LIGHT_SENSOR_PIN  = 34;
static constexpr uint8_t SOIL_SENSOR_PIN  = 35;  
#pragma once

// WIFI Conection
// Replace with your actual network credentials before flashing.
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// MQTT Mosquitto Broker
// For a cloud MQTT broker, type the domain name or IP
// Mosquitto broker running via Docker Compose on the local network.
#define MQTT_HOST IPAddress(192, 168, 1, 100)  // ← update to your host machine's IP
#define MQTT_PORT 1883

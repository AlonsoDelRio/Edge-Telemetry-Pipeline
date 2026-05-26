/**
 * @file    main.cpp
 * @brief   ESP32 IoT edge node for the SIPA monitoring system.
 *
 * Reads temperature and humidity from a DHT22 sensor and publishes the
 * values to an MQTT broker at a fixed interval. Subscribes to a command
 * topic to control a water pump actuator. Designed to feed data into a
 * Node-RED dashboard for visualization and control.
 *
 * Hardware:
 *   - Board  : ESP32
 *   - Sensor : DHT22  (GPIO 4)
 *   - Actuator: Water pump relay (GPIO 22)
 *   - Status LED (GPIO 2, built-in)
 *
 * MQTT Topics:
 *   Publish  : sipa/edge-pipeline/sym/dht/temp
 *   Publish  : sipa/edge-pipeline/sym/dht/hum
 *   Subscribe: sipa/edge-pipeline/sym/waterpump  ("on" | "off")
 *
 * Dependencies:
 *   - AsyncMqttClient  https://github.com/marvinroger/async-mqtt-client
 *   - DHT sensor library (Adafruit)
 *   - ESP32 Arduino core (FreeRTOS timers included)
 */
// TODO: Implement automated water pump control logic based on soil moisture and temperature thresholds.
#include <Arduino.h>
#include <WiFi.h>
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#include "DHT.h"
#include "config/config.h"

// Actuators
static const uint8_t WATERPUMP_PIN = 25;

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

// Objects initializer
DHT dht(DHT_PIN, DHTTYPE);
// raw ADC reading for soil sensor max and min
#define SOIL_DRY_VALUE 3200
#define SOIL_WET_VALUE 1200
// raw ADC reading for light sensor max and min
#define NIGHT_LIGHT_VALUE 4095
#define SUN_LIGHT_VALUE 200

void connectToWifi() {
  Serial.printf("Connecting to %s \r\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.printf("IP address: %s\r\n", WiFi.localIP().toString().c_str());
      connectToMqtt();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0);  // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  // ESP subscribed to topic waterpump
  uint16_t packetIdSub2 = mqttClient.subscribe(MQTT_SUB_WATERPUMP, 0);
  Serial.printf("[MQTT] Subscribing at %s QoS 0, Id %i\r\n", MQTT_SUB_WATERPUMP, packetIdSub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

/**
 * @brief Callback que se ejecuta al recibir un mensaje del broker MQTT
 * Evalúa el tópico y acciona la bomba de agua si corresponde.
 */
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index,
                   size_t total) {
  String messageTemp;
  for (int i = 0; i < len; i++) {
    // Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  // Check if the MQTT message was received on topic WaterPump
  if (strcmp(topic, MQTT_SUB_WATERPUMP) == 0) {
    Serial.println(messageTemp);
    if (messageTemp == "on") {
      digitalWrite(WATERPUMP_PIN, HIGH);
    }
    if (messageTemp == "off") {
      digitalWrite(WATERPUMP_PIN, LOW);
    }
  }
  Serial.printf("[MQTT-RECV] Topic: %s | Msg: %s | QoS: %d | Dup: %d | Retain: %d | Len: %d\r\n", topic,
                messageTemp.c_str(), properties.qos, properties.dup, properties.retain, len);
}

int soil_readPercent() {
  int raw = analogRead(SOIL_SENSOR_PIN);

  // Clamp to calibrated range to avoid out-of-bound percentages.
  raw = constrain(raw, SOIL_WET_VALUE, SOIL_DRY_VALUE);

  // Map: DRY (high ADC) → 0%, WET (low ADC) → 100%
  return map(raw, SOIL_DRY_VALUE, SOIL_WET_VALUE, 0, 100);
}

int light_readPercent() {
  int raw = analogRead(LIGHT_SENSOR_PIN);

  // Clamp to calibrated range to avoid out-of-bound percentages.
  raw = constrain(raw, SUN_LIGHT_VALUE, NIGHT_LIGHT_VALUE);

  // Map: NIGHT (high ADC) → 0%, SUNNY (low ADC) → 100%
  return map(raw, NIGHT_LIGHT_VALUE, SUN_LIGHT_VALUE, 0, 100);
}

// Variables
float dht_temp, dht_hum;
int soil_hum, light_level;
unsigned long previousMillis = 0;  // Stores last time temperature was published
const long interval = 2000;        // Interval at which to publish sensor readings

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.onEvent(WiFiEvent);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  // mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);
  connectToWifi();
  dht.begin();
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(WATERPUMP_PIN, OUTPUT);
  digitalWrite(WATERPUMP_PIN, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds)
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    // Read temperature as Celsius, humidity and enviroment metrics
    dht_hum = dht.readHumidity();
    dht_temp = dht.readTemperature();
    soil_hum = soil_readPercent();
    light_level = light_readPercent();

    // Check if any reads failed and exit early (to try again).
    if (isnan(dht_temp) || isnan(dht_hum)) {
      Serial.println("[ERROR] Failed to read from DHT sensor!");
      return;
    }

    // Publish an MQTT message on topic sipa/edge-pipeline/sym/dht/temperature
    uint16_t dht_temp_packetID = mqttClient.publish(MQTT_PUB_DHT_TEMP, 1, true, String(dht_temp).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetID: %i \r\n", MQTT_PUB_DHT_TEMP, dht_temp_packetID);
    Serial.printf("Message: %.2f \r\n", dht_temp);

    // Publish an MQTT message on topic sipa/edge-pipeline/sym/dht/humidity
    uint16_t dht_hum_packetID = mqttClient.publish(MQTT_PUB_DHT_HUM, 1, true, String(dht_hum).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetID: %i \r\n", MQTT_PUB_DHT_HUM, dht_hum_packetID);
    Serial.printf("Message: %.2f \r\n", dht_hum);

    // Publish an MQTT message on topic sipa/edge-pipeline/sym/soil/hum
    uint16_t soil_hum_packetID = mqttClient.publish(MQTT_PUB_SOIL_HUM, 1, true, String(soil_hum).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetID: %i \r\n", MQTT_PUB_SOIL_HUM, soil_hum_packetID);
    Serial.printf("Message: %d \r\n", soil_hum);

    // Publish an MQTT message on topic sipa/edge-pipeline/sym/light
    uint16_t light_level_packetID = mqttClient.publish(MQTT_PUB_LIGHT_LEVEL, 1, true, String(light_level).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetID: %i \r\n", MQTT_PUB_LIGHT_LEVEL, light_level_packetID);
    Serial.printf("Message: %d \r\n", light_level);
  }
}

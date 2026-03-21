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
#include <Arduino.h>
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "DHT.h"

// WIFI Conection
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// MQTT Mosquitto Broker
// For a cloud MQTT broker, type the domain name or IP
//#define MQTT_HOST IPAddress(XXX, XXX, XXX, XXX)
#define MQTT_HOST "broker.emqx.io"
#define MQTT_PORT 1883

// MQTT Suscribe topics
static const char* MQTT_SUB_WATERPUMP = "sipa/edge-pipeline/sym/waterpump";

// MQTT publish topics
static const char* MQTT_PUB_DHT_TEMP  = "sipa/edge-pipeline/sym/dht/temp";
static const char* MQTT_PUB_DHT_HUM   = "sipa/edge-pipeline/sym/dht/hum";

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


// Sensors
static const uint8_t DHT_PIN  = 4;  
#define DHTTYPE DHT22

// Actuators
static const uint8_t PIN_LED_BLUE   = 2;  // LED de estado (incorporado)
static const uint8_t PIN_WATERPUMP = 22; // Control de bomba de agua

// Objects initializer
DHT dht(DHT_PIN, DHTTYPE);

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
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.printf("IP address: %s\r\n", WiFi.localIP().toString().c_str());
      connectToMqtt();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

// Add more topics that want your ESP to be subscribed to
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
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String messageTemp;
  for (int i = 0; i < len; i++) {
    //Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  // Check if the MQTT message was received on topic LED
  if (strcmp(topic, MQTT_SUB_WATERPUMP) == 0) {
    Serial.println(messageTemp);
    if(messageTemp == "on") {
      digitalWrite(PIN_WATERPUMP, HIGH);
      digitalWrite(PIN_LED_BLUE, HIGH);
    }
    if(messageTemp == "off"){
      digitalWrite(PIN_WATERPUMP, LOW);
      digitalWrite(PIN_LED_BLUE, LOW);

    }
  }
 Serial.printf("[MQTT-RECV] Topic: %s | Msg: %s | QoS: %d | Dup: %d | Retain: %d | Len: %d\r\n", 
              topic, messageTemp.c_str(), properties.qos, properties.dup, properties.retain, len);
}

// Variables
float dht_temp; 
float dht_hum;
unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 2000;         // Interval at which to publish sensor readings

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(PIN_LED_BLUE, OUTPUT);
  digitalWrite(PIN_LED_BLUE, LOW);

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onPublish(onMqttPublish);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);
  connectToWifi();
}

void loop() {
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    // New DHT sensor readings
    dht_hum = dht.readHumidity();
    // Read temperature as Celsius (the default)
    dht_temp = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //temp = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(dht_temp) || isnan(dht_hum)) {
      Serial.println("[ERROR] Failed to read from DHT sensor!");
      return;
    }
    
    // Publish an MQTT message on topic esp32/dht/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_DHT_TEMP, 1, true, String(dht_temp).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i \r\n", MQTT_PUB_DHT_TEMP, packetIdPub1);
    Serial.printf("Message: %.2f \r\n", dht_temp);

    // Publish an MQTT message on topic esp32/dht/humidity
    uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_DHT_HUM, 1, true, String(dht_hum).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i \r\n", MQTT_PUB_DHT_HUM, packetIdPub2);
    Serial.printf("Message: %.2f \r\n", dht_hum);
  }
}
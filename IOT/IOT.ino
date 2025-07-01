#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// DHT settings
#define DHTPIN 4        // DHT11 data pin verbonden met GPIO 4
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// MQTT settings
const char* mqtt_server = "pi-martijn";
const char* mqtt_topic = "sensor_data";

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Setup WiFiManager
WiFiManager wifiManager;

// Functie om met MQTT te verbinden
void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT Server");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // Begin seriële communicatie
  Serial.begin(115200);

  // Initializeer DHT sensor
  dht.begin();

  // WiFiManager portal opstarten
  wifiManager.autoConnect("AutoConnectAP");

  // Stel de MQTT server in
  client.setServer(mqtt_server, 1883);

  // Verbinden met de MQTT server
  connectToMQTT();
}

void loop() {
  // Controleer of we nog steeds verbonden zijn met de MQTT broker
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Lees de temperatuur en vochtigheid van de DHT11 sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Controleer of er geen leesfouten zijn
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Maak een JSON-document
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  doc["humidity"] = h;

  // Serialiseer de JSON-document naar een string
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  // Verzenden van apparatuurgegevens naar de MQTT server
  Serial.print("Publishing message: ");
  Serial.println(jsonBuffer);

  // Publiceer de JSON-data naar het ingestelde topic
  client.publish(mqtt_topic, jsonBuffer);

  // Wacht 10 seconden tussen metingen
  delay(10000);
}
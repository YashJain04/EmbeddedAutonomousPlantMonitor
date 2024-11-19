#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

char wifiSSID[] = "";
char wifiPassword[] = "";

#include <SoftwareSerial.h>
SoftwareSerial EspSerial(4, 5); // RX, TX pins

#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);

#include "DHT.h"

#define SENSOR_PIN 6
#define SENSOR_TYPE DHT22
DHT tempHumidSensor(SENSOR_PIN, SENSOR_TYPE);

#define LIGHT_SENSOR_PIN A4
#define MOISTURE_SENSOR_PIN A5
#define WATER_PUMP_PIN 9

float currentHumidity = 0;
float currentTemperature = 0;

BlynkTimer sensorTimer;

BLYNK_WRITE(V1) {
  if (param.asInt() == 1) {
    digitalWrite(WATER_PUMP_PIN, HIGH);
  } else {
    digitalWrite(WATER_PUMP_PIN, LOW);
  }
}

int getSoilMoisture() {
  return analogRead(MOISTURE_SENSOR_PIN);
}

int getAmbientLight() {
  return analogRead(LIGHT_SENSOR_PIN);
}

bool fetchSensorData() {
  currentHumidity = tempHumidSensor.readHumidity();
  currentTemperature = tempHumidSensor.readTemperature();

  if (isnan(currentHumidity) || isnan(currentTemperature)) {
    return false;
  }
  return true;
}

void updateSensorValues() {
  bool sensorStatus = fetchSensorData();
  int lightLevel = getAmbientLight();
  int moistureLevel = getSoilMoisture();

  if (sensorStatus) {
    Blynk.virtualWrite(V2, currentHumidity);
    Blynk.virtualWrite(V3, currentTemperature);
  }
  Blynk.virtualWrite(V4, lightLevel);
  Blynk.virtualWrite(V5, moistureLevel);
}

void setup() {
  Serial.begin(115200);

  tempHumidSensor.begin();

  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, wifiSSID, wifiPassword);

  sensorTimer.setInterval(2000L, updateSensorValues);

  pinMode(WATER_PUMP_PIN, OUTPUT);
}

void loop() {
  Blynk.run();
  sensorTimer.run();
}
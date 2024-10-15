#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "credentials.h"
#include "ports.h"
#include "htmlContent.h"

ESP8266WebServer server(80);

const float maxHeight = 200.0;
const int maxReadings = 30;
float distanceReadings[maxReadings];
String timeReadings[maxReadings];
int currentIndex = 0;
int waitTimeInSeconds = 5;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset = 3600;
const int daylightOffset = 3600;

void setup() {
  Serial.begin(115200);
  delay(5000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.print("Current IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  configTime(gmtOffset, daylightOffset, ntpServer);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  static unsigned long lastMeasurementTime = 0;

  if (millis() - lastMeasurementTime >= waitTimeInSeconds * 1000) {
    lastMeasurementTime = millis();
    measureDistance();
  }
}

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void measureDistance() {
  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.0344 / 2;

  distanceReadings[currentIndex] = distance;

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // char timeStringBuff[6];
  // strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);

  char timeStringBuff[9];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);

  timeReadings[currentIndex] = String(timeStringBuff);

  currentIndex = (currentIndex + 1) % maxReadings;
}

void handleData() {
  String json = "{ \"labels\": [";
  for (int i = 0; i < maxReadings; i++) {
    int index = (currentIndex + i) % maxReadings;
    json += "\"" + timeReadings[index] + "\"";
    if (i < maxReadings - 1) {
      json += ",";
    }
  }

  json += "], \"readings\": [";

  for (int i = 0; i < maxReadings; i++) {
    int index = (currentIndex + i) % maxReadings;
    json += String(maxHeight - distanceReadings[index]);
    if (i < maxReadings - 1) {
      json += ",";
    }
  }

  json += "] }";
  server.send(200, "application/json", json);
}

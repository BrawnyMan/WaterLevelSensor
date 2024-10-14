// #include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <FS.h>

#include "credentials.h"
#include "ports.h"
#include "htmlContent.h"

ESP8266WebServer server(80);

const int maxReadings = 30;
float distanceReadings[maxReadings];
int currentIndex = 0;
int waitTimeInSeconds = 5;

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
  currentIndex = (currentIndex + 1) % maxReadings;
}

void handleData() {
  String json = "{ \"labels\": [";
  for (int i = 0; i < maxReadings; i++) {
    json += String(i * waitTimeInSeconds);
    if (i < maxReadings - 1) {
      json += ",";
    }
  }
  
  json += "], \"readings\": [";

  for (int i = maxReadings - 1; i >= 0; i--) {
    int index = (currentIndex + i) % maxReadings;
    json += String(distanceReadings[index]);
    if (i > 0) {
      json += ",";
    }
  }

  json += "] }";
  server.send(200, "application/json", json);
}

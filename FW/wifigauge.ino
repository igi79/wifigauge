#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid     = "*****";
const char* password = "*****";

ESP8266WebServer server(80);

const int led = 13;
const int gauge = 2;
uint32_t lastmillis;

int value;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "Hello from WifiGauge!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void handleGauge() {
  int v = server.arg(0).toInt();
  server.send(200, "text/plain", "Gauge value: " + server.arg(0));

  int x = value;
  while (abs(v - x) > 4){
    if(v > value){
      x = x + (v - x)/2.5;
    } else {
      x = x - (x - v)/2.5;
    }
    analogWrite(gauge, x);
    delay(150);
  }
  analogWrite(gauge, v);
  value = v;
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(gauge, OUTPUT);
  digitalWrite(led, 0);
  WiFi.hostname("WifiGauge");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);



  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  if (MDNS.begin("esp8266")) {

  }

  server.on("/", handleRoot);
  server.on("/value", handleGauge);
  server.onNotFound(handleNotFound); 
  server.begin();
}

void loop(void) {
  server.handleClient();
}

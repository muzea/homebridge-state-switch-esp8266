#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define powerPin 12
#define lightPin 4
#define buttonPin 5

/*
#define STASSID "your-ssid"
#define STAPSK  "your-password"
*/
#include "password.h"

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

String StateName = "switch_1";
String StateValue = "off";

void updateOutput()
{
  if (StateValue == "on")
  {
    digitalWrite(lightPin, LOW);
    digitalWrite(powerPin, HIGH);
  }
  else
  {
    digitalWrite(lightPin, HIGH);
    digitalWrite(powerPin, LOW);
  }
}

void setup(void)
{

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(lightPin, OUTPUT);
  pinMode(powerPin, OUTPUT);

  digitalWrite(lightPin, HIGH);
  digitalWrite(powerPin, LOW);
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("ESP8266 Start");

  // Wait for connection
  if (WiFi.waitForConnectResult() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/state", HTTP_GET, []() {
      Serial.println("Request State Get");
      server.sendHeader("Connection", "close");
      server.send(200, "application/json", "{\"" + StateName + "\":\"" + StateValue + "\"}");
    });

    server.on("/state", HTTP_PUT, []() {
      Serial.println("Request State Put");
      server.sendHeader("Connection", "close");
      String body = server.arg("plain");
      int valueStart = body.indexOf(StateName + "\":\"") + StateName.length() + 3;
      int valueEnd = body.indexOf("\"", valueStart);
      String nextValue = body.substring(valueStart, valueEnd);
      Serial.println("State Update " + StateName + ":" + nextValue);
      StateValue = nextValue;
      updateOutput();
      server.send(200, "application/json", "{\"" + StateName + "\":\"" + StateValue + "\"}");
    });

    server.enableCORS(true);
    server.begin();
    Serial.println("TCP server started");
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi connected failed.");
  }
}

void loop(void)
{
  server.handleClient();
}

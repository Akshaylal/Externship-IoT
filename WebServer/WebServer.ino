#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define LED_PIN D4

const char* ssid = "<WiFi_SSID>";
const char* password = "<WiFi_password>";
 
ESP8266WebServer server(80);

bool ledStatus = false;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP8266 LED</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    async function toggle() {
      const response = await fetch("/toggle");
      const data = await response.json();
      document.getElementById("status").innerText = `LED ${data.led}`;
    }
  </script>
  </head><body>
  <div id="status">LED OFF</div>
  <button onclick="toggle()">Toggle</button>
</body></html>)rawliteral";


void toggleLed(){
  ledStatus = !ledStatus;
  if (ledStatus) {
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/json", "{\"led\": \"ON\"}");
  } else {
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/json", "{\"led\": \"OFF\"}");
  }
}
 
void setup(void) {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Set server routing
  
  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", index_html);
  });
  server.on(F("/toggle"), HTTP_GET, toggleLed);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) {
  server.handleClient();
}
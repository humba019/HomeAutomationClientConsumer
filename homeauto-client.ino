#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

EthernetClient client;

const size_t capacity = JSON_ARRAY_SIZE(1) + 60;
DynamicJsonDocument doc(capacity);

void setup() {
  
  // Initialize Serial port
  Serial.begin(9600);
  while (!Serial) continue;

  // Initialize Ethernet library
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  delay(1000);

  Serial.println(F("Connecting..."));

  // Connect to HTTP server
  client.setTimeout(10000);
  if (!client.connect("homeauto-control.herokuapp.com", 80)) {
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));

  // Send HTTP request
  client.println(F("GET /home/device HTTP/1.0"));
  client.println(F("Host: homeauto-control.herokuapp.com"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }
  
  // LED test
  pinMode(13, OUTPUT);
  
}

void loop() {

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  Serial.println(F("Response:"));  
  Serial.println(doc[0]["id"].as<long>());
  Serial.println(doc[0]["port"].as<int>());
  if(doc[0]["status"].as<bool>() == 0 ){    
    digitalWrite(13, LOW); 
    Serial.println("false");  
  }else{
    digitalWrite(13, HIGH); 
    Serial.println("true");      
  }

  // Disconnect
  client.stop();

}

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>

#define WIDTH 32
#define HEIGHT 32

const char* ssid = "your_ssid";
const char* password = "your_password";

// Our hypothetical URL for DFW view
const char* apiUrl = "https://opensky-network.org/api/states/all?lamin=32.86698&lomin=-96.98489&lamax=32.89334&lomax=-96.94915";

// RGB Matrix config (Matrix Portal S3 defaults)
int rgbpins[] = { 7, 8, 9, 10, 11, 12 };
Adafruit_Protomatter matrix(
  WIDTH, 4, 1, rgbpins, 6, 2, 3, 4, 5); // width, bitDepth, numPanels, rgbpins, addr, latch, OE, clock, etc.

void updateFlightDisplay() {
  HTTPClient http;
  http.begin(apiUrl);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Received JSON:");
    Serial.println(payload);

    // Allocate JSON document (20 KB should be PLENTY. Really only if your bounding box is near the airport and tons of planes are around)
    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson(doc, payload); // error checking

    if (!error) {
      JsonArray states = doc["states"];
      if (!states.isNull() && states.size() > 0) {
        JsonArray plane = states[0];  // take first plane

        String callsign = plane[1] | "-----";
        String country = plane[2] | "---";
        float velocity = plane[9] | 0.0;

        // Clean up the text
        callsign.trim();
        country.trim();
        int vel = round(velocity);

        // Display
        matrix.fillScreen(0);
        matrix.setCursor(0, 0);
        matrix.print(callsign);
        matrix.setCursor(0, 8);
        matrix.print("v="); matrix.print(vel);
        matrix.setCursor(0, 16);
        matrix.print(country);
        matrix.show();
      } else {
        Serial.println("No states found");
      }
    } else {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  // WiFi setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("    Connected!");

  // Matrix setup
  matrix.begin();
  matrix.fillScreen(0);
  matrix.setTextColor(matrix.color565(255, 255, 255));

  // Get flight data
  updateFlightDisplay();
}

void loop() {
  delay(2 * 60 * 1000); // 2 min delay between updates, but change as you wish. This freezes the microcontroller though. Not a problem for us.
  updateFlightDisplay();
}

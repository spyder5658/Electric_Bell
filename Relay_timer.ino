#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>

// Replace these with your WiFi credentials
const char* ssid = "orionspace_2";  //mention WiFi name
const char* password = "CLB2C85750";  //mention WiFi password

// Define NTP Server address
const char* ntpServer = "in.pool.ntp.org";  //Modify as per your country
const long  gmtOffset_sec = 20700; // Offset from UTC (in seconds) (India GMT 5:30 // 5.5*60*60 = 19800) Modify as per your country
const int   daylightOffset_sec = 3600; // Daylight offset (in seconds)

// Define relay pin
const int relayPin = 4; // Change this to the pin connected to your relay

// Define NTP and WiFi objects
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start NTP time sync
  timeClient.begin();
  timeClient.update();

  // Initialize relay pin as an output
  pinMode(relayPin, OUTPUT);
}

void loop() {
  // Update NTP time
  timeClient.update();
  
  // Get current time
  time_t currentTime = timeClient.getEpochTime();
  struct tm * timeinfo;
  timeinfo = localtime(&currentTime);
  
  // Display current time on Serial Monitor
  Serial.print("Time: ");
  Serial.println(timeClient.getFormattedTime());

  // Check if it's time to toggle the relay (every minute)
  if (timeinfo->tm_sec == 0) { 
    digitalWrite(relayPin, HIGH); // Turn on the relay
    Serial.println("Relay ON");
    delay(4000);
    digitalWrite(relayPin,LOW); // Turn on the relay

    
  } 

  // Wait for 1 second before checking again
  delay(1000);
}

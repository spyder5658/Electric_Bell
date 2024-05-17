#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace these with your WiFi credentials
const char* ssid = "orionspace_2";  //mention WiFi name
const char* password = "CLB2C85750";  //mention WiFi password

String subject;
String startTime;
int ringCount;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Bell Settings</title>
</head>
<body>
  <h2>Bell Settings</h2>
  <form action="/set-settings" method="POST">
    <label for="subject">Subject:</label><br>
    <input type="text" id="subject" name="subject"><br><br>

    <label for="startTime">Starting Time:</label><br>
    <input type="time" id="startTime" name="startTime"><br><br>

    <label for="ringCount">Number of Times to Ring:</label><br>
    <input type="number" id="ringCount" name="ringCount" min="1" value="1"><br><br>

    <input type="submit" value="Submit">
  </form>
</body>
</html>

)rawliteral";


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
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  // Route for handling the form submission
  server.on("/set-settings", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("subject", true) && request->hasParam("startTime", true) && request->hasParam("ringCount", true)) {
      AsyncWebParameter* subjectParam = request->getParam("subject", true);
      AsyncWebParameter* startTimeParam = request->getParam("startTime", true);
      AsyncWebParameter* ringCountParam = request->getParam("ringCount", true);

      subject = subjectParam->value();
      startTime = startTimeParam->value();
      ringCount = ringCountParam->value().toInt();

      // Handle the settings as needed, you can store them in variables, EEPROM, or a database
      // For now, let's just print them
      Serial.print("Subject: ");
      Serial.println(subject);
      Serial.print("Starting Time: ");
      Serial.println(startTime);
      Serial.print("Number of Times to Ring: ");
      Serial.println(ringCount);
      
      request->send(200, "text/html", "<html><body><h1>Settings Updated</h1></body></html>");
    
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });


  // Start NTP time sync
  timeClient.begin();
  timeClient.update();

  // Initialize relay pin as an output
  pinMode(relayPin, OUTPUT);

  // Start server
  server.begin();
}

void loop() {
  // Update NTP time
  timeClient.update();
  
  // Get current time
  time_t currentTime = timeClient.getEpochTime();
  struct tm * timeinfo;
  timeinfo = localtime(&currentTime);
  
  // Parse starting time
  int setHour = startTime.substring(0, 2).toInt();
  int setMinute = startTime.substring(3).toInt();

  // Check if it's time to ring the bell
  if (timeinfo->tm_hour == setHour && timeinfo->tm_min == setMinute) {
    for (int i = 0; i < ringCount; i++) {
      ring(); // Ring the bell according to the specified count
      delay(2000); // Adjust delay between bell rings as needed
    }
  }

  // Other loop code remains the same
}

void ring() {
  digitalWrite(relayPin, HIGH); // Turn on the relay
  Serial.println("Bell Ringing!");
  delay(5000); // Adjust the duration of the bell ring as needed
  digitalWrite(relayPin, LOW); // Turn off the relay
}

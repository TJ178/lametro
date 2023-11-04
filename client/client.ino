/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <HTTPClient.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>

//const char* ssid = "IEEE";
//const char* password = "Ilovesolder";

const char* ssid = "FBI Van 2.4G";
const char* password = "WhaleHyenaPlop";

//Your Domain name with URL path or IP address with path
String serverName = "http://76.176.83.210:7200/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 20000;

//display stuff
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200); 
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(100);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  
  display.println("Connecting to WiFi");
  display.display();
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  display.print("Connected.");
  display.display();
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + "?temperature=24.37";
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();

      display.clearDisplay();
    
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(0, 0);     // Start at top-left corner
      display.cp437(true);         // Use full 256 char 'Code Page 437' font
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        //display.println(payload);
        if(payload.length() > 2){
          time_t busTime = (time_t) payload.substring(1, 11).toInt();  
          
          busTime -= 7*60*60;

          Serial.println(busTime);
          Serial.print(hourFormat12(busTime));
          Serial.print(':');
          Serial.println(minute(busTime));
          
          display.print(hourFormat12(busTime));
          display.print(':');
          display.println(minute(busTime));
  
          if(payload.length()>13){
            time_t bus2Time = (time_t) payload.substring(12, 23).toInt();
            bus2Time -= 7*60*60;
            display.print(hourFormat12(bus2Time));
            display.print(':');
            display.println(minute(bus2Time));
          }
        }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        display.print("Error: ");
        display.print(httpResponseCode);
      }
      display.display();
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

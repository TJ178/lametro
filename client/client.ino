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

time_t* parseTimes(String s, int* numTimes);
void displayTimes(time_t* busTimes, int numTimes);

void setup() {
  Serial.begin(115200); 
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  int num;
  parseTimes("[1699158516,1699160411,1699160412]", &num);
  
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
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);

        int numTimes = 0;
        time_t* busTimes = parseTimes(payload, &numTimes);
        Serial.println("parsed");
        Serial.println(numTimes);
        displayTimes(busTimes, numTimes);
        Serial.println("display");
        free(busTimes);
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


time_t* parseTimes(String s, int* numTimes){
  if(s.length() <= 2){
    *numTimes = 0;
    return (time_t*) 0;
  }
  *numTimes = 1;
  int idx = 0;
  while(1){
    idx = s.indexOf(',', idx) + 1;
    if(idx == 0) break;
    *numTimes = *numTimes + 1;
  }

  Serial.println(*numTimes);
  time_t* out = (time_t*) malloc(sizeof(time_t) * *numTimes);

  int lastIdx = 1;
  for(int i = 0; i < *numTimes; i++){
    idx = s.indexOf(',', lastIdx);
    Serial.println(lastIdx);
    Serial.println(idx);
    if(idx == -1) idx = s.length()-1;
    Serial.println(s.substring(lastIdx, idx));
    out[i] = (time_t) s.substring(lastIdx, idx).toInt();
    out[i] -= 7*60*60;
    lastIdx = idx+1;
  }
  return out;
}

void displayTimes(time_t* busTimes, int numTimes){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  if(numTimes <= 1){
    display.print("No buses \ncurrently scheduled");
  }else{
    display.setTextSize(1);
    display.println("Next bus:");
    time_t nextMin = busTimes[1] - busTimes[0];
    Serial.println(nextMin);
    display.setTextSize(3);
    display.print(minute(nextMin));
    display.println(" min");

    if(numTimes >= 3){
      nextMin = busTimes[2] - busTimes[0];
      if(minute(nextMin) > 15){
        display.setTextSize(1);
        char buff[20];
        sprintf(buff, "Also at %d:%0d.2", hour(busTimes[2]), minute(busTimes[2]));
        display.println(buff);
      }else{
        display.print(minute(nextMin));
        display.println(" min");
      }
    }
  }     
}

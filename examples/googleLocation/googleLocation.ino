
#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#error Wrong platform
#endif 

#include "WifiLocation.h"

#define GOOGLE_KEY "YOUR_GOOGLE_API_KEY"
#define SSID "SSID"
#define PASSWD "PASSWD"

WifiLocation location (GOOGLE_KEY);

void setup()
{
    Serial.begin(115200);
    
    int status = 0;
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(SSID);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(SSID, PASSWD);

        // wait 10 seconds for connection:
        delay(10000);
    }

    location_t loc = location.getGeoFromWiFi();

    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));


}

void loop()
{


}

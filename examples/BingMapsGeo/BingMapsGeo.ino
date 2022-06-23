
#include <Arduino.h>
#if defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#error Wrong platform
#endif 

#include <bingMapsGeocoding.h>

#if __has_include("wificonfig.h")
#include "wificonfig.h"
#else
const char* ssid = "SSID";
const char* passwd = "PASSWD";
const char* bingMapsKey = "YOUR_BING_MAPS_KEY";
#endif

// Microsoft headquarters in Redmond, WA
float lat = 47.641595;
float lon = -122.134087;

BingGeoCoding bingGeoCoding (bingMapsKey);

// Set time via NTP, as required for x.509 validation
void setClock () {
    configTime (0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print ("Waiting for NTP time sync: ");
    time_t now = time (nullptr);
    while (now < 8 * 3600 * 2) {
        delay (500);
        Serial.print (".");
        now = time (nullptr);
    }
    struct tm timeinfo;
    gmtime_r (&now, &timeinfo);
    Serial.print ("\n");
    Serial.print ("Current time: ");
    Serial.print (asctime (&timeinfo));
}

void setup () {
    Serial.begin (115200);
    // Connect to WPA/WPA2 network
    WiFi.mode (WIFI_STA);
    WiFi.begin (ssid, passwd);
    while (WiFi.status () != WL_CONNECTED) {
        Serial.print ("Attempting to connect to WPA SSID: ");
        Serial.println (ssid);
        // wait 5 seconds for connection:
        Serial.print ("Status = ");
        Serial.println (WiFi.status ());
        delay (500);
    }
    Serial.println ("Connected");
    setClock ();

    bingGeoData_t geoData = bingGeoCoding.getGeoFromPosition (lat, lon);

    Serial.printf ("Address: %s\n", bingGeoCoding.getCompleteAddress (&geoData).c_str());

}

void loop () {

}

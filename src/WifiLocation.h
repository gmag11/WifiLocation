// WifiLocation.h

#ifndef _WIFILOCATION_h
#define _WIFILOCATION_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#define DEBUG_WIFI_LOCATION
//#define USE_CORE_PRE_2_5_0 // Uncomment this if you are using Arduino Core < 2.5.0

#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiClientSecure.h>
#else
#error Wrong platform
#endif

#define MAX_CONNECTION_TIMEOUT 5000
#define MAX_WIFI_SCAN 127

typedef struct {
    float lat = 0;
    float lon = 0;
    int accuracy = 40000;
} location_t;

class WifiLocation {
public:
    WifiLocation(String googleKey = "");
    location_t getGeoFromWiFi();
    static String getSurroundingWiFiJson();

protected:
    String _googleApiKey;
    //String _googleApiFingerprint = "2c 86 e4 67 e7 b5 ca df 11 9e bd 2e 41 c2 4b e8 b6 7e cd aa";
    //IPAddress _googleApiIP = IPAddress(216, 58, 214, 170);
#if defined ESP8266 || defined ARDUINO_ARCH_ESP32
    WiFiClientSecure  _client;
#elif defined ARDUINO_ARCH_SAMD
    WiFiSSLClient  _client;
#endif

    static String MACtoString(uint8_t* macAddress);
};

#endif

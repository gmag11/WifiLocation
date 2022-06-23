#ifndef BING_MAPS_GEOCODING_H
#define BING_MAPS_GEOCODING_H

#include "Arduino.h"
//#define DEBUG_GEO_CODING 1
#include <QuickDebug.h>

#include <ArduinoJson.h>

#if defined ESP8266
#include <ESP8266WiFi.h>
#elif defined ESP32
#include <WiFi.h>
#include <WiFiClientSecure.h>
#else
#error Wrong platform
#endif

#define MAX_CONNECTION_TIMEOUT 5000

typedef enum {
    BG_OK = 0,
    BG_UNKNOWN = -1,
    BG_API_CONNECT_ERROR = -2,
    BG_TIME_NOT_SET = -3,
    BG_INVALID_RESPONSE = -4,
    BG_INVALID_CREDENTIALS = -5
} bingGeo_status_t;

typedef struct {
    bool valid = false;
    float lat = 0;
    float lon = 0;
    String state = "";
    String district2 = "";
    String country = "";
    String city = "";
    String postalCode = "";
    String address = "";
    String formattedAddress = "";
} bingGeoData_t;


class BingGeoCoding {
public:
    BingGeoCoding (String bingKey = "");
    bingGeoData_t getGeoFromPosition(float lat, float lon);
    static String bgStatusStr (int status);
    bingGeo_status_t getStatus () {
        return status;
    }
    String getCompleteAddress (bingGeoData_t* geoData = NULL);

protected:
    bingGeo_status_t status = BG_UNKNOWN;
    String _bingMapsKey;
    WiFiClientSecure _client;
    bingGeoData_t _geoData;

    static String dumpGeoData (bingGeoData_t geoData);
};

#endif // BING_MAPS_GEOCODING_H
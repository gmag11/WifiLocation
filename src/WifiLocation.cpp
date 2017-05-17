//
//
//

#include "WifiLocation.h"

const char* googleApisHost = "www.googleapis.com";
const char* googleApiUrl = "/geolocation/v1/geolocate";

#ifdef ARDUINO_ARCH_ESP32
const char* googleCA = "-----BEGIN CERTIFICATE-----\n" \
"MIID8DCCAtigAwIBAgIDAjqSMA0GCSqGSIb3DQEBCwUAMEIxCzAJBgNVBAYTAlVT\n" \
"MRYwFAYDVQQKEw1HZW9UcnVzdCBJbmMuMRswGQYDVQQDExJHZW9UcnVzdCBHbG9i\n" \
"YWwgQ0EwHhcNMTUwNDAxMDAwMDAwWhcNMTcxMjMxMjM1OTU5WjBJMQswCQYDVQQG\n" \
"EwJVUzETMBEGA1UEChMKR29vZ2xlIEluYzElMCMGA1UEAxMcR29vZ2xlIEludGVy\n" \
"bmV0IEF1dGhvcml0eSBHMjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AJwqBHdc2FCROgajguDYUEi8iT/xGXAaiEZ+4I/F8YnOIe5a/mENtzJEiaB0C1NP\n" \
"VaTOgmKV7utZX8bhBYASxF6UP7xbSDj0U/ck5vuR6RXEz/RTDfRK/J9U3n2+oGtv\n" \
"h8DQUB8oMANA2ghzUWx//zo8pzcGjr1LEQTrfSTe5vn8MXH7lNVg8y5Kr0LSy+rE\n" \
"ahqyzFPdFUuLH8gZYR/Nnag+YyuENWllhMgZxUYi+FOVvuOAShDGKuy6lyARxzmZ\n" \
"EASg8GF6lSWMTlJ14rbtCMoU/M4iarNOz0YDl5cDfsCx3nuvRTPPuj5xt970JSXC\n" \
"DTWJnZ37DhF5iR43xa+OcmkCAwEAAaOB5zCB5DAfBgNVHSMEGDAWgBTAephojYn7\n" \
"qwVkDBF9qn1luMrMTjAdBgNVHQ4EFgQUSt0GFhu89mi1dvWBtrtiGrpagS8wDgYD\n" \
"VR0PAQH/BAQDAgEGMC4GCCsGAQUFBwEBBCIwIDAeBggrBgEFBQcwAYYSaHR0cDov\n" \
"L2cuc3ltY2QuY29tMBIGA1UdEwEB/wQIMAYBAf8CAQAwNQYDVR0fBC4wLDAqoCig\n" \
"JoYkaHR0cDovL2cuc3ltY2IuY29tL2NybHMvZ3RnbG9iYWwuY3JsMBcGA1UdIAQQ\n" \
"MA4wDAYKKwYBBAHWeQIFATANBgkqhkiG9w0BAQsFAAOCAQEACE4Ep4B/EBZDXgKt\n" \
"10KA9LCO0q6z6xF9kIQYfeeQFftJf6iZBZG7esnWPDcYCZq2x5IgBzUzCeQoY3IN\n" \
"tOAynIeYxBt2iWfBUFiwE6oTGhsypb7qEZVMSGNJ6ZldIDfM/ippURaVS6neSYLA\n" \
"EHD0LPPsvCQk0E6spdleHm2SwaesSDWB+eXknGVpzYekQVA/LlelkVESWA6MCaGs\n" \
"eqQSpSfzmhCXfVUDBvdmWF9fZOGrXW2lOUh1mEwpWjqN0yvKnFUEv/TmFNWArCbt\n" \
"F4mmk2xcpMy48GaOZON9muIAs0nH5Aqq3VuDx3CQRk6+0NtZlmwu9RY23nHMAcIS\n" \
"wSHGFg==\n" \
"-----END CERTIFICATE-----\n";
#endif

String WifiLocation::MACtoString(uint8_t* macAddress) {
    uint8_t mac[6];
    char macStr[18] = { 0 };
#ifdef ARDUINO_ARCH_SAMD
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[5], macAddress[4], macAddress[3], macAddress[2], macAddress[1], macAddress[0]);
#elif defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
#endif
    return  String(macStr);
}

WifiLocation::WifiLocation(String googleKey) {
    _googleApiKey = googleKey;
}

// Function to get a list of surrounding WiFi signals in JSON format to get location via Google Location API
String WifiLocation::getSurroundingWiFiJson() {
    String wifiArray = "[\n";

    int8_t numWifi = WiFi.scanNetworks();
#ifdef DEBUG_WIFI_LOCATION
    Serial.println(String(numWifi) + " WiFi networks found");
#endif // DEBUG_WIFI_LOCATION
    for (uint8_t i = 0; i < numWifi; i++) {//numWifi; i++) {
      //Serial.print("WiFi.BSSID(i) = ");
      //Serial.println((char *)WiFi.BSSID(i));
#ifdef ARDUINO_ARCH_SAMD
        byte bssid[6];

        wifiArray += "{\"macAddress\":\"" + MACtoString(WiFi.BSSID(i,bssid)) + "\",";
#elif defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
        wifiArray += "{\"macAddress\":\"" + MACtoString(WiFi.BSSID(i)) + "\",";
#else
#error Only ESP8266 and SAMD platforms are supported
#endif
        wifiArray += "\"signalStrength\":" + String(WiFi.RSSI(i)) + ",";
        wifiArray += "\"channel\":" + String(WiFi.channel(i)) + "}";
        if (i < (numWifi - 1)) {
            wifiArray += ",\n";
        }
    }
#if defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
    WiFi.scanDelete();
#endif
    wifiArray += "]";
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("WiFi list :\n" + wifiArray);
#endif // DEBUG_WIFI_LOCATION
    return wifiArray;
}

// Calls Google Location API to get current location using surrounding WiFi signals inf
location_t WifiLocation::getGeoFromWiFi() {

    location_t location;
    String response = "";
#ifdef ARDUINO_ARCH_ESP32
    _client.setCACert(googleCA);
#endif
    if (_client.connect(googleApisHost, 443)) {
#ifdef DEBUG_WIFI_LOCATION
        Serial.println("Connected to API endpoint");
#endif // DEBUG_WIFI_LOCATION
    } else {
#ifdef DEBUG_WIFI_LOCATION
        Serial.println("HTTPS error");
#endif // DEBUG_WIFI_LOCATION
        return location;
    }

    String body = "{\"wifiAccessPoints\":" + getSurroundingWiFiJson() + "}";
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("requesting URL: " + String(googleApiUrl) + "?key=" + _googleApiKey);
#endif // DEBUG_WIFI_LOCATION
    String request = String("POST ") + String(googleApiUrl);
    if (_googleApiKey != "")
        request += "?key=" + _googleApiKey;
    request += " HTTP/1.1\r\n";
    request += "Host: " + String(googleApisHost) + "\r\n";
    request += "User-Agent: ESP8266\r\n";
    request += "Content-Type:application/json\r\n";
    request += "Content-Length:" + String(body.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += body;
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("request: \n" + request);
#endif // DEBUG_WIFI_LOCATION

    _client.println(request);
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("request sent");
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
#endif // DEBUG_WIFI_LOCATION
    int timer = millis();

    // Wait for response
    while (millis() - timer < MAX_CONNECTION_TIMEOUT) {
        if (_client.available())
            break;
    }
    while (_client.available()) {
#ifdef DEBUG_WIFI_LOCATION
        Serial.print(".");
#endif // DEBUG_WIFI_LOCATION
        response += _client.readString();
    }
#ifdef DEBUG_WIFI_LOCATION
    Serial.println();
#endif // DEBUG_WIFI_LOCATION
    if (response != "") {
#ifdef DEBUG_WIFI_LOCATION
        Serial.println("Got response:");
        Serial.println(response);
#endif // DEBUG_WIFI_LOCATION

        int index = response.indexOf("\"lat\":");
        if (index != -1) {
            String tempStr = response.substring(index);
            //Serial.println(tempStr);
            tempStr = tempStr.substring(tempStr.indexOf(":") + 1, tempStr.indexOf(","));
            //Serial.println(tempStr);
            location.lat = tempStr.toFloat();
#ifdef DEBUG_WIFI_LOCATION
            Serial.println("\nLat: " + String(location.lat, 7));
#endif // DEBUG_WIFI_LOCATION
        }

        index = response.indexOf("\"lng\":");
        if (index != -1) {
            String tempStr = response.substring(index);
            //Serial.println(tempStr);
            tempStr = tempStr.substring(tempStr.indexOf(":") + 1, tempStr.indexOf(","));
            //Serial.println(tempStr);
            location.lon = tempStr.toFloat();
#ifdef DEBUG_WIFI_LOCATION
            Serial.println("Lon: " + String(location.lon, 7));
#endif // DEBUG_WIFI_LOCATION
        }

        index = response.indexOf("\"accuracy\":");
        if (index != -1) {
            String tempStr = response.substring(index);
            //Serial.println(tempStr);
            tempStr = tempStr.substring(tempStr.indexOf(":") + 1, tempStr.indexOf("\n"));
            //Serial.println(tempStr);
            location.accuracy = tempStr.toFloat();
#ifdef DEBUG_WIFI_LOCATION
            Serial.println("Accuracy: " + String(location.accuracy) + "\n");
#endif // DEBUG_WIFI_LOCATION
        }
    }

    return location;
}

//
//
//

#include "WifiLocation.h"

String WifiLocation::MACtoString(uint8_t* macAddress) {
    uint8_t mac[6];
    char macStr[18] = { 0 };
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
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
        wifiArray += "{\"macAddress\":\"" + MACtoString(WiFi.BSSID(i)) + "\",";
        wifiArray += "\"signalStrength\":" + String(WiFi.RSSI(i)) + ",";
        wifiArray += "\"channel\":" + String(WiFi.channel(i)) + "}";
        if (i < (numWifi - 1)) {
            wifiArray += ",\n";
        }
    }
#ifdef ARDUINO_ARCH_ESP8266
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

    if (_client.connect(googleApisHost.c_str(), 443)) {
#ifdef DEBUG_WIFI_LOCATION
        Serial.println("Connected to API endpoint");
#endif // DEBUG_WIFI_LOCATION
    } else {
        return location;
    }

    String body = "{\"wifiAccessPoints\":" + getSurroundingWiFiJson() + "}";
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("requesting URL: " + googleApiUrl + "?key=" + _googleApiKey);
#endif // DEBUG_WIFI_LOCATION
    String request = String("POST ") + googleApiUrl;
    if (_googleApiKey != "")
        request += "?key=" + _googleApiKey;
    request += " HTTP/1.1\r\n";
    request += "Host: " + googleApisHost + "\r\n";
    request += "User-Agent: ESP8266\r\n";
    request += "Content-Type:application/json\r\n";
    request += "Content-Length:" + String(body.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += body;
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("request: \n" + request);
#endif // DEBUG_WIFI_LOCATION

    _client.print(request);
#ifdef DEBUG_WIFI_LOCATION
    Serial.println("request sent");
#endif // DEBUG_WIFI_LOCATION

    while (_client.connected()) {
        response += _client.readString();
    }

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

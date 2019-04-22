//
//
//

#include "WifiLocation.h"

const char* googleApisHost = "www.googleapis.com";
const char* googleApiUrl = "/geolocation/v1/geolocate";

// GlobalSign CA certificate valid until 15th december 2021
#if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_ARCH_ESP8266
//static const char GoogleCA[] PROGMEM = R"EOF(
//-----BEGIN CERTIFICATE-----
//MIIEXDCCA0SgAwIBAgINAeOpMBz8cgY4P5pTHTANBgkqhkiG9w0BAQsFADBMMSAw
//HgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFs
//U2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEy
//MTUwMDAwNDJaMFQxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3Qg
//U2VydmljZXMxJTAjBgNVBAMTHEdvb2dsZSBJbnRlcm5ldCBBdXRob3JpdHkgRzMw
//ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDKUkvqHv/OJGuo2nIYaNVW
//XQ5IWi01CXZaz6TIHLGp/lOJ+600/4hbn7vn6AAB3DVzdQOts7G5pH0rJnnOFUAK
//71G4nzKMfHCGUksW/mona+Y2emJQ2N+aicwJKetPKRSIgAuPOB6Aahh8Hb2XO3h9
//RUk2T0HNouB2VzxoMXlkyW7XUR5mw6JkLHnA52XDVoRTWkNty5oCINLvGmnRsJ1z
//ouAqYGVQMc/7sy+/EYhALrVJEA8KbtyX+r8snwU5C1hUrwaW6MWOARa8qBpNQcWT
//kaIeoYvy/sGIJEmjR0vFEwHdp1cSaWIr6/4g72n7OqXwfinu7ZYW97EfoOSQJeAz
//AgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUH
//AwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHfCuFCa
//Z3Z2sS3ChtCDoH6mfrpLMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd/cGYYu
//MDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdv
//b2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dz
//cjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYc
//aHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEA
//HLeJluRT7bvs26gyAZ8so81trUISd7O45skDUmAge1cnxhG1P2cNmSxbWsoiCt2e
//ux9LSD+PAj2LIYRFHW31/6xoic1k4tbWXkDCjir37xTTNqRAMPUyFRWSdvt+nlPq
//wnb8Oa2I/maSJukcxDjNSfpDh/Bd1lZNgdd/8cLdsE3+wypufJ9uXO1iQpnh9zbu
//FIwsIONGl1p3A8CgxkqI/UAih3JaGOqcpcdaCIzkBaR9uYQ1X4k2Vg5APRLouzVy
//7a8IVk6wuy6pm+T7HT4LY8ibS5FEZlfAFLSW8NwsVz9SBK2Vqn1N0PIMn5xA6NZV
//c7o835DLAFshEWfC7TIe3g==
//-----END CERTIFICATE-----
//)EOF";

static const char GlobalSignCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G
A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp
Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1
MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG
A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL
v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8
eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq
tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd
C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa
zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB
mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH
V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n
bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG
3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs
J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO
291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS
ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd
AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7
TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==
-----END CERTIFICATE-----
)EOF";
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
    if(numWifi > MAX_WIFI_SCAN) {
        numWifi = MAX_WIFI_SCAN;
    }

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

#if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_ARCH_ESP8266
// Set time via NTP, as required for x.509 validation
void setClock () {
    configTime (3600, 0, "pool.ntp.org", "time.nist.gov");

#ifdef DEBUG_WIFI_LOCATION
    Serial.print ("Waiting for NTP time sync: ");
#endif
    time_t now = time (nullptr);
    while (now < 8 * 3600 * 2) {
        delay (500);
#ifdef DEBUG_WIFI_LOCATION
        Serial.print (".");
#endif
        now = time (nullptr);
    }
    struct tm timeinfo;
    gmtime_r (&now, &timeinfo);
#ifdef DEBUG_WIFI_LOCATION
    Serial.println ();
    Serial.print ("Current time: ");
    Serial.print (asctime (&timeinfo));
#endif
}
#endif //ARDUINO_ARCH_ESP32 || ARDUINO_ARCH_ESP8266

// Calls Google Location API to get current location using surrounding WiFi signals inf
location_t WifiLocation::getGeoFromWiFi() {

    location_t location;
    String response = "";
#if defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
    setClock ();
#ifdef ARDUINO_ARCH_ESP8266
    BearSSL::X509List cert (GlobalSignCA);
    _client.setTrustAnchors (&cert);
#endif // ARDUINO_ARCH_ESP8266
#endif
#ifdef ARDUINO_ARCH_ESP32
    _client.setCACert(GlobalSignCA);
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
        yield ();
    }
    while (_client.available()) {
#ifdef DEBUG_WIFI_LOCATION
        Serial.print(".");
#endif // DEBUG_WIFI_LOCATION
        response += _client.readString();
        yield ();
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

//
//
//

#include "WifiLocation.h"
#include "time.h"

const char* googleApisHost = "www.googleapis.com";
const char* googleApiUrl = "/geolocation/v1/geolocate";

#if DEBUG_WIFI_LOCATION
#define DEBUG_WL Serial.print
#else
#define DEBUG_WL(...)
#endif

#if !defined ESP32 && !defined ESP8266
#error Only ESP8266 and ESP32 platforms are supported
#endif


// GlobalSign CA certificate valid until 28th January 2028
static const char GlobalSignCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG
A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv
b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw
MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i
YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT
aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ
jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp
xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp
1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG
snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ
U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8
9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E
BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B
AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz
yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE
38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP
AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad
DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME
HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==
-----END CERTIFICATE-----
)EOF";

String WifiLocation::MACtoString(uint8_t* macAddress) {
    const int MAC_ADDR_SIZE = 18;
    char macStr[MAC_ADDR_SIZE] = { 0 };
    snprintf (macStr, MAC_ADDR_SIZE, "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
    return  String(macStr);
}

WifiLocation::WifiLocation(String googleKey) {
    _googleApiKey = googleKey;
}

// Function to get a list of surrounding WiFi signals in JSON format to get location via Google Location API
String WifiLocation::getSurroundingWiFiJson() {
    String wifiArray = "[";

    int8_t numWifi = WiFi.scanNetworks();
    if(numWifi > MAX_WIFI_SCAN) {
        numWifi = MAX_WIFI_SCAN;
    }

    DEBUG_WL (String (numWifi) + " WiFi networks found\n");
    for (uint8_t i = 0; i < numWifi; i++) {//numWifi; i++) {
        //DEBUG_WL("WiFi.BSSID(i) = ");
        //DEBUG_WL((char *)WiFi.BSSID(i));
        //DEBUG_WL ("\n");
        wifiArray += "{\"macAddress\":\"" + MACtoString(WiFi.BSSID(i)) + "\",";
        wifiArray += "\"signalStrength\":" + String(WiFi.RSSI(i)) + ",";
        wifiArray += "\"channel\":" + String(WiFi.channel(i)) + "}";
        if (i < (numWifi - 1)) {
            wifiArray += ",\n";
        }
    }
    WiFi.scanDelete();
    wifiArray += "]";
    DEBUG_WL ("WiFi list:\n" + wifiArray + "\n\n");
    return wifiArray;
}

// // Set time via NTP, as required for x.509 validation
// void setClock () {
//     configTime (3600, 0, "pool.ntp.org", "time.nist.gov");

//     DEBUG_WL ("Waiting for NTP time sync: ");
//     time_t now = time (nullptr);
//     while (now < 8 * 3600 * 2) {
//         delay (500);
//         DEBUG_WL (".");
//         now = time (nullptr);
//     }
// #if DEBUG_WIFI_LOCATION
//     struct tm timeinfo;
//     gmtime_r (&now, &timeinfo);
//     DEBUG_WL ("\n");
//     DEBUG_WL ("Current time: ");
//     DEBUG_WL (asctime (&timeinfo));
//     DEBUG_WL ("\n");
// #endif
// }

// Calls Google Location API to get current location using surrounding WiFi signals inf
location_t WifiLocation::getGeoFromWiFi() {

    location_t location;
    String response = "";
// 	setClock ();
    if (time (nullptr) < 8 * 3600 * 2) {
        status = WL_TIME_NOT_SET;
        return location;
    }
#ifdef ESP8266 
#if (defined BR_BEARSSL_H__ && not defined USE_CORE_PRE_2_5_0)
	BearSSL::X509List cert (GlobalSignCA);
	_client.setTrustAnchors (&cert);
#else
	_client.setCACert (reinterpret_cast<const uint8_t*>(GlobalSignCA), sizeof (GlobalSignCA));
#endif
#endif // ESP8266

#ifdef ESP32
    _client.setCACert(GlobalSignCA);
#endif
    if (_client.connect(googleApisHost, 443)) {
        DEBUG_WL ("Connected to API endpoint\n");
    } else {
        DEBUG_WL ("HTTPS error\n");
        status = WL_API_CONNECT_ERROR;
        return location;
    }

    String body = "{\"considerIP\":false,\"wifiAccessPoints\":" + getSurroundingWiFiJson () + "}";
    DEBUG_WL ("requesting URL: " + String (googleApiUrl) + "?key=" + _googleApiKey + "\n");
    String request = String("POST ") + String(googleApiUrl);
    if (_googleApiKey != "")
        request += "?key=" + _googleApiKey;
    request += " HTTP/1.1\r\n";
    request += "Host: " + String (googleApisHost) + "\r\n";
#ifdef ESP32
    request += "User-Agent: ESP32\r\n";
#else
    request += "User-Agent: ESP8266\r\n";
#endif
    request += "Content-Type:application/json\r\n";
    request += "Content-Length:" + String (body.length ()) + "\r\n";
    request += "Connection: keep-alive\r\n\r\n";
    //request += "Connection: close\r\n\r\n";
    request += body;
    DEBUG_WL ("request: \n" + request + "\n");

    _client.println(request);
    DEBUG_WL ("request sent\n");
    DEBUG_WL ("Free heap: ");
    DEBUG_WL (ESP.getFreeHeap ());
    DEBUG_WL ("\n");
    int timer = millis();

    // Wait for response
    while (millis() - timer < MAX_CONNECTION_TIMEOUT) {
        if (_client.available())
            break;
        yield ();
    }
    while (_client.available()) {
        DEBUG_WL (".");
        response += _client.readString();
        yield ();
    }
    _client.stop ();
    DEBUG_WL ("\n");
    if (response != "") {
        DEBUG_WL ("Got response:\n");
        DEBUG_WL (response);
        DEBUG_WL ("\n");

        int index = response.indexOf("\"lat\":");
        if (index != -1) {
            String tempStr = response.substring(index);
            //Serial.println(tempStr);
            tempStr = tempStr.substring(tempStr.indexOf(":") + 1, tempStr.indexOf(","));
            //Serial.println(tempStr);
            location.lat = tempStr.toFloat();
            DEBUG_WL ("\nLat: " + String (location.lat, 7) + "\n");
        }

        index = response.indexOf("\"lng\":");
        if (index != -1) {
            String tempStr = response.substring(index);
            // DEBUG_WL (tempStr + "\n");
            tempStr = tempStr.substring(tempStr.indexOf(":") + 1, tempStr.indexOf(","));
            // DEBUG_WL (tempStr + "\n");
            location.lon = tempStr.toFloat();
            DEBUG_WL ("Lon: " + String (location.lon, 7) + "\n");
        }

        index = response.indexOf("\"accuracy\":");
        if (index != -1) {
            String tempStr = response.substring(index);
            // DEBUG_WL (tempStr + "\n");
            tempStr = tempStr.substring(tempStr.indexOf(":") + 1, tempStr.indexOf("\n"));
            // DEBUG_WL (tempStr + "\n");
            location.accuracy = tempStr.toFloat();
            DEBUG_WL ("Accuracy: " + String (location.accuracy) + "\n\n");
        }
    }

    status = WL_OK;
    return location;
}

String WifiLocation::wlStatusStr (int status) {
    switch (status) {
    case WL_API_CONNECT_ERROR:
        return "API connection error";
        break;
    case WL_OK:
        return "OK";
        break;
    case WL_TIME_NOT_SET:
        return "Time not set";
        break;            
    default:
        return "Unknown error";
    }
}

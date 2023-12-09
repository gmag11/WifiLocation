#include "bingMapsGeocoding.h"
#include "time.h"

const char* bingMapsApisHost = "dev.virtualearth.net";
const char* bingMapsApiUrl = "/REST/v1/Locations/";

const char* TAG_BING_GEO = "BINGGEO";

#if !defined ESP32 && !defined ESP8266
#error Only ESP8266 and ESP32 platforms are supported
#endif


// GlobalSign CA certificate valid until 13th May 2025
static const char DigiCertRootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl
MrY=
-----END CERTIFICATE-----
)EOF";

BingGeoCoding::BingGeoCoding (String bingMapsKey) {
    _bingMapsKey = bingMapsKey;
}

bingGeoData_t BingGeoCoding::getGeoFromPosition (float lat, float lon) {

    String response = "";
    _geoData.valid = false;
    if (time (nullptr) < 8 * 3600 * 2) {
        status = BG_TIME_NOT_SET;
        return _geoData;
    }
#ifdef ESP8266 
#if (defined BR_BEARSSL_H__ && not defined USE_CORE_PRE_2_5_0)
    BearSSL::X509List cert (DigiCertRootCA);
    _client.setTrustAnchors (&cert);
#else
    _client.setCACert (reinterpret_cast<const uint8_t*>(DigiCertRootCA), sizeof (DigiCertRootCA));
#endif
#endif // ESP8266

#ifdef ESP32
    _client.setCACert (DigiCertRootCA);
#endif
    if (_client.connect (bingMapsApisHost, 443)) {
        DEBUG_DBG (TAG_BING_GEO, "Connected to API endpoint");
    } else {
        DEBUG_WARN (TAG_BING_GEO, "HTTPS error");
        status = BG_API_CONNECT_ERROR;
        return _geoData;
    }

    String url = String (bingMapsApiUrl) + String (lat, 6) + "," + String (lon, 6) + "?o=json&key=" + _bingMapsKey;
    DEBUG_DBG (TAG_BING_GEO, "requesting URL: %s", url.c_str ());
    String request = String ("GET ") + String (url);
    request += " HTTP/1.1\r\n";
    request += "Host: " + String (bingMapsApisHost) + "\r\n";
#ifdef ESP32
    request += "User-Agent: ESP32\r\n";
#else
    request += "User-Agent: ESP8266\r\n";
#endif
    request += "Connection: keep-alive\r\n\r\n";

    DEBUG_DBG (TAG_BING_GEO, "request: %s", request.c_str ());

    _client.println (request);
    DEBUG_DBG (TAG_BING_GEO, "request sent");
    int timer = millis ();

    // Wait for response
    while (millis () - timer < MAX_CONNECTION_TIMEOUT) {
        if (_client.available ())
            break;
        yield ();
    }

    StaticJsonDocument<96> filter;
    filter["authenticationResultCode"] = true;
    filter["resourceSets"][0]["resources"][0]["address"] = true;

    StaticJsonDocument<1024> doc;

    while (_client.available ()) {
        response += _client.readString ();
        yield ();
    }

    _client.stop ();
    if (response != "") {
        DEBUG_VERBOSE (TAG_BING_GEO, "Got response: %s", response.c_str ());
        int index = response.indexOf ("{\"auth");
        response = response.substring (index);
        DEBUG_DBG (TAG_BING_GEO, ARDUHAL_LOG_COLOR(ARDUHAL_LOG_COLOR_CYAN) "Response Body: %s", response.c_str ());
        DeserializationError error = deserializeJson (doc, response, DeserializationOption::Filter (filter));

        if (error) {
            DEBUG_WARN (TAG_BING_GEO, "deserializeJson() failed:%s", error.c_str ());
            status = BG_INVALID_RESPONSE;
            return _geoData;
        }

        if (doc["authenticationResultCode"] != "ValidCredentials") {
            DEBUG_WARN (TAG_BING_GEO, "authenticationResultCode: %s", doc["authenticationResultCode"].as<String> ().c_str ());
            status = BG_INVALID_CREDENTIALS;
            return _geoData;
        }
        
        _geoData.lat = lat;
        _geoData.lon = lon;
        _geoData.address = doc["resourceSets"][0]["resources"][0]["address"]["addressLine"].as<String> ();
        _geoData.country = doc["resourceSets"][0]["resources"][0]["address"]["countryRegion"].as<String> ();
        _geoData.postalCode = doc["resourceSets"][0]["resources"][0]["address"]["postalCode"].as<String> ();
        _geoData.state = doc["resourceSets"][0]["resources"][0]["address"]["adminDistrict"].as<String> ();
        _geoData.city = doc["resourceSets"][0]["resources"][0]["address"]["locality"].as<String> ();
        _geoData.district2 = doc["resourceSets"][0]["resources"][0]["address"]["adminDistrict2"].as<String> ();
        _geoData.formattedAddress = doc["resourceSets"][0]["resources"][0]["address"]["formattedAddress"].as<String> ();
        _geoData.valid = true;

        DEBUG_DBG (TAG_BING_GEO, "Geodata:\n%s", dumpGeoData(_geoData).c_str ());
    }

    status = BG_OK;
    return _geoData;
}

String BingGeoCoding::bgStatusStr (int status) {
    switch (status) {
    case BG_API_CONNECT_ERROR:
        return "API connection error";
        break;
    case BG_OK:
        return "OK";
        break;
    case BG_TIME_NOT_SET:
        return "Time not set";
        break;
    case BG_INVALID_RESPONSE:
        return "Invalid response";
        break;
    case BG_INVALID_CREDENTIALS:
        return "Invalid credentials";
        break;
    default:
        return "Unknown error";
    }
}

String BingGeoCoding::dumpGeoData (bingGeoData_t geoData) {
    String str = "";
    if (geoData.valid) {
        str += "lat: " + String (geoData.lat, 6) + "\n";
        str += "lon: " + String (geoData.lon, 6) + "\n";
        str += "address: " + geoData.address + "\n";
        str += "country: " + geoData.country + "\n";
        str += "postalCode: " + geoData.postalCode + "\n";
        str += "state: " + geoData.state + "\n";
        str += "city: " + geoData.city + "\n";
        str += "district2: " + geoData.district2 + "\n";
        str += "formattedAddress: " + geoData.formattedAddress;
    } else {
        str += "Invalid geodata";
    }
    return str;
}

String BingGeoCoding::getCompleteAddress (bingGeoData_t* geoData) {
    if (geoData) {

        if (geoData->valid) {
            return geoData->address + ", " + geoData->postalCode + " " + geoData->city + ", " + geoData->state + ", " + geoData->country;
        } else {
            return "Invalid address";
        }
    } else {
        return getCompleteAddress (&_geoData);
    }
}

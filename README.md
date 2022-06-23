[![Compile examples for ESP8266](https://github.com/gmag11/WifiLocation/actions/workflows/examples_ESP8266.yml/badge.svg)](https://github.com/gmag11/WifiLocation/actions/workflows/examples_ESP8266.yml)
[![Compile examples for ESP32](https://github.com/gmag11/WifiLocation/actions/workflows/examples_ESP32.yml/badge.svg)](https://github.com/gmag11/WifiLocation/actions/workflows/examples_ESP32.yml)

# WiFi Location

> ## Important notice:
> Since July 2018, Google's conditions for Maps APIs have changed. **You have to assign a billing method o Google Cloud Console** to it so that you are able to use it. Check more information here: https://cloud.google.com/maps-platform/pricing/. Be careful about number of request you make using this library. There is a free tier but you know this can change in any moment. I am not responsible of billing cost caused by use of Google API.

> ## Platform compatibility:
> Support of SAMD microcontrollers have been deprecated. If you use MKR series boards from Arduino you can keep using version 1.2.5.
> Newer developments will be done for ESP8266 and ESP32.

> ## Google CA certificate expiration
> Current Google CA certificate will expire on Jan 28th 2028. This is needed to validate security Google API address. I'll update it as soon Google release a new one. But keep in mind that you will need to update before that date to keep this library working

> ## Bing Maps CA certificate expiration
> Current Bing Maps CA certificate will expire on May 15th 2025. This is needed to validate security Bing Maps API address. I'll update it as soon Microsoft release a new one. But keep in mind that you will need to update before that date to keep this library working

## Introduction
When location information is needed in an electronic project, we normally think about a GPS module. But we know that mobile phones can get approximate location listening WiFi signals, when GPS is disabled or not usable because we are inside a building.

If your project needs approximate location or you are indoors, and it is connected to the Internet, you can use same mechanism to get latitude and longitude. So, you don't need additional hardware.

This code uses access to Google Maps GeoLocation API. Please check [Google Policies](https://developers.google.com/maps/documentation/geolocation/policies).

After version 1.3.0, this library will use Bing Maps GeoLocation API to get real address from latitude and longitude.

Both functions are independent and can be used separately.

## Description
This is a library that sends Google Maps GeoLocaion API a request with a list of all WiFi AP's BSSID that your microcontroller listens to. Google, then answer with location and accuracy in JSON format.

Request body has this form:

```json
{
	"wifiAccessPoints": [
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-58,"channel":11},
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-85,"channel":11},
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-82,"channel":1},
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-89,"channel":6},
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-86,"channel":13},
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-89,"channel":4},
		{"macAddress":"XX:XX:XX:XX:XX:XX","signalStrength":-42,"channel":5}
   ]
}
```
If information is correct, Google GeoLocation API response will be like this:

```json
{
 "location": {
  "lat": 37.3689919,
  "lng": -122.1054095
 },
 "accuracy": 39.0
}
```

You need a google API key to validate with Google API. Navigate to [Google Developers Console](https://console.developers.google.com/apis) to enable GeoLocation API. Without this API key you will get an error as response.

Go to https://developers.google.com/maps/documentation/geolocation/intro to learn how to create an API key.

That API key has to be provided to WifiLocation class constructor like a String.

Once WifiLocation object is created, you can get location calling `getGeoFromWiFi()`.

Google requires using secure TLS connection in order to use GeoLocation API. `WifiLocation.cpp` includes GoogleCA certificate to check server chain. This certificate may expire.

You may get current certificate using this command (if you are using Linux)

``` bash
openssl s_client -servername www.googleapis.com -showcerts \
 -connect www.googleapis.com:443 < /dev/null \
 |  awk '/^-----BEGIN CERTIFICATE-----/,/^-----END CERTIFICATE-----/{if(++m==1)n++;if(n==2)print;if(/^-----END CERTIFICATE-----/)m=0}' \
 > googleCA.cer
```

Then, you can copy file content and overwrite [this part](https://github.com/gmag11/WifiLocation/blob/dev/src/WifiLocation.cpp#L24-L44) of `WifiLocation.cpp`, from line 24.

## Getting real address from location

From version 1.3.0, this library also provides a function to get real address from latitude and longitude. With it you can get address from your current location or from any other latitude and longitude coordinates.

This feature uses Bing Maps API and needs an additional API key. This API is free for use for 1 million requests per year. But Microsoft may change this in the future.

In order to get an API key, navigate to [Bing Maps Developer Center](https://www.bingmapsportal.com).

Details about this API can be found in https://docs.microsoft.com/en-us/bingmaps/rest-services/locations/find-a-location-by-point.

Bing Maps Root CA certificate is also included in this library. It is valid until May 15th 2025. You can get a new valid CA certificate with:

``` bash
openssl s_client -servername dev.virtualearth.net -showcerts \
 -connect dev.virtualearth.net:443 < /dev/null \
 |  awk '/^-----BEGIN CERTIFICATE-----/,/^-----END CERTIFICATE-----/{if(++m==1)n++;if(n==2)print;if(/^-----END CERTIFICATE-----/)m=0}' \
 > bingMapsCA.cer
```

You can copy file content and overwrite [this part](https://github.com/gmag11/WifiLocation/blob/dev/src/bingMapsGeocoding.cpp#L16-L36) of `bingMapsGeocoding.cpp`, from line 16.

## Required libraries

Using this code on ESP8266 or ESP32 requires [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for Bing Maps Geocoding response processing and [QuickDebug](https://github.com/gmag11/QuickDebug) library for Debug Messages.

There are a couple of examples that make use of this feature:
- BingMApsGeo.ino just use hardcoded coordinates to get real address
- LocationAndGeo.ino uses WiFi to get current location and then it passes it to Bing Maps API to get real address

## Limitations

Notice that in order to get location this library has to scan for all surrounding Wi-Fi networks and make the request to Google API. Don't expect to get this immediately. All needed steps may take up to 20 seconds.

So, if your project is powered with batteries, you need to drastically limit the frequency of position updates or use a GPS instead.

Current version uses synchronous programming. This means that main code will be blocked while location is being resolved. As this can be quite long you need to take it into account while programming your project.

In future versions, I'll try to add non blocking options. I think I can keep back compatibility so that anyone can get updated versions without using new features.

## Code example

Valid for ESP32 and ESP8266

```Arduino
#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#error Wrong platform
#endif 

#include <WifiLocation.h>

const char* googleApiKey = "YOUR_GOOGLE_API_KEY";
const char* ssid = "SSID";
const char* passwd = "PASSWD";

WifiLocation location(googleApiKey);

void setup() {
    Serial.begin(115200);
    // Connect to WPA/WPA2 network
#ifdef ARDUINO_ARCH_ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif
#ifdef ARDUINO_ARCH_ESP8266
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin(ssid, passwd);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // wait 5 seconds for connection:
        Serial.print("Status = ");
        Serial.println(WiFi.status());
        delay(500);
    }
    location_t loc = location.getGeoFromWiFi();

    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));


}

void loop() {

}
```

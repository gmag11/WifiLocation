# WiFi Location

> ## Important notice:
> Since July 2018, Google's conditions for Maps APIs have changed. You have to assign a billing method to it so that you are able to use it. Check more information here: https://cloud.google.com/maps-platform/pricing/. Be careful about number of request you made using this library. I am not responsible of billing cost caused by use of  Google API.

> ## Platform compatibility:
> Support of SAMD microcontrollers have been deprecated. If you use MKR series boards from Arduino you can keep using version 1.2.5.
> Newer developments will be done for ESP8266 and ESP32.

## Introduction
When location information is needed in an electronic project, we normally think about a GPS module. But we know that mobile phones can get approximate location listening WiFi signals, when GPS is disabled or not usable because we are inside a building.

If your project needs approximate location or you are indoors, and it is connected to the Internet, you can use same mechanism to get latitude and longitude. So, you don't need additional hardware.

This code uses access to Google Maps GeoLocation API. Please check [Google Policies](https://developers.google.com/maps/documentation/geolocation/policies).

## Description
This is a library that sends Google Maps GeoLocaion API a request with a list of all WiFi AP's BSSID that your microcontroller listens to. Google, then answer with location and accuracy in JSON format.

Request body has this form:

```
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

```
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

Then, you can copy file content and overwrite [this part](https://github.com/gmag11/WifiLocation/blob/dev/src/WifiLocation.cpp#L14-L38) of `WifiLocation.cpp`, from line 14.

## Required libraries

Using this code on ESP8266 or ESP32 platform does not require any external library.

In order to compile this on MRK1000, WiFi101 library is needed.

## Limitations

Notice that in order to get location this library has to scan for all surrounding Wi-Fi networks and make the request to Google API. Don't expect to get this immediately. All needed steps may take up to 20 seconds.

So, if your project is powered with batteries, you need to drastically limit the frequency of position updates or use a GPS instead.

Current version uses synchronous programming. This means that main code will be blocked while location is being resolved. As this can be quite long you need to take it into account while programming your project.

In future versions, I'll try to add non blocking options. I think I can keep back compatibility so that anyone can get updated versions without using new features.

## Code example

Valid for ESP32, ESP8266 and MKR1000 (SAMD architecture)

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

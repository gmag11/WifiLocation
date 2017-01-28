#WiFi Location

##Introduction
When location information is needed in an electronic project, we normally think about a GPS module. But we know that mobile phones can get approximate location listening WiFi signals, when GPS is disabled or not usable because we are inside a building.

If your project needs approximate location or you are indoors, and it is connected to the Internet, you can use same mechanism to get latitude and longitude. So, you don't need additional hardware.

This code uses access to Google Maps GeoLocation API. Please check [Google Policies](https://developers.google.com/maps/documentation/geolocation/policies).

##Description
This is a library that sends Google Maps Geolocaion API a request with a list of all WiFi AP's BSSID that your microcontroller listens to. Google, then answer with location and accuracy in JSON format.

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

You need a google API key to validate against Google. Navigate to [Google Developers Console](https://console.developers.google.com/apis) to enable GeoLocation API. Without this API key you will get an error as response.

That API key has to be provided to WifiLocation class contructor like a String.

Once WifiLocation object is created, you can get location calling `getGeoFromWiFi()`.

##Code example

```Arduino
#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#error Wrong platform
#endif

#include "WifiLocation.h"

#define GOOGLE_KEY "YOUR_GOOGLE_API_KEY"
#define SSID "SSID"
#define PASSWD "PASSWD"

WifiLocation location (GOOGLE_KEY);

void setup()
{
    Serial.begin(115200);

    int status = 0;
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(SSID);
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(SSID, PASSWD);

        // wait 10 seconds for connection:
        delay(10000);
    }

    location_t loc = location.getGeoFromWiFi();

    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));


}

void loop()
{
}
```

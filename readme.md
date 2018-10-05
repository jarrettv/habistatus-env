# habistatus-env
ESP8266 NodeMCU habitat status environment monitor (temperature &amp; humidity)

Publishes temperature and humidity messages over MQTT from a BME280 sensor every 5 minutes

## Must set secrets in the **config.h** file, for example:

```
#define WLAN_SSID  "mynetwork"
#define WLAN_PASS  "mypassword"

#define SERVER     "io.adafruit.com"
#define SERVER_PORT 8883

#define SERVER_USER "myname"
#define SERVER_PASS "mykey"
```

## Must set location in the main.cpp file

`#define LOCATION garage`

The data will be published to the following topics

```
SERVER_USER/feeds/habistatus.LOCATION-temp
SERVER_USER/feeds/habistatus.LOCATION-humidity
```
for example:
```
myname/feeds/habistatus.garage-temp
myname/feeds/habistatus.garage-humidity
```
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Wire.h"
#include "SparkFunBME280.h"

#include "config.h" // <-- put secrets here

#define LOCATION "attic"
  
WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVER_PORT, SERVER_USER, SERVER_PASS);
Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, SERVER_USER "/feeds/habistatus." LOCATION "-temp");
Adafruit_MQTT_Publish humidityFeed = Adafruit_MQTT_Publish(&mqtt, SERVER_USER "/feeds/habistatus." LOCATION "-humidity");

BME280 mySensor; //Global sensor object
 
void setup()
{
  Serial.begin(9600);
  delay(10);

  Serial.println(F("\nHabistatus " LOCATION));

  // Connect to WiFi access point.
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());

  Wire.begin();
  Wire.setClock(400000);
 
  mySensor.setI2CAddress(0x76);
  mySensor.beginI2C(Wire);
}
 
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void loop()
{
    MQTT_connect();

    float relative_humidity = mySensor.readFloatHumidity();
    Serial.print("Humidity: ");
    Serial.print(relative_humidity, 0);

    Serial.print(" Pressure: ");
    Serial.print(mySensor.readFloatPressure(), 0);

    Serial.print(" Alt: ");
    //Serial.print(mySensor.readFloatAltitudeMeters(), 1);
    Serial.print(mySensor.readFloatAltitudeFeet(), 1);

    Serial.print(" Temp: ");
    float fahrenheit = mySensor.readTempF();
    Serial.print(fahrenheit, 2);
    
    Serial.println();

    Serial.print(F("Publishing MQTT messages"));

    if (!tempFeed.publish(fahrenheit)) {    
        Serial.println("...Fail to publish to tempFeed");
    } else if (!humidityFeed.publish(relative_humidity)) {
        Serial.println("..Fail to publish to humidityFeed");
    } else {
        Serial.println("...Success");
    }

    delay(10000);
}

// Test Sensor/ThingSpeak

#include <SmingCore.h>
#include <Wire.h>
#include <Temperature_LM75_Derived.h>
#include <Adafruit_VL53L0X.h>


// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "SSID" // Put your SSID and password here
#define WIFI_PWD "123456"
#endif

namespace
{
SimpleTimer procTimer;
int sensorValue;
HttpClient thingSpeak;
// Change Address!
//Generic_LM75 temperature(0x4d);       // generic LM75 0.5°C resolution (9bit)
NXP_LM75A temperature(0x4d);            // LM75A 0.125°C resolution (11bit)
Adafruit_VL53L0X lox;

int onDataSent(HttpConnection& client, bool successful)
{
	Serial.println(successful ? _F("Success sent") : _F("Failed"));

	String response = client.getResponse()->getBody();
	Serial << _F("Server response: '") << response << '\'' << endl;
	if(response.length() > 0) {
		int intVal = response.toInt();

		if(intVal == 0) {
			Serial.println(_F("Sensor value wasn't accepted. May be we need to wait a little?"));
		}
	}

	return 0;
}

void sendData()
{
	// Read our sensor values :)
	sensorValue++;

        VL53L0X_RangingMeasurementData_t measure;

        Serial.print(_F("Reading a measurement... "));
        lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

        if(measure.RangeStatus != 4) { // phase failures have incorrect data
                Serial << _F("Distance: ") << measure.RangeMilliMeter << " mm" << endl;
        } else {
                Serial.println(_F("Out of range"));
        }

	/*
	   Here is an alternative method of URL construction, which is helpful for more complex URLs.
	   The resulting URL string is equivalent to:

	     "http://api.thingspeak.com/update?key=7XXUJWCWYTMXKN3L&field1=" + String(sensorValue)
	 */
	Url url;
	url.Scheme = URI_SCHEME_HTTP;
	url.Host = F("api.thingspeak.com");
	url.Path = F("/update");
	url.Query["key"] = F("Q1PNH2L3RH8SPIQV");
	url.Query["field1"] = String(sensorValue);
        url.Query["field2"] = String(temperature.readTemperatureC());
        url.Query["field3"] = String(measure.RangeMilliMeter);
	thingSpeak.downloadString(url, onDataSent);

	Serial.println(url);
}

// Will be called when WiFi station timeout was reached
void connectFail(const String& ssid, MacAddress bssid, WifiDisconnectReason reason)
{
	Serial.println(_F("I'm NOT CONNECTED. Need help :("));

	// Start soft access point
	WifiAccessPoint.config(F("CONFIG ME PLEEEEASE..."), "", AUTH_OPEN);
	WifiAccessPoint.enable(true);

	// .. some you code for configuration ..
}

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	// Start send data loop
	procTimer.initializeMs<30 * 1000>(sendData).start(); // every 30 seconds
}

} // namespace

void init()
{
        Wire.begin();
	Serial.begin(SERIAL_BAUD_RATE);  // 115200 by default
	Serial.systemDebugOutput(false); // Disable debug output to serial

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

        if(!lox.begin(VL53L0X_I2C_ADDR, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE)) {
                Serial.println(_F("Failed to boot VL53L0X"));
                return;
        }


	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}

// --------------------------------------
// LM75 i2c temperature  (Sming version)
//


#include <SmingCore.h>
//#include <Wire.h>
#include <Temperature_LM75_Derived.h>

namespace
{
SimpleTimer procTimer;
// Change Address!
//Generic_LM75 temperature(0x4d);  	// generic LM75 0.5°C resolution (9bit)
NXP_LM75A temperature(0x4d);		// LM75A 0.125°C resolution (11bit)
}

void getTemp()
{
  Serial.print(temperature.readTemperatureC());
  Serial.println(" C");
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);  // 115200 by default
	Serial.systemDebugOutput(false); // Disable debug output

	Wire.begin();

	procTimer.initializeMs<3000>(getTemp).start();
}

#include <SmingCore.h>
#include <Libraries/Adafruit_ST7735/Adafruit_ST7735.h>
#include <Libraries/Adafruit_GFX/BMPDraw.h>
#include <Adafruit_GFX.h>
#include <Task.h>

/*
 * Hardware SPI mode:
 * GND      (GND)         GND
 * VCC      (VCC)         3.3v
 * D0       (CLK)         GPIO14
 * D1       (MOSI)        GPIO13
 * RES      (RESET)       GPIO16
 * DC       (DC)          GPIO0
 * CS       (CS)          GPIO2
 */
#define TFT_SCLK 14
#define TFT_MOSI 13
#define TFT_RST 16
#define TFT_DC 0
#define TFT_CS 2

namespace
{
//Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);



void initialise()
{
	// Use this initializer if you're using a 1.8" TFT
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

	// Use this initializer (uncomment) if you're using a 1.44" TFT
	//tft.initR(INITR_144GREENTAB); // initialize a ST7735S chip, black tab

	tft.setRotation(2);
	tft.fillScreen(ST7735_BLACK);
}


SimpleTimer linerTimer;


void liner()
{
	tft.drawLine(rand()%tft.width(), rand()%tft.height(),rand()%tft.width(), rand()%tft.height(),rand()%0xFFFF);
}

} // namespace

void init()
{
#ifdef ARCH_HOST
	setDigitalHooks(nullptr);
#endif

	spiffs_mount(); // Mount file system, in order to work with files

	Serial.begin(74880); // 115200 by default
	Serial.systemDebugOutput(false); // Allow debug output to serial

#ifndef DISABLE_WIFI
	//WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(false);
	WifiAccessPoint.enable(false);
#endif

	Serial.println(_F("Initialise display"));
	initialise();
	linerTimer.initializeMs<5>(liner).start();

}

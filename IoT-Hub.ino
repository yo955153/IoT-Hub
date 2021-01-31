#include <RFM69.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "serialSettings.h"
#include "pinDefs.h"
#include "lcdSettings.h"
#include "radioSettings.h"

char buff[61]; //61 max payload for radio

// Init Radio Object
RFM69 radio;

// Init LCD Object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
#ifdef SERIAL_EN
	Serial.begin(SERIAL_BAUD);
	delay(2000);
#endif

	// Debug
	pinMode(LED, OUTPUT);
	Blink(LED, 200);

	// RADIO
	if (!radio.initialize(FREQUENCY, HUBID, NETWORKID))
		DEBUGln("radio.init() FAIL");
	else
		DEBUGln("radio.init() SUCCESS");

	radio.setHighPower();
	radio.setCS(RFM69_CS);

	// LCD
	tft.begin();
	tft.fillScreen(ILI9341_BLACK);
	tft.setRotation(DEFAULT_ROTATION);
	tft.cp437(true);
	yield();

	// DEBUG
	tft.setCursor(0, 0);
	tft.setTextSize(3);
	tft.setTextColor(ILI9341_OLIVE);

	attachInterrupt(digitalPinToInterrupt(BUTTON_INT), pin_ISR, RISING);
}

int i = 0;
bool newData = false;

void loop()
{
	if (newData) // put your main code here, to run repeatedly:
	{
		DEBUGln(i);
		tft.println(i);
		newData = false;
	}

	if (radio.receiveDone())
	{
		// Message format
		// [Zone_ID, Data_Type, Value]
		for (byte i = 0; i < radio.DATALEN; i++)
		{
			tft.print((char)radio.DATA[i]);
			DEBUG((char)radio.DATA[i]);
		}

		tft.println();
		DEBUGln();

		if (radio.ACKRequested())
		{
			radio.sendACK();
			DEBUGln("ACK sent");
		}
	}
}

void pin_ISR()
{
	newData = true;
	i++;
}

// Blink an LED for a given number of ms
void Blink(byte PIN, int DELAY_MS)
{
	digitalWrite(PIN, HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN, LOW);
}
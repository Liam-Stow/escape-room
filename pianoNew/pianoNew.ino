/*
 Name:		piano.ino
 Created:	8/15/2020 3:18:55 PM
 Author:	ss
*/
#include "src/AnalogReader/AnalogReader.h"
#include "src/ButtonsArray.hpp"
#include "Pins.hpp"
#include "src/SimpleTimer/SimpleTimer.h"
#include "src/delay.h"
#include "pianoPlay.hpp"
#include "src/free_memory.h"

AnalogReader g_ar(14, A0, true, false);
uint8_t winKeySeq[6] = { 0, 4, 9, 0, 4, 9};

ButtonsArray g_pianoBttns(&g_ar, AR_PIANO_BTTN_START_INDEX, AR_PIANO_BTTN_END_INDEX, "keys", ButtonsArray::DELTA, ButtonsArray::PRESS, winKeySeq, 6);
PianoPlay g_pianoPlay;
SimpleTimer g_Timer;

void(*softReset)(void) = 0;

void handlers()
{
	g_pianoBttns.onWinHandler([]()
	{
		digitalWrite(D_PIANO_KEYS_POWER_INDEX, HIGH);
		g_Timer.setTimeout(150, []()
		{
			g_pianoPlay.startPlay();
		});
		g_Timer.setTimeout(35000, []()
		{
			digitalWrite(D_PIANO_KEYS_POWER_INDEX, LOW);
		});
		g_Timer.setTimeout(36000, []()
		{
			digitalWrite(D_LOCK_INDEX, LOW);
		});
	});
}

void setup() {
	Serial.begin(115200);
	Serial.println(F("########## Hello ##########"));

	pinMode(D_PIANO_KEYS_POWER_INDEX, OUTPUT);
	pinMode(D_LOCK_INDEX, OUTPUT);
	
	digitalWrite(D_PIANO_KEYS_POWER_INDEX, LOW);
	digitalWrite(D_LOCK_INDEX, HIGH);
	g_ar.calibrateAll();
	delay(500);
	g_ar.calibrateAll();
	g_pianoBttns.activate();

	g_Timer.setInterval(2000, []()
	{
		//freeRam();
		//g_ar.printAll();
	});
	handlers();
}

// the loop function runs over and over again until power down or reset
void loop() {
	g_Timer.run();
	g_pianoBttns.process();
	g_pianoPlay.process();
}

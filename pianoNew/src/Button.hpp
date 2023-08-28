#pragma once
#include "AnalogReader/AnalogReader.h"
#include "GameBaseObject.hpp"

class Button : public GameBaseObject
{

public:
	enum CheckType : uint8_t
	{
		HIGHER,
		LOWER,
		DELTA,
		DUP,
		DDOWN
	};
	
protected:
	
	AnalogReaderMultUtc4052_Quad* m_ar = nullptr;
	uint8_t m_pinNum;	
	CheckType m_checkType; //0 - higher than max, 1 - lower than min, 2 - calibrated higher than delta
	String m_bttnName;		
	uint64_t m_winDelayMs;
	
	int16_t m_sensitivityMin = 250;
	int16_t m_sensitivityMax = 750;
	int16_t m_sensitivityDelta = 50;
	
	uint64_t m_lastProcessMs = 0;
	uint32_t m_delayProcessMs = 50;

	uint64_t m_lastTriggeredMs = 0;
	uint32_t m_triggeredMs = 0;
	
	
	uint64_t m_lastDebounceMs = 0;
	uint32_t m_debounceDelayMs = 125;

	uint64_t m_lastWinTriggeredMs = 0;
	
	uint64_t m_lastOFFMs = 0;
	uint64_t m_lastONMs = 0;

	bool m_debounceOff = false;
	bool UseMultiplexer = false;
	
	bool lastState = false;
	bool state = false;
	bool pressed = false;
	
public:
	Button(AnalogReaderMultUtc4052_Quad* ar, uint8_t num, String name, CheckType type, uint64_t winDelay) : m_ar(ar), m_pinNum(num), m_bttnName(name), m_checkType(type), m_winDelayMs(winDelay)
	{
		UseMultiplexer = true;
	}

	Button(uint8_t num, String name, CheckType type, uint64_t winDelay) : m_pinNum(num), m_bttnName(name), m_checkType(type), m_winDelayMs(winDelay)
	{
		UseMultiplexer = false;
	}
	
	void reset() override
	{
		deactivate();
	}

	void deactivate() override
	{
		GameBaseObject::deactivate();
		setWin(false);
		Serial.print(F("Button "));
		Serial.print(m_pinNum);
		Serial.print(F(" "));
		Serial.print(m_bttnName);
		Serial.println(F(" deactivated"));
	}

	void activate() override
	{
		GameBaseObject::activate(); 
		Serial.print(F("Button "));
		Serial.print(m_pinNum);
		Serial.print(F(" "));
		Serial.print(m_bttnName);
		Serial.println(F(" activated"));
	}

	int16_t getSensorValue() const
	{
		if (UseMultiplexer)
		{
			m_ar->readAll();
			return (m_ar->get(m_pinNum));
		}
		
		if (m_checkType == DUP || m_checkType == DDOWN)
		{
			return digitalRead(m_pinNum);
		}
		
		return analogRead(m_pinNum);
	}

	bool process() override
	{
		if (isWin())
		{
			return true;
		}

		if (!isActive())
		{
			return false;
		}



		//	antiflood
		if (m_lastProcessMs == 0 || millis() - m_lastProcessMs >= m_delayProcessMs)
		{
			m_lastProcessMs = millis();
		}
		else {
			return false;
		}

		//	checking
		switch (m_checkType)
		{
			
			case HIGHER:	pressed = isPressedMax(); break;
			case LOWER:		pressed = isPressedMin(); break;
			case DELTA:		pressed = isPressedDelta(); break;
			case DUP:		pressed = !digitalRead(m_pinNum); break;
			case DDOWN:		pressed = digitalRead(m_pinNum); break;
		}

		//	debounce
		if (pressed != lastState)
		{
			m_lastDebounceMs = millis();
		}

		if (millis() - m_lastDebounceMs > m_debounceDelayMs || m_debounceOff)
		{
			if (pressed != state) {
				state = pressed;

				Serial.print(F("Button "));
				Serial.print(m_pinNum);
				Serial.print(F(" "));
				Serial.print(m_bttnName);

				if (pressed)
				{
					Serial.println(F(" pressed"));
				}
				else
				{
					Serial.println(F(" unpressed"));
				}
				callOnStateChangedCallback(lastState, false);
			}
			
		}
		
		lastState = pressed;
		

		//	checking win time
		if (m_winDelayMs>0)
		{
			if (pressed)
			{
				if (millis() - m_lastOFFMs >= m_winDelayMs)
				{
					callOnStateChangedCallback(-1, true);
					printState();
					setWin(true);
					return true;
				}
			} 
			else
			{
				m_lastOFFMs = millis();
			}
		}
		return false;
	}

	bool isPressedMax() const
	{
		return getSensorValue() > m_sensitivityMax;
	}
	
	bool isPressedMin() const
	{
		return getSensorValue() < m_sensitivityMin;
	}

	bool isPressedDelta() const
	{
		m_ar->readAll();
		return m_ar->getCalibrated(m_pinNum) > m_sensitivityDelta;
	}

	void setSensitivity(uint16_t sensitivity)
	{
		switch (m_checkType)
		{
			case HIGHER: m_sensitivityMax = sensitivity; break;
			case LOWER: m_sensitivityMin = sensitivity; break;
			case DELTA: m_sensitivityDelta = sensitivity; break;
		}
	}	

	void setOffDebounce(bool off = true)
	{
		m_debounceOff = off;
	}

	void printState() const
	{
		Serial.print(F("Button "));
		Serial.print(m_pinNum);
		Serial.print(F(" "));
		Serial.print(m_bttnName);
		Serial.print(F(" state:"));
		Serial.print(getSensorValue());
		Serial.println("");
	}

	void onWin() override
	{
		//Serial.println("Altar Win");
	}

};
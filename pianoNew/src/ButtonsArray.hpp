#pragma once
#include "AnalogReader/AnalogReader.h"
#include "GameBaseObject.hpp"

class ButtonsArray : public GameBaseObject
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

	enum SequenceType : uint8_t
	{
		HOLD,
		PRESS
	};
	
protected:
	
	AnalogReader* m_ar = nullptr;
	uint8_t m_pinStartNum;	
	uint8_t m_pinEndNum;	
	uint8_t m_pinNum = 0;	
	CheckType m_checkType; //0 - higher than max, 1 - lower than min, 2 - calibrated higher than delta
	SequenceType m_seqType; //0 - higher than max, 1 - lower than min, 2 - calibrated higher than delta
	String m_bttnName;		
	
	int16_t m_sensitivityMin = 250;
	int16_t m_sensitivityMax = 750;
	int16_t m_sensitivityDelta = 50;
	
	uint64_t m_lastProcessMs = 0;
	uint32_t m_delayProcessMs = 50;

	uint8_t m_correctButtonsCounter = 0;

	

	bool UseMultiplexer = false;
	
	bool *sensStateNew = nullptr;
	bool *sensState = nullptr;
	uint8_t *m_winSeq = nullptr;
	uint8_t winSeqLen = 0;

public:
	ButtonsArray(AnalogReader* ar, uint8_t numStart, uint8_t numEnd, String name, CheckType type, SequenceType seq, uint8_t* win, uint8_t len) : m_ar(ar), m_pinStartNum(numStart), m_pinEndNum(numEnd), m_bttnName(name), m_checkType(type), m_seqType(seq), m_winSeq(win), winSeqLen(len)
	{
		UseMultiplexer = true;
		m_pinNum = m_pinEndNum - m_pinStartNum + 1;
		sensStateNew = new bool[m_pinNum];
		sensState = new bool[m_pinNum];
		for (auto i = 0; i < m_pinNum; i++)
		{
			sensStateNew[i] = 0;
			sensState[i] = 0;
		}
	}

	ButtonsArray(uint8_t numStart, uint8_t numEnd, String name, CheckType type, SequenceType seq, uint8_t* win, uint8_t len) : m_pinStartNum(numStart), m_pinEndNum(numEnd), m_bttnName(name), m_checkType(type), m_seqType(seq), m_winSeq(win), winSeqLen(len)
	{
		UseMultiplexer = false;
		m_pinNum = m_pinEndNum - m_pinStartNum + 1;
		sensStateNew = new bool[m_pinNum];
		sensState = new bool[m_pinNum];
		for (auto i = 0; i < m_pinNum; i++)
		{
			sensStateNew[i] = 0;
			sensState[i] = 0;
		}
		
	}
	
	void reset() override
	{
		deactivate();
	}

	void deactivate() override
	{
		GameBaseObject::deactivate();
		setWin(false);
		Serial.print(m_bttnName);
		Serial.println(F(" buttons deactivated"));
	}

	void activate() override
	{
		GameBaseObject::activate(); 
		Serial.print(m_bttnName);
		Serial.println(F(" buttons activated"));
	}

	int16_t getSensorValue(uint8_t num) const
	{
		if (UseMultiplexer)
		{
			m_ar->readAllDeffered();
			return (m_ar->get(num));
		}
		
		if (m_checkType == DUP || m_checkType == DDOWN)
		{
			return digitalRead(num);
		}
		
		return analogRead(num);
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
		else 
		{
			return false;
		}
		
		//	check
		uint8_t index = 0;
		m_ar->readAll();
		//m_ar->printAll();

		for (int8_t i = 0; i < 14; i++)
		{
		/*Serial.print(m_ar->get(i));
		Serial.print("  ");
		Serial.print(m_ar->getCalibrated(i));
		Serial.print("  ");
		Serial.println(abs(m_ar->getCalibrated(i)) > m_sensitivityDelta);*/
			switch (m_checkType)
			{
				
				case HIGHER:	sensStateNew[index] = isPressedMax(i);  break;
				case LOWER:		sensStateNew[index] = isPressedMin(i); break;
				case DELTA:		sensStateNew[index] = isPressedDelta(i); break;
				case DUP:		sensStateNew[index] = !digitalRead(i); break;
				case DDOWN:		sensStateNew[index] = digitalRead(i); break;
			}
			index++;
		}
		
		//  react
		for (uint8_t i = 0; i < m_pinNum; i++)
		{
			if (sensStateNew[i] != sensState[i])
			{
				Serial.print(m_bttnName);
				if (sensStateNew[i])
				{
					Serial.print(F(" pressed "));
				}
				else
				{
					Serial.print(F(" unpressed "));
				}
				Serial.println(i);

				onStateChangedCallback(i , sensStateNew[i]);
			}
		}

		// check seq
		if (m_seqType == HOLD && m_winSeq)
		{
			index = 0;
			for (auto i = 0; i < m_pinNum; i++)
			{
				if (sensStateNew[i] == m_winSeq[i]) index++;
			}
			
			if (index != m_correctButtonsCounter)
			{
				m_correctButtonsCounter = index;
				Serial.print(m_bttnName);
				Serial.print(F(" correct: "));
				Serial.println(m_correctButtonsCounter);
			}

			if (m_correctButtonsCounter == m_pinNum )
			{
				onWin();
				return true;
			}
		}
		
		// check seq
		if (m_seqType == PRESS && m_winSeq)
		{
			for (uint8_t i = 0; i < m_pinNum; i++)
			{
				if (!sensStateNew[i] && sensState[i])
				{
					if (m_winSeq[m_correctButtonsCounter] == i)
					{
						m_correctButtonsCounter++;
						if (m_correctButtonsCounter == winSeqLen)
						{
							onWin();
							return true;
						}
					}
					else
					{
						if (m_winSeq[m_correctButtonsCounter - 1] == i && m_correctButtonsCounter)
						{
							continue;
						}
						if (m_winSeq[0] == i) {
							m_correctButtonsCounter = 1;
						}
						else
						{
							m_correctButtonsCounter = 0;
						}
					}
					Serial.print(F("Correct: "));
					Serial.println(m_correctButtonsCounter);
				}			
			}
		}		

		// update states
		for (uint8_t i = 0; i < m_pinNum; i++)
		{
			sensState[i] = sensStateNew[i];
		}

		return false;
	}

	bool isPressedMax(uint8_t num) const
	{
		return getSensorValue(num) > m_sensitivityMax;
	}
	
	bool isPressedMin(uint8_t num) const
	{
		return getSensorValue(num) < m_sensitivityMin;
	}

	bool isPressedDelta(uint8_t num) const
	{
	//	m_ar->readAll();

		return abs(m_ar->getCalibrated(num)) > m_sensitivityDelta;
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

	void printStates() const
	{   Serial.print(m_bttnName);
		Serial.println(F(" buttons state:"));
		uint8_t index = 0;
		for (uint8_t i = m_pinStartNum; i <= m_pinEndNum; i++)
		{
			Serial.print(index++); Serial.print(":: "); Serial.println(getSensorValue(i));
		}
		Serial.println("");
	}

	void onWin() override
	{
		setWin();
		onWinCallback();
		Serial.print(m_bttnName);
		Serial.println(F(" buttons Win"));
		//printStates();
	}

};
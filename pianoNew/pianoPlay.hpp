#pragma once
#include "src/GameBaseObject.hpp"


class PianoPlay
{
protected:	
	//enum keys { A3, B3, C4, D4, E4, F4, G4, A4, B4, C5, D5, E5, F5, G5, keyRight };
	//            +   +       +   +           +   +   +   +   +   +   +
	enum keys {   A3, B3,     D4, E4,         A4, B4, C5, D5, E5, F5, G5, keyRight };
	const static uint16_t halfDelay = 500;
	const static uint16_t quarterDelay = 250;


	struct Note
	{
		int8_t note1;
		int8_t note2;
		uint16_t delay;
	};

	static const Note m_noteMelody[63] PROGMEM;

	const uint8_t m_pianoKeysSequence[12] = { 0, 4, 9, 0, 4, 9, 1, 4, 10, 1, 4, 10 };
	const uint8_t m_melodySize = 63; // sizeof in constructor initializer // sizeof(m_noteMelody)/m_noteMelody(Note)
	
	int8_t m_currPlayNotePos = -1; // disable playing (NonBlockingPlay)
	int16_t m_prevPlayNote1 = -1; // prev note1 pin for disabing (NonBlockingPlay)
	int16_t m_prevPlayNote2 = -1; // prev note2 pin for disabing (NonBlockingPlay)
	unsigned long m_playNoteNextTimeMs = 0; // delay for note (NonBlockingPlay)

	unsigned long m_lastProcessMs = 0; // для ограничение flood'a событий
	uint8_t m_delayProcessMs = 50; // 50 ms задержка перед считыванием analogRead
	int16_t sensitivity = 150;
	//void(*m_onStateChangeCallback)(int16_t countWinElements, bool isWin) = nullptr;
	void(*m_ptrOnFinishPlayMelodyCallback)(bool) = nullptr;
	
	void keyPrevUnpress()
	{
		// отжимаем предыдущие кнопки
		if (m_prevPlayNote1 >= 0)
		{
			digitalWrite(D_PIANO_KEYS_INDEX[m_prevPlayNote1], 0);
			delay_ms(10);
			digitalWrite(D_PIANO_KEYS_INDEX[m_prevPlayNote1], 0);
		}
			

		if (m_prevPlayNote2 >= 0)
		{
			digitalWrite(D_PIANO_KEYS_INDEX[m_prevPlayNote2], 0);
			delay_ms(10);
			digitalWrite(D_PIANO_KEYS_INDEX[m_prevPlayNote2], 0);
		}
			

		//Serial.println("Piano unpress notes: " + String(m_prevPlayNote1) + "," + String(m_prevPlayNote2));
		m_prevPlayNote1 = -1;
		m_prevPlayNote2 = -1;

	}
	// возвращает true если можно проигрывать след ноту
	bool keyPressNonBlocking(int16_t note1, int16_t note2, uint16_t noteDelay)
	{	
		if (m_playNoteNextTimeMs == 0 || m_playNoteNextTimeMs <= millis()) {
			// отжимаем предыдущие кнопки
			keyPrevUnpress();

			Serial.println("Piano play notes: " + String(note1) + "," + String(note2) + ", delay: " + String(noteDelay));

			m_prevPlayNote1 = note1;
			m_prevPlayNote2 = note2;

			digitalWrite(D_PIANO_KEYS_INDEX[note1], 1);
			if (note2 >= 0) {
				digitalWrite(D_PIANO_KEYS_INDEX[note2], 1);
			}
			m_playNoteNextTimeMs = millis() + noteDelay;
			return true;
		}
		return false;
	}


public:
	PianoPlay()
	{	
		for (auto i =0; i < 12; i++)
		{
			pinMode(D_PIANO_KEYS_INDEX[i], OUTPUT);
			digitalWrite(D_PIANO_KEYS_INDEX[i], LOW);
		}
	}

	bool isNeedPlay() const
	{
		return m_currPlayNotePos >= 0 && m_currPlayNotePos <= m_melodySize;
	}

	void play4Notes() const
	{
		//powerOn();
		Serial.println("@Bam");
		digitalWrite(A3, 1);
		digitalWrite(F5, 1);
		
		
		delay_ms(1000);
		digitalWrite(A3, 0);
		digitalWrite(F5, 0);

		delay_ms(1000);
		digitalWrite(A3, 0);
		digitalWrite(F5, 0);
		
	}

	void testPlay() const
	{
		//enum keys { A3, B3, D4, E4, A4, B4, C5, D5, E5, F5, G5, keyRight, powerL, powerN, keyRear, NA };
	
		Serial.println("testplay");
		
		digitalWrite(A3, 1);
		delay_ms(1000);
		digitalWrite(A3, 0);

		digitalWrite(B3, 1);
		delay_ms(1000);
		digitalWrite(B3, 0);



		digitalWrite(D4, 1);
		delay_ms(1000);
		digitalWrite(D4, 0);
		
		digitalWrite(E4, 1);
		delay_ms(1000);
		digitalWrite(E4, 0);


		digitalWrite(A4, 0);
		delay_ms(1000);
		digitalWrite(A4, 1);
		
		digitalWrite(B4, 1);
		delay_ms(1000);
		digitalWrite(B4, 0);

		digitalWrite(C5, 1);
		delay_ms(1000);
		digitalWrite(C5, 0);

		digitalWrite(D5, 1);
		delay_ms(1000);
		digitalWrite(D5, 0);

		digitalWrite(E5, 1);
		delay_ms(1000);
		digitalWrite(E5, 0);

		digitalWrite(F5, 1);
		delay_ms(1000);
		digitalWrite(F5, 0);

		digitalWrite(G5, 1);
		delay_ms(1000);
		digitalWrite(G5, 0);
	}
	
	bool process() 
	{

		// в этом блоке играем мелодию
		if (isNeedPlay())
		{
			processPlayMelodyNonBlocked(); // проверка проигрывания ноты
			return true;
		}
		return false;
	}

	void startPlay()
	{
		Serial.println(F("@Piano start play"));
		m_prevPlayNote1 = -1;
		m_prevPlayNote2 = -1;
		m_playNoteNextTimeMs = 0;
		m_currPlayNotePos = 0;
	}
	void stopPlay()
	{
		m_currPlayNotePos = -1;

	}

	void printMelody(int start=0, int end=63)
	{
		Note note;
		
		for (auto i = start; i < end; i++)
		{
			note = getNoteFromMelodyByIdx(i);
			Serial.println("Notes: " + String(note.note1) + "," + String(note.note2) + "," + String(note.delay));
		}
	}
	
	Note getNoteFromMelodyByIdx(uint8_t index)
	{
		Note note;
		memcpy_P(&note, &m_noteMelody[index], sizeof(Note));
		return note;
	}

	void processPlayMelodyNonBlocked()
	{
	
		
		if (isNeedPlay() && m_currPlayNotePos < m_melodySize) {
			
			Note note = getNoteFromMelodyByIdx(m_currPlayNotePos);

			if (keyPressNonBlocking(note.note1, note.note2, note.delay))
			{
				m_currPlayNotePos++; // след нота
				callOnFinishPlayMelody(false);
			}
		} 
		else
		{
			// отжимаем последнюю кнопку и выключаем воспроизведение
			if (m_playNoteNextTimeMs == 0 || m_playNoteNextTimeMs <= millis()) {
				m_currPlayNotePos = -1; 
				keyPrevUnpress();
				callOnFinishPlayMelody(true);
			}
		}
	}

	void onFinishPlayMelody(void(*ptrFinishFunction)(bool))
	{

		m_ptrOnFinishPlayMelodyCallback = ptrFinishFunction;
	}
	void callOnFinishPlayMelody(bool status)
	{
		if (m_ptrOnFinishPlayMelodyCallback != nullptr)
		{
			m_ptrOnFinishPlayMelodyCallback(status);
		}
	}

	

};


// мелодия в PGM
const PianoPlay::Note PianoPlay::m_noteMelody[63] PROGMEM =
{

	// First quadrante
	{ A3, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ C5, -1, halfDelay },
	// Second quadrante
	{ A3, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ C5, -1, halfDelay },
	// Third quadrante
	{ B3, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ D5, -1, halfDelay },
	// Fourth quadrante
	{ B3, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ D5, -1, halfDelay },
	// Second string
	// First quadrante
	{ A3, E5, quarterDelay },
	{ D5, -1, quarterDelay },
	{ E4, C5, quarterDelay },
	{ D5, -1, quarterDelay },
	{ A4, E5, quarterDelay },
	{ F5, -1, quarterDelay },
	// Second quadrante
	{ A3, E5, halfDelay },
	{ E4, -1, halfDelay },
	{ A4, C5, halfDelay },
	// Third quadrante
	{ A3, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ A4, C5, halfDelay },
	// Fourth quadrante
	{ A3, E5, quarterDelay },
	{ D5, -1, quarterDelay },
	{ E4, C5, quarterDelay },
	{ D5, -1, quarterDelay },
	{ A4, E5, quarterDelay },
	{ F5, -1, quarterDelay },
	// Third string
	// First quadrante
	{ D4, D5, halfDelay },
	{ A4, -1, halfDelay },
	{ D5, F5, halfDelay },
	// Second quadrante
	{ D4, -1, halfDelay },
	{ A4, -1, halfDelay },
	{ D5, F5, halfDelay },
	// Third quadrante
	{ D4, D5, quarterDelay },
	{ C5, -1, quarterDelay },
	{ A4, B4, quarterDelay },
	{ C5, -1, quarterDelay },
	{ D5, -1, quarterDelay },
	{ E5, -1, quarterDelay },
	// Fourth quadrante
	{ D4, D5, halfDelay },
	{ A4, -1, halfDelay },
	{ E5, -1, halfDelay },
	// Fourth string 
	// First quadrante
	{ D4, D5, halfDelay },
	{ A4, -1, halfDelay },
	{ E5, -1, halfDelay },
	// Second quadrante
	{ D4, F5, halfDelay },
	{ D5, -1, halfDelay },
	{ G5, -1, halfDelay },
	// Third quadrante
	{ A4, E5, halfDelay },
	{ E4, -1, halfDelay },
	{ A4, C5, halfDelay },
	// Fourth quadrante
	{ A4, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ A4, C5, halfDelay },
	// Fifth quadrante
	{ A4, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ A4, C5, halfDelay },
	{ A4, -1, halfDelay },
	{ E4, -1, halfDelay },
	{ A4, C5, halfDelay }
};

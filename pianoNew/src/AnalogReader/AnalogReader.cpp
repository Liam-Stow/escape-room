/*
Name:		AnalogReader.hpp
Desc:		Классы для чтения аналоговых значений, через гололый analogRead с возможностью использования мультиплексоров, одного или двух.
			Содержит функционал калибровочных данных getCalibrated() и предыдущих значений для вычисления изменений значений getDelta()

Created:	1/05/2016 2:44:50 PM
Author:	netdog@tut.by

Edited: 13/12/2016 by starij

ChangeLog: добавлена конструкция AnalogReaderMultUtc4052_Quad вместо AnalogReaderMultUtc4052_Double

*/
#include "AnalogReader.h"
AnalogReader::~AnalogReader()
{
	delete[] m_analogData;
	delete[] m_analogDataCalibrate;
	delete[] m_analogDataOld;
	delete[] m_remapIndexes;
}

AnalogReaderBool::~AnalogReaderBool()
{
	delete[] m_analogData;
	delete[] m_remapIndexes;
}

///////////////////////////////////////////////////////////
AnalogReader::AnalogReader(uint8_t size, uint8_t analogPinStart, bool useCalibateArray, bool useDeltaArray):m_size(size), m_analogPinStart(analogPinStart), m_bUseCalibrate(useCalibateArray), m_bUseDelta(useDeltaArray)
{
	m_analogData = new int16_t[size];
	memset(m_analogData, 0xff, sizeof(int16_t) * size);

	if (m_bUseCalibrate) {
		m_analogDataCalibrate = new int16_t[size];
		memset(m_analogDataCalibrate, 0, sizeof(int16_t) * size);
	}

	if (m_bUseDelta) {
		m_analogDataOld = new int16_t[size];
		memset(m_analogDataOld, 0, sizeof(int16_t) * size);
	}
}

void AnalogReader::_delay() const
{
	::delay(1);
}
// если много раз подряд вызывается в разных классах readAll, 
// то тики тратятся впустую создавая некоторые задержки, 
// можно игнорировать новое чтение некоторое время читая сенсоры этой функцией
void AnalogReader::readAllDeffered(uint32_t interval)
{
	if (getLastUpdateMs() + interval <= millis())
	{
		readAll();
	}
}

void AnalogReader::readAll()
{
	for (uint8_t i = 0; i < m_size; i++)
	{
		auto analogData = analogRead(m_analogPinStart + i);
		if (m_bUseDelta)
			m_analogDataOld[i] = m_analogData[i];
		
		m_analogData[i] = analogData;
	}
	m_lastReadMs = millis();	
}

int16_t AnalogReader::getCalibrated(uint8_t index) const
{
	
	if (m_bUseCalibrate)
	{
		return  (m_analogData[index] - m_analogDataCalibrate[index]);
	}
	return 0x7fff;
}

int16_t AnalogReader::getCalibratedM(uint8_t index) const
{
	if (m_remapIndexes == nullptr)
	{
		return 0x7fff;
	}
	return getCalibrated(m_remapIndexes[index]);
}

void AnalogReader::calibrateAll(int16_t* calbratedValues, uint8_t size)
{
	if (!m_bUseCalibrate)
		return;

	if (size > m_size) size = m_size;
	for (uint8_t i = 0; i < size; i++)
	{
		m_analogDataCalibrate[i] = calbratedValues[i];
		if (calbratedValues[i] < 0 || calbratedValues[i] > 1023)
		{
			Serial.println(F("ERROR: Invalid custom calibration value"));
		}
	}
}

void AnalogReader::calibrateAll()
{

	if (!m_bUseCalibrate)
		return;
	readAll();

	for (uint8_t i = 0; i < m_size; i++)
	{
		m_analogDataCalibrate[i] = m_analogData[i];
	}
}

uint8_t AnalogReader::getSize() const
{
	return m_size;
}

int16_t AnalogReader::get(uint8_t index) const
{
	if (index > m_size - 1)
	{
		return 0xffff;
	}

	return m_analogData[index];
}

uint32_t AnalogReader::getLastUpdateMs() const
{
	
	return m_lastReadMs;
}

void AnalogReader::printAll() const
{
	printAll(Serial);
}

void AnalogReader::print(uint8_t index) const
{
	print(Serial, index);
}

void AnalogReader::printAll(HardwareSerial &serial) const
{
	char buffer[7];
	for (uint8_t i = 0; i < m_size; i++) {
		snprintf(buffer, sizeof(buffer), "%02d", i);
		serial.print(buffer);
		serial.print(". Raw:");
		snprintf(buffer, sizeof(buffer), "%4d", get(i));
		serial.print(buffer);
		serial.print("  ");

		if (m_bUseCalibrate) {
			snprintf(buffer, sizeof(buffer), "%4d", getCalibrated(i));
			serial.print("Cal:");
			serial.print(buffer);
			serial.print("  ");
		}

		if (m_bUseDelta) {
			snprintf(buffer, sizeof(buffer), "%4d", getDelta(i));
			serial.print("Delta:");
			serial.print(buffer);
		}
		serial.println();
	}
	serial.println();
}

void AnalogReader::print(HardwareSerial &serial, uint8_t index) const
{
	char buffer[7]; // не переполнять by sprintf

	//sprintf(buffer, "%4d", get(index));
	snprintf(buffer, sizeof(buffer), "%4d", get(index));
	serial.print(index);
	serial.print(". Raw:");
	serial.print(get(index));
	serial.print("  ");

	if (m_bUseCalibrate) {
		snprintf(buffer, sizeof(buffer), "%4d", getCalibrated(index));
		serial.print("Cal:");
		serial.print(buffer);
		serial.print("  ");
	}

	if (m_bUseDelta) {
		snprintf(buffer, sizeof(buffer), "%4d", getDelta(index));
		serial.print("Delta:");
		serial.print(buffer);
	}
	serial.println();
}

void AnalogReader::remap(const uint8_t* remapIndexes, uint8_t size)
{
	if (m_size != size)
	{
		Serial.println(F("Remap array size != AnalogReader.m_size"));
		return;
	}

	if (m_remapIndexes == nullptr)
	{
		m_remapIndexes = new uint8_t[size];
	}
	memcpy(m_remapIndexes, remapIndexes, size * 1);

}

int16_t AnalogReader::getDelta(uint8_t index) const
{
	if (index > m_size - 1)
	{
		return 0xffff;
	}

	if (m_bUseDelta)
	{
		return m_analogData[index] - m_analogDataOld[index];
	}
	return 0xffff;
}

int16_t AnalogReader::getDeltaM(uint8_t index) const
{
	if (m_remapIndexes == nullptr)
	{
		return 0xffff;
	}
	return getDelta(m_remapIndexes[index]);
}


///////////////////////////////////////////////////
AnalogReaderBool::AnalogReaderBool(uint8_t size, uint8_t analogPinStart) :m_size(size), m_analogPinStart(analogPinStart)
{
	m_analogData = new bool[size];
	memset(m_analogData, 0xff, sizeof(bool) * size);
}

void AnalogReaderBool::_delay() const
{
	::delay(1);
}

void AnalogReaderBool::readAllDeffered(uint32_t interval)
{
	if (getLastUpdateMs() + interval <= millis())
	{
		readAll();
	}
}

void AnalogReaderBool::readAll()
{
	for (uint8_t i = 0; i < m_size; i++)
	{
		auto analogData = digitalRead(m_analogPinStart + i);
		m_analogData[i] = analogData;
	}
	m_lastReadMs = millis();
}

uint8_t AnalogReaderBool::getSize() const
{
	return m_size;
}

bool AnalogReaderBool::get(uint8_t index) const
{
	if (index > m_size - 1)
	{
		return 0xffff;
	}

	return m_analogData[index];
}

uint32_t AnalogReaderBool::getLastUpdateMs() const
{

	return m_lastReadMs;
}

void AnalogReaderBool::printAll() const
{
	printAll(Serial);
}

void AnalogReaderBool::print(uint8_t index) const
{
	print(Serial, index);
}

void AnalogReaderBool::printAll(HardwareSerial &serial) const
{
	char buffer[7];
	for (uint8_t i = 0; i < m_size; i++) {
		snprintf(buffer, sizeof(buffer), "%02d", i);
		serial.print(buffer);
		serial.print(". Raw:");
		snprintf(buffer, sizeof(buffer), "%4d", get(i));
		serial.print(buffer);
		serial.print("  ");
		serial.println();
	}
	serial.println();
}

void AnalogReaderBool::print(HardwareSerial &serial, uint8_t index) const
{
	char buffer[7]; // не переполнять by sprintf

					//sprintf(buffer, "%4d", get(index));
	snprintf(buffer, sizeof(buffer), "%4d", get(index));
	serial.print(index);
	serial.print(". Raw:");
	serial.print(get(index));
	serial.print("  ");
	serial.println();
}

////////////////////////////////////////////////////////
AnalogReaderMultUtc4052::AnalogReaderMultUtc4052(uint8_t size, uint8_t pinMainA, uint8_t pinMainB, uint8_t analogPinX, uint8_t analogPinY, bool useCalibateArray, bool useDeltaArray) 
: AnalogReader(size, 0, useCalibateArray, useDeltaArray), 
m_pinMainA(pinMainA), m_pinMainB(pinMainB), m_analogPinX(analogPinX), m_analogPinY(analogPinY)
{
	pinMode(m_pinMainA, OUTPUT);
	pinMode(m_pinMainB, OUTPUT);

	digitalWrite(m_pinMainA, LOW);
	digitalWrite(m_pinMainB, LOW);
}

void AnalogReaderMultUtc4052::readAll()
{
	uint8_t k = 0;

	
	if (m_size == 0)
		return;
	
	for (uint8_t i = 0; i < 4; i++)
	{

		digitalWrite(m_pinMainA, bitRead(i, 0));
		digitalWrite(m_pinMainB, bitRead(i, 1));
		
		if (m_bUseDelta) {
			m_analogDataOld[k] = m_analogData[k];
		}

		// @HINT: без задержки может быть проблема (Столкнулись с Dolls)
		// @TODO: проверить еще без FAST ADC
		delayMicroseconds(16);
		m_analogData[k] = analogRead(m_analogPinX);
		if (++k >= m_size)
			break;

		if (m_bUseDelta) {
			m_analogDataOld[k] = m_analogData[k];
		}

		m_analogData[k] = analogRead(m_analogPinY);

		if (++k >= m_size)
			break;

	}
	m_lastReadMs = millis();
}

////////////////////////////////////////////////////////
AnalogReaderMultUtc4052Bool::AnalogReaderMultUtc4052Bool(uint8_t size, uint8_t pinMainA, uint8_t pinMainB, uint8_t analogPinX, uint8_t analogPinY)
	: AnalogReaderBool(size, 0),
	m_pinMainA(pinMainA), m_pinMainB(pinMainB), m_analogPinX(analogPinX), m_analogPinY(analogPinY)
{
	pinMode(m_pinMainA, OUTPUT);
	pinMode(m_pinMainB, OUTPUT);

	digitalWrite(m_pinMainA, LOW);
	digitalWrite(m_pinMainB, LOW);
}

void AnalogReaderMultUtc4052Bool::readAll()
{
	uint8_t k = 0;

	if (m_size == 0)
		return;

	for (uint8_t i = 0; i < 4; i++)
	{
		digitalWrite(m_pinMainA, bitRead(i, 0));
		digitalWrite(m_pinMainB, bitRead(i, 1));

		// @HINT: без задержки может быть проблема (Столкнулись с Dolls)
		// @TODO: проверить еще без FAST ADC
		delayMicroseconds(16);
		m_analogData[k] = digitalRead(m_analogPinX);
		if (++k >= m_size)
			break;

		m_analogData[k] = digitalRead(m_analogPinY);

		if (++k >= m_size)
			break;
	}
	m_lastReadMs = millis();
}

////////////////////////////////////////////////////////
AnalogReaderMultUtc4052_Quad::AnalogReaderMultUtc4052_Quad(uint8_t size, uint8_t sensIn0, uint8_t sensIn1, uint8_t sensIn2, uint8_t sensIn3, uint8_t pinMainA, uint8_t pinMainB, uint8_t pinA, uint8_t pinB, uint8_t analogPinX, uint8_t analogPinY, bool useCalibateArray, bool useDeltaArray) :
	AnalogReaderMultUtc4052(size, pinMainA, pinMainB, analogPinX, analogPinY, useCalibateArray, useDeltaArray),
	m_pinA(pinA), m_pinB(pinB), m_sensIn0(sensIn0), m_sensIn1(sensIn1), m_sensIn2(sensIn2), m_sensIn3(sensIn3)
{
	pinMode(m_pinA, OUTPUT);
	pinMode(m_pinB, OUTPUT);

	digitalWrite(m_pinA, LOW);
	digitalWrite(m_pinB, LOW);
}

void AnalogReaderMultUtc4052_Quad::readAll()
{

	uint8_t size[4] = { m_sensIn0, m_sensIn1, m_sensIn2, m_sensIn3 };
	uint8_t m_sensInMain = m_size - (m_sensIn0 + m_sensIn1 + m_sensIn2 + m_sensIn3);
	if (m_size == 0)
	{
		Serial.println(F("Sens must be not zero"));
		return;
	}
	for (auto i = 0; i < 4; i++)
	{
		if (size[i] > 8)
		{
			Serial.print(F("Wrong sensors sum in "));
			Serial.print(i);
			Serial.println(F(" mult"));
			return;
		}

	}

	if (m_sensInMain) {
		auto sensMainMax = 0;
		for (auto i = 0; i < 4; i++) {
			if (size[i] < 2)
				sensMainMax++;
			if (size[i] < 1)
				sensMainMax++;
		}
		if (sensMainMax < m_sensInMain) {
			Serial.println(F("wrong sensors num in main mult"));
			return;
		}
	}

	uint8_t sensorsReadAllIterator = 0;
	for (uint8_t i = 0; i < 4; i++)
	{

		digitalWrite(m_pinMainA, bitRead(i, 0));
		digitalWrite(m_pinMainB, bitRead(i, 1));
		delayMicroseconds(32);
		uint8_t sensorsReadMult = 0;
		for (uint8_t j = 0; j < 4; j++) {
			if (size[i] == 0)
			{
				break;
			}
			digitalWrite(m_pinA, bitRead(j, 0));
			digitalWrite(m_pinB, bitRead(j, 1));
			delayMicroseconds(32);
			if (m_bUseDelta) {
				m_analogDataOld[sensorsReadAllIterator] = m_analogData[sensorsReadAllIterator];
			}

			m_analogData[sensorsReadAllIterator] = analogRead(m_analogPinX);
			/*Serial.print(F("X:"));
			Serial.print(F("readall:"));
			Serial.print(sensorsReadAll);
			Serial.print(F("  val:"));
			Serial.println(m_analogData[sensorsReadAll]);
			Serial.print(F("mainA:"));
			Serial.print(digitalRead(m_pinMainA));
			Serial.print(F("  mainB:"));
			Serial.print(digitalRead(m_pinMainB));
			Serial.print(F("    A:"));
			Serial.print(digitalRead(m_pinA));
			Serial.print(F("    B:"));
			Serial.println(digitalRead(m_pinB));*/
			sensorsReadMult++;
			sensorsReadAllIterator++;


			if (sensorsReadAllIterator == m_size) {
				m_lastReadMs = millis();
				return;
			}
			if (sensorsReadMult == size[i])
			{
				break;
			}


			if (m_bUseDelta) {
				m_analogDataOld[sensorsReadAllIterator] = m_analogData[sensorsReadAllIterator];
			}

			m_analogData[sensorsReadAllIterator] = analogRead(m_analogPinY);
			/*Serial.print(F("Y:"));
			Serial.print(F("readall:"));
			Serial.print(sensorsReadAll);
			Serial.print(F("  val:"));
			Serial.println(m_analogData[sensorsReadAll]);
			Serial.print(F("mainA:"));
			Serial.print(digitalRead(m_pinMainA));
			Serial.print(F("  mainB:"));
			Serial.print(digitalRead(m_pinMainB));
			Serial.print(F("    :"));
			Serial.print(digitalRead(m_pinA));
			Serial.print(F("    :"));
			Serial.println(digitalRead(m_pinB));*/
			sensorsReadMult++;
			sensorsReadAllIterator++;


			if (sensorsReadAllIterator == m_size) {
				m_lastReadMs = millis();
				return;
			}
			if (sensorsReadMult == size[i])
			{
				break;
			}

		}
	}

	if (m_sensInMain) {

		uint8_t sensorsReadMultMain = 0;
		for (auto i = 0; i < 4; i++)
		{
			if (size[i] == 0)
			{
				digitalWrite(m_pinMainA, bitRead(i, 0));
				digitalWrite(m_pinMainB, bitRead(i, 1));
				delayMicroseconds(32);

				if (m_bUseDelta) {
					m_analogDataOld[sensorsReadAllIterator] = m_analogData[sensorsReadAllIterator];
				}

				m_analogData[sensorsReadAllIterator] = analogRead(m_analogPinX);
				/*Serial.print(F("readall:"));
				Serial.print(sensorsReadAll);
				Serial.print(F("  val:"));
				Serial.println(m_analogData[sensorsReadAll]);
				Serial.print(F("mainA:"));
				Serial.print(digitalRead(m_pinMainA));
				Serial.print(F("  mainB:"));
				Serial.print(digitalRead(m_pinMainB));
				Serial.print(F("    :"));
				Serial.print(digitalRead(m_pinA));
				Serial.print(F("    :"));
				Serial.println(digitalRead(m_pinB));*/
				sensorsReadMultMain++;
				sensorsReadAllIterator++;

				if (sensorsReadAllIterator == m_size) {
					m_lastReadMs = millis();
					return;
				}
				if (sensorsReadMultMain == m_sensInMain)
					break;

				if (m_bUseDelta) {
					m_analogDataOld[sensorsReadAllIterator] = m_analogData[sensorsReadAllIterator];
				}
				m_analogData[sensorsReadAllIterator] = analogRead(m_analogPinY);

				/*Serial.print(F("readall:"));
				Serial.print(sensorsReadAll);
				Serial.print(F("  val:"));
				Serial.println(m_analogData[sensorsReadAll]);
				Serial.print(F("mainA:"));
				Serial.print(digitalRead(m_pinMainA));
				Serial.print(F("  mainB:"));
				Serial.print(digitalRead(m_pinMainB));
				Serial.print(F("    :"));
				Serial.print(digitalRead(m_pinA));
				Serial.print(F("    :"));
				Serial.println(digitalRead(m_pinB));*/
				sensorsReadMultMain++;
				sensorsReadAllIterator++;


				if (sensorsReadAllIterator == m_size) {
					m_lastReadMs = millis();
					return;
				}
				if (sensorsReadMultMain == m_sensInMain)
					break;

			}
		}
	}




	m_lastReadMs = millis();
}

////////////////////////////////////////////////////////
AnalogReaderMultUtc4052_QuadBool::AnalogReaderMultUtc4052_QuadBool(uint8_t size, uint8_t sensIn0, uint8_t sensIn1, uint8_t sensIn2, uint8_t sensIn3,  uint8_t pinMainA, uint8_t pinMainB, uint8_t pinA, uint8_t pinB, uint8_t analogPinX, uint8_t analogPinY):
	AnalogReaderMultUtc4052Bool(size, pinMainA, pinMainB, analogPinX, analogPinY),
	 m_pinA(pinA), m_pinB(pinB), m_sensIn0(sensIn0), m_sensIn1(sensIn1), m_sensIn2(sensIn2), m_sensIn3(sensIn3)
{
	pinMode(m_pinA, OUTPUT);
	pinMode(m_pinB, OUTPUT);

	digitalWrite(m_pinA, LOW);
	digitalWrite(m_pinB, LOW);
}

void AnalogReaderMultUtc4052_QuadBool::readAll()
{
	
	uint8_t size[4] = {m_sensIn0, m_sensIn1, m_sensIn2, m_sensIn3};
	uint8_t m_sensInMain = m_size - (m_sensIn0 + m_sensIn1 + m_sensIn2 + m_sensIn3);
	if (m_size == 0)
	{
		Serial.println(F("Sens must be not zero"));
		return;
	}
	for (auto i = 0; i < 4; i++)
	{
		if (size[i] > 8)
		{
			Serial.print(F("Wrong sensors sum in "));
			Serial.print(i);
			Serial.println(F(" mult"));
			return;
		}

	}
	
	if (m_sensInMain) {
		auto sensMainMax = 0;
		for (auto i = 0; i < 4; i++){
			if (size[i] < 2)
				sensMainMax++;
			if (size[i] < 1)
				sensMainMax++;
		}
		if (sensMainMax < m_sensInMain){
			Serial.println(F("wrong sensors num in main mult"));
			return;
		}
	}

	uint8_t sensorsReadAllIterator = 0;
	for (uint8_t i = 0; i < 4; i++)
	{

		digitalWrite(m_pinMainA, bitRead(i, 0));
		digitalWrite(m_pinMainB, bitRead(i, 1));
		delayMicroseconds(32);
		uint8_t sensorsReadMult = 0;
		for (uint8_t j = 0; j < 4; j++) {
			if (size[i] == 0)
			{
				break;
			}
			digitalWrite(m_pinA, bitRead(j, 0));
			digitalWrite(m_pinB, bitRead(j, 1));
			delayMicroseconds(32);
			
			m_analogData[sensorsReadAllIterator] = digitalRead(m_analogPinX);
			/*Serial.print(F("X:"));
			Serial.print(F("readall:"));
			Serial.print(sensorsReadAll);
			Serial.print(F("  val:"));
			Serial.println(m_analogData[sensorsReadAll]);
			Serial.print(F("mainA:"));
			Serial.print(digitalRead(m_pinMainA));
			Serial.print(F("  mainB:"));
			Serial.print(digitalRead(m_pinMainB));
			Serial.print(F("    A:"));
			Serial.print(digitalRead(m_pinA));
			Serial.print(F("    B:"));
			Serial.println(digitalRead(m_pinB));*/
			sensorsReadMult++;
			sensorsReadAllIterator++;
			
			
			if (sensorsReadAllIterator == m_size) {
				m_lastReadMs = millis();
				return;
			}
			if (sensorsReadMult == size[i])
			{
				break;
			}

			m_analogData[sensorsReadAllIterator] = digitalRead(m_analogPinY);
			/*Serial.print(F("Y:"));
			Serial.print(F("readall:"));
			Serial.print(sensorsReadAll);
			Serial.print(F("  val:"));
			Serial.println(m_analogData[sensorsReadAll]);
			Serial.print(F("mainA:"));
			Serial.print(digitalRead(m_pinMainA));
			Serial.print(F("  mainB:"));
			Serial.print(digitalRead(m_pinMainB));
			Serial.print(F("    :"));
			Serial.print(digitalRead(m_pinA));
			Serial.print(F("    :"));
			Serial.println(digitalRead(m_pinB));*/
			sensorsReadMult++;
			sensorsReadAllIterator++;
			

			if (sensorsReadAllIterator == m_size) {
				m_lastReadMs = millis();
				return;
			}
			if (sensorsReadMult == size[i])
			{
				break;
			}

		}
	}
	
	if (m_sensInMain) {
		
		uint8_t sensorsReadMultMain = 0;
		for (auto i = 0; i < 4; i++)
		{
			if (size[i] == 0)
			{
				digitalWrite(m_pinMainA, bitRead(i, 0));
				digitalWrite(m_pinMainB, bitRead(i, 1));
				delayMicroseconds(32);

				m_analogData[sensorsReadAllIterator] = digitalRead(m_analogPinX);
				/*Serial.print(F("readall:"));
				Serial.print(sensorsReadAll);
				Serial.print(F("  val:"));
				Serial.println(m_analogData[sensorsReadAll]);
				Serial.print(F("mainA:"));
				Serial.print(digitalRead(m_pinMainA));
				Serial.print(F("  mainB:"));
				Serial.print(digitalRead(m_pinMainB));
				Serial.print(F("    :"));
				Serial.print(digitalRead(m_pinA));
				Serial.print(F("    :"));
				Serial.println(digitalRead(m_pinB));*/
				sensorsReadMultMain++;
				sensorsReadAllIterator++;
								
				if (sensorsReadAllIterator == m_size) {
					m_lastReadMs = millis();
					return;
				}
				if (sensorsReadMultMain == m_sensInMain)
					break;

				m_analogData[sensorsReadAllIterator] = digitalRead(m_analogPinY);
				
				/*Serial.print(F("readall:"));
				Serial.print(sensorsReadAll);
				Serial.print(F("  val:"));
				Serial.println(m_analogData[sensorsReadAll]);
				Serial.print(F("mainA:"));
				Serial.print(digitalRead(m_pinMainA));
				Serial.print(F("  mainB:"));
				Serial.print(digitalRead(m_pinMainB));
				Serial.print(F("    :"));
				Serial.print(digitalRead(m_pinA));
				Serial.print(F("    :"));
				Serial.println(digitalRead(m_pinB));*/
				sensorsReadMultMain++;
				sensorsReadAllIterator++;
				
				
				if (sensorsReadAllIterator == m_size) {
					m_lastReadMs = millis();
					return;
				}
				if (sensorsReadMultMain == m_sensInMain)
					break;

			}
		}
	}



	
	m_lastReadMs = millis();
}

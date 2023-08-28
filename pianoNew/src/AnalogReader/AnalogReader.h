/*
Name:		AnalogReader.hpp
Desc:		классы для чтения аналоговых значений, через гололый analogRead с возможностью использования мультиплексоров, одного или двух.
			содержит функционал калибровочных данных getCalibrated() и предыдущих значений для вычисления изменений значений getDelta()

Created:	1/05/2016 2:44:50 PM
Author:	netdog@tut.by
*/

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class AnalogReader
{
	// блочим копирование
	AnalogReader(const AnalogReader&) = delete; 
	void operator=(const AnalogReader&) = delete;
	//const AnalogReader& operator=(const AnalogReader&);
	
public:
	virtual ~AnalogReader();
	AnalogReader(uint8_t size, uint8_t analogPinStart, bool useCalibateArray, bool useDeltaArray);

	// read sensors
	virtual void readAll();

	// read sensors deffered
	void readAllDeffered(uint32_t interval=16);

	// get raw value
	int get(uint8_t index) const;	

	// get raw value by mapped index
	int getM(uint8_t index) const;

	// get calibrated value
	int getCalibrated(uint8_t index) const;

	// get calibrated value by mapped index
	int getCalibratedM(uint8_t index) const;

	// get delta value
	int getDelta(uint8_t index) const;

	// get delta value by mapped index
	int getDeltaM(uint8_t index) const;

	// calibrate by custom array
	void calibrateAll(int16_t calbratedValues[], uint8_t size);

	// calibrate by readAll
	void calibrateAll();

	// get last readAll time in ms
	uint32_t getLastUpdateMs() const;

	// debug print all sensors
	void printAll() const;

	// debug print all sensors
	void printAll(HardwareSerial& serial) const;
	// debug print by sensor id
	void print(uint8_t index) const;

	// debug print to serial by sensor index
	void print(HardwareSerial& serial, uint8_t index) const;

	// not implemented yet
	void remap(const uint8_t* remapIndexes, uint8_t size);

	// get count sensors
	uint8_t getSize() const;

protected:
	void _delay() const;

	//void setDelayUpdateDelta(int millis);
	
	// raw data ptr
	int16_t* m_analogData = nullptr;
	
	// calibrate data ptr
	int16_t* m_analogDataCalibrate = nullptr;

	// old data for delta
	int16_t* m_analogDataOld = nullptr;

	uint8_t * m_remapIndexes = nullptr;

	// количество датчиков
	const uint8_t m_size;

	uint8_t m_analogPinStart;

	//int16_t m_delayMillisUpdateDelta = 0;  
	uint32_t m_lastReadMs = 0;

	bool m_bUseCalibrate;
	bool m_bUseDelta;
};

class AnalogReaderBool
{
	// блочим копирование
	AnalogReaderBool(const AnalogReaderBool&) = delete;
	void operator=(const AnalogReaderBool&) = delete;
	//const AnalogReader& operator=(const AnalogReader&);
	
public:
	virtual ~AnalogReaderBool();
	AnalogReaderBool(uint8_t size, uint8_t analogPinStart);

	// read sensors
	virtual void readAll();

	// read sensors deffered
	void readAllDeffered(uint32_t interval=16);

	// get raw value
	bool get(uint8_t index) const;	

	// get last readAll time in ms
	uint32_t getLastUpdateMs() const;

	// debug print all sensors
	void printAll() const;

	// debug print all sensors
	void printAll(HardwareSerial& serial) const;
	// debug print by sensor id
	void print(uint8_t index) const;

	// debug print to serial by sensor index
	void print(HardwareSerial& serial, uint8_t index) const;

	// not implemented yet
	void remap(const uint8_t* remapIndexes, uint8_t size);

	// get count sensors
	uint8_t getSize() const;

protected:
	void _delay() const;

	//void setDelayUpdateDelta(int millis);
	
	// raw data ptr
	bool* m_analogData = nullptr;

	uint8_t * m_remapIndexes = nullptr;

	// количество датчиков
	const uint8_t m_size;

	uint8_t m_analogPinStart;

	//int16_t m_delayMillisUpdateDelta = 0;  
	uint32_t m_lastReadMs = 0;
};

class AnalogReaderMultUtc4052 : public AnalogReader
{
public:
	AnalogReaderMultUtc4052(uint8_t size, uint8_t pinA, uint8_t pinB, uint8_t analogPinX, uint8_t analogPinY, bool useCalibateArray, bool useDeltaArray);
	virtual void readAll() override;

protected:
	uint8_t m_pinMainA;
	uint8_t m_pinMainB;
	uint8_t m_analogPinX;
	uint8_t m_analogPinY;

};

class AnalogReaderMultUtc4052Bool : public AnalogReaderBool
{
public:
	AnalogReaderMultUtc4052Bool(uint8_t size, uint8_t pinA, uint8_t pinB, uint8_t analogPinX, uint8_t analogPinY);
	virtual void readAll() override;

protected:
	uint8_t m_pinMainA;
	uint8_t m_pinMainB;
	uint8_t m_analogPinX;
	uint8_t m_analogPinY;

};

class AnalogReaderMultUtc4052_Quad : public AnalogReaderMultUtc4052
{
public:
	AnalogReaderMultUtc4052_Quad(uint8_t size, uint8_t sensIn0, uint8_t sensIn1, uint8_t sensIn2, uint8_t sensIn3,  uint8_t pinMainA, uint8_t pinMainB, uint8_t pinA, uint8_t pinB, uint8_t analogPinX, uint8_t analogPinY, bool useCalibateArray, bool useDeltaArray);
	void readAll() override;
protected:
	uint8_t m_pinA;
	uint8_t m_pinB;
	uint8_t m_sensIn0;
	uint8_t m_sensIn1;
	uint8_t m_sensIn2;
	uint8_t m_sensIn3;
	uint8_t m_sensInMain;
};

class AnalogReaderMultUtc4052_QuadBool : public AnalogReaderMultUtc4052Bool
{
public:
	AnalogReaderMultUtc4052_QuadBool(uint8_t size, uint8_t sensIn0, uint8_t sensIn1, uint8_t sensIn2, uint8_t sensIn3, uint8_t pinMainA, uint8_t pinMainB, uint8_t pinA, uint8_t pinB, uint8_t analogPinX, uint8_t analogPinY);
	void readAll() override;
protected:
	uint8_t m_pinA;
	uint8_t m_pinB;
	uint8_t m_sensIn0;
	uint8_t m_sensIn1;
	uint8_t m_sensIn2;
	uint8_t m_sensIn3;
	uint8_t m_sensInMain;
};


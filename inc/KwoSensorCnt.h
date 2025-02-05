#pragma once

const size_t SENSORLOGCNT = 1000;

class KwoSensorTupel
{
protected:
	std::string sensorName;
	size_t sensorCount;
	double sensorValue;

public:
	inline KwoSensorTupel(const char* _sensorName, double _value)
		: sensorName(_sensorName)
		, sensorCount(1)
		, sensorValue(_value)
	{
	};

	inline bool isNameEqual(const char* _sensorName) const
	{
		return (sensorName == _sensorName);
	}

	inline size_t incCnt(size_t inc = 1) { sensorCount += inc; return sensorCount; };
};

class KwoSensorCnt
{
protected:
	std::vector <KwoSensorTupel> theSensors;

public:
	KwoSensorCnt();
	virtual ~KwoSensorCnt();

	size_t addSensor(const char* _sensorName, double _value);
};
#include "TempSensorReadDefs.h"


KwoSensorCnt::KwoSensorCnt()
: theSensors()
{

}

KwoSensorCnt::~KwoSensorCnt()
{
}

size_t KwoSensorCnt::addSensor(const char* _sensorName, double _value)
{
	bool notFoundSensor = true;
	for (auto aSensor : theSensors)
	{
		if (aSensor.isNameEqual(_sensorName))
		{
			size_t sensorCnt = aSensor.incCnt();
			notFoundSensor = false;
			if ((sensorCnt % SENSORLOGCNT) == 0)
			{
				syslog(LOG_NOTICE, "found Sensor %s %llu times (Actual %.3lf Dec C)", _sensorName, sensorCnt, _value);
			}

		}
	}

	if (notFoundSensor)
	{
		theSensors.push_back(KwoSensorTupel(_sensorName, _value));
		syslog(LOG_NOTICE, "found Sensor %s first time with %.3lf Deg C", _sensorName, _value);
	}

	return theSensors.size();
}

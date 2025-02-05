#pragma once
class SensorValueTupel
{
public:
  double         m_Value;
  struct timeval m_TimeStamp;

  inline SensorValueTupel(const SensorValueTupel& that)
  {
    m_Value = that.m_Value;
    memcpy(&m_TimeStamp, &that.m_TimeStamp, sizeof(m_TimeStamp));
  };

  inline SensorValueTupel(const double _value)
    : m_Value(_value)
  {
    gettimeofday(&m_TimeStamp, NULL);
  }
};


class SensorValues
{
protected:
  std::string m_PhySensorName;
  std::string m_LogSensorName;
  std::string m_SensorType;
  std::string m_SensorUnit;
  std::deque<SensorValueTupel> m_ValueList;
  size_t storeCounter;
   
public:
  SensorValues(const char *_sensorName, const char *_sensorType, const char *_sensorUnit, sql::Connection* sqlCon);
  SensorValues(const SensorValues& that);
  virtual ~SensorValues();
  inline const char *phySensorName() { return m_PhySensorName.c_str(); };
  inline const char *logSensorName() { return m_LogSensorName.c_str(); };
  inline const char* sensorType()    { return m_SensorType.c_str();    };
  inline const char* sensorUnit()    { return m_SensorUnit.c_str();    };
  size_t StoreValue(double _value, sql::Connection* sqlCon);
  

protected:
  size_t StoreValueDB(double _value, sql::Connection* sqlCon);
  void getLogSensorFromPhy(sql::Connection* sqlCon);
};


class SensorValueList
{
protected:
  std::vector<SensorValues> m_TheSensors;

public:
  SensorValueList();
  virtual ~SensorValueList();

  size_t StoreValue(const char *_sensorName, double _value, sql::Connection* sqlCon);

};


#include "TempSensorReadDefs.h"

// Retrieves the logical sensor name from the physical sensor name using the database connection.
void SensorValues::getLogSensorFromPhy(sql::Connection* sqlCon)
{
  if (sqlCon != nullptr)
  {
    try
    {
      // Construct the SQL query to select the logical sensor name based on the physical sensor name.
      std::string selectStr, insertStr;
      selectStr = "SELECT LogName FROM SensorAssigns WHERE PhyName = \'";
      selectStr += m_PhySensorName;
      selectStr += "\' ;";
      sql::SQLString theStatementStr(selectStr);
      sql::Statement* stmt = sqlCon->createStatement();
      sql::ResultSet* res = stmt->executeQuery(theStatementStr);

      // If a result is found, set the logical sensor name.
      if (res->next())
      {
        sql::SQLString tmpLogName = res->getString("LogName");
        m_LogSensorName = tmpLogName;
      }
      else
      {
        // If no result is found, create a new logical sensor name and insert it into the database.
        std::string logSensorName("ungeordnet_");
        std::string last12;
        if (m_PhySensorName.length() > RELEVANTSENSORNAMELEN)
        {
          last12 = m_PhySensorName.substr(m_PhySensorName.length() - RELEVANTSENSORNAMELEN);
        }
        else
        {
          last12 = m_PhySensorName;
        }
        logSensorName.append(last12);

        insertStr = "INSERT INTO SensorAssigns (PhyName, LogName, Type, Unit) VALUES (";
        insertStr += "\'" + m_PhySensorName + "\', ";
        insertStr += "\'" + logSensorName + "\', ";
        insertStr += "\'" + m_SensorType + "\', ";
        insertStr += "\'" + m_SensorUnit + "\') ";
        theStatementStr = insertStr;
        stmt->execute(theStatementStr);
        m_LogSensorName = logSensorName;
      }
      delete res;
      delete stmt;
    }
    catch (sql::SQLException& sqlErr)
    {
      // Log any SQL exceptions that occur.
      syslog(LOG_NOTICE, "# ERR: SQLException in %s (%s) on line %d: %s (MySQL error code: %d, SQLState: %s)\n", __FILE__, __FUNCTION__, __LINE__, sqlErr.what(), sqlErr.getErrorCode(), sqlErr.getSQLState().c_str());
    }

    return;
  }
  else
  {
    // Log a message if the SQL connection is null.
    syslog(LOG_NOTICE, "SQL Connection = Nullptr");
    return;
  }
}

// Copy constructor for SensorValues.
SensorValues::SensorValues(const SensorValues& that)
  : m_PhySensorName(that.m_PhySensorName)
  , m_LogSensorName(that.m_LogSensorName)
  , m_SensorType(that.m_SensorType)
  , m_SensorUnit(that.m_SensorUnit)
  , m_ValueList(that.m_ValueList)
  , storeCounter(that.storeCounter)
{

}

// Constructor for SensorValues that initializes member variables and retrieves the logical sensor name.
SensorValues::SensorValues(const char* _sensorName, const char* _sensorType, const char* _sensorUnit, sql::Connection* sqlCon)
  : m_PhySensorName(_sensorName)
  , m_LogSensorName("defaultLogSensor")
  , m_SensorType(_sensorType)
  , m_SensorUnit(_sensorUnit)
  , storeCounter(0)
{
  getLogSensorFromPhy(sqlCon);
}

// Destructor for SensorValues.
SensorValues::~SensorValues()
{
  // The destructor is currently empty, but it could be used to clean up resources if needed.
}

// Stores a sensor value in the database and adds it to the value list.
int SensorValues::StoreValueDB(double _value, sql::Connection* sqlCon)
{
  int retVal = -1;
  if (sqlCon != nullptr)
  {
    // Construct the SQL query to insert the sensor value into the database.
    sql::SQLString theStatementStr("INSERT INTO SensorValues(MeasuringTime, PhyName, LogName, Type, Unit, MeasuringValue) VALUES (NOW(), ?, ?, ?, ?, ?) ");
    sql::PreparedStatement* pstmt = sqlCon->prepareStatement(theStatementStr);
    pstmt->setString(1, m_PhySensorName);
    pstmt->setString(2, m_LogSensorName);
    pstmt->setString(3, m_SensorType);
    pstmt->setString(4, m_SensorUnit);
    pstmt->setDouble(5, _value);
    retVal = pstmt->executeUpdate();
    KwoDbAccess::getInstance()->IncInsertCnt();
    delete pstmt;
  }

  return retVal;
}

// Stores a sensor value, logging it and storing it in the database if necessary.
size_t SensorValues::StoreValue(const double _value, sql::Connection* sqlCon)
{
  // Log the sensor value at regular intervals.
  if ((storeCounter % SENSORLOGCNT) == 0)
  {
    if (storeCounter == 0)
    {
      syslog(LOG_NOTICE, "found Sensor %s first time with %.3lf Deg C", m_PhySensorName.c_str(), _value);
    }
    else
    {
      syslog(LOG_NOTICE, "found Sensor %s %llu times (Actual %.3lf Dec C)", m_PhySensorName.c_str(), storeCounter, _value);
    }
  }

  storeCounter++;

  // Store the value in the database if the value list is empty or if the value has changed significantly.
  if (m_ValueList.empty() || fabs(m_ValueList.back().m_Value - _value) >= 0.01)
  {
    StoreValueDB(_value, sqlCon);
    // Add the sensor value to the value list.
    m_ValueList.push_back(SensorValueTupel(_value));

    if (m_ValueList.size() > MAXNOVALUESVECTOR)
    {
      m_ValueList.pop_front();
    }
  }


  return m_ValueList.size();
}

// Constructor for SensorValueList.
SensorValueList::SensorValueList()
{

}

// Destructor for SensorValueList.
SensorValueList::~SensorValueList()
{
  return;
}

// Stores a sensor value for a specific sensor, creating a new sensor entry if necessary.
size_t SensorValueList::StoreValue(const char* _sensorName, double _value, sql::Connection* sqlCon)
{
  // Iterate through the list of sensors to find the matching sensor.
  for (auto& aSensor : m_TheSensors)
  {
    if (strcmp(_sensorName, aSensor.phySensorName()) == 0)
    {
      return aSensor.StoreValue(_value, sqlCon);
    }
  }

  // If the sensor is not found, create a new sensor entry and store the value.
  m_TheSensors.push_back(SensorValues(_sensorName, SENSORTYPE, SENSORUNIT, sqlCon));

  return m_TheSensors.back().StoreValue(_value, sqlCon);
}


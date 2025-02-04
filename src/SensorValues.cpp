#include "TempSensorReadDefs.h"


SensorValues::SensorValues(const char* _sensorName, const char* _sensorType, const char* _sensorUnit, sql::Connection* sqlCon)
: m_PhySensorName(_sensorName)
, m_SensorType(_sensorType)
, m_SensorUnit(_sensorUnit)
, storeCounter(0)
{
  if (sqlCon != nullptr)
  {
    try
    {
        /* '?' is the supported placeholder syntax */
        std::string selectStr, insertStr;
        selectStr = "SELECT LogName FROM SensorAssigns WHERE PhyName = \'";
        selectStr += m_PhySensorName;
        selectStr += "\' ;";
        sql::SQLString theStatementStr(selectStr);
        sql::Statement* stmt = sqlCon->createStatement();
        sql::ResultSet* res = stmt->executeQuery(theStatementStr);

        // syslog(LOG_NOTICE, "%s;\n", theStatementStr.c_str());
        if (res->next())
        {
            sql::SQLString tmpLogName = res->getString("LogName");
            m_LogSensorName = tmpLogName;
        }
        else
        {
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
        syslog(LOG_NOTICE, "# ERR: SQLException in %s (%s) on line %d: %s (MySQL error code: %d, SQLState: %s)\n", __FILE__, __FUNCTION__, __LINE__, sqlErr.what(), sqlErr.getErrorCode(), sqlErr.getSQLState().c_str());
    }

    return;
  }
  else
  {
      syslog(LOG_NOTICE, "SQL Connection = Nullptr");
      return;
  }
}

SensorValues::~SensorValues()
{
    //size_t noTupels = m_ValueList.size();
    //SensorValueTupel* theValuesPtr = nullptr;

    //while (noTupels)
    //{
    //    noTupels--;
    //    theValuesPtr = m_ValueList[noTupels];
    //    if (theValuesPtr)
    //    {
    //        delete theValuesPtr;
    //    }
    //}
    //m_ValueList.clear();

}

size_t SensorValues::StoreValueDB(double _value, sql::Connection* sqlCon)
{
    if (sqlCon != nullptr)
    {
        /* '?' is the supported placeholder syntax */
        sql::SQLString theStatementStr("INSERT INTO SensorValues(MeasuringTime, PhyName, LogName, Type, Unit, MeasuringValue) VALUES (NOW(), ?, ?, ?, ?, ?) ");
        sql::PreparedStatement* pstmt = sqlCon->prepareStatement(theStatementStr);
        pstmt->setString(1, m_PhySensorName);
        pstmt->setString(2, m_LogSensorName);
        pstmt->setString(3, m_SensorType);
        pstmt->setString(4, m_SensorUnit);
        pstmt->setDouble(5, _value);
        pstmt->executeUpdate();
        KwoDbAccess::getInstance()->IncInsertCnt();
        delete pstmt;
    }
    m_ValueList.push_back(SensorValueTupel(_value));
    return m_ValueList.size();
}


size_t SensorValues::StoreValue(const double _value, sql::Connection* sqlCon)
{
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

  if (m_ValueList.empty())
  {
    StoreValueDB(_value, sqlCon);
    //syslog(LOG_NOTICE, "Sensor:%s Value=%+.3lf C", sensorName(), m_ValueList.back()->m_Value);
  }
  else
  {
    if (fabs(m_ValueList.back().m_Value - _value) >= 0.01)
    {
      StoreValueDB(_value, sqlCon);
      //syslog(LOG_NOTICE, "Sensor:%s Value=%+.3lf C", sensorName(), m_ValueList.back()->m_Value);
      if (m_ValueList.size() > MAXNOVALUESVECTOR)
      {
        m_ValueList.pop_front();
        //SensorValueTupel *first = m_ValueList.front();
        //m_ValueList.erase(m_ValueList.begin());
        //delete first;
      }
    }
  }

  return m_ValueList.size();
}


SensorValueList::SensorValueList()
{

}

SensorValueList::~SensorValueList()
{
  return;
}

size_t SensorValueList::StoreValue(const char* _sensorName, double _value, sql::Connection* sqlCon)
{
  for (auto aSensor : m_TheSensors)
  {
    if (strcmp(_sensorName, aSensor.phySensorName()) == 0)
    {
      return aSensor.StoreValue(_value, sqlCon);
    }
  }

  m_TheSensors.push_back(SensorValues(_sensorName, SENSORTYPE, SENSORUNIT, sqlCon));

  return m_TheSensors.back().StoreValue(_value, sqlCon);
}

//
// @(#) 2019-09-28 (c) 2019 W.Kager
//
// This module implements mainly the methods of class KwoDbAccess as been found below;
// Module name and class name normally is similar
//
// This class is used to deal an access to a MariaDB Database. So that several different modules of one process can use a connection to a
// database and don't need to deal with database connection etc. 
//


#include "TempSensorReadDefs.h"


// The static pointer to the one and only instance of the class
KwoDbAccess* KwoDbAccess::m_MySelf = nullptr;

// The accessor to the one and only instance of the class
KwoDbAccess* KwoDbAccess::getInstance()
{

  if (m_MySelf == nullptr)
  {
    m_MySelf = new KwoDbAccess();
  }
  return m_MySelf;
}

// The constructor
KwoDbAccess::KwoDbAccess()
: m_SqlDriver(nullptr)
, m_SqlCon   (nullptr)
, m_SqlStmt  (nullptr)
, m_SqlRes   (nullptr)
, m_Schema   ("")
, m_InsertCntr(0LL)
{

}

// the desctructor
KwoDbAccess::~KwoDbAccess()
{
  // Disconnects the database, if there was a connection
  if (m_SqlCon)
  {
    m_SqlCon->close();
    delete m_SqlCon;
    m_SqlCon = nullptr;
  }
}

// This functions sets the Database schema and connects to the database
//
// Input:    _schema   the name of the database schema to be accessed
// Output:   a Pointer to the connection instance in case of success ; nullptr in case of error
// 
// Remarks: Portnumber, Host, User, Password are expected to be supplied as fixed defined in another header.
// 
sql::Connection* KwoDbAccess::init(const char *_schema)
{
  try
  {
    if (m_SqlDriver == nullptr)
    {
      m_SqlDriver = get_driver_instance();
    }
    if (m_SqlDriver)
    {
      if (m_SqlCon == nullptr)
      {
        m_SqlCon = m_SqlDriver->connect(MYSQLHOSTPORT, MYSQLHOSTUSER, MYSQLHOSTPASS);
      }
      else if (!m_SqlCon->isValid())
      {
        m_SqlCon->reconnect();
      }
      if (m_Schema != _schema)
      {
        m_Schema = _schema;
        m_SqlCon->setSchema(m_Schema);
      }

      return m_SqlCon;
    }
    else
    {
      syslog(LOG_ERR, "Cannot get SQL Driver instance");
      return nullptr;
    }
  }
  catch (sql::SQLException& sqlErr)
  {
    syslog(LOG_NOTICE, "# ERR: SQLException in %s (%s) on line %d: %s (MySQL error code: %d, SQLState: %s)\n", __FILE__, __FUNCTION__, __LINE__, sqlErr.what(), sqlErr.getErrorCode(), sqlErr.getSQLState().c_str());
    return nullptr;
  }
}

#include "TempSensorReadDefs.h"

const char* PRGINIPATH = "/";
// Some central defines
const int SLEEPINGDELAYSEC = 10;                 // Sleeping Delay; In case of the delay timer is failed, this will be the cycle when it should come again
const int FIRSTRUNMESS = 60;
const int STILLALIVETIMESTAMP = 14401;           // Delay: Average after this time at least should see a timestamp message in the logfile, remembering that we are still alive
const long long MAXNOROWSINDB = 100000;          // The number of rows (approximatly) we will keep in the database table; If this is exeeded the oldest rows will become deleted;
                                                 // For performance reasons this limit is not checked at every database insert, instead of it is checked time cyclic
                                                 // the real existing number of rows can exceed the number of the here specified limit- 
const std::size_t RELEVANTSENSORNAMELEN = 12;
const char* SENSORTYPE = "Temperature";
const char* SENSORUNIT = "[Deg C]";
                                                               
const char* PROGRAMVERSION = "3.0 (c) 2019-2025 W.Kager (kwo)"; // nomen est omen: a short text about program version and author
 
#if defined(WIN32)
const char* PIDFILEPATH = "C:\\TEMP\\";           // The Path for the Pid-file
const char* TEMPSENSORBASEPATH = "C:\\TMEP\\";
#else
const char* PIDFILEPATH = "/var/run/";                     // The Path for the Pid-file - (Pid-File is a short text file containing the process identification number of the process running here)
const char* TEMPSENSORBASEPATH = "/sys/bus/w1/devices";    // this is the path of the "special device" where we find our temperature sensors
#endif
const char* PIDFILESUFF = ".pid";                          // Pid-file Suffix


TempSensorRead* TempSensorRead::mySelf = NULL;

TempSensorRead::TempSensorRead()
: m_Running(true)
, m_TheSensorValueListPtr(nullptr)
{
  m_TheSensorValueListPtr = new SensorValueList();
}

TempSensorRead::~TempSensorRead()
{
  if (m_TheSensorValueListPtr)
  {
    delete m_TheSensorValueListPtr;
  }
  m_TheSensorValueListPtr = nullptr;
}

void TempSensorRead::doCyclicRead(int theSignal)
{

  DIR *dir = nullptr;
  struct dirent* dirseaPtr = NULL;

  if ((dir = opendir(TEMPSENSORBASEPATH)) != nullptr)
  {
    try
    {
      KwoDbAccess *dbax = KwoDbAccess::getInstance();
      sql::Connection *sqlCon = dbax->init(THEDBSCHEMA);

      while ((dirseaPtr = readdir(dir)) != nullptr)
      {
        long sensorno = -1;
        long sensorFamily = -1;
        int lenScanf = 0;
        // syslog(LOG_NOTICE, "Found %s in directory", dirseaPtr->d_name);
        lenScanf = sscanf(dirseaPtr->d_name, "28-%4x%8x", &sensorFamily, &sensorno);
        // syslog(LOG_NOTICE, "noScanf=%d no=%d 0x%08x", lenScanf, sensorno, sensorno);
        if (lenScanf == 2)
        {
          char sensorFile[256];
          char firstLine[1024];
          char secondLine[1024];
          // syslog(LOG_NOTICE, "Try Sensor:%s", dirseaPtr->d_name);
          sprintf(sensorFile, "%s/%s/w1_slave", TEMPSENSORBASEPATH, dirseaPtr->d_name);
          // syslog(LOG_NOTICE, "sensorFile:%s", sensorFile);
          std::ifstream sensorStream(sensorFile, std::ifstream::in);

          if (sensorStream.good())
          {
            int aa[9];
            int sensorTemp;
            sensorStream.getline(firstLine, sizeof(firstLine));
            sensorStream.getline(secondLine, sizeof(secondLine));
            if (sscanf(secondLine, "%2x %2x %2x %2x %2x %2x %2x %2x %2x t=%d",
              &aa[0], &aa[1], &aa[2], &aa[3], &aa[4], &aa[5], &aa[6], &aa[7], &aa[8], &sensorTemp) == 10)
            {
              double value = sensorTemp;
              value *= 1.0e-3;
              SensorValueList *theSensorListValuePtr = TempSensorRead::getInstance()->m_TheSensorValueListPtr;
              if (theSensorListValuePtr)
              {
                theSensorListValuePtr->StoreValue(dirseaPtr->d_name, value, sqlCon);
              }
              else
              {
                syslog(LOG_NOTICE, "Sensor %08x Temp=%d", sensorno, sensorTemp);
              }
            }
          }
          else
          {
            syslog(LOG_NOTICE, "sensorFile:%s not good", sensorFile);
          }
        
        }
        else
        {
          
          //sql::SQLException sqlErrx;
          //throw(sqlErrx);
        }
      }
    }
    catch (sql::SQLException& sqlErr)
    {      
      syslog(LOG_NOTICE, "# ERR: SQLException in %s (%s) on line %d: %s (MySQL error code: %d, SQLState: %s)\n", __FILE__, __FUNCTION__, __LINE__, sqlErr.what(), sqlErr.getErrorCode(), sqlErr.getSQLState().c_str());
    }

    closedir(dir);
  }
  else
  {
    int theError = errno;
    char errmsg[256];
    sprintf(errmsg, "Error at open Directory %s:%s", TEMPSENSORBASEPATH, strerror(theError));
    syslog(LOG_NOTICE, errmsg);
  }
   
	return ;
}


long long  TempSensorRead::CleanUpOld(const long long &maxRows)
{
  try
  {
    long long noRows = 0LL;
    KwoDbAccess* dbax = KwoDbAccess::getInstance();
    sql::Connection* sqlCon = dbax->init(THEDBSCHEMA);

    if (sqlCon != nullptr)
    {
      /* '?' is the supported placeholder syntax */
      sql::SQLString theStatementStr("SELECT COUNT(*) FROM SensorValues ");
      sql::Statement* stmt = sqlCon->createStatement();

      sql::ResultSet* res = stmt->executeQuery(theStatementStr);

      // syslog(LOG_NOTICE, "%s;\n", theStatementStr.c_str());

      if (res->next())
      {
        noRows = res->getInt64("COUNT(*)");

        if (noRows > maxRows)
        {
          char numMsg[64];
          long long restRows = noRows - maxRows;

          sprintf(numMsg, "%lld", restRows);
          syslog(LOG_NOTICE, "DB Table SensorValues full; Delete %s rows", numMsg);
          theStatementStr = " DELETE FROM SensorValues ORDER BY MeasuringTime ASC LIMIT ";
          theStatementStr += numMsg;
          sql::PreparedStatement* pstmt = sqlCon->prepareStatement(theStatementStr);
          pstmt->executeUpdate();
          noRows = maxRows;
        }

        delete res;    res = NULL;
        delete stmt;   stmt = NULL;
        return noRows;
      }
    }
    else
    {
      syslog(LOG_NOTICE, "SQL Connection = Nullptr");
      return -1;
    }
  }
  catch (sql::SQLException& sqlErr)
  {
    syslog(LOG_NOTICE, "# ERR: SQLException in %s (%s) on line %d: %s (MySQL error code: %d, SQLState: %s)\n", __FILE__, __FUNCTION__, __LINE__, sqlErr.what(), sqlErr.getErrorCode(), sqlErr.getSQLState().c_str());
  }
  return -1;
}



// Stop our loop
void TempSensorRead::weAreDone(int  theSignal)
{
  // Get our one and only instance
  TempSensorRead* thePtr = TempSensorRead::getInstance();

  // Stop our external endless Loop
  thePtr->m_Running = false;

  // Tell the world that we should terminate
  syslog(LOG_NOTICE, "Received signal to stop\n");

  return;
}

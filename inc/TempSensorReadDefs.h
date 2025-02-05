#pragma once
#include <sys/types.h>                                 // for pid_t, size_t ... 
#include <sys/stat.h>                                  // For open() File function
#include <sys/time.h>                                  // For timeval
#include <stdio.h>                                     // For sscanf
#include <time.h>                                      // for struct tm and time Function
#include <fcntl.h>                                     // For File Control
#include <signal.h>                                    // For signal Handling: Timer Calls and Re-read of the configuration is Signal-Based
#include <unistd.h>                                    // For signal handling/daemon creation fork, exec
#include <string>                                      // For general standard c++ String Functions
#include <string.h>                                    // for memset etc.
#include <iostream>                                    // For cerr, cout etc.
#include <fstream>                                     // For file Handling
#include <list>                                        // For List Handling 
#include <vector>                                      // For vector Handling
#include <deque>                                       // For deque Handling
#include <stdlib.h>                                    // For "exit()" Function Call
#include <syslog.h>                                    // For syslog Messages
#include <string>                                      // For std::string
#include <string.h>                                    // for strlen, strcpy
#include <dirent.h>                                    // for directory handling
#include "math.h"                                      // for fabs, sin, cos etc
//#include <wiringPi.h>                                  // Raspberry Wiring, handling of direct I/O Pins

const extern char * PRGINIPATH;
// Some central defines
const extern int SLEEPINGDELAYSEC;             // Sleeping Delay; In case of the delay timer is failed, this will be the cycle when it should come again
const extern int FIRSTRUNMESS;
const extern int STILLALIVETIMESTAMP;          // Delay: Average after this time at least should see a timestamp message in the logfile, remembering that we are still alive
const extern long long MAXNOROWSINDB;          // Maximum number of Rows in DB-Table
const extern char *PROGRAMVERSION;             // Program Version;

#if defined(WIN32)
const extern char* PIDFILEPATH;           // The Path for the Pid-file
const extern char* TEMPSENSORBASEPATH;
#else
const extern char* PIDFILEPATH;           // The Path for the Pid-file
const extern char* TEMPSENSORBASEPATH;
#endif
const extern char* PIDFILESUFF;                // Pid-file Suffix

const long long MAXNOVALUESVECTOR = 1000;

const extern std::size_t RELEVANTSENSORNAMELEN;
const extern char* SENSORTYPE;;
const extern char* SENSORUNIT;;

//#define     __MYSQL_USE_ 1
#if defined(__MYSQL_USE_)                             
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include "mysql_connection.h"

const char MYSQLHOSTPORT[] = "tcp://127.0.0.1:3306";   // Database Host and Port
const char MYSQLHOSTUSER[] = "umlaufpumpe";            // Datebase User
const char MYSQLHOSTPASS[] = "mySql@felix!!22";        // Database Password
const char THEDBSCHEMA[]   = "umlaufpumpe";            // Database Schema
#endif

#define     __MARIADB_USE_ 1
#if defined(__MARIADB_USE_)    
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include "mysql_connection.h"

const char MYSQLHOSTPORT[] = "tcp://127.0.0.1:3306";   // Database Host and Port
const char MYSQLHOSTUSER[] = "umlaufpumpe";            // Datebase User
const char MYSQLHOSTPASS[] = "mySql@felix!!22";        // Database Password
const char THEDBSCHEMA[] = "umlaufpumpe";            // Database Schema
#endif


#include "KwoDbAccess.h"
#include "KwoPidFileHdl.h"
#include "SensorValues.h"
#include "TempSensorRead.h"
#include "KwoSensorCnt.h"
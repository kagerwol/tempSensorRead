//
// (c) 2016 kwo W.Kager all rights reserverd
//
// This File declares the named class.
// The class is made to handle the Process-Identification-File (PID)
// It is used to deny double starting of the program, and find the PID for the Program when running as daemon


// The one and only Include
#include "TempSensorReadDefs.h"

// String Overload
std::string &operator+=(std::string &str, int iix)
{
  static char xxtxt[128];
  sprintf(xxtxt, "%d", iix);
  str += xxtxt;
  return str;
}


//
// This function tries to find a pid-file, and if found tries to read it, and check if the process is still existing.
// If not it writes the Process Pid into the pid-file. 
// 
int KwoPidFileHdl::lockFile() 
{ 
  std::fstream  iFile;                          // input File Stream
  std::fstream  oFile;                          // 
  std::string   aLine("");                      // Input Buffer for one Line, intermediate String Buffer
  int iPid;                                     // Variable to read one Pid
  struct stat sts;                              // File attribute structure

  if (m_FileCreationOk) return 0;               // If File Creation was OK nothing left to do

  // Is our Process Identification File already existing 
  if (stat(m_PidFileName.c_str(), &sts)==0)                
  {
    // thant tell the world .. and
    syslog(LOG_NOTICE, "Pid-file \"%s\" already exists\n", m_PidFileName.c_str());

    // Try to open for reading
    iFile.open(m_PidFileName.c_str(), std::fstream::in); 

    // File open wasn't so good?
    if (!iFile.good())
    {
      // Give a message and good by
      syslog(LOG_NOTICE, "Failed to open pid-file \"%s\" for reading\n", m_PidFileName.c_str());
      if (deleteFile(m_PidFileName.c_str()) == -1)
      {
        return -1;
      }
    }

    // Check afain if the file is still existing. 
    if (stat(m_PidFileName.c_str(), &sts)==0)
    { 
      // if so read one line from the file 
      std::getline(iFile, aLine);

      // Can we recognize a valid number? 
      if (sscanf(aLine.c_str(), " %d", &iPid) != 1)
      {
        syslog(LOG_NOTICE, "Pid-file \"%s\" does not contain valid PID!\n", m_PidFileName.c_str());
        iFile.close();
        if (deleteFile(m_PidFileName.c_str()) == -1)
        {
          return -1;
        }
      }
      else 
      {

        iFile.close();                // Close the found pid-file
        aLine = "";                   // On unix systems all processes can be found under /proc/"process-id"
        aLine += "/proc/";
        aLine += iPid;
        aLine += "/";

        // So we check if we find one file "/proc/process-id" ...
        if (stat(aLine.c_str(), &sts)==0)
        { 
          // If we found such file, that means our process is already running.
          syslog(LOG_NOTICE, "process %s seems already running with pid=%d\n", m_ProgramName.c_str(), iPid);    
          return -1;
        } 
        else  
        {
          //delete broken pid file
          syslog(LOG_NOTICE, "deletinge file \"%s\" (since process seems died)\n", m_PidFileName.c_str());
          if (deleteFile(m_PidFileName.c_str()) == -1)
          {
            return -1;
          }
        }
      }
    }
  }

  // When we come to here, an existing pid file should be deleted.  
  // Open the Pid-file exclusive with special rights.
  if ((iPid = open(m_PidFileName.c_str(), O_RDWR | O_EXCL | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP)) == -1) 
  {
    close(iPid);             
    syslog(LOG_NOTICE, "failed to open pid-file \"%s\" for exclusive read/write\n", m_PidFileName.c_str());
    return -1;
  } 
  else 
  {
    aLine = "";                               // Prepare the pid a text
    aLine += m_Pid;

    // And write it to the file
    if (write(iPid, aLine.c_str(), aLine.length()) != (ssize_t)aLine.length())
    {
      close(iPid);
      syslog(LOG_NOTICE, "Error write to pid-file \"%s\"\n", m_PidFileName.c_str());
      return -1;
    }
    else
    {
      close(iPid); 
      syslog(LOG_NOTICE, "Pid-File writing was OK\n"); 
      m_FileCreationOk = true;
    }
  }

  return 0;
}



int KwoPidFileHdl::deleteFile(const char *fileName)
{
  int ret = unlink(fileName);
  if (ret == -1)
  {
    syslog(LOG_NOTICE, "failed to remove \"%s\"\n", fileName);
  }
  else
  {
    syslog(LOG_NOTICE, "removed old pid-file \"%s\"\n", fileName);
  }
  return ret;    
}

KwoPidFileHdl::KwoPidFileHdl(const char *_progName, pid_t _pid)
: m_ProgramName(_progName)
, m_Pid(_pid)
, m_PidFileName(PIDFILEPATH)
, m_FileCreationOk(false)
{
  m_PidFileName += m_ProgramName;
  m_PidFileName += PIDFILESUFF;
  return;
}


KwoPidFileHdl::~KwoPidFileHdl(void)
{
  if (m_FileCreationOk)
  {
    deleteFile(m_PidFileName.c_str());
  }
}

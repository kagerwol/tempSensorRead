/* daemon.c */
#include "TempSensorReadDefs.h"

// This function installs a signal handler
static sighandler_t handle_signal(int sig_nr, sighandler_t signalhandler)
{
  struct sigaction neu_sig, alt_sig;
  neu_sig.sa_handler = signalhandler;
  sigemptyset(&neu_sig.sa_mask);

  neu_sig.sa_flags = SA_RESTART;

  if (sigaction(sig_nr, &neu_sig, &alt_sig) < 0)
  {
    return SIG_ERR;
  }
  return alt_sig.sa_handler;
}

// This function let the process become a daemon in background
static void start_daemon(const char* log_name, int facility)
{
  int iix;                          // Simple Loop counter
  pid_t pid;                        // Process Id of the child process

  // stop parent process. We will get an orphan which the "init" process will take care about 
  if ((pid = fork()) != 0)
  {
    // I am the Parent
    std::cerr << "1st: Started [" << (int)pid << "]" << std::endl;
    exit(EXIT_FAILURE);
  }

  // child-process become session leader.
  // If this fails, possible the process is already a session-leader.
  if (setsid() < 0)
  {
    std::cerr << log_name << " can\'t become session-leader " << std::endl;
    exit(EXIT_FAILURE);
  }

  // Ignore Signal Hangup
  handle_signal(SIGHUP, SIG_IGN);

  // Terminate child again
  // 
  if ((pid = fork()) != 0)
  {
    // I am the Parent
    std::cerr << "2nd: Started [" << (int)pid << "]" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Change the working directory:
  // Resons to change the Directory
  // + the core file is always in actual working directory
  // + umount should work clear when stopping
  chdir(PRGINIPATH);

  // We should not inherit the bitmask from parent process
  // We reset to zero
  umask(0);

  // Close all File Descriptors
  for (iix = (int)sysconf(_SC_OPEN_MAX); iix > 0; iix--)
  {
    close(iix);
  }

  // We do not have a terminal so we open a logfile for our outputs
  openlog(log_name, LOG_PID | LOG_CONS | LOG_NDELAY, facility);

  // We should now be a daemon and return
  return; // to sender
}

// This is the standard main entry point 
int main(int argc, char* argv[], char* envp[])
{
  size_t retPidLog;                       // PidLog File
  KwoPidFileHdl* pPidHdl = NULL;          // Pid File Handler

  time_t RestTimeRun = STILLALIVETIMESTAMP;  // some time dely (in [sec])
  time_t RestTimeCyc = SLEEPINGDELAYSEC;     // 
  struct timeval startTime;               // Time of Start
  struct timeval actTime;                 // Actual Momentary Time
  struct timeval futureTimeCyc;           // Future waiting time till Cycle finish
  struct timeval futureTimeRun;           // Future waiting time till print out running finish
  struct timeval deltaTimeCyc;            // Delta Time Running
  struct timeval deltaTimeRun;            // Delta Time Cyclic
  long  days;                             // Days
  short hour, min, sec;                   // Hours Minutes Seconds
  char msgtxt[64];                        // For Printout
  std::string argvNo1 = "";               // First argument
  long long noRowsInDb = -1;              // Number of Rows in DB Table

  if (argc > 1)
  {
    for (retPidLog = 0; retPidLog < strlen(argv[1]); retPidLog++)
    {
      argvNo1 += (char)tolower((int)argv[1][retPidLog]);
    }
  }

  // Build our own name
  std::string myname(basename(argv[0]));

  // KwoPumpCycle::readConfig(0);
  // Make ourself a daemon (background process)
  if (argvNo1 == "asdaemon")
  {

    start_daemon(myname.c_str(), LOG_LOCAL0);

    pPidHdl = new KwoPidFileHdl(myname.c_str(), getpid());

    // Give a message that we are startet
    syslog(LOG_NOTICE, "started as Daemon with PID(%d) ...\n", getpid());

    retPidLog = pPidHdl->lockFile();

  }
  else
  {
    std::cout << "Not started as daemon. Use \"" << myname.c_str() << " asdaemon\" to start as daemon" << std::endl;
    retPidLog = 0;
  }

  // Create and lock a PID File and continue only if the Pid-file was successfully written
  if (retPidLog == 0)
  {
    gettimeofday(&startTime, NULL);         // Remember the Start Time Stamp

    // Get our one and only Cycler Instance
    TempSensorRead* theCycler = TempSensorRead::getInstance();

#if defined(WIN32)
    KwoPumpCycle::readConfig(1);
#endif

    // Handle the signal to stop ourself
    handle_signal(SIGUSR2, TempSensorRead::weAreDone);

    // Do an endless Loop, which do nothing, except randomly put an alive timestamp in the log. 
    // The main thing what we want actually is done in signal Handlers.
    // Also the running flag is reset in the signal Handler of SIGUSR2
    gettimeofday(&futureTimeCyc, NULL);
    memcpy(&futureTimeRun, &futureTimeCyc, sizeof(futureTimeRun));
    RestTimeRun  = FIRSTRUNMESS;              // Make a random Time for sleep
    RestTimeCyc  = SLEEPINGDELAYSEC;
    futureTimeRun.tv_sec += RestTimeRun;
    futureTimeCyc.tv_sec += RestTimeCyc;

    syslog(LOG_NOTICE, "%s from %s: starting loop", PROGRAMVERSION, __DATE__);

    while (theCycler->running())
    {
      if (RestTimeRun < RestTimeCyc)
      {
        sleep((unsigned int)RestTimeRun);
      }
      else
      {
        sleep((unsigned int)RestTimeCyc);
      }                                                 // Sleeping and waiting
      gettimeofday(&actTime, NULL);                     // Remember the Timestamp
      timersub(&futureTimeCyc, &actTime, &deltaTimeCyc);
      timersub(&futureTimeRun, &actTime, &deltaTimeRun);

      if (deltaTimeCyc.tv_sec > 0)
      {
        RestTimeCyc = deltaTimeCyc.tv_sec;
      }
      else
      {
        gettimeofday(&futureTimeCyc, NULL);
        RestTimeCyc = SLEEPINGDELAYSEC;
        futureTimeCyc.tv_sec += RestTimeCyc;
        TempSensorRead::doCyclicRead(0);
        gettimeofday(&actTime, NULL);                     // Remember the Timestamp
        timersub(&futureTimeRun, &actTime, &deltaTimeRun);
      }
      if (deltaTimeRun.tv_sec > 0)
      {
        RestTimeRun = deltaTimeRun.tv_sec;
      }
      else
      {
        gettimeofday(&futureTimeRun, NULL);
        timersub(&futureTimeRun, &startTime, &deltaTimeRun);       // Calculate the time since start
        RestTimeRun = rand() % STILLALIVETIMESTAMP;
        futureTimeRun.tv_sec += RestTimeRun;

        days = deltaTimeRun.tv_sec;                                // Convert from Seconds to Days hour:minutes:seconds:miliseconds
        sec = (short)(days % 60);
        days /= 60;
        min = (short)(days % 60);
        days /= 60;
        hour = (short)(days % 24);
        days /= 24;

        if (days == 0)                                    // Some small geek to handle singular and plural for days
        {
          sprintf(msgtxt, "");                            // No milk today
        }
        else if (days == 1)
        {
          sprintf(msgtxt, "%d day ", days);               // A day of sunshine
        }
        else
        {
          sprintf(msgtxt, "%d days ", days);              // These days are made for us.  
        }

        noRowsInDb = theCycler->CleanUpOld(MAXNOROWSINDB);
        // And proudly present our alive message in the logfile
        syslog(LOG_NOTICE, "running since %s%02d:%02d:%02d.%03d | %lld Values stored during run | %lld Values in Table total |\n", msgtxt, hour, min, sec, deltaTimeRun.tv_usec / 1000, KwoDbAccess::getInstance()->InsertCnt(), noRowsInDb);

      }

    }
  }

  // Our loop has to terminate
  syslog(LOG_NOTICE, "stopped\n");

  // Close the logfile
  closelog();

  // Delete the Pid File Handler
  if (pPidHdl != NULL)
  {
    delete pPidHdl;
    pPidHdl = NULL;
  }

  // and bye bye
  return EXIT_SUCCESS;
}


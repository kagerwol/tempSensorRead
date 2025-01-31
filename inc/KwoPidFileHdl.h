//
// (c) 2016 kwo W.Kager all rights reserverd
//
// This File declares the named class.
// The class is made to handle the Process-Identification-File (PID)
// It is used to deny double starting of the program, and find the PID for the Program when running as daemon
#pragma once
class KwoPidFileHdl
{
protected:
  std::string m_ProgramName;                            // The program-name
  pid_t       m_Pid;                                    // The program-process-identification
  std::string m_PidFileName;                            // The Pid-File-Name
  bool        m_FileCreationOk;                         // Pid-file creation was OK

public:
  KwoPidFileHdl(const char *progName, pid_t pid);
  virtual ~KwoPidFileHdl(void);
  int lockFile(void);                                   // Manage the Pid-file
  static int deleteFile(const char *fileName);          // Delete a file
  inline const bool &fileCreaionOk(void) { return m_FileCreationOk; }; // Accessor Routine
};


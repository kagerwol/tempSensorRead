#pragma once

class TempSensorRead
{
protected:
  bool m_Running;                                                // Keep outself running
  static TempSensorRead *mySelf;
  TempSensorRead();
  SensorValueList * m_TheSensorValueListPtr;

public:
  static inline TempSensorRead* getInstance()
  {
    if (mySelf == NULL)
    {
      mySelf = new TempSensorRead();
    }
    return mySelf;
  }

  virtual ~TempSensorRead();

  static void doCyclicRead(int theSignal);
  static void weAreDone(int theSignal) ;                         // Function called when a SIGUSR2 Signal instigated
  long long CleanUpOld(const long long &maxRows);                // Delete old Rows from DB

  inline const bool& running() const { return m_Running; };      // Accessor Routine

};
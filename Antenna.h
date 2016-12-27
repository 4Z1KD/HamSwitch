#ifndef Antenna_h
#define Antenna_h

#include "Arduino.h"

class Antenna
{
  public:
    Antenna(String port, int* band, String description);
    String GetPort();
    int* GetBand();
    String GetDescription();
  private:
    String m_Port;
    int* m_Band;
    String m_Description;
};


#endif

#ifndef Antenna_h
#define Antenna_h

#include "Arduino.h"
#include <SimpleList.h>

class Antenna
{
  public:
    Antenna(String port, SimpleList<int> bands, String description);
    String GetPort();
    SimpleList<int>* GetBands();
    String GetDescription();
  private:
    String m_Port;
    SimpleList<int> m_Bands;
    String m_Description;
};


#endif

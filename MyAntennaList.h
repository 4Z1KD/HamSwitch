#ifndef MyAntennaList_h
#define MyAntennaList_h

#include "Arduino.h"
#include "Antenna.h"

#define NUM_OF_ANTANNA 8

class MyAntennaList
{
  public:
    MyAntennaList();
    Antenna* AntennaList[NUM_OF_ANTANNA];
};


#endif

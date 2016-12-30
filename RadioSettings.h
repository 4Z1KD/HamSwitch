#include "Arduino.h"

const int ICOM = 0;
const int YAESU = 1;
const int KENWOOD = 2;

const int MY_RADIO = 2; //0 for ICOM, 1 for Yaesu, 2 for KENWOOD

String GetFrequencyCommand()
{
  switch (MY_RADIO) {
    case ICOM:
      return "FA;";
      break;
    
    case YAESU:
      return "FA;";
      break;
    
    case KENWOOD:
      return "FA;";
      break;
    
    default:
      return "FA;";
      break;
  }
  return "";
}


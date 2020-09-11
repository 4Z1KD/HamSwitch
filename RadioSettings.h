#include "Arduino.h"
#include "UserSettings.h"
#include <SimpleList.h>
#include <AltSoftSerial.h>

//Generic Request/Response function definition - every radio should implement these methods
typedef void (*RadioRequest) (AltSoftSerial* serial);
typedef unsigned long (*RadioResponse) (AltSoftSerial* serial);


//********************************************************** Radio Implementations ******************************************************************//
//************* 5 byte req/res *************
void FT8X7_Request(AltSoftSerial* serial)
{
  byte ReadFreq[] = {0x00, 0x00, 0x00, 0x00, 0x03};
  for (byte i = 0; i < sizeof(ReadFreq); i++)
  {
    serial->write(ReadFreq[i]);
  }
}

unsigned long FT8X7_Response(AltSoftSerial* serial)
{
  int i = 0;
  String response = "";
  while (serial->available()) {
    if (i++ < 4)
    {
      int inChar = (int)serial->read();
      response += (inChar < 10) ? "0" + String(inChar, HEX) : String(inChar, HEX);
      delay(5);
    }
    else
    {
      serial->read();
    }
  }
  unsigned long j = 0;
  String pattern = "%lu";
  sscanf (&response[0], &pattern[0], &j);
  return j;
}
//***************** FA ******************
void FA_Request(AltSoftSerial* serial)
{
  serial->print("FA;");
}

unsigned long FA_Response(AltSoftSerial* serial)
{
  String response = "";
  while (serial->available()) {
    char inChar = (char)serial->read();
    response += inChar;
    if (inChar == ';') {
      response.trim(); //trim the string
      break;
    }
    delay(5);
  }
  unsigned long j = 0;
  String pattern = "FA%lu";
  sscanf (&response[0], &pattern[0], &j);
  return j/10;
}
//**************************************************************************************************************************************************//

//Radio Enumeration
const int FT817 =     101;
const int FT857 =     102;
const int FT897 =     103;
const int FT991 =     104;

const int TS450 =     201;
const int TS480 =     202;
const int TS590 =     203;
const int TS690 =     204;
const int TS850 =     205;
const int TS950 =     206;

const int K2 =        301;
const int KX2 =       302;
const int K3S =       303;
const int K3 =        304;
const int KX3 =       305;

const int SDR1000 =   401;

/*
   This struct represents a radio.
   It has an Id, and two callbacks for sending commands to the radio and receiving radio response
*/
struct Radio {
  int Id;
  char* displayname;
  RadioRequest FrequencyCommand;
  RadioResponse FrequencyResponse;
};

//the supported radios at the moment
Radio DummyRadio =      {-1,        (char*)"DUMMY RADIO", 0,              0             };

Radio FT817_Radio =     {FT817,     (char*)"FT-817",      FT8X7_Request,  FT8X7_Response};
Radio FT857_Radio =     {FT857,     (char*)"FT-857",      FT8X7_Request,  FT8X7_Response};
Radio FT897_Radio =     {FT897,     (char*)"FT-897",      FT8X7_Request,  FT8X7_Response};
Radio FT991_Radio =     {FT991,     (char*)"FT-991",      FA_Request,     FA_Response};

Radio TS450_Radio =     {TS450,     (char*)"TS-450",      FA_Request,     FA_Response};
Radio TS480_Radio =     {TS480,     (char*)"TS-480",      FA_Request,     FA_Response};
Radio TS590_Radio =     {TS590,     (char*)"TS-590",      FA_Request,     FA_Response};
Radio TS690_Radio =     {TS690,     (char*)"TS-690",      FA_Request,     FA_Response};
Radio TS850_Radio =     {TS850,     (char*)"TS-850",      FA_Request,     FA_Response};
Radio TS950_Radio =     {TS950,     (char*)"TS-950",      FA_Request,     FA_Response};

Radio K2_Radio =        {K2,        (char*)"K2",          FA_Request,     FA_Response};
Radio KX2_Radio =       {KX2,       (char*)"KX2",         FA_Request,     FA_Response};
Radio K3S_Radio =       {K3S,       (char*)"K3S",         FA_Request,     FA_Response};
Radio K3_Radio =        {K3,        (char*)"K3",          FA_Request,     FA_Response};
Radio KX3_Radio =       {KX3,       (char*)"KX3",         FA_Request,     FA_Response};

Radio SDR1000_Radio =   {SDR1000,   (char*)"SDR-1000",    FA_Request,     FA_Response};




SimpleList<Radio> RadioList; //a list that holds the supported radios

void InitRadioList()
{
  RadioList.push_back(FT857_Radio);
  RadioList.push_back(FT897_Radio);
  RadioList.push_back(FT991_Radio);
  RadioList.push_back(TS590_Radio);
  RadioList.push_back(TS480_Radio);
  RadioList.push_back(K2_Radio);
  RadioList.push_back(K3_Radio);
}

Radio GetMyRadio()
{
  for (SimpleList<Radio>::iterator itr = RadioList.begin(); itr != RadioList.end(); ++itr)
  {
    if (itr->Id == MY_RADIO)
    {
      return *itr;
    }
  }
  return DummyRadio;
}

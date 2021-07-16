#include "UserSettings.h"
#include <SimpleList.h>
#include <AltSoftSerial.h>

byte rdI[10];   //read data icom
int state = 1;  // state machine

//Generic Request/Response function definition - every radio should implement these methods
typedef void (*RadioRequest) (AltSoftSerial* serial);
typedef unsigned long (*RadioResponse) (AltSoftSerial* serial);

/************************************************************* Radio/Interface Implementations ******************************************************************
    Every Radio may have a different CAT interface.
    In reality, there are 3 major interfaces:
    KENWOOD - FA interface
    YAESU   - Y interface (5 bit request / response)
    ICOM    - CI V

    Most manufacturers use KENWOOD interface
 ***************************************************************************************************************************************************************/

//************* Y Interface *************
void Y_Request(AltSoftSerial* serial)
{
  byte ReadFreq[] = {0x00, 0x00, 0x00, 0x00, 0x03};
  for (byte i = 0; i < sizeof(ReadFreq); i++)
  {
    serial->write(ReadFreq[i]);
  }
}

unsigned long Y_Response(AltSoftSerial* serial)
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
//***************** FA Interface ******************
void FA_Request(AltSoftSerial* serial)
{
  serial->println("FA;");
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
  return j / 10;
}
//************************************** ICOM Interface ***************************************

int icomSM(byte b) {     // state machine
  // This filter solves read from 0x00 0x05 0x03 commands and 00 E0 F1 address used by software
  switch (state) {
    case 1: if ( b == 0xFE ) {
        state = 2;
        rdI[0] = b;
      }; break;
    case 2: if ( b == 0xFE ) {
        state = 3;
        rdI[1] = b;
      } else {
        state = 1;
      }; break;
    // addresses that use different software 00-trx, e0-pc-ale, winlinkRMS, f1-winlink trimode
    case 3: if ( b == 0x00 || b == 0xE0 || b == 0xF1 ) {
        state = 4; rdI[2] = b;                           // choose command $03
      } else if ( b == CIV_ADRESS ) {
        state = 6;
        rdI[2] = b;
      } else {
        state = 1;
      }; break;                       // or $05

    case 4: if ( b == CIV_ADRESS ) {
        state = 5;
        rdI[3] = b;
      } else {
        state = 1;
      }; break;                      // select command $03
    case 5: if ( b == 0x00 || b == 0x03 ) {
        state = 8;
        rdI[4] = b;
      } else {
        state = 1;
      }; break;
    case 6: if ( b == 0x00 || b == 0xE0 || b == 0xF1 ) {
        state = 7;
        rdI[3] = b;
      } else {
        state = 1;
      }; break;  // select command $05
    case 7: if ( b == 0x00 || b == 0x05 ) {
        state = 8;
        rdI[4] = b;
      } else {
        state = 1;
      }; break;
    case 8: if ( b <= 0x99 ) {
        state = 9;
        rdI[5] = b;
      } else {
        state = 1;
      }; break;
    case 9: if ( b <= 0x99 ) {
        state = 10;
        rdI[6] = b;
      } else {
        state = 1;
      }; break;
    case 10: if ( b <= 0x99 ) {
        state = 11;
        rdI[7] = b;
      } else {
        state = 1;
      }; break;
    case 11: if ( b <= 0x99 ) {
        state = 12;
        rdI[8] = b;
      } else {
        state = 1;
      }; break;
    case 12: if ( b <= 0x99 ) {
        state = 13;
        rdI[9] = b;
      } else {
        state = 1;
      }; break;
    case 13: if ( b == 0xFD ) {
        state = 1;
        rdI[10] = b;
      } else {
        state = 1;
        rdI[10] = 0;
      }; break;
  }
}

void IC_Request(AltSoftSerial* serial)
{
  byte ReadFreq[] = {0xFE, 0xFE, CIV_ADRESS, 0xE0, 0x03, 0xFD};
  for (byte i = 0; i < sizeof(ReadFreq); i++)
  {
    serial->write(ReadFreq[i]);
  }
}

unsigned long IC_Response(AltSoftSerial* serial)
{
  String response = "";
  String rdIS = "";
  long freq = 0;
  while (serial->available()) {
    char inChar = serial->read();
    icomSM(inChar);
    rdIS = "";
    if (rdI[10] == 0xFD) {                // state machine end
      for (int i = 9; i >= 5; i-- ) {
        if (rdI[i] < 10) {                                                         // leading zero
          rdIS = rdIS + 0;
        }
        rdIS = rdIS + String(rdI[i], HEX);                                           // append BCD digit from HEX variable to string
      }
      freq = rdIS.toInt();
    }
  }
  Serial.println(freq);
  return freq / 10;
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

const int ICOM =      501;

/*
   This struct represents a radio.
   It has an Id, a friendly name, and two callbacks for sending commands to the radio and receiving radio response
*/
struct Radio {
  int Id;
  char* displayname;
  RadioRequest FrequencyCommand;
  RadioResponse FrequencyResponse;
};

//the supported radios at the moment
Radio DummyRadio =        { -1,        (char*)"---",  0,           0          };
Radio Generic_FA_Radio =  {1,         (char*)"FA",           FA_Request,  FA_Response};
Radio Generic_Y_Radio =   {2,         (char*)"Y",            Y_Request,   Y_Response};

Radio FT817_Radio =       {FT817,     (char*)"FT-817",       Y_Request,   Y_Response};
Radio FT857_Radio =       {FT857,     (char*)"FT-857",       Y_Request,   Y_Response};
Radio FT897_Radio =       {FT897,     (char*)"FT-897",       Y_Request,   Y_Response};
Radio FT991_Radio =       {FT991,     (char*)"FT-991",       FA_Request,  FA_Response};

Radio TS450_Radio =       {TS450,     (char*)"TS-450",       FA_Request,  FA_Response};
Radio TS480_Radio =       {TS480,     (char*)"TS-480",       FA_Request,  FA_Response};
Radio TS590_Radio =       {TS590,     (char*)"TS-590",       FA_Request,  FA_Response};
Radio TS690_Radio =       {TS690,     (char*)"TS-690",       FA_Request,  FA_Response};
Radio TS850_Radio =       {TS850,     (char*)"TS-850",       FA_Request,  FA_Response};
Radio TS950_Radio =       {TS950,     (char*)"TS-950",       FA_Request,  FA_Response};

Radio K2_Radio =          {K2,        (char*)"K2",           FA_Request,  FA_Response};
Radio KX2_Radio =         {KX2,       (char*)"KX2",          FA_Request,  FA_Response};
Radio K3S_Radio =         {K3S,       (char*)"K3S",          FA_Request,  FA_Response};
Radio K3_Radio =          {K3,        (char*)"K3",           FA_Request,  FA_Response};
Radio KX3_Radio =         {KX3,       (char*)"KX3",          FA_Request,  FA_Response};

Radio SDR1000_Radio =     {SDR1000,   (char*)"SDR-1000",     FA_Request,  FA_Response};

Radio ICOM_Radio =      {ICOM,        (char*)"ICOM",         IC_Request,  IC_Response};



SimpleList<Radio> RadioList; //a list that holds the supported radios

/**********************************************************************************************************
   IMPORTANT: uncomment the radio you want in the list (delete // at the begining of the row)
 **********************************************************************************************************/
void InitRadioList()
{
  //Arduino nano is limited to 14 radio memories ???
  
  RadioList.reserve(20); // Pre-reserve the memory, so later the inserts and deletes will be much faster

  RadioList.push_back(FT817_Radio);
  RadioList.push_back(FT857_Radio);
  RadioList.push_back(FT897_Radio);
  RadioList.push_back(FT991_Radio);

  RadioList.push_back(TS450_Radio);
  RadioList.push_back(TS480_Radio);
  RadioList.push_back(TS590_Radio);
  RadioList.push_back(TS690_Radio);
  RadioList.push_back(TS850_Radio);
  RadioList.push_back(TS950_Radio);
  
  RadioList.push_back(K2_Radio);
  RadioList.push_back(KX2_Radio);
  RadioList.push_back(K3S_Radio);
  RadioList.push_back(K3_Radio);
  RadioList.push_back(KX3_Radio);

  RadioList.push_back(SDR1000_Radio);

  RadioList.push_back(ICOM_Radio);
}

Radio GetInterface(AltSoftSerial* serial)
{
  //TODO: figure out why it only works when Y is first and FA is second??

  //Tty "Y;" interface
  Y_Request(serial);
  delay(100);
  unsigned long y = 0;
  y = Y_Response(serial);
  if (y > 10000)
    return Generic_Y_Radio;

  //Tty "FA;" interface
  FA_Request(serial);
  delay(500);
  unsigned long fa = 0;
  fa = FA_Response(serial);
  if (fa > 0)
    return Generic_FA_Radio;

  //Tty "IC;" interface
  IC_Request(serial);
  delay(500);
  unsigned long ic = 0;
  ic = IC_Response(serial);
  if (ic > 0)
    return ICOM_Radio;


  return DummyRadio;
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

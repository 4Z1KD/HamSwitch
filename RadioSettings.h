#include "Arduino.h"
#include "UserSettings.h"
#include <SimpleList.h>


//MatchState ms; //The regex object - used for parsing the radio response
//char buf[20];//MatchState works with char* rather than Strings so this is the buffer that holds the command in a char array

//Radio Enumeration
const int ICOM = 0;
const int YAESU = 1;
const int KENWOOD = 2;
const int ELECRAFT = 3;

/*
   This struct represents a radio.
   It has an Id, some variables that holds supported commands and a matching list of regex patterns to parse the response.
*/
struct Radio {
  int Id;
  String FrequencyCommand;
  String FrequencyResponsePattern;
};

//the supported radios at the moment
Radio DummyRadio = { -1, "", ""};
Radio Kenwood = {KENWOOD, "FA;", "FA%lu"};
Radio Icom = {ICOM, "FA;", "FA%lu"};
Radio Yaesu = {YAESU, "FA;", "FA%lu"};
Radio Elecraft = {ELECRAFT, "FA;", "FA%lu"};

SimpleList<Radio> RadioList; //a list that holds the supported radios

void InitRadioList()
{
  //RadioList.reserve(4);
  RadioList.push_back(Kenwood);
  RadioList.push_back(Icom);
  RadioList.push_back(Yaesu);
  RadioList.push_back(Elecraft);
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

//********************************************************** Parser **********************************************************************************//

unsigned long ParseFrequencyResponse(String radioResponse)
{
  unsigned long i;
  Radio myRadio = GetMyRadio(); //Get a struct that represents a Radio
  String pattern = myRadio.FrequencyResponsePattern; //get the FrequencyResponsePattern from the struct
  sscanf (&radioResponse[0], &pattern[0], &i);
  return i;
}

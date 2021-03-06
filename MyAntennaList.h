#include "Arduino.h"
#include "Antenna.h"
#include <SimpleList.h>

#define NUM_OF_ANTANNAS 5 // define number of antennas - this is important for the rotary encoder to loop through the list

Antenna* AntennaList[NUM_OF_ANTANNAS];

void InitAntennaList()
{
  //************************************** Dummy Load ******************************************************//
  SimpleList<int> Dummy;
  Dummy.push_back(0);
  AntennaList[0] = new Antenna("0", Dummy, "Dummy Load");

  //***************************************** ANT 1 ********************************************************//
  SimpleList<int> ant1_BandList;
  ant1_BandList.push_back(60);
  ant1_BandList.push_back(20);
  ant1_BandList.push_back(17);
  ant1_BandList.push_back(6);
  AntennaList[1] = new Antenna("1", ant1_BandList, "Fan 60/20/17");

  //***************************************** ANT 2 ********************************************************//
  SimpleList<int> ant2_BandList;
  ant2_BandList.push_back(30);
  ant2_BandList.push_back(15);
  ant2_BandList.push_back(12);
  AntennaList[2] = new Antenna("2", ant2_BandList, "Fan 30/15/12");

  //***************************************** ANT 3 ********************************************************//
  SimpleList<int> ant3_BandList;
  ant3_BandList.push_back(80);
  AntennaList[3] = new Antenna("3", ant3_BandList, "Dipole 80");

  //***************************************** ANT 4 ********************************************************//
  SimpleList<int> ant4_BandList;
  ant4_BandList.push_back(40);
  AntennaList[4] = new Antenna("4", ant4_BandList, "Dipole 40");

  //un-comment these lines to add more antennas (do not forget to define band and to give then a name
  
  //***************************************** ANT 5 ********************************************************//
  //SimpleList<int> ant5_BandList;
  //ant5_BandList.push_back(??????);
  //AntennaList[5] = new Antenna("5", ant5_BandList, "???????");

  //***************************************** ANT 6 ********************************************************//
  //SimpleList<int> ant6_BandList;
  //ant6_BandList.push_back(??????);
  //AntennaList[6] = new Antenna("6", ant6_BandList, "???????");

  //***************************************** ANT 7 ********************************************************//
  //SimpleList<int> ant7_BandList;
  //ant7_BandList.push_back(??????);
  //AntennaList[7] = new Antenna("7", ant7_BandList, "???????");

  
}

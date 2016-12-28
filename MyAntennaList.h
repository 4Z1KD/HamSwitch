#include "Arduino.h"
#include "Antenna.h"
#include <SimpleList.h>
#define NUM_OF_ANTANNAS 8

Antenna* AntennaList[NUM_OF_ANTANNAS];

void InitAntennaList()
{
  //************************************** Dummy Load ******************************************************//
  SimpleList<int> Dummy;
  Dummy.push_back(0);
  AntennaList[0] = new Antenna("0", Dummy, "Dummy Load");

  //***************************************** ANT 1 ********************************************************//
  SimpleList<int> ant1_BandList;
  ant1_BandList.push_back(20);
  ant1_BandList.push_back(15);
  ant1_BandList.push_back(10);
  AntennaList[1] = new Antenna("1", ant1_BandList, "KLM34xa");

  //***************************************** ANT 2 ********************************************************//
  SimpleList<int> ant2_BandList;
  ant2_BandList.push_back(40);
  AntennaList[2] = new Antenna("2", ant2_BandList, "Dipole 40");

  //***************************************** ANT 3 ********************************************************//
  SimpleList<int> ant3_BandList;
  ant3_BandList.push_back(80);
  AntennaList[3] = new Antenna("3", ant3_BandList, "Dipole 80");

  //***************************************** ANT 4 ********************************************************//
  SimpleList<int> ant4_BandList;
  ant4_BandList.push_back(6);
  AntennaList[4] = new Antenna("4", ant4_BandList, "Yagi 6");

  //***************************************** ANT 5 ********************************************************//
  SimpleList<int> ant5_BandList;
  ant5_BandList.push_back(17);
  AntennaList[5] = new Antenna("5", ant5_BandList, "GP 17");

  //***************************************** ANT 6 ********************************************************//
  SimpleList<int> ant6_BandList;
  ant6_BandList.push_back(12);
  AntennaList[6] = new Antenna("6", ant6_BandList, "Delta 12");

  //***************************************** ANT 7 ********************************************************//
  SimpleList<int> ant7_BandList;
  ant7_BandList.push_back(60);
  AntennaList[7] = new Antenna("7", ant7_BandList, "Dipole 60");
}

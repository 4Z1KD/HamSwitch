#include "Arduino.h"
#include "Antenna.h"
#include <SimpleList.h>
#define NUM_OF_ANTANNA 8

Antenna* AntennaList[NUM_OF_ANTANNA];

void InitAntennaList()
{
  //************************************** Dummy Load ******************************************************//
  SimpleList<int> Dummy;
  Dummy.push_back(0);
  AntennaList[0] = new Antenna("0", Dummy, "Dummy Load");

  //***************************************** ANT 1 ********************************************************//
  SimpleList<int> ant1;
  ant1.push_back(20);
  ant1.push_back(15);
  ant1.push_back(10);
  AntennaList[1] = new Antenna("1", ant1, "KLM34xa");

  //***************************************** ANT 2 ********************************************************//
  SimpleList<int> ant2;
  ant2.push_back(40);
  AntennaList[2] = new Antenna("2", ant2, "Dipole 40");

  //***************************************** ANT 3 ********************************************************//
  SimpleList<int> ant3;
  ant3.push_back(80);
  AntennaList[3] = new Antenna("3", ant3, "Dipole 80");

  //***************************************** ANT 4 ********************************************************//
  SimpleList<int> ant4;
  ant4.push_back(6);
  AntennaList[4] = new Antenna("4", ant4, "Yagi 6");

  //***************************************** ANT 5 ********************************************************//
  SimpleList<int> ant5;
  ant5.push_back(17);
  AntennaList[5] = new Antenna("5", ant5, "GP 17");

  //***************************************** ANT 6 ********************************************************//
  SimpleList<int> ant6;
  ant6.push_back(12);
  AntennaList[6] = new Antenna("6", ant6, "Delta 12");

  //***************************************** ANT 7 ********************************************************//
  SimpleList<int> ant7;
  ant7.push_back(60);
  AntennaList[7] = new Antenna("7", ant7, "Dipole 60");
}

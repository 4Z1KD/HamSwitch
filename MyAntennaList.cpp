#include "Arduino.h"
#include "MyAntennaList.h"
#include "Antenna.h"
#include <SimpleList.h>

MyAntennaList::MyAntennaList()
{
  SimpleList<int> Dummy;
  Dummy.push_back(0);
  AntennaList[0] = new Antenna("0",Dummy,"Dummy Load");
  
  SimpleList<int> ant1;
  ant1.push_back(20);
  ant1.push_back(15);
  ant1.push_back(12);
  ant1.push_back(10);
  AntennaList[1] = new Antenna("1",ant1,"KLM34xa");
  
  SimpleList<int> ant2;
  ant2.push_back(40);
  AntennaList[2] = new Antenna("2",ant2,"Dipole 40");
  
  SimpleList<int> ant3;
  ant3.push_back(80);
  AntennaList[3] = new Antenna("3",ant3,"Dipole 80");
  
  SimpleList<int> ant4;
  ant4.push_back(6);
  AntennaList[4] = new Antenna("4",ant4,"Yagi 6");
  
  SimpleList<int> ant5;
  ant5.push_back(17);
  AntennaList[5] = new Antenna("5",ant5,"GP 17");
  
  SimpleList<int> ant6;
  ant6.push_back(-1);
  AntennaList[6] = new Antenna("6",ant6,"NC!");
  
  SimpleList<int> ant7;
  ant7.push_back(60);
  AntennaList[7] = new Antenna("7",ant7,"Dipole 60");
}

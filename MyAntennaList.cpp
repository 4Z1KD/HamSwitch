#include "Arduino.h"
#include "MyAntennaList.h"
#include "Antenna.h"

MyAntennaList::MyAntennaList()
{
  int bands0[] = {0};
  AntennaList[0] = new Antenna("0",bands0,"Dummy Load");
  int bands1[] = {20,15,12,10};
  AntennaList[1] = new Antenna("1",bands1,"KLM34xa");
  int bands2[] = {40};
  AntennaList[2] = new Antenna("2",bands2,"Dipole 40");
  int bands3[] = {80};
  AntennaList[3] = new Antenna("3",bands3,"Dipole 80");
  int bands4[] = {6};
  AntennaList[4] = new Antenna("4",bands4,"Yagi 6");
  int bands5[] = {17};
  AntennaList[5] = new Antenna("5",bands5,"GP 17");
  int bands6[] = {-1};
  AntennaList[6] = new Antenna("6",bands6,"NC!");
  int bands7[] = {60};
  AntennaList[7] = new Antenna("7",bands7,"Dipole 60");
}

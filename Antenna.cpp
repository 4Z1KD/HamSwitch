/*
  Antenna.cpp - A structure for holding antenna definition in a selector.
  Each antenna has:
  Port - the socket number it is connected to.
  Bands list - the list of bands it supports.
  Description - a friendly name e.g. 'Dipole 40'
  Created by Gil 4Z1KD, December 24, 2016.
*/

#include "Arduino.h"
#include "Antenna.h"
#include <SimpleList.h>

Antenna::Antenna(String port, SimpleList<int> bands, String description)
{
  m_Port = port;
  m_Bands = bands;
  m_Description = description;
}

String Antenna::GetPort()
{
  return m_Port;
}

SimpleList<int>* Antenna::GetBands()
{
  return &m_Bands;
}

String Antenna::GetDescription()
{
  return m_Description;
}

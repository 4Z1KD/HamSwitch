#include "Arduino.h"
#include "Antenna.h"

Antenna::Antenna(String port, int* band, String description)
{
  m_Port = port;
  m_Band = band;
  m_Description = description;
}

String Antenna::GetPort()
{
  return m_Port;
}

int* Antenna::GetBand()
{
  return m_Band;
}

String Antenna::GetDescription()
{
  return m_Description;
}

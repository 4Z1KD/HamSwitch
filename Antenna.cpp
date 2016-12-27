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

SimpleList<int> Antenna::GetBands()
{
  return m_Bands;
}

String Antenna::GetDescription()
{
  return m_Description;
}

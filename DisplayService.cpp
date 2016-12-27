#include "Arduino.h"
#include "DisplayService.h"
#include <LiquidCrystal_I2C.h>

DisplayService::DisplayService()
{

}

DisplayService::DisplayService(LiquidCrystal_I2C* lcdx)
{
  //LiquidCrystal_I2C m_lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
  m_lcd = lcdx;
}

void DisplayService::Init()
{
  m_lcd->begin(16, 2);
  m_lcd->setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  m_lcd->setBacklight(HIGH);
  m_lcd->home();
}

void DisplayService::Log(String str, int col, int row, boolean isToClear)
{
  if (isToClear)
    m_lcd->clear();
  m_lcd->setCursor(col, row);
  m_lcd->print("                ");
  m_lcd->setCursor(col, row);
  m_lcd->print(str);
}


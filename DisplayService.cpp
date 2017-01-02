/*
  DisplayService.cpp - A library for printing text to lcd
  Created by Gil 4Z1KD, December 24, 2016.
*/
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

void DisplayService::Log11(String str, int col, int row, boolean isToClear, int t_delay)
{
  if (isToClear)
    m_lcd->clear();
  m_lcd->setCursor(col, row);
  char buf[str.length() + 1];
  str.toCharArray(buf, str.length() + 1);
  for (int i = 0; i < str.length(); i++)
  {
    m_lcd->print(buf[i]);
    delay(t_delay);
  }
}

void DisplayService::Blink(int repeats)
{
  for (int i = 0; i < repeats; i++)
  {
    m_lcd->backlight();
    delay(250);
    m_lcd->noBacklight();
    delay(250);
  }
  m_lcd->backlight();
}


#ifndef DisplayService_h
#define DisplayService_h

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR  0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

class DisplayService
{
  public:
    DisplayService();
    DisplayService(LiquidCrystal_I2C* lcdx);
    void Init();
    void Log(String str, int col, int row, boolean isToClear);
    void Log11(String str, int col, int row, boolean isToClear, int t_delay);
    void DisplaySelectedBand(int band);
    void DisplaySelectedAntenna(int antenna);
    void Blink(int repeats);
  private:
    LiquidCrystal_I2C* m_lcd;
};


#endif

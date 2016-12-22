// OLD LCD Lib
//#include <LiquidCrystal.h>
//LiquidCrystal lcd( 12, 11, 10, 9, 8, 7 );// initialize the library with the numbers of the interface pins

#include <LiquidCrystal_I2C.h>
#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

void Log(String str, int col, int row, boolean isToClear);
void DisplaySelectedBand(int band);
void DisplaySelectedAntenna(int antenna);

void DisplaySelectedBand(int band)
{
  switch (band)
  {
    case 160:
      Log("160M Band", 0, 0, 0);
      break;
    case 80:
      Log("80M Band", 0, 0, 0);
      break;
    case 60:
      Log("60M Band", 0, 0, 0);
      break;
    case 40:
      Log("40M Band", 0, 0, 0);
      break;
    case 30:
      Log("30M Band", 0, 0, 0);
      break;
    case 20:
      Log("20M Band", 0, 0, 0);
      break;
    case 17:
      Log("17M Band", 0, 0, 0);
      break;
    case 15:
      Log("15M Band", 0, 0, 0);
      break;
    case 12:
      Log("12M Band", 0, 0, 0);
      break;
    case 10:
      Log("10M Band", 0, 0, 0);
      break;
    case 6:
      Log("6M Band", 0, 0, 0);
      break;
    case 2:
      Log("2M Band", 0, 0, 0);
      break;
    case 430:
      Log("70CM Band", 0, 0, 0);
      break;
    default:
      Log("Non HAM Freq", 0, 0, 0);
      break;
  }
}
void DisplaySelectedAntenna(int antenna)
{
  switch (antenna)
  {
    case 0:
      Log("Dummy Load", 0, 1, 0);
      break;
    case 1:
      Log("ANT1 - KLM34xa", 0, 1, 0);
      break;
    case 2:
      Log("ANT2 - Dipole 40", 0, 1, 0);
      break;
    case 3:
      Log("ANT3 - Dipole 80", 0, 1, 0);
      break;
    case 4:
      Log("ANT4 - Yagi 6", 0, 1, 0);
      break;
    case 5:
      Log("ANT5 - GP 17", 0, 1, 0);
      break;
    case 6:
      Log("ANT6 - NC!", 0, 1, 0);
      break;
    case 7:
      Log("ANT7 - Dipole 60", 0, 1, 0);
      break;
    default:
      Log("Dummy Load", 0, 1, 0);
      break;
  }
}

//A helper method for writing to the LCD
void Log(String str, int col, int row, boolean isToClear)
{
  if (isToClear)
    lcd.clear();
  lcd.setCursor(col, row);
  lcd.print("                ");
  lcd.setCursor(col, row);
  lcd.print(str);
  //Serial.println(str);
}

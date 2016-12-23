# HamSwitch
###Arduino based auto antenna switch

The hardware is based on an Arduino 'Uno' and a 74HC238 - 3 to 8 Line Decoder chip:<br>
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/74HC238.png?raw=true "74HC238 Chip")

The application allows the user to toggle between 2 antenna selection modes:<br>
1. Automatic + Manual selection (Radio Priority Mode)<br>
2. Manual selection (Manual Only Mode)<br>

In "Radio Priority Mode", the 3 to 8 decoder selects the relay (antenna)<br>
based on the frequency input from the radio, while maintaining the user manual selection capability.<br>
if the user turns the rotary encoder, HamSwitch will change back to "Manual only" mode.<br>

In "Manual Only Mode", the radio input is ignored (The controller does not send CAT requests) and the 3 to 8 decoder<br>
selects the relay (antenna) based on the user input from the rotary encoder.<br>

-- Emergency Button --<br>
Clicking the rotary encoder button changes to "Manual Only Mode" and selects the dummy load.<br>

Here is the main Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Main%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Main%20Schema.PNG?raw=true "HamSwitch Schema")
</a>

Here is the Realy Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Relay%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Relay%20Schema.PNG?raw=true "HamSwitch Relay Schema")
</a>

Here is the Display Service code:
```
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
```

Here is the main HamSwitch application code:
```

#include <AltSoftSerial.h>
#include "DisplayService.h"

//***************************************************** PIN Definition ***********************************************/

//This is a switch between auto and manual modes
//It is a INPUT_PULLUP pin so by default it is set to 'HIGH'
//In order to change to 'Manual' Mode -> ground this pin temporarily (through toggle switch)
int ToggleModeSwitch_PIN = 2; //Only Pin 2 or Pin 3 allowed here!

//This is an emergency DummyLoad switch
//When pressed, the antenna immediately switches to manual mode and selects the dummy load
int EmergencyDummyLoadSwitch_PIN = 3; //Only Pin 2 or Pin 3 allowed here!

//Define Decoder Pins (for Auto mode)
int Decoder1_PIN_A0 = 4;
int Decoder1_PIN_A1 = 5;
int Decoder1_PIN_A2 = 6;

//Pins 8 and 9 are used as Rx and Tx in AltSoftSerial to communicate with the radio

//Define Rotary Encoder Pins (for Manual mode)
int RotaryEncoder_PIN_CLK = 10;
int RotaryEncoder_PIN_DT = 11;

int AutoMode_RedLED_PIN = 12;
int AutoMode_GreenLED_PIN = 13;

//***********************************************************************************************************************/

//Members (This is the place to define variables with global scope used in the application )
AltSoftSerial radioSerial; //a serial port to the radio (pins: 8-RX ,9-TX)
boolean stringComplete = false; // A flag that indicate a command has arrived through the serial port
String inputString = ""; //The raw string of the command
unsigned long Frequency = 0; //The frequency after parsing
unsigned long PreviousFrequency = -1; //The frequency after parsing

byte prev_channel_value[2];
int selectedAntenna; //Holds the selected antenna, either in auto or manual mode
int prevSelectedAntenna; //Holds the previously selected antenna - it is used to check if the antenna has changed and print only in this case
bool isManualyAnttenaChanged = false;

//Since we use a physical switch for Auto/Manual mode selection we also need a debounce mechanism
long debouncing_time = 15; //Debouncing Time in Milliseconds
volatile unsigned long last_micros; //in Auto and Manual interrupts
volatile unsigned long emergencyDummyLoad_last_micros; // in emergencyDummyLoad interrupts
volatile unsigned long isr_micros; // in ISR interrupts

volatile bool isModeChanged = true; //Selection Mode change flag - used to display a short message only when the mode changes
volatile bool IsAuto = true; //Auto mode flag

unsigned long previousMillis = 0; // required for delay of CAT command
const long interval = 1000; // delay interval (milliseconds)

bool IS_DEBUG = false; //debug flag
String UserCallsign = "YOUR CALLSIGN";

//**************************************************************** setup / loop ********************************************************************//
void setup() {

  //set Decoder Pin modes
  pinMode(Decoder1_PIN_A0, OUTPUT);
  pinMode(Decoder1_PIN_A1, OUTPUT);
  pinMode(Decoder1_PIN_A2, OUTPUT);

  pinMode(RotaryEncoder_PIN_CLK, INPUT_PULLUP);
  pinMode(RotaryEncoder_PIN_DT, INPUT_PULLUP);

  pinMode(AutoMode_RedLED_PIN, OUTPUT);
  pinMode(AutoMode_GreenLED_PIN, OUTPUT);

  pinMode(ToggleModeSwitch_PIN, INPUT_PULLUP);//Allow automatic switching Pin
  pinMode(EmergencyDummyLoadSwitch_PIN, INPUT_PULLUP);//Emergency Dummy Load Pin - This is the rotary encoder switch

  //Register to interrupts
  attachInterrupt(digitalPinToInterrupt(ToggleModeSwitch_PIN), ToggleMode, FALLING);
  attachInterrupt(digitalPinToInterrupt(EmergencyDummyLoadSwitch_PIN), EmergencyDummyLoad, FALLING);

  //set the interrupt msk
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT2); //Interrupt on pin10
  PCMSK0 |= (1 << PCINT3); //Interrupt on pin11

  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home (); // go home

  radioSerial.begin(9600);
  Serial.begin(9600); // Open serial communications and wait for port to open:
  while (!Serial) {}
  Log(UserCallsign, 0, 0, 0);
  Log("Ready...", 0, 1, 0);

  IsAuto = true;
  prevSelectedAntenna = -1; //initialize with out of range value for the first print

  delay(3000);
}

void loop() {
  unsigned long currentMillis = millis();
  //----------------------------------------------------------------------------------------------------------//
  //if in auto mode, and it is time to send command (every interval) -> write the command to the radioSerial
  if (IsAuto && (currentMillis - previousMillis >= interval)) {
    previousMillis = currentMillis;
    radioSerial.println("FA;");
  }
  //----------------------------------------------------------------------------------------------------------//
  //Only alert the user if there was a change in the state
  if (isModeChanged)
  {
    isModeChanged = false; //reset the flag
    if (IsAuto)
    {
      Log("Auto Mode", 0, 0, 0); //display a message
      analogWrite(AutoMode_GreenLED_PIN, 0); //Turn green led off
      analogWrite(AutoMode_RedLED_PIN, 130); //Turn red led on
    }
    else
    {
      Log("Manual Mode Only", 0, 0, 0);
      analogWrite(AutoMode_GreenLED_PIN, 130); //Turn green led on
      analogWrite(AutoMode_RedLED_PIN, 0); //Turn red led off
    }
  }
  //----------------------------------------------------------------------------------------------------------//
  //if the antenna was manually changed -> show a short messege
  if (isManualyAnttenaChanged)
  {
    Log("Manual Selection", 0, 0, 0);
    isManualyAnttenaChanged = false;
  }
  //----------------------------------------------------------------------------------------------------------//
  //if the entire string has arrived -> parse the frequency and reset the input string
  if (stringComplete && IsAuto) //Auto mode
  {
    if (ParseRawInput())
    {
      if (Frequency != PreviousFrequency)
      {
        AutoAntennaSelector();
        PreviousFrequency = Frequency;
      }
    }
    inputString = ""; //reset the input string
    stringComplete = false; //reset the complete flag
  }
  //----------------------------------------------------------------------------------------------------------//
  //To prevent flickering, only display the selected antenna if there was a change
  if (prevSelectedAntenna != selectedAntenna)
  {
    DisplaySelectedAntenna(selectedAntenna);
    prevSelectedAntenna = selectedAntenna;
  }
  //----------------------------------------------------------------------------------------------------------//
}


//**************************************************************** Antenna Selection *****************************************************************//
void AutoAntennaSelector()
{
  //convert to Mhz (this is actually not required, but is just an example of what can be done when you have numeric value instead of string..)
  float FreqInMHZ = Frequency / 1000000.0;

  if (FreqInMHZ >= 1.8 && FreqInMHZ <= 2.0) {
    DisplaySelectedBand(160);
    SwitchTo(0);
  }
  else if (FreqInMHZ >= 3.5 && FreqInMHZ <= 3.8) {
    DisplaySelectedBand(80);
    SwitchTo(3);
  }
  else if (FreqInMHZ >= 5.35 && FreqInMHZ <= 5.4) {
    DisplaySelectedBand(60);
    SwitchTo(7);
  }
  else if (FreqInMHZ >= 7.0 && FreqInMHZ <= 7.2) {
    DisplaySelectedBand(40);
    SwitchTo(2);
  }
  else if (FreqInMHZ >= 10.1 && FreqInMHZ <= 10.15) {
    DisplaySelectedBand(30);
    SwitchTo(0);
  }
  else if (FreqInMHZ >= 14.0 && FreqInMHZ <= 14.35) {
    DisplaySelectedBand(20);
    SwitchTo(1);
  }
  else if (FreqInMHZ >= 18.06 && FreqInMHZ <= 18.17) {
    DisplaySelectedBand(17);
    SwitchTo(5);
  }
  else if (FreqInMHZ >= 21.0 && FreqInMHZ <= 21.45) {
    DisplaySelectedBand(15);
    SwitchTo(1);
  }
  else if (FreqInMHZ >= 24.89 && FreqInMHZ <= 24.99) {
    DisplaySelectedBand(12);
    SwitchTo(0);
  }
  else if (FreqInMHZ >= 28.0 && FreqInMHZ <= 29.7) {
    DisplaySelectedBand(10);
    SwitchTo(1);
  }
  else if (FreqInMHZ >= 50.0 && FreqInMHZ <= 50.4) {
    DisplaySelectedBand(6);
    SwitchTo(4);
  }
  else if (FreqInMHZ >= 144.0 && FreqInMHZ <= 146.0) {
    DisplaySelectedBand(2);
    SwitchTo(0);
  }
  else if (FreqInMHZ >= 430.0 && FreqInMHZ <= 440.0) {
    DisplaySelectedBand(430);
    SwitchTo(0);
  }
  else {
    DisplaySelectedBand(0);
    SwitchTo(0);
  }
}

//********************************************************************************************************************************************//

//This method gets 3 bits - DecoderA0,DecoderA1, and DecoderA2 and changes the Decoder pins
void SetDecoder1Pins(int DecoderA2, int DecoderA1, int DecoderA0)
{
  (DecoderA0 == 1) ? digitalWrite(Decoder1_PIN_A0, HIGH) : digitalWrite(Decoder1_PIN_A0, LOW);
  (DecoderA1 == 1) ? digitalWrite(Decoder1_PIN_A1, HIGH) : digitalWrite(Decoder1_PIN_A1, LOW);
  (DecoderA2 == 1) ? digitalWrite(Decoder1_PIN_A2, HIGH) : digitalWrite(Decoder1_PIN_A2, LOW);
}

//This method gets the antenna number (0-7) and calls 'SetDecoder1Pins' to change the correct Pins
void SwitchTo(int antenna)
{
  selectedAntenna = antenna;
  switch (antenna)
  {
    case 1:
      SetDecoder1Pins(0, 0, 1);
      break;
    case 2:
      SetDecoder1Pins(0, 1, 0);
      break;
    case 3:
      SetDecoder1Pins(0, 1, 1);
      break;
    case 4:
      SetDecoder1Pins(1, 0, 0);
      break;
    case 5:
      SetDecoder1Pins(1, 0, 1);
      break;
    case 6:
      SetDecoder1Pins(1, 1, 0);
      break;
    case 7:
      SetDecoder1Pins(1, 1, 1);
      break;
    case 0:
      SetDecoder1Pins(0, 0, 0);
      break;
    default:
      SetDecoder1Pins(0, 0, 0);
      break;
  }
}

//********************************************************** Parser **********************************************************************************//

//The command structure is: "S 6529@1537.01> FA001800000;"
//This method retrieves only the frequency
bool ParseRawInput()
{
  //Ignore non-frequency commands (if you cant find 'FA' in the command - it is not a frequency command - return false)
  if (inputString.indexOf("FA") == -1) return false;

  String FA = inputString.substring(inputString.indexOf("FA") + 2);//get the frequency value - it begins 2 characters after the index of 'FA'
  Frequency = FA.toInt();//parse to int
  return true;
}

//********************************************************* Events ***********************************************************************//

void ToggleMode()
{
  static unsigned long last_ToggleModeInterrupt_time = 0;
  unsigned long toggleModeInterrupt_time = millis();
  // If interrupts come faster than 100ms, assume it's a bounce and ignore
  if (toggleModeInterrupt_time - last_ToggleModeInterrupt_time > 500)
  {
    inputString = ""; //reset the input string
    stringComplete = false; //reset the complete flag
    isModeChanged = true;
    IsAuto = !IsAuto;
  }
  last_ToggleModeInterrupt_time = toggleModeInterrupt_time;
}

void EmergencyDummyLoad()
{
  static unsigned long last_EmergencyDummyLoadInterrupt_time = 0;
  unsigned long emergencyDummyLoadInterrupt_time = millis();
  // If interrupts come faster than 100ms, assume it's a bounce and ignore
  if (emergencyDummyLoadInterrupt_time - last_EmergencyDummyLoadInterrupt_time > 500)
  {
    isManualyAnttenaChanged = true;
    isModeChanged = true;
    IsAuto = false;
    Frequency = 0;
    PreviousFrequency = -1;
    SwitchTo(0);
  }
  last_EmergencyDummyLoadInterrupt_time = emergencyDummyLoadInterrupt_time;
}

ISR(PCINT0_vect) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 1ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 1)
  {
    //this methods find out which pin triggered the interrupt and what to do..
    /***************** interupt on pin 10 *****************************/
    //High to Low
    if (prev_channel_value[0] == 1 && !(PINB & B00000100))
    {
      prev_channel_value[0] = 0;
      if (PINB & B00001000)
      {
        selectedAntenna++;
        if (selectedAntenna == 8) selectedAntenna = 0;
      }
      else
      {
        selectedAntenna--;
        if (selectedAntenna == -1) selectedAntenna = 7;
      }
      isManualyAnttenaChanged = true;
      isModeChanged = true;
      IsAuto = false;
      SwitchTo(selectedAntenna);
    }
    //Low to High - just save the new value - the selection only changes on High to Low interrupt.
    else if (prev_channel_value[0] == 0 && PINB & B00000100)
    {
      prev_channel_value[0] = 1;
    }
  }
  last_interrupt_time = interrupt_time;
}
//********************************************************************************************************************************************//

//This method executes between every loop execution and monitors the Serial stream
//If the stream is not empty - it reads all the data from the stream until it is empty
//if it gets a semicoloumn (';') - that indicates the end of a data chunk - it sets the 'stringComplete' to true
//so the main loop can do its work only on a complete data chunks.
void serialEvent() {
  while (radioSerial.available()) {
    // get the new byte:
    char inChar = (char)radioSerial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is semicoloumn, set the flag
    if (inChar == ';') {
      inputString.trim(); //trim the string
      stringComplete = true;
    }
  }
}

void DebugDecoder()
{
  delay(200);
  SwitchTo(0);
  delay(200);
  SwitchTo(1);
  delay(200);
  SwitchTo(2);
  delay(200);
  SwitchTo(3);
  delay(200);
  SwitchTo(4);
  delay(200);
  SwitchTo(5);
  delay(200);
  SwitchTo(6);
  delay(200);
  SwitchTo(7);
  delay(200);
}
```

License: This code is FREE for private use by Amateur Radio Operators<br>
Created: December 2016<br>
Design and Code: Gil, 4Z1KD (by request from Dubi, 4Z5DZ)<br>

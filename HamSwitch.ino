/**************************************************************************
  # HamSwitch
  <h2>Arduino based auto antenna switch</h2>

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

  <h3>Emergency Button</h3>
  Clicking the rotary encoder button changes to "Manual Only Mode" and selects the dummy load.<br>

  <h3>State Persistence</h3>
  Every change in the selected antenna and in the selection mode is saved to the eeprom and loaded when HamSwitch is turned on.
  This way the user gets his HamSwitch just as it was left..

  Here is the main Schema:<br>
  <a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Main%20Schema.PNG" target="_blank">
  ![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Main%20Schema.PNG?raw=true "HamSwitch Schema")
  </a>

  Here is the Realy Schema:<br>
  <a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Relay%20Schema.PNG" target="_blank">
  ![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Relay%20Schema.PNG?raw=true "HamSwitch Relay Schema")
  </a>

**License: This code is FREE for private use by Amateur Radio Operators<br>**
  Created: December 2016<br>
  Design and Code: **_Gil, 4Z1KD_** (by request from Dubi, 4Z5DZ)<br>
**************************************************************************/

#include <AltSoftSerial.h>
#include <EEPROM.h>
#include "DisplayService.h"
#include "Antenna.h"
#include "MyAntennaList.h"
#include <LiquidCrystal_I2C.h>
#include <SimpleList.h>

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

//************************************************** Members *******************************************************/

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

LiquidCrystal_I2C lcdx(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
DisplayService dispServe(&lcdx);

//**************************************************************** setup / loop ********************************************************************//
void setup() {
  dispServe.Init();
  InitAntennaList();
  
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

  radioSerial.begin(9600);
  Serial.begin(9600); // Open serial communications and wait for port to open:
  while (!Serial) {}
  dispServe.Log(UserCallsign, 0, 0, 0);
  dispServe.Log("Ready...", 0, 1, 0);

  prevSelectedAntenna = -1; //initialize with out of range value for the first print
  GetFromMemory(); //get the value of the last antenna and the selection mode, before HamSwitch was switched off

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
    SaveModeToMemory(IsAuto);
    if (IsAuto)
    {
      dispServe.Log("Auto Mode", 0, 0, 0); //display a message
      analogWrite(AutoMode_GreenLED_PIN, 0); //Turn green led off
      analogWrite(AutoMode_RedLED_PIN, 130); //Turn red led on
    }
    else
    {
      dispServe.Log("Manual Mode Only", 0, 0, 0);
      analogWrite(AutoMode_GreenLED_PIN, 130); //Turn green led on
      analogWrite(AutoMode_RedLED_PIN, 0); //Turn red led off
    }
  }
  //----------------------------------------------------------------------------------------------------------//
  //if the antenna was manually changed -> show a short messege
  if (isManualyAnttenaChanged)
  {
    dispServe.Log("Manual Selection", 0, 0, 0);
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
  //To prevent flickering, only display the selected antenna if it was a change
  if (prevSelectedAntenna != selectedAntenna)
  {
    DisplaySelectedAntenna(selectedAntenna);
    prevSelectedAntenna = selectedAntenna;
    SwitchTo(selectedAntenna);
    SaveAntennaToMemory(selectedAntenna);
  }
  //----------------------------------------------------------------------------------------------------------//
}


//**************************************************************** Antenna Selection *****************************************************************//
void AutoAntennaSelector()
{
  //convert to Mhz (this is actually not required, but is just an example of what can be done when you have numeric value instead of string..)
  float freqInMHZ = Frequency / 1000000.0;

  int band = FrequencyToBand(freqInMHZ); //Figure out what is the band equivalent of this frequency
  if (band == 0) //make sure it is on the HAM bands
  {
    dispServe.Log("Non HAM Freq", 0, 0, 0);
    SwitchTo(0);
  }
  else //if it is
  {
    //iterate over the antenna list
    for (int i = 0; i < NUM_OF_ANTANNAS; i++) {
      SimpleList<int>* mybands = AntennaList[i]->GetBands();
      //and for each antenna, iterate over the bands it supports
      for (SimpleList<int>::iterator itr = mybands->begin(); itr != mybands->end(); ++itr)
      {
        //if you find a match i.e. the antenna supports the selected band
        if (*itr == band)
        {
          dispServe.Log(String(band) + "M Band", 0, 0, 0); //display the selected band
          SwitchTo(i); //and switch to that antenna
          return;
        }
      }
    }
  }
  dispServe.Log("No ant for " + String(band) + "M", 0, 0, 0);
  SwitchTo(0);
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
    }
    //Low to High - just save the new value - the selection only changes on High to Low interrupt.
    else if (prev_channel_value[0] == 0 && PINB & B00000100)
    {
      prev_channel_value[0] = 1;
    }
  }
  last_interrupt_time = interrupt_time;
}
//************************************************* Display ************************************************************************//

void DisplaySelectedAntenna(int antenna)
{
  dispServe.Log("ANT" + AntennaList[antenna]->GetPort() + " - " + AntennaList[antenna]->GetDescription(), 0, 1, 0);
}

//*************************************************************************************************************************//
//This method executes between every loop execution and monitors the Serial stream
//If the stream is not empty - it reads all the data from the stream until it is empty
//if it gets a semicoloumn (';') - that indicates the end of a data chunk - it sets the 'stringComplete' to true
//so the main loop can do its work only on a complete data chunks.
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is semicoloumn, set the flag
    if (inChar == ';') {
      inputString.trim(); //trim the string
      stringComplete = true;
    }
  }
}
//void serialEvent() {
//  while (radioSerial.available()) {
//    // get the new byte:
//    char inChar = (char)radioSerial.read();
//    // add it to the inputString:
//    inputString += inChar;
//    // if the incoming character is semicoloumn, set the flag
//    if (inChar == ';') {
//      inputString.trim(); //trim the string
//      stringComplete = true;
//    }
//  }
//}

void GetFromMemory()
{
  int antenna;
  bool isAuto;
  EEPROM.get( 0, antenna);
  EEPROM.get( sizeof(int), isAuto);
  if (antenna >= 0 && antenna < 8)
  {
    selectedAntenna = antenna;
  }
  IsAuto = isAuto;
}
void SaveAntennaToMemory(int antenna)
{
  EEPROM.put(0, antenna);
}
void SaveModeToMemory(bool isAuto)
{
  EEPROM.put(sizeof(int), isAuto);
}

int FrequencyToBand(float freqInMHZ)
{

  if (freqInMHZ >= 1.8 && freqInMHZ <= 2.0) {
    return 160;
  }
  else if (freqInMHZ >= 3.5 && freqInMHZ <= 3.8) {
    return 80;
  }
  else if (freqInMHZ >= 5.35 && freqInMHZ <= 5.4) {
    return 60;
  }
  else if (freqInMHZ >= 7.0 && freqInMHZ <= 7.2) {
    return 40;
  }
  else if (freqInMHZ >= 10.1 && freqInMHZ <= 10.15) {
    return 30;
  }
  else if (freqInMHZ >= 14.0 && freqInMHZ <= 14.35) {
    return 20;
  }
  else if (freqInMHZ >= 18.06 && freqInMHZ <= 18.17) {
    return 17;
  }
  else if (freqInMHZ >= 21.0 && freqInMHZ <= 21.45) {
    return 15;
  }
  else if (freqInMHZ >= 24.89 && freqInMHZ <= 24.99) {
    return 12;
  }
  else if (freqInMHZ >= 28.0 && freqInMHZ <= 29.7) {
    return 10;
  }
  else if (freqInMHZ >= 50.0 && freqInMHZ <= 50.4) {
    return 6;
  }
  else if (freqInMHZ >= 144.0 && freqInMHZ <= 146.0) {
    return 2;
  }
  else if (freqInMHZ >= 430.0 && freqInMHZ <= 440.0) {
    return 430;
  }
  else {
    return 0;
  }
}


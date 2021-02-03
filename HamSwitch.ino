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
  Last Update: Feb 2021<br>
  Design and Code: **_Gil, 4Z1KD_** (by request from Dubi, 4Z5DZ)<br>
**************************************************************************/

#include <AltSoftSerial.h>
#include <EEPROM.h>
#include "DisplayService.h"
#include "Antenna.h"
#include "MyAntennaList.h"
#include "RadioSettings.h"
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
int Decoder1_PIN_A0 = 6;
int Decoder1_PIN_A1 = 5;
int Decoder1_PIN_A2 = 4;

//Pins 8 and 9 are used as Rx and Tx in AltSoftSerial to communicate with the radio

//Define Rotary Encoder Pins (for Manual mode)
int RotaryEncoder_PIN_CLK = 10;
int RotaryEncoder_PIN_DT = 11;

int AutoMode_RedLED_PIN = 12;
int AutoMode_GreenLED_PIN = 13;

//************************************************** Members *******************************************************/

//Members (This is the place to define variables with global scope used in the application )
AltSoftSerial radioSerial; //a serial port to the radio (pins: 8-RX ,9-TX)
unsigned long Frequency = 0; //The frequency after parsing
unsigned long PreviousFrequency = -1; //The frequency after parsing

bool lastStateCLK = LOW;
bool currentStateCLK = LOW;
bool DT_State = LOW;

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

LiquidCrystal_I2C lcdx(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
DisplayService dispServe(&lcdx);
Radio myRadio;

//**************************************************************** setup / loop ********************************************************************//
void setup() {
  dispServe.Init();
  InitAntennaList();
  InitRadioList();

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
  //PCICR |= (1 << PCIE0);
  //PCMSK0 |= (1 << PCINT2); //Interrupt on pin10
  //PCMSK0 |= (1 << PCINT3); //Interrupt on pin11
  lastStateCLK = digitalRead(RotaryEncoder_PIN_CLK);

  radioSerial.begin(9600);
  Serial.begin(9600); // Open serial communications and wait for port to open:
  while (!Serial) {}
  delay(100);

  if (AUTO_DETECT_INTERFACE)
  {
    myRadio = GetInterface(&radioSerial); //try the different interfaces and select the one for the current radio
    dispServe.Blink(1);
    dispServe.Log11("HamSwitch v2.1", 0, 0, 0, 100);
    dispServe.Log11("Interface: ", 0, 1, 0, 240);
    dispServe.Log11(myRadio.displayname, 11, 1, 0, 300);
  }
  else
  {
    myRadio = GetMyRadio(); //Get a struct that represents a Radio (defined @ RadioSettings.h)
    dispServe.Blink(1);
    dispServe.Log11("HamSwitch v2.1", 0, 0, 0, 100);
    dispServe.Log11("Radio: ", 0, 1, 0, 150);
    dispServe.Log11(myRadio.displayname, 7, 1, 0, 300);
  }

  prevSelectedAntenna = -1; //initialize with out of range value for the first print
  GetFromMemory(); //get the value of the last antenna and the selection mode, before HamSwitch was switched off

  delay(1000);
}

void loop() {
  CheckForData();
  unsigned long currentMillis = millis();
  //----------------------------------------------------------------------------------------------------------//
  //if in auto mode, and it is time to send command (every interval) -> write the command to the radioSerial
  if (IsAuto && (currentMillis - previousMillis >= interval)) {
    previousMillis = currentMillis;
    RadioRequest FrequencyCommandFunction = myRadio.FrequencyCommand; //get the FrequencyCommand from the struct
    if (myRadio.Id != -1) //make sure it is not the DummyRadio
    {
      FrequencyCommandFunction(&radioSerial);
    }
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
      dispServe.Log("Manual Mode", 0, 0, 0);
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
  if (IsAuto) //Auto mode
  {
    if ((Frequency != 0) && (Frequency != PreviousFrequency)) //if the frequency is not 0 (i.e. valid) and has changed -> select the antenna..
    {
      AutoAntennaSelector();
      PreviousFrequency = Frequency;
    }
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
  updateEncoder();
}


//**************************************************************** Antenna Selection *****************************************************************//
void AutoAntennaSelector()
{
  int band = FrequencyToBand(); //Figure out what is the band equivalent of this frequency
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

//********************************************************* Events ***********************************************************************//

void ToggleMode()
{
  static unsigned long last_ToggleModeInterrupt_time = 0;
  unsigned long toggleModeInterrupt_time = millis();
  // If interrupts come faster than 100ms, assume it's a bounce and ignore
  if (toggleModeInterrupt_time - last_ToggleModeInterrupt_time > 500)
  {
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

void  updateEncoder()
{
  currentStateCLK = digitalRead(RotaryEncoder_PIN_CLK);
  if (currentStateCLK != lastStateCLK)
  {
    DT_State = digitalRead(RotaryEncoder_PIN_DT);
    if (DT_State != currentStateCLK)
    {
      selectedAntenna++;
      if (selectedAntenna == 8) selectedAntenna = 0;
    }
    else {
      selectedAntenna--;
      if (selectedAntenna == -1) selectedAntenna = 7;
    }
    lastStateCLK = currentStateCLK;
    isManualyAnttenaChanged = true;
    PreviousFrequency = -1;
    isModeChanged = true;
    IsAuto = false;
  }
}

//************************************************* Display ************************************************************************//

void DisplaySelectedAntenna(int antenna)
{
  dispServe.Log(AntennaList[antenna]->GetPort() + ") " + AntennaList[antenna]->GetDescription(), 0, 1, 0);
}

//*************************************************************************************************************************//
void CheckForData() {

  RadioResponse FrequencyResponseFunction = myRadio.FrequencyResponse; //get the FrequencyCommand from the struct
  if (myRadio.Id != -1) //make sure it is not the DummyRadio
  {
    Frequency = FrequencyResponseFunction(&radioSerial);
    //Serial.println(Frequency);
  }
}

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

int FrequencyToBand()
{

  if (Frequency >= 180000 && Frequency <= 200000) {
    return 160;
  }
  else if (Frequency >= 350000 && Frequency <= 400000) {
    return 80;
  }
  else if (Frequency >= 500000 && Frequency <= 550000) {
    return 60;
  }
  else if (Frequency >= 700000 && Frequency <= 730000) {
    return 40;
  }
  else if (Frequency >= 1000000 && Frequency <= 1030000) {
    return 30;
  }
  else if (Frequency >= 1400000 && Frequency <= 1435000) {
    return 20;
  }
  else if (Frequency >= 1800000 && Frequency <= 1817000) {
    return 17;
  }
  else if (Frequency >= 2100000 && Frequency <= 2145000) {
    return 15;
  }
  else if (Frequency >= 2489000 && Frequency <= 2500000) {
    return 12;
  }
  else if (Frequency >= 2800000 && Frequency <= 3000000) {
    return 10;
  }
  else if (Frequency >= 5000000 && Frequency <= 5040000) {
    return 6;
  }
  else if (Frequency >= 14400000 && Frequency <= 14600000) {
    return 2;
  }
  else if (Frequency >= 43000000 && Frequency <= 44000000) {
    return 430;
  }
  else {
    return 0;
  }
}

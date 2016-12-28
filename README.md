# HamSwitch
##Arduino based auto antenna switch

Hardware
Arduino 'Uno' | 74HC238 - 3 to 8 Line Decoder chip | Rotary Encoder
--- | --- | ---
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/arduino uno.png?raw=true "Arduino UNO") | ![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/74HC238.png?raw=true "74HC238 Chip") | ![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/rotary encoder.jpg?raw=true "rotary encoder")

###Application
The application allows the user to toggle between 2 antenna selection modes:<br>
1. Automatic + Manual selection (Radio Priority Mode)<br>
2. Manual selection (Manual Only Mode)<br>

In "Radio Priority Mode", the 3 to 8 decoder selects the relay (antenna)<br>
based on the frequency input from the radio, while maintaining the user manual selection capability.<br>
if the user turns the rotary encoder, HamSwitch will change back to "Manual only" mode.<br>

In "Manual Only Mode", the radio input is ignored (The controller does not send CAT requests) and the 3 to 8 decoder<br>
selects the relay (antenna) based on the user input from the rotary encoder.<br>

###Emergency Button
Clicking the rotary encoder button changes to "Manual Only Mode" and selects the dummy load.<br>

###State Persistence
Every change in the selected antenna and in the selection mode is saved to the eeprom and loaded when HamSwitch is turned on.
This way the user gets his HamSwitch just as it was left..

Here is the main Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Main%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Main%20Schema.PNG?raw=true "HamSwitch Schema")
</a>

Here is the Realy Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Relay%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Relay%20Schema.PNG?raw=true "HamSwitch Relay Schema")
</a>

##Usage
All you have to do is to define your list of antennas<br>
**Antenna has 3 parameters:**<br>
**Port** - The Socket number it is connected to. Since HamSwitch supports 8 sockets, this number is in the range of 0-7.<br>
**Bands** - The list of bands it supports.<br>
**Description** - a friendly name e.g. 'Dipole 40'.<br>

If you open **MyAntennaList.h** you'll find a declaration of an array called *AntennaList*.<br>
This is an array of *Antenna* type objects.<br>
```javascript
Antenna* AntennaList[NUM_OF_ANTANNAS];
```

You will also find a method called *InitAntennaList()* and this is the place to define your antennas.<br>
The method is devided to 8 sections - one for each antenna.<br>
The Port is simply the index of the antenna in *AntennaList* array.<br>
Port 0 is **always** the Dummy Load - **Do not change that!**<br>

In order to define the band list, just create a *SimpleList* object.<br>
Use *push_back(int)* to add the band to the list.<br>

Here is an example of how to define a 3 bands (20,15,10) KLM kt-34xa antenna on port 1:<br>

```javascript
SimpleList<int> ant1_BandList;
ant1_BandList.push_back(20);`
ant1_BandList.push_back(15);`
ant1_BandList.push_back(10);`
AntennaList[1] = new Antenna("1", ant1_BandList, "KLM34xa");
```

##License
**This code is FREE for private use by Amateur Radio Operators<br>**
Created: December 2016<br>
Design and Code: **_Gil, 4Z1KD_** (by request from Dubi, 4Z5DZ)<br>

# HamSwitch
# Arduino based auto antenna switch

The hardware is based on an Arduino 'Uno' and a 74HC238 - 3 to 8 Line Decoder chip:<br>
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/74HC238.png?raw=true "74HC238 Chip")

This application allows the user to select between 2 antenna selection modes:<br>
1. Automatic + Manual selection (Radio Priority Mode)<br>
2. Manual selection (Manual Only Mode)<br>

In "Radio Priority Mode", the 3 to 8 decoder selects the correct<br>
relay based on the frequency input from the radio, while maintaining the user manual selection capability.<br>
if the user turns the rotary encoder, the Antenna Switch will change to "Manual only" mode.<br>

In "Manual Only Mode", the radio input is ignored (The controller does not send CAT requests) and the 3 to 8 decoder<br>
selects the relay based on the user input from a rotary encoder.<br>

-- Emergency State --<br>
clicking the rotary encoder button changes to "Manual Only Mode" and selects the dummy load.<br>

Here is the main Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Main%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Main%20Schema.PNG?raw=true "HamSwitch Schema")
</a>

License: This code is FREE for private use by Amateur Radio Operators<br>
Created: December 2016<br>
Design and Code: Gil, 4Z1KD (by request from Dubi, 4Z5DZ)<br>

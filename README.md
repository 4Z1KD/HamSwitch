# HamSwitch
# Arduino based auto antenna switch

This application allows the user to select between 2 antenna selection modes:
1. Automatic + Manual selection (Radio Priority Mode)
2. Manual selection (Manual Only Mode)

In "Radio Priority Mode", the 3to8 decoder selects the correct
relay based on the frequency input from the radio, while maintaining the user manual selection capability.
if the user turns the rotary encoder, the Antenna Switch will change to "Manual only" mode.

In "Manual Only Mode", the radio input is ignored (The controller does not send CAT requests) and the 3to8 decoder
selects the relay based on the user input from a rotary encoder.

-- Emergency State --
clicking the rotary encoder button changes to "Manual Only Mode" and selects the dummy load.

License: This code is FREE for private use by Amateur Radio Operators
Created: December 2016
Design and Code: Gil, 4Z1KD (by request from Dubi, 4Z5DZ)

<meta property="og:image" content="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/HamSwitch.png" />
<h1>HamSwitch</h1>

<h2>Hardware</h2>
<table>
<tr>
  <td>Arduino<br/>'Uno'</td>
  <td>74HC238 -<br/>3 to 8 Decoder</td>
  <td>Rotary<br/>Encoder</td>
  <td>LCD</td>
  <td>DC Relay<br/>(Array of 8)</td>
</tr>
<tr>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/arduino uno.png' width='120px'></td>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/74HC238.png' width='100px'></td>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/rotary encoder.jpg' width='40px'></td>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/lcd.jpg' width='130px'></td>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/relay.png' width='90px'></td>
</tr>
</table>
<table>
<tr>
  <td>Resistors</td>
  <td>2n2222 Transistors</td>
</tr>
<tr>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/resistors.jpg' width='100px'></td>
  <td><img src='https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/transistors.jpg' width='100px'></td>
</tr>
</table>

<h2>Application</h2>
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

<h2>Schematics</h2>

Here is the main Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Main%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Main%20Schema.PNG?raw=true "HamSwitch Schema")
</a>

Here is the Realy Schema:<br>
<a href="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Relay%20Schema.PNG" target="_blank">
![Alt text](https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/Relay%20Schema.PNG?raw=true "HamSwitch Relay Schema")
</a>

<h2>Usage</h2>
All you have to do is telling HamSwitch which radio you use and define your list of antennas<br>

<h4>Selecting the radio</h4>
Just open **UserSettings.h** and set *MY_RADIO* to the right value (HamSwitch needs to know which radio you use so it can send the correct commands and correctly parse the response)

```javascript
/*
  ICOM 0
  YAESU 1
  KENWOOD 2
  ELECRAFT 3
*/
const int MY_RADIO = 2;
```

<h4>Setting the antenna list</h4>
Antenna has 3 parameters:<br>
**Port** - The Socket number it is connected to. Since HamSwitch supports 8 sockets, this number is in the range of 0-7.<br>
**Bands** - The list of bands it supports.<br>
**Description** - a friendly name e.g. 'Dipole 40'.<br>

If you open **MyAntennaList.h** you'll find a declaration of an array called *AntennaList*.<br>
This is an array of *Antenna* type objects.

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
ant1_BandList.push_back(20);
ant1_BandList.push_back(15);
ant1_BandList.push_back(10);
AntennaList[1] = new Antenna("1", ant1_BandList, "KLM34xa");
```

<h2>License</h2>
**The project and the code is FREE for non commercial use by Amateur Radio Operators**<br>

<h2>Download</h2>
<a href="https://github.com/4Z1KD/HamSwitch/archive/master.zip" target="_blank">Download HamSwitch</a>

<h2>Authors</h2>
Requirements and Analysis: **_Dubi, 4Z5DZ_**<br>
Design and Code: **_Gil, 4Z1KD_**

Created: December 2016<br>
<script>
    var x = document.getElementsByClassName("button");
    x[0].outerHTML = '<img src="https://raw.githubusercontent.com/4Z1KD/HamSwitch/master/Images/HamSwitch.png" width="156px" style="position:absolute; top:-25px; right:-70px; background:transparent">'
</script>
<style>
.inner
{
  width: 80%;
}
#main-content
{
 width: 80%;
}
#sidebar
{
float:none;
 width: 100%;
}
</style>

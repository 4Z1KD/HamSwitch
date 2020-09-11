//Select the radio identifier from the list
/*
FT817     101
FT857     102
FT897     103
FT991     104

TS450     201
TS480     202
TS590     203
TS690     204
TS850     205
TS950     206

K2        301
KX2       302
K3S       303
K3        304
KX3       305

SDR1000   401
*/


/*
 * true - the system will send the radio a set of frequency requests using all implemented interfaces
 *        and determine the correct one according to the radio response
 * 
 * false - the user must set MY_RADIO flag with the correct radio identifier from the list
 */
const bool AUTO_DETECT_INTERFACE = true;


/*
 * this flag should hold the Radio identifier from the list
 * if IS_AUTO_DETECT_INTERFACE is set to true, this flag will be ignored
 */
const int MY_RADIO = 104;

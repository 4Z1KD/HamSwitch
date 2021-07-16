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

  ICOM      501 
 */
const int MY_RADIO = 102;

/*
 * in case your radio is an ICOM you should specify the IC-V address
  IC-271        0x20
  IC-275        0x10
  IC-375        0x12
  IC-471        0x22
  IC-475        0x14
  IC-575        0x16
  IC-7000       0x70
  IC-703        0x68
  IC-705        0xA4
  IC-706        0x48
  IC-706MkII    0x4E
  IC-706MkIIG   0x58
  IC-707        0x3e
  IC-718        0x5E
  IC-7100       0x88
  IC-7200       0x76
  IC-725        0x28
  IC-726        0x30
  IC-728        0x38
  IC-729        0x3A
  IC-735        0x04
  IC-736        0x40
  IC-737        0x3C
  IC-738        0x44
  IC-7300       0x94
  IC-7400 (IC-746Pro) 0x66
  IC-7410       0x80
  IC-746        0x56
  IC-751A       0x1C
  IC-756        0x50
  IC-756Pro     0x5C
  IC-756ProII   0x64
  IC-756ProIII  0x6e
  IC-761        0x1E
  IC-765        0x2C
  IC-775        0x46
  IC-7600       0x7A
  IC-7610       0x98
  IC-7700       0x74
  IC-78         0x62
  IC-7800       0x6A
  IC-7850/51    0x8E
  IC-781        0x26
  IC-820        0x42
  IC-821        0x4C
  IC-910        0x60
  IC-9100       0x7C
  IC-970        0x2E
  IC-9700       0xA2
  IC-1271       0x24
  IC-1275       0x18
  IC-R10        0x52
  IC-R20        0x6C
  IC-R30        0x9C
  IC-R71        0x1A
  IC-R72        0x32
  IC-R75        0x5A
  IC-R7000      0x08
  IC-R7100      0x34
  IC-R8500      0x4A
  IC-R8600      0x96
  IC-R9000      0x2A
  IC-R9500      0x72
  IC-RX7        0x78
  Controller/PC 0xE0
  Mini Scout    0x94
  Optoscan 456/535  0x80
  ID-1          0x01
  ID-4100       0x9A
  ID-51         0x86
  FC-3002       0x58
 */
const int CIV_ADRESS = 0x94;

/*
 4800
 9600
 19200
 38400
 57600
 115200
 */
const int BAUD_RATE = 9600;

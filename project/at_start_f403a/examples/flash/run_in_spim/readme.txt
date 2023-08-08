/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, using on-board
  en25qh128a as spim flash, the program in bank1 will init spim. and the program
  in spim will toggle the three leds.

  executing this project£¬step should be follow.
  step : 
  add the homologous algorithm file of spim in the flash download interface.
  this demo select spim algorithm file is type2 remap_1 ext.flash
  the pins connection as follow:
  - spim sck ---> pb1
  - spim cs  ---> pa8
  - spim io0 ---> pb10
  - spim io1 ---> pb11
  - spim io2 ---> pb7
  - spim io3 ---> pb6
  for more detailed information. please refer to the application note document AN0042.
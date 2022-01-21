/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, using on-board
  en25qh128a as spim flash, the program in bank1 will init spim. and the program
  in spim will toggle the three leds.

  executing this project£¬step should be follow.
  step : add the homologous algorithm file of spim in the flash download interface.
  the pins connection as follow:
  - spim sck ---> pb1
  - spim cs  ---> pa8
  - spim io0 ---> pb10
  - spim io1 ---> pb11
  - spim io2 ---> pb7
  - spim io3 ---> pb6

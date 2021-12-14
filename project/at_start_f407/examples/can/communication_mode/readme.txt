/**
  **************************************************************************
  * @file     readme.txt 
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use the
  can communication mode. every 1s transmit one message and the led4 blink, 
  if receive a message, les2 blink(message id is 0x400) or led3 blink(message
  id is not equal to 0x400).
  set-up
  - can tx      --->   pb9
  - can rx      --->   pb8
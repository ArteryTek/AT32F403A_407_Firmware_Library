/**
  **************************************************************************
  * @file     readme.txt 
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, show the receiver
  mute mode in usart3.
  - the usart3 config own match id and then enable receiver mute.
  - the usart2 transmit string data buf(include match id, error id and some data). 
  - if usart3 only receive match id and the data after match id correctly, the
    three leds will blink.
 
  set-up
  - connect usart2 tx pin (pa2) -> usart3 rx pin (pb11)

  for more detailed information. please refer to the application note document AN0099.
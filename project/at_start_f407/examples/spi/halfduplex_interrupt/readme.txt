/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.1.0
  * @date     2022-06-09
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  halfduplex mode transfer data by interrupt mode.
  the pins connection as follow:
  - spi2 slaver               spi1 master
    pb13(sck)       <--->     pa5(sck)
    pb14(miso)      <--->     pa7(mosi)

  for more detailed information. please refer to the application note document AN0102.
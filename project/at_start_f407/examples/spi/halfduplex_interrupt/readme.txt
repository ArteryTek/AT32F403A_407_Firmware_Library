/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  halfduplex mode transfer data by interrupt mode.
  the pins connection as follow:
  - spi2 slaver               spi1 master
    pb13(sck)       <--->     pa5(sck)
    pb14(miso)      <--->     pa7(mosi)

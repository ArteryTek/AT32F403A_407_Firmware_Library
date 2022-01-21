/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  halfduplex mode transfer data by interrupt mode. use single line switch
  realize data send and receive.  
  the pins connection as follow:
  - spi2 slaver               spi1 master
    pb13(sck)       <--->     pa5(sck)
    pb14(miso)      <--->     pa7(mosi)

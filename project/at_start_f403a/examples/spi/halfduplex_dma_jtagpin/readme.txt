/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  dma transmit/recieve data. spi3 use jtag pin as spi pin.
  the pins connection as follow:
  - spi2 slaver               spi3 master
  - pb12(cs)        <--->     pa15(cs)
    pb13(sck)       <--->     pb3(sck)
    pb14(miso)      <--->     pb5(mosi)

  for more detailed information. please refer to the application note document AN0102.


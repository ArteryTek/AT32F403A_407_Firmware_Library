/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  fullduplex mode transfer data by dma.
  the pins connection as follow:
  - spi2 slave                spi1 master
    pb12(cs)        <--->     pa15(cs)
    pb13(sck)       <--->     pb3(sck)
    pb14(miso)      <--->     pb4(miso)
    pb15(mosi)      <--->     pb5(mosi)

  for more detailed information. please refer to the application note document AN0102.


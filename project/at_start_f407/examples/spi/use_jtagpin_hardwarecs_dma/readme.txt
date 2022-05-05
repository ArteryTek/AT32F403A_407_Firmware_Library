/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.9
  * @date     2022-04-25
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  dma recieve data. spi3 use jtag pin as spi pin,and config spi in hardware
  cs mode.
  the pins connection as follow:
  - spi2 slaver               spi1 master
  - pb12(cs)        <--->     pa15(cs)
    pb13(sck)       <--->     pb3(sck)
    pb14(miso)      <--->     pb5(miso)

  for more detailed information. please refer to the application note document AN0102.
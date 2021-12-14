/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  spi access the w25q flash chip.
  the pins use as follow:
  - cs        <--->   pa4(software cs, pa4 as a general io to control flash cs)
  - sck       <--->   pa5
  - miso      <--->   pa6
  - mosi      <--->   pa7
  - usart1_tx <--->   pa9

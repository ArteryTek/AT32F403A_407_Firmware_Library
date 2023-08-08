/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, provides a short
  description of how to use the jtag/swd ios as standard gpios and gives a
  configuration sequence.
  first, the swj-dp is enabled, five swd-dp pins are not toggled. if the
  pb2 is pressed the swj-dp will be disabled. the swj-dp pins are configured
  as output push-pull. the pa13(jtms/swdat), pa14(jtck/swclk), pa15(jtdi),
  pb3(jtdo) and pb4(jtrst) pins are toggled in an infinite loop.
  for more detailed information. please refer to the application note document AN0110.
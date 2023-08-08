/**
  **************************************************************************
  * @file     wdt_reset/readme.txt 
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, during normal 
  operation, the wdt count value is continuously reloaded in the main function, 
  and wdt reset will not occur at this time. when the wakeup button is pressed, 
  the function stops reloading the wdt count value, resulting in wdt reset.
  for more detailed information. please refer to the application note document AN0045.
/**
  **************************************************************************
  * @file     wwdt_reset/readme.txt 
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, during normal 
  operation, the wwdt count value is continuously reloaded in the main function, 
  and wwdt reset will not occur at this time. when the wakeup button is pressed, 
  the function stops reloading the wwdt count value, resulting in wwdt reset.

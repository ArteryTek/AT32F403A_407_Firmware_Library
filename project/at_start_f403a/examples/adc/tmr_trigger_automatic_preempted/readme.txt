/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  the preempt group automatic conversion(whitch ordinary trigger source is
  tmr).
  the convert data as follow:
  - adc1_ordinary_valuetab[n][0]  --->  adc1_channel_4
  - adc1_ordinary_valuetab[n][1]  --->  adc1_channel_5
  - adc1_ordinary_valuetab[n][2]  --->  adc1_channel_6
  - adc1_preempt_valuetab[n][0] --->  adc1_channel_7
  - adc1_preempt_valuetab[n][1] --->  adc1_channel_8
  - adc1_preempt_valuetab[n][2] --->  adc1_channel_9
  for more detailed information. please refer to the application note document AN0112.
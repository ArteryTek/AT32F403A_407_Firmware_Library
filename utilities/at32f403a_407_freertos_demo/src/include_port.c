/**
  **************************************************************************
  * @file     include_port.c
  * @brief    include_port program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

/** @addtogroup UTILITIES_examples
  * @{
  */

/** @addtogroup FreeRTOS_demo
  * @{
  */

/* support ac5 and ac6 compiler */
#if (__ARMCC_VERSION > 6000000)

#include "..\..\..\middlewares\freertos\source\portable\GCC\ARM_CM4F\port.c"

#else

#include "..\..\..\middlewares\freertos\source\portable\rvds\ARM_CM4F\port.c"

#endif

/**
  * @}
  */

/**
  * @}
  */

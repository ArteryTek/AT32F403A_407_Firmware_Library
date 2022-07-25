/**
  **************************************************************************
  * @file     main.c
  * @version  v2.1.1
  * @date     2022-07-22
  * @brief    main program
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

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_FLASH_fap_enable FLASH_fap_enable
  * @{
  */

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  flash_status_type status = FLASH_OPERATE_DONE; 
  system_clock_config();
  at32_board_init();
  if(flash_fap_status_get() == RESET)
  {
    flash_unlock();
    /* wait for operation to be completed */
    status = flash_operation_wait_for(OPERATION_TIMEOUT);
    
    if(status != FLASH_OPERATE_TIMEOUT)
    {
      if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
        flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);
      
      status = flash_fap_enable(TRUE);
      if(status == FLASH_OPERATE_DONE)
        nvic_system_reset();
    }
  }else
  {
    at32_led_on(LED2);
    at32_led_on(LED3);
    at32_led_on(LED4);
  }
  while(1)
  {
  }
}

/**
  * @}
  */

/**
  * @}
  */

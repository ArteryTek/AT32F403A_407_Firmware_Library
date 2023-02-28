/**
  **************************************************************************
  * @file     main.c
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
#include "at32_emac.h"
#include "stdio.h"
#include "netconf.h"
#include "flash.h"
#include "iap.h"
#include "tmr.h"

/** @addtogroup UTILITIES_examples
  * @{
  */

/** @addtogroup EMAC_IAP_app_led3_toggle
  * @{
  */

volatile uint32_t local_time = 0;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  error_status status;
  /* config vector table offset */
  nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x10000);

  /* config nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  system_clock_config();

  at32_board_init();
  
  uart_print_init(115200);

  delay_init();

   /* check and clear iap_upgrade_flag flag */
  iap_init();

  status = emac_system_init();

  while(status == ERROR);

  tcpip_stack_init();

  tmr_init();

  while(1)
  {
    lwip_rx_loop_handler();
    lwip_periodic_handle(local_time);
  }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  **************************************************************************
  * @file     at32f403a_407_int.c
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f403a_407_int.h"
#include "at32f403a_407_clock.h"
#include "netconf.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_EMAC_wake_on_lan
  * @{
  */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
}

/**
  * @brief  this function handles emac handler.
  * @param  none
  * @retval none
  */
void EMAC_IRQHandler(void)
{
  /* handles all the received frames */
  while(emac_received_packet_size_get() != 0)
  {
    lwip_pkt_handle();
  }

  /* clear the emac dma rx it pending bits */
  emac_dma_flag_clear(EMAC_DMA_RI_FLAG);
  emac_dma_flag_clear(EMAC_DMA_NIS_FLAG);
}

/**
  * @brief  this function handles wakeup frames and magic packets
  * @param  none
  * @retval none
  */
void EMAC_WKUP_IRQHandler(void)
{
  volatile uint32_t reg_val;

  emac_power_down_set(FALSE);
  reg_val = EMAC->pmtctrlsts;

  emac_trasmitter_enable(TRUE);
  emac_dma_operations_set(EMAC_DMA_OPS_START_STOP_TRANSMIT, TRUE);
  emac_dma_operations_set(EMAC_DMA_OPS_START_STOP_RECEIVE, TRUE);

  EMAC_DMA->sts = 0x10000000;
  exint_flag_clear(EXINT_LINE_19);
  system_clock_config();
}

/**
  * @}
  */

/**
  * @}
  */
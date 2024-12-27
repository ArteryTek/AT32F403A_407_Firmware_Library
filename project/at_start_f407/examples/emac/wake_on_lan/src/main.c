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
#include "netconf.h"
#include "ip_addr.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_EMAC_wake_on_lan EMAC_wake_on_lan
  * @{
  */

#define DELAY                            100
#define FAST                             1
#define SLOW                             4

uint8_t g_speed = FAST;
volatile uint32_t local_time = 0;
__IO uint32_t systick_index = 0;

void emac_wake_on_lan_init(void);
void system_clock_recover(void);

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  error_status status;

  system_clock_config();

  at32_board_init();
  at32_led_on(LED3);
  
  uart_print_init(115200);
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  delay_init();

  status = emac_system_init();
  while(status == ERROR);

  status = emac_speed_config(EMAC_AUTO_NEGOTIATION_OFF, EMAC_FULL_DUPLEX, EMAC_SPEED_100MBPS);
  while(status == ERROR);
  
  tcpip_stack_init();

  emac_wake_on_lan_init();

  at32_led_toggle(LED3);
  
  while(1)
  {
    /* lwip receive handle */
    lwip_rx_loop_handler();
    
    /*timeout handle*/
    lwip_periodic_handle(local_time);
  }
}

/**
  * @brief  exint line19 config. configure EMAC_WKUP in interrupt mode
  * @param  None
  * @retval None
  */
void emac_exint_config(void)
{
  exint_init_type exint_init_structure;
  
  exint_init_structure.line_select = EXINT_LINE_19;
  exint_init_structure.line_enable = TRUE;
  exint_init_structure.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_structure.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
  exint_init(&exint_init_structure);
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(EMAC_WKUP_IRQn, 0, 0);
}

/**
  * @brief  wake on lan configuration, it will be woken by magic packet
  * @param  none
  * @retval none
  */
void emac_wake_on_lan_init(void)
{
  emac_dma_operations_set(EMAC_DMA_OPS_START_STOP_TRANSMIT, FALSE);
  while(emac_dma_flag_get(EMAC_DMA_TI_FLAG) == SET);
  
  emac_trasmitter_enable(FALSE);
  emac_receiver_enable(FALSE);
  
  while(emac_dma_flag_get(EMAC_DMA_RI_FLAG) == SET);
  emac_dma_operations_set(EMAC_DMA_OPS_START_STOP_RECEIVE, FALSE);

  emac_exint_config();
  
  emac_magic_packet_enable(TRUE);

  emac_power_down_set(TRUE);

  emac_receiver_enable(TRUE);

  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE); 
  
  /* save systick register configuration */
  systick_index = SysTick->CTRL;
  systick_index &= ~((uint32_t)0xFFFFFFFE);
  
  /* disable systick */
  SysTick->CTRL &= (uint32_t)0xFFFFFFFE;
  
  pwc_voltage_regulate_set(PWC_REGULATOR_LOW_POWER);
  pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);
  
  /* restore systick register configuration */
  SysTick->CTRL |= systick_index;

  /* wait clock stable */
  delay_us(120);

  /* wake up from deep sleep mode, congfig the system clock */
  system_clock_recover();
  
  delay_ms(500);
}

/**
  * @brief  systemclock recover.
  * @param  none
  * @retval none
  */
void system_clock_recover(void)
{
  /* enable external high-speed crystal oscillator - hext */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* wait till hext is ready */
  while(crm_hext_stable_wait() == ERROR);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
}
/**
  * @}
  */

/**
  * @}
  */

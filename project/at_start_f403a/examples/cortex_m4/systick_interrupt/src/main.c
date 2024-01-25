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

/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_CORTEX_m4_systick_interrupt CORTEX_m4_systick_interrupt
  * @{
  */

#define MS_TICK                          (system_core_clock / 1000U)
#define DELAY                            200 /* 200 ms ticks */

/**
  * @brief  systick interrupt handler.
  * @param  none
  * @retval none
  */
void systick_handler(void)
{
  static uint32_t ticks = 0;

  ticks ++;

  /* toggle led */
  if(ticks > DELAY)
  {
    at32_led_toggle(LED2);
    ticks = 0;
  }
}

/**
  * @brief  config systick and enable interrupt.
  * @param  none
  * @retval none
  */
static uint32_t systick_interrupt_config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
  SysTick->VAL   = 0UL;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk |
                   SysTick_CTRL_ENABLE_Msk;
  return (0UL);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  /* config systick clock source */
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);

  /* config systick reload value and enable interrupt */
  systick_interrupt_config(MS_TICK);

  /* configure led */
  at32_led_init(LED2);

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

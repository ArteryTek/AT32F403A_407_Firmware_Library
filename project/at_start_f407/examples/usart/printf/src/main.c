/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.4
  * @date     2021-11-26
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
#include "stdio.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */
  
/** @addtogroup 407_USART_printf USART_printf
  * @{
  */

/* suport printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
  __asm (".global __use_no_semihosting\n\t");
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
  FILE __stdout;
#else
 #ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
 #endif
#endif

#if defined ( __GNUC__ ) && !defined (__clang__)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
  * @brief  retargets the c library printf function to the usart.
  * @param  none
  * @retval none
  */
PUTCHAR_PROTOTYPE
{
  while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
  usart_data_transmit(USART1, ch);
  return ch;
}

__IO uint32_t time_cnt = 0;

/**
  * @brief  initialize uart
  * @param  baudrate: uart baudrate
  * @retval none
  */
void uart_print_init(uint32_t baudrate)
{
  gpio_init_type gpio_init_struct;

  /* enable the uart1 and gpio clock */
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the uart1 tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure uart param */
  usart_init(USART1, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART1, TRUE);
  usart_enable(USART1, TRUE);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);

  /* output a message on hyperterminal using printf function */
  printf("usart printf example: retarget the c library printf function to the usart\r\n");

  while(1)
  {
    printf("usart printf counter: %u\r\n",time_cnt++);
    delay_sec(1);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

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

/** @addtogroup AT32F403A_periph_examples
  * @{
  */
  
/** @addtogroup 403A_TMR_pwm_input TMR_pwm_input
  * @{
  */

tmr_input_config_type tmr_ic_init_structure;

__IO uint16_t duty_cycle = 0;
__IO uint32_t frequency = 0;

void crm_configuration(void);
void gpio_configuration(void);
void uart_init(uint32_t baudrate);

/**
  * @brief  configure the tmr3 pins.
  * @param  none
  * @retval none
  */
void gpio_configuration(void)
{
  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
}

/**
  * @brief  configures the different peripheral clocks.
  * @param  none
  * @retval none
  */
void crm_configuration(void)
{
  /* tmr3 clock enable */
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* gpioa clock enable */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
}

/**
  * @brief  initialize print usart
  * @param  baudrate: uart baudrate
  * @retval none
  */
void uart_init(uint32_t baudrate)
{
  gpio_init_type gpio_init_struct;

  /* enable the uart1 and gpio clock */
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the uart1 tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure uart param */
  usart_init(USART1, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART1, TRUE);
  usart_enable(USART1, TRUE);
}

/* suport printf function, usemicrolib is unnecessary */
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

#ifdef __GNUC__
  /* with gcc/raisonance, small printf (option ld linker->libraries->small printf set to 'yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __gnuc__ */

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

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  /* peripheral clocks configuration */
  crm_configuration();

  /* nvic configuration */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);

  /* gpio configuration */
  gpio_configuration();

  uart_init(115200);

  tmr_input_default_para_init(&tmr_ic_init_structure);
  tmr_ic_init_structure.input_filter_value = 0;
  tmr_ic_init_structure.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_structure.input_polarity_select = TMR_INPUT_RISING_EDGE;

  tmr_pwm_input_config(TMR3, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);

  /* select the tmr3 input trigger: C2IF2 */
  tmr_trigger_input_select(TMR3, TMR_SUB_INPUT_SEL_C2DF2);

  /* select the sub mode: reset mode */
  tmr_sub_mode_select(TMR3, TMR_SUB_RESET_MODE);

  /* enable the sub sync mode */
  tmr_sub_sync_mode_set(TMR3, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(TMR3, TRUE);

  /* enable the c2 interrupt request */
  tmr_interrupt_enable(TMR3, TMR_C2_INT, TRUE);

  while(1)
  {
    printf("Frequency = %dHZ,Dutycycle = %d%%\r\n", frequency, duty_cycle);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

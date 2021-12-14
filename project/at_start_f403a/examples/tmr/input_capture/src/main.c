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
  
/** @addtogroup 403A_TMR_input_capture TMR_input_capture
  * @{
  */

gpio_init_type  gpio_init_struct = {0};
crm_clocks_freq_type crm_clocks_freq_struct = {0};
tmr_input_config_type  tmr_input_config_struct;
void delay(uint32_t time);
void uart_init(uint32_t baudrate);
__IO uint32_t tmr3freq = 0;
__IO uint32_t sys_counter = 0;
__IO uint16_t capturenumber = 0;
__IO uint16_t ic3readvalue1 = 0, ic3readvalue2 = 0;
__IO uint32_t capture = 0;

/**
  * @brief  delay function
  * @param  time:number of time to delay
  * @retval none
  */
void delay(uint32_t time)
{
  uint32_t i;

  for(i = 0; i < time; i++);
}

/**
  * @brief  initialize print usart
  * @param  baudrate: uart baudrate
  * @retval none
  */
void uart_init(uint32_t baudrate)
{
  gpio_init_type gpio_init_struct;

  /* enable the uart clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure uart tx gpio */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);
  /* configure uart rx gpio */
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_10;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure usart */
  usart_init(USART1, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_parity_selection_config(USART1, USART_PARITY_NONE);
  usart_transmitter_enable(USART1, TRUE);
  usart_receiver_enable(USART1, TRUE);
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
  * @brief  this function handles tmr3 trigger exception.
  * @param  none
  * @retval none
  */
void TMR3_GLOBAL_IRQHandler(void)
{
  if(tmr_flag_get(TMR3, TMR_C2_FLAG) != RESET)
  {
    tmr_flag_clear(TMR3, TMR_C2_FLAG);
    if(capturenumber == 0)
    {
      /* get the Input Capture value */
      ic3readvalue1 = tmr_channel_value_get(TMR3, TMR_SELECT_CHANNEL_2);
      capturenumber = 1;
    }
    else if(capturenumber == 1)
    {
      /* get the Input Capture value */
      ic3readvalue2 = tmr_channel_value_get(TMR3, TMR_SELECT_CHANNEL_2);

      /* capture computation */
      if(ic3readvalue2 > ic3readvalue1)
      {
        capture = (ic3readvalue2 - ic3readvalue1);
      }
      else
      {
        capture = ((0xFFFF - ic3readvalue1) + ic3readvalue2);
      }

      /* frequency computation */
      tmr3freq = (uint32_t) crm_clocks_freq_struct.sclk_freq / capture;
      capturenumber = 0;
    }
  }
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

  uart_init(115200);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* turn led2/led3/led4 on */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* enable tmr3/gpioa clock */
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* timer3 input pin Configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* tmr3 configuration: input capture mode
     the external signal is connected to tmr3 ch2 pin (pa.07)
     the rising edge is used as active edge,
     the tmr3 c2dt is used to compute the frequency value */

  /* tmr3 counter mode configuration */
  tmr_base_init(TMR3, 0xFFFF, 0);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);

  /* configure tmr3 channel2 to get clock signal */
  tmr_input_config_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_input_config_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_config_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_channel_init(TMR3, &tmr_input_config_struct, TMR_CHANNEL_INPUT_DIV_1);

  tmr_interrupt_enable(TMR3, TMR_C2_INT, TRUE);

  /* tmr2 trigger interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);

  /* enable tmr3 */
  tmr_counter_enable(TMR3, TRUE);

  while(1)
  {
    delay(10000);
    printf("The external signal frequece is : %d\r\n",tmr3freq);
    tmr3freq = 0;
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

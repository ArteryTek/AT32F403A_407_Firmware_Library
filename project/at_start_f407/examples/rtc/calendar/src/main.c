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
#include "rtc.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */
  
/** @addtogroup 407_RTC_calendar RTC_calendar
  * @{
  */

char const weekday_table[7][10] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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

/**
  * @brief  initialize uart1
  * @param  baudrate: uart baudrate
  * @retval none
  */
void uart_print_init(uint32_t baud_rate)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;  
  gpio_initstructure.gpio_pull           = GPIO_PULL_NONE;  
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;  
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_9;
  gpio_init(GPIOA, &gpio_initstructure);
  
  usart_init(USART1, baud_rate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_parity_selection_config(USART1, USART_PARITY_NONE);
  usart_transmitter_enable(USART1, TRUE);
  usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);
  usart_enable(USART1, TRUE);
}

/**
  * @brief  init alarm.
  * @param  none
  * @retval none
  */
void alarm_init(void)
{
  calendar_type alarm_struct;
  
  /* clear alarm flag */
  rtc_flag_clear(RTC_TA_FLAG); 
  
  /* wait for the register write to complete */
  rtc_wait_config_finish();
  
  /* configure and enable rtc interrupt */     
  nvic_irq_enable(RTC_IRQn, 0, 0);   
  
  /* enable alarm interrupt */
  rtc_interrupt_enable(RTC_TA_INT, TRUE);
  
  /* wait for the register write to complete */
  rtc_wait_config_finish();
  
  /* config alarm */
  
  alarm_struct.year  = 2021;
  alarm_struct.month = 5;
  alarm_struct.date  = 1;
  alarm_struct.hour  = 12;
  alarm_struct.min   = 0;
  alarm_struct.sec   = 5;
  
  rtc_alarm_clock_set(&alarm_struct);  
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  calendar_type time_struct;
  
  /* config nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  
  system_clock_config();
  
  at32_board_init();
  
  uart_print_init(115200);

  /* config calendar */
  time_struct.year  = 2021;
  time_struct.month = 5;
  time_struct.date  = 1;
  time_struct.hour  = 12;
  time_struct.min   = 0;
  time_struct.sec   = 0;
  rtc_init(&time_struct);
  
  /* config alarm */
  alarm_init();
  
  printf("initial ok\r\n");

  while(1)
  {
    if(rtc_flag_get(RTC_TS_FLAG) != RESET)
    {
      at32_led_toggle(LED3);
      
      /* get time */
      rtc_time_get();

      /* print time */
      printf("%d/%d/%d ", calendar.year, calendar.month, calendar.date);
      printf("%02d:%02d:%02d %s\r\n", calendar.hour, calendar.min, calendar.sec, weekday_table[calendar.week]);

      /* wait for the register write to complete */
      rtc_wait_config_finish();
      
      /* clear the rtc second flag */
      rtc_flag_clear(RTC_TS_FLAG);

      /* wait for the register write to complete */
      rtc_wait_config_finish();
    }
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

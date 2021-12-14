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

/** @addtogroup 403A_BPR_data BPR_data
  * @{
  */

#define BPR_DR_NUMBER                    42

bpr_data_type bpr_addr_tab[BPR_DR_NUMBER] =
{
  BPR_DATA1,  BPR_DATA2,  BPR_DATA3,  BPR_DATA4,  BPR_DATA5,
  BPR_DATA6,  BPR_DATA7,  BPR_DATA8,  BPR_DATA9,  BPR_DATA10,
  BPR_DATA11, BPR_DATA12, BPR_DATA13, BPR_DATA14, BPR_DATA15,
  BPR_DATA16, BPR_DATA17, BPR_DATA18, BPR_DATA19, BPR_DATA20,
  BPR_DATA21, BPR_DATA22, BPR_DATA23, BPR_DATA24, BPR_DATA25,
  BPR_DATA26, BPR_DATA27, BPR_DATA28, BPR_DATA29, BPR_DATA30,
  BPR_DATA31, BPR_DATA32, BPR_DATA33, BPR_DATA34, BPR_DATA35,
  BPR_DATA36, BPR_DATA37, BPR_DATA38, BPR_DATA39, BPR_DATA40,
  BPR_DATA41, BPR_DATA42
};

void bpr_reg_write(void);
uint8_t bpr_reg_check(void);
void uart_init(uint32_t baudrate);

/**
  * @brief  write data to bpr dt register.
  * @param  none.
  * @retval none
  */
void bpr_reg_write(void)
{
  uint32_t index = 0;

  for(index = 0; index < BPR_DR_NUMBER; index++)
  {
    bpr_data_write(bpr_addr_tab[index], 0x5A00 | bpr_addr_tab[index]);
  }
}

/**
  * @brief  checks if the bpr dt register values are correct or not.
  * @param  none.
  * @retval
  *          - TRUE: all bpr dt register values are correct
  *          - FALSE: number of the bpr register
  *            which value is not correct
  */
uint8_t bpr_reg_check(void)
{
  uint32_t index = 0;

  for(index = 0; index < BPR_DR_NUMBER; index++)
  {
    if(bpr_data_read(bpr_addr_tab[index]) != (0x5A00 | bpr_addr_tab[index]))
    {
      return FALSE;
    }
  }

  return TRUE;
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
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  uart_init(115200);

  /* enable pwc and bpr clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);

  /* enable write access to bpr domain */
  pwc_battery_powered_domain_access(TRUE);

  /* clear tamper pin event pending flag */
  bpr_flag_clear(BPR_TAMPER_EVENT_FLAG);

  /* check data from bpr dt register */
  if(bpr_reg_check() == TRUE)
  {
    printf("bpr reg => none reset\r\n");
  }
  else
  {
    printf("bpr reg => reset\r\n");
  }

  /* reset bpr domain */
  bpr_reset();

  /* write data to bpr dt register */
  bpr_reg_write();

  /* check if the written data are correct */
  if(bpr_reg_check() == TRUE)
  {
    printf("write bpr reg ok\r\n");
  }
  else
  {
    printf("write bpr reg fail\r\n");
  }

  /* disable pwc and bpr clock for power saving */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, FALSE);
  crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, FALSE);

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

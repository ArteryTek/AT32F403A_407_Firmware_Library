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
#include <stdio.h>
#include "spi_flash.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_SPI_w25q_flash SPI_w25q_flash
  * @{
  */

#define FLASH_TEST_ADDR                  0x1000
#define BUF_SIZE                         0x256

uint8_t tx_buffer[BUF_SIZE];
uint8_t rx_buffer[BUF_SIZE];
volatile error_status transfer_status = ERROR;

static void usart1_config(uint32_t baudrate);
void tx_data_fill(void);
error_status buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length);

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
  /* with gcc/raisonance, small printf (option ld linker->libraries->small printf set to 'yes') calls

__io_putchar() */
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
  * @brief  usart1 configuration.
  * @param  none
  * @retval none
  */
static void usart1_config(uint32_t baudrate)
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
  * @brief  transfer data fill.
  * @param  none
  * @retval none
  */
void tx_data_fill(void)
{
  uint32_t data_index = 0;
  for(data_index = 0; data_index < BUF_SIZE; data_index++)
  {
    tx_buffer[data_index] = data_index;
  }
}

/**
  * @brief  buffer compare function.
  * @param  pbuffer1, pbuffer2: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval the result of compare
  */
error_status buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      return ERROR;
    }

    pbuffer1++;
    pbuffer2++;
  }
  return SUCCESS;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t index = 0;
  __IO uint32_t flash_id_index = 0;
  system_clock_config();
  at32_board_init();
  tx_data_fill();
  usart1_config(115200);
  spiflash_init();
  flash_id_index = spiflash_read_id();
  if(flash_id_index != W25Q128)
  {
    printf("flash id check error!\r\n");
    for(index = 0; index < 50; index++)
    {
      at32_led_toggle(LED2);
      at32_led_toggle(LED3);
      delay_ms(200);
    }
    return 1;
  }
  else
  {
    printf("flash id check success! id: %x\r\n", flash_id_index);
  }

  /* erase sector */
  spiflash_sector_erase(FLASH_TEST_ADDR / SPIF_SECTOR_SIZE);

  /* write data */
  spiflash_write(tx_buffer, FLASH_TEST_ADDR, BUF_SIZE);

  /* read data */
  spiflash_read(rx_buffer, FLASH_TEST_ADDR, BUF_SIZE);

  /* printf read data */
  printf("Read Data: ");
  for(index = 0; index < BUF_SIZE; index++)
  {
    printf("%x ", rx_buffer[index]);
  }

  /* test result:the data check */
  transfer_status = buffer_compare(rx_buffer, tx_buffer, BUF_SIZE);

  /* test result indicate:if SUCCESS ,led2 lights */
  if(transfer_status == SUCCESS)
  {
    printf("\r\nflash data read write success!\r\n");
    at32_led_on(LED2);
  }
  else
  {
    printf("\r\nflash data read write ERROR!\r\n");
    at32_led_off(LED2);
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

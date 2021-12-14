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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "at32_sdio.h"
#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "ff.h" 

/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_SDIO_sdio_fatfs SDIO_sdio_fatfs
  * @{
  */

FATFS fs;
FIL file;
BYTE work[FF_MAX_SS];

uint8_t buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length);
static void sd_test_error(void);
static void nvic_configuration(void);

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
  * @brief  compares two buffers.
  * @param  pbuffer1, pbuffer2: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval 1: pbuffer1 identical to pbuffer2
  *         0: pbuffer1 differs from pbuffer2
  */
uint8_t buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      return 0;
    }
    pbuffer1++;
    pbuffer2++;
  }
  return 1;
}

/**
  * @brief  led2 on off every 300ms for sd test failed.
  * @param  none
  * @retval none
  */
static void sd_test_error(void)
{
  at32_led_on(LED2);
  delay_ms(300);
  at32_led_off(LED2);
  delay_ms(300);
}

/**
  * @brief  configures sdio1 irq channel.
  * @param  none
  * @retval none
  */
static void nvic_configuration(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(SDIO1_IRQn, 0, 0);
}

/**
  * @brief  fatfs file read/write test.
  * @param  none
  * @retval ERROR: fail.
  *         SUCCESS: success.
  */
static error_status fatfs_test(void)
{
  FRESULT ret; 
  char filename[] = "1:/test1.txt";
  const char wbuf[] = "this is my file for test fatfs!\r\n";
  char rbuf[50];
  UINT bytes_written = 0;
  UINT bytes_read = 0;
  DWORD fre_clust, fre_sect, tot_sect;
  FATFS* pt_fs;
  
  ret = f_mount(&fs, "1:", 1);
  if(ret){
    printf("fs mount err:%d.\r\n", ret);
    if(ret == FR_NO_FILESYSTEM){
      printf("create fatfs..\r\n");
      ret = f_mkfs("1:", 0, work, sizeof(work));
      if(ret){
        printf("creates fatfs err:%d.\r\n", ret);
        return ERROR;
      }
      else{
        printf("creates fatfs ok.\r\n");
      }
      ret = f_mount(NULL, "1:", 1);
      ret = f_mount(&fs, "1:", 1);
      if(ret){
        printf("fs mount err:%d.\r\n", ret);
        return ERROR;
      }
      else{
        printf("fs mount ok.\r\n");
      }
    }
    else{
      return ERROR;
    }
  }
  else{
    printf("fs mount ok.\r\n");
  }
  
  ret = f_open(&file, filename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
  if(ret){
    printf("open file err:%d.\r\n", ret);
  }
  else{
    printf("open file ok.\r\n");
  }
  ret = f_write(&file, wbuf, sizeof(wbuf), &bytes_written);
  if(ret){
    printf("write file err:%d.\r\n", ret);
  }
  else{
    printf("write file ok, byte:%u.\r\n", bytes_written);
  }
  f_lseek(&file, 0);
  ret = f_read(&file, rbuf, sizeof(rbuf), &bytes_read);
  if(ret){
    printf("read file err:%d.\r\n", ret);
  }
  else{
    printf("read file ok, byte:%u.\r\n", bytes_read);
  }
  ret = f_close(&file);
  if(ret){
    printf("close file err:%d.\r\n", ret);
  }
  else{
    printf("close file ok.\r\n");
  }
  
  pt_fs = &fs;
  /* get volume information and free clusters of drive 1 */
  ret = f_getfree("1:", &fre_clust, &pt_fs);
  if(ret == FR_OK)
  {
    /* get total sectors and free sectors */
    tot_sect = (pt_fs->n_fatent - 2) * pt_fs->csize;
    fre_sect = fre_clust * pt_fs->csize;

    /* print the free space (assuming 512 bytes/sector) */
    printf("%10u KiB total drive space.\r\n%10u KiB available.\r\n", tot_sect / 2, fre_sect / 2);
  }
  
  ret = f_mount(NULL, "1:", 1);
  
  if(1 == buffer_compare((uint8_t*)rbuf, (uint8_t*)wbuf, sizeof(wbuf))){
    printf("r/w file data test ok.\r\n");
  }
  else{
    printf("r/w file data test fail.\r\n");
    return ERROR;
  }
  
  return SUCCESS;
}

/* gloable functions ---------------------------------------------------------*/
/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();

  nvic_configuration();
  
  uart_print_init(115200);
  printf("start test fatfs r0.14b..\r\n");
  
  if(SUCCESS != fatfs_test())
  {
    while(1)
    {
      sd_test_error();
    }
  }
  
  /* all tests pass, led3 and led4 fresh */
  while(1)
  {
    at32_led_toggle(LED3);
    at32_led_toggle(LED4);
    delay_ms(300);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

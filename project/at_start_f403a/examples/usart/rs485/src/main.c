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

/** @addtogroup 403A_USART_rs485
  * @{
  */

#define RS485_BAUDRATE                       9600
#define RS485_BUFFER_SIZE                    128

uint8_t rs485_buffer_rx[RS485_BUFFER_SIZE];
uint8_t rs485_buffer_rx_cnt = 0;

/**
  *  @brief  rs485 configiguration.
  *  @param  none
  *  @retval none
  */
static void rs485_config(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the uart2 and gpio clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the uart2 tx,rx,de pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_2;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_3;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_bits_reset(GPIOA, GPIO_PINS_1);

  /* configure uart2 param */
  usart_init(USART2, RS485_BAUDRATE, USART_DATA_8BITS, USART_STOP_1_BIT);

  usart_flag_clear(USART2, USART_RDBF_FLAG);
  usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);

  usart_receiver_enable(USART2, TRUE);
  usart_transmitter_enable(USART2, TRUE);
  usart_enable(USART2, TRUE);

  nvic_irq_enable(USART2_IRQn, 1, 0);
}

/**
  *  @brief  rs485 send data
  *  @param  buf: pointer to the buffer that contain the data to be transferred.
  *  @param  cnt: size of buffer in bytes.
  *  @retval none
  */
static void rs485_send_data(u8* buf, u8 cnt)
{
  gpio_bits_set(GPIOA, GPIO_PINS_1);
  while(cnt--){
    while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(USART2, *buf++);
  }
  while(usart_flag_get(USART2, USART_TDC_FLAG) == RESET);
  gpio_bits_reset(GPIOA, GPIO_PINS_1);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  char str[]="start test..\r\n";
  u8 len = 0;

  system_clock_config();
  at32_board_init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  rs485_config();

  len = sizeof(str);
  rs485_send_data((u8*)str, len);
  while(1)
  {
    if(usart_flag_get(USART2, USART_IDLEF_FLAG) != RESET)
    {
      usart_data_receive(USART2);
      usart_interrupt_enable(USART2, USART_RDBF_INT, FALSE);
      rs485_send_data(rs485_buffer_rx, rs485_buffer_rx_cnt);
      rs485_buffer_rx_cnt = 0;
      usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
    }
  }
}

/**
  * @brief  usart2 interrupt handler
  * @param  none
  * @retval none
  */
void USART2_IRQHandler(void)
{
  uint16_t tmp;

  if(usart_interrupt_flag_get(USART2, USART_RDBF_FLAG) != RESET)
  {
    tmp = usart_data_receive(USART2);
    if(rs485_buffer_rx_cnt < RS485_BUFFER_SIZE)
    {
      rs485_buffer_rx[rs485_buffer_rx_cnt++] = tmp;
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */

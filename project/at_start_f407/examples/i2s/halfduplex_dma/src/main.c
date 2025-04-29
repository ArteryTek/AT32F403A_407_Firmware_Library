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
#include <stdio.h>


/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_I2S_halfduplex_dma I2S_halfduplex_dma
  * @{
  */

uint16_t i2s3_buffer_tx[32] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                               0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                               0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                               0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                               0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                               0x3D3E, 0x3F40};
uint16_t i2s2_buffer_rx[32];
volatile error_status transfer_status = ERROR;

/**
  * @brief  buffer_compare function.
  * @param  none
  * @retval the result of compare
  */
error_status buffer_compare(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length)
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
  * @brief  dma configuration.
  * @param  none
  * @retval none
  */
static void dma_config(void)
{
  dma_init_type dma_init_struct;

  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);
  
  /* use dma2_channel2 as spi3 transmit channel */
  dma_reset(DMA2_CHANNEL2);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)i2s3_buffer_tx;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI3->dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA2_CHANNEL2, &dma_init_struct);
  
  /* use dma1_channel4 as spi2 receive channel */
  dma_reset(DMA1_CHANNEL4);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)i2s2_buffer_rx;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI2->dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL4, &dma_init_struct);
}

/**
  * @brief  i2s configuration.
  * @param  none
  * @retval none
  */
static void i2s_config(void)
{
  i2s_init_type i2s_init_struct;
  
  /* master i2s initialization */
  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  i2s_default_para_init(&i2s_init_struct);
  
  /* master transmission mode */
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_32BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_MASTER_TX;
  i2s_init(SPI3, &i2s_init_struct);
  
  /* use dma transmit */
  spi_i2s_dma_transmitter_enable(SPI3, TRUE);
  
  i2s_enable(SPI3, TRUE);
  
  /* slave i2s initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  
  /* slave reception mode */
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_32BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_SLAVE_RX;
  i2s_init(SPI2, &i2s_init_struct);

  /* use dma receive */
  spi_i2s_dma_receiver_enable(SPI2, TRUE);
  
  i2s_enable(SPI2, TRUE);
}

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  gpio_pin_remap_config(SWJTAG_GMUX_010, TRUE);

  /* master i2s ws pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_15;
  gpio_init(GPIOA, &gpio_initstructure);

  /* master i2s ck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_3;
  gpio_init(GPIOB, &gpio_initstructure);

  /* master i2s sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_initstructure);

  /* master i2s mck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_7;
  gpio_init(GPIOC, &gpio_initstructure);

  /* slave i2s ws pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_12;
  gpio_init(GPIOB, &gpio_initstructure);

  /* slave i2s ck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_initstructure);

  /* slave i2s sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_15;
  gpio_init(GPIOB, &gpio_initstructure);
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
  at32_led_on(LED4);
  gpio_config();
  dma_config();
  i2s_config();
  
  /* enable i2s slave dma to get data */
  dma_channel_enable(DMA1_CHANNEL4, TRUE);
  
  /* enable i2s master dma to fill data */
  dma_channel_enable(DMA2_CHANNEL2, TRUE);

  /* wait slave i2s data receive end */
  while(dma_flag_get(DMA1_FDT4_FLAG) == RESET)
  {
  }
  
  /* wait master and slave idle when communication end */
  while(spi_i2s_flag_get(SPI3, SPI_I2S_BF_FLAG) != RESET);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);

  /* test result:the data check */
  transfer_status = buffer_compare(i2s2_buffer_rx, i2s3_buffer_tx, 32);

  /* test result indicate:if passed ,led2 lights */
  if(transfer_status==SUCCESS)
  {
    at32_led_on(LED2);
  }
  else
  {
    at32_led_on(LED3);
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

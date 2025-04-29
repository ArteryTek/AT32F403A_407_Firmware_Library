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

/** @addtogroup 403A_SPI_fullduplex_dma_jtagpin SPI_fullduplex_dma_jtagpin
  * @{
  */

#define SPI_MASTER_CS_HIGH   gpio_bits_set(GPIOA, GPIO_PINS_15)
#define SPI_MASTER_CS_LOW    gpio_bits_reset(GPIOA, GPIO_PINS_15)
#define BUFFER_SIZE          32

spi_init_type spi_init_struct;
uint8_t spi1_tx_buffer[BUFFER_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                       0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20};
uint8_t spi2_tx_buffer[BUFFER_SIZE] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                                       0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,
                                       0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                                       0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70};
uint8_t spi1_rx_buffer[BUFFER_SIZE], spi2_rx_buffer[BUFFER_SIZE];
volatile error_status transfer_status1 = ERROR, transfer_status2 = ERROR;
                                       
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
  * @brief  dma configuration.
  * @param  none
  * @retval none
  */
static void dma_config(void)
{
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  
  /* use dma1_channel1 as spi1 transmit channel */
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)spi1_tx_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI1->dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);
  dma_flexible_config(DMA1, FLEX_CHANNEL1, DMA_FLEXIBLE_SPI1_TX);
  
  /* use dma1_channel2 as spi1 receive channel */
  dma_reset(DMA1_CHANNEL2);
  dma_init_struct.buffer_size = BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)spi1_rx_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI1->dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL2, &dma_init_struct);
  dma_flexible_config(DMA1, FLEX_CHANNEL2, DMA_FLEXIBLE_SPI1_RX);
  
  crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);
  
  /* use dma2_channel1 as spi2 transmit channel */
  dma_reset(DMA2_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)spi2_tx_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI2->dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA2_CHANNEL1, &dma_init_struct);
  dma_flexible_config(DMA2, FLEX_CHANNEL1, DMA_FLEXIBLE_SPI2_TX);
  
  /* use dma2_channel2 as spi2 receive channel */
  dma_reset(DMA2_CHANNEL2);
  dma_init_struct.buffer_size = BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)spi2_rx_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI2->dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA2_CHANNEL2, &dma_init_struct);
  dma_flexible_config(DMA2, FLEX_CHANNEL2, DMA_FLEXIBLE_SPI2_RX);
}

/**
  * @brief  spi configuration.
  * @param  none
  * @retval none
  */
static void spi_config(void)
{
  /* spi master initialization */
  crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
  spi_default_para_init(&spi_init_struct);
  
  /* dual line unidirectional full-duplex mode */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_LSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI1, &spi_init_struct);
  
  /* use dma transmit and receive */
  spi_i2s_dma_transmitter_enable(SPI1, TRUE);
  spi_i2s_dma_receiver_enable(SPI1, TRUE);
  
  spi_enable(SPI1, TRUE);
  
  /* spi slave initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  
  /* dual line unidirectional full-duplex mode */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_SLAVE;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_LSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_HARDWARE_MODE;
  spi_init(SPI2, &spi_init_struct);
  
  /* use dma transmit and receive */
  spi_i2s_dma_transmitter_enable(SPI2, TRUE);
  spi_i2s_dma_receiver_enable(SPI2, TRUE);
  
  spi_enable(SPI2, TRUE);
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
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  gpio_pin_remap_config(SWJTAG_MUX_010, TRUE);
  gpio_pin_remap_config(SPI1_MUX_01, TRUE);
  gpio_default_para_init(&gpio_initstructure);
  
  /* spi master gpio initialization */
  /* spi1 cs pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_15;
  gpio_init(GPIOA, &gpio_initstructure);
  
  /* non communication time: master pull up CS pin release slave */
  SPI_MASTER_CS_HIGH;
  
  /* spi1 sck pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_3;
  gpio_init(GPIOB, &gpio_initstructure);

  /* spi1 miso pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;
  gpio_init(GPIOB, &gpio_initstructure);

  /* spi1 mosi pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_initstructure);
  
  /* spi2 gpio initialization */
  /* spi2 cs pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_12;
  gpio_init(GPIOB, &gpio_initstructure);

  /* spi2 sck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_initstructure);

  /* spi2 miso pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_14;
  gpio_init(GPIOB, &gpio_initstructure);

  /* spi2 mosi pin */
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
  __IO uint32_t index = 0;
  system_clock_config();
  at32_board_init();
  at32_led_on(LED4);
  dma_config();
  gpio_config();
  spi_config();
  
  /* start communication: master pull down CS pin select slave */
  SPI_MASTER_CS_LOW;
  
  /* enable spi slave dma to fill and get data */
  dma_channel_enable(DMA2_CHANNEL1, TRUE);
  dma_channel_enable(DMA2_CHANNEL2, TRUE);
  
  /* enable spi master dma to fill and get data */
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
  dma_channel_enable(DMA1_CHANNEL2, TRUE);
  
  /* wait master and slave spi data receive end */
  while(dma_flag_get(DMA1_FDT2_FLAG) == RESET)
  {
  }
  while(dma_flag_get(DMA2_FDT2_FLAG) == RESET)
  {
  }
  
  /* wait master and slave idle when communication end */
  while(spi_i2s_flag_get(SPI1, SPI_I2S_BF_FLAG) != RESET);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);
  
  /* end communication: master pull up CS pin release slave */
  SPI_MASTER_CS_HIGH;

  /* test result:the data check */
  transfer_status1 = buffer_compare(spi2_rx_buffer, spi1_tx_buffer, BUFFER_SIZE);
  transfer_status2 = buffer_compare(spi1_rx_buffer, spi2_tx_buffer, BUFFER_SIZE);

  /* test result indicate:if SUCCESS ,led2 lights */
  if((transfer_status1 == SUCCESS) && (transfer_status2 == SUCCESS))
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

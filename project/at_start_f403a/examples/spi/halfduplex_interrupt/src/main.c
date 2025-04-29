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

/** @addtogroup 403A_SPI_halfduplex_interrupt SPI_halfduplex_interrupt
  * @{
  */

#define SPI_MASTER_CS_HIGH   gpio_bits_set(GPIOA, GPIO_PINS_4)
#define SPI_MASTER_CS_LOW    gpio_bits_reset(GPIOA, GPIO_PINS_4)
#define BUFFER_SIZE          32

spi_init_type spi_init_struct;

uint8_t spi1_tx_buffer[BUFFER_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                       0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20};
uint8_t spi2_rx_buffer[BUFFER_SIZE];
volatile uint32_t tx_index = 0, rx_index = 0;
volatile error_status transfer_status = ERROR;

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
  * @brief  spi configuration.
  * @param  none
  * @retval none
  */
static void spi_config(void)
{
  spi_init_type spi_init_struct;
  
  /* master spi initialization */
  crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(SPI1_IRQn, 0, 0);
  spi_default_para_init(&spi_init_struct);
  
  /* single line bidirectional half duplex mode - transmitting */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI1, &spi_init_struct);
  
  /* enable transmit data buffer empty interrupt */
  spi_i2s_interrupt_enable(SPI1, SPI_I2S_TDBE_INT, TRUE);
  
  /* slave spi initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(SPI2_I2S2EXT_IRQn, 0, 0);

  /* single line bidirectional half duplex mode - receiving */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_RX;
  spi_init_struct.master_slave_mode =SPI_MODE_SLAVE;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_HARDWARE_MODE;
  spi_init(SPI2, &spi_init_struct);
  
  /* enable receive data buffer full interrupt */
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_RDBF_INT, TRUE);
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
  
  gpio_default_para_init(&gpio_initstructure);
  
  /* master spi cs pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;
  gpio_init(GPIOA, &gpio_initstructure);
  
  /* non communication time: master pull up CS pin release slave */
  SPI_MASTER_CS_HIGH;

  /* master spi sck pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_5;
  gpio_init(GPIOA, &gpio_initstructure);

  /* master spi mosi pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_7;
  gpio_init(GPIOA, &gpio_initstructure);
  
  /* slave spi cs pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_12;
  gpio_init(GPIOB, &gpio_initstructure);
  
  /* slave spi sck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_initstructure);

  /* slave spi miso pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_14;
  gpio_init(GPIOB, &gpio_initstructure);
}

/**
  * @brief  spi1 interrupt function
  * @param  none
  * @retval none
  */
 void SPI1_IRQHandler(void)
{
  if(spi_i2s_interrupt_flag_get(SPI1, SPI_I2S_TDBE_FLAG) != RESET)
  {
    spi_i2s_data_transmit(SPI1, spi1_tx_buffer[tx_index++]);
    if(tx_index == BUFFER_SIZE)
    {
      spi_i2s_interrupt_enable(SPI1, SPI_I2S_TDBE_INT, FALSE);
    }
  }
}

/**
  * @brief  spi2 interrupt function
  * @param  none
  * @retval none
  */
 void SPI2_I2S2EXT_IRQHandler(void)
{
  if(spi_i2s_interrupt_flag_get(SPI2, SPI_I2S_RDBF_FLAG) != RESET)
  {
    spi2_rx_buffer[rx_index++] = spi_i2s_data_receive(SPI2);
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
  at32_led_on(LED4);
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  gpio_config();
  /* config spi1 send spi2 receive */
  spi_config();
  
  /* start communication: master pull down CS pin select slave */
  SPI_MASTER_CS_LOW;
  
  spi_enable(SPI2, TRUE);
  spi_enable(SPI1, TRUE);
  
  /* wait slave data receive end */
  while(rx_index < BUFFER_SIZE);
  
  /* wait master and slave idle when communication end */
  while(spi_i2s_flag_get(SPI1, SPI_I2S_BF_FLAG) != RESET);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);
  
  /* end communication: master pull up CS pin release slave */
  SPI_MASTER_CS_HIGH;

  /* test result:the data check */
  transfer_status = buffer_compare(spi2_rx_buffer, spi1_tx_buffer, BUFFER_SIZE);

  /* test result indicate:if SUCCESS ,led2 lights */
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

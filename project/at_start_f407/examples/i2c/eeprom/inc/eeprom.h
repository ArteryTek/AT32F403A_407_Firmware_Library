/**
  **************************************************************************
  * @file     eeprom.h
  * @brief    i2c eeprom libray header file
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
  
#ifndef __EERPOM_H
#define __EERPOM_H

#include "i2c_application.h"

#define EEPROM_BUSY_TIMEOUT              1000 /*!< eeprom busy waiting timeout */
#define EEPROM_PAGE_SIZE                 8    /*!< eeprom page size */
#define EEPROM_I2C_ADDRESS               0xA0 /*!< eeprom i2c address */

typedef enum
{
  EE_MODE_POLL                           = 0x01, /*!< polling communication */
  EE_MODE_INT                            = 0x02, /*!< interrupt communication */
  EE_MODE_DMA                            = 0x03, /*!< dma communication */
} eeprom_mode_type;

i2c_status_type eeprom_write_buffer(i2c_handle_type* hi2c, eeprom_mode_type mode, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout);
i2c_status_type eeprom_read_buffer (i2c_handle_type* hi2c, eeprom_mode_type mode, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout);

#endif

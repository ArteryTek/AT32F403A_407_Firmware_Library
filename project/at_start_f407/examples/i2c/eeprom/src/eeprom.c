/**
  **************************************************************************
  * @file     eeprom.c
  * @brief    the driver library of the i2c eeprom
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
#include "eeprom.h"

/**
  * @brief  check if the eeprom device is ready 
  * @param  hi2c: the handle points to the operation information.
  * @param  address: eeprom address.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type eeprom_ready_check(i2c_handle_type* hi2c, uint16_t address, uint32_t timeout)
{
  /* wait for the busy flag to be reset */
  if (i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_1;
  }
  
  /* ack acts on the current byte */
  i2c_master_receive_ack_set(hi2c->i2cx, I2C_MASTER_ACK_CURRENT);
 
  /* generate start condtion */
  i2c_start_generate(hi2c->i2cx);

  /* wait for the start flag to be set */
  if(i2c_wait_flag(hi2c, I2C_STARTF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    /* generate stop condtion */
    i2c_stop_generate(hi2c->i2cx);
    
    return I2C_ERR_STEP_2;
  }

  /* send slave address */
  i2c_7bit_address_send(hi2c->i2cx, address, I2C_DIRECTION_TRANSMIT);
  
  /* wait for the addr7 flag to be set */
  if(i2c_wait_flag(hi2c, I2C_ADDR7F_FLAG, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_3;
  }
  
  /* clear addr flag */
  i2c_flag_clear(hi2c->i2cx, I2C_ADDR7F_FLAG);
  
  /* generate stop condtion */
  i2c_stop_generate(hi2c->i2cx);

  /* wait for the busy flag to be reset */
  if(i2c_wait_flag(hi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK)
  {
    return I2C_ERR_STEP_4;
  }  

  return I2C_OK;  
}

/**
  * @brief  wait for eeprom device to be ready 
  * @param  hi2c: the handle points to the operation information.
  * @param  address: eeprom address.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type eeprom_ready_wait(i2c_handle_type* hi2c, uint16_t address, uint32_t timeout)
{
  uint32_t i = 0;
 
  for(i = 0; i < EEPROM_BUSY_TIMEOUT; i++)
  {
    if(eeprom_ready_check(hi2c, address, timeout) == I2C_OK)
    {
      return I2C_OK;
    }
  }
  
  return I2C_ERR_TIMEOUT;
}


/**
  * @brief  write data to the eeprom page.
  * @param  hi2c: the handle points to the operation information.
  * @param  mode: transfer mode.
  *         this parameter can be one of the following values:
  *         - EE_MODE_POLL: poll mode
  *         - EE_MODE_INT: interrupt mode
  *         - EE_MODE_DMA: dma mode
  * @param  mem_address_width: memory address width.
  *         this parameter can be one of the following values:
  *         - I2C_MEM_ADDR_WIDIH_8: memory address is 8 bit 
  *         - I2C_MEM_ADDR_WIDIH_16: memory address is 16 bit 
  * @param  address: eeprom address.
  * @param  mem_address: memory address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type eeprom_write_page(i2c_handle_type* hi2c, eeprom_mode_type mode, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  i2c_status_type status;
  
  /* write data to eeprom */
  if(mode == EE_MODE_POLL)     
  {
    return i2c_memory_write(hi2c, mem_address_width, address, mem_address, pdata, size, timeout);
  }
  else if(mode == EE_MODE_INT)
  {
    status = i2c_memory_write_int(hi2c, mem_address_width, address, mem_address, pdata, size, timeout);
    
    if(status != I2C_OK)
    {
      return status;
    }
    
    /* wait for the communication to end */
    return i2c_wait_end(hi2c, timeout);
  }  
  else
  {
    status = i2c_memory_write_dma(hi2c, mem_address_width, address, mem_address, pdata, size, timeout);
    
    if(status != I2C_OK)
    {
      return status;
    }
    
    /* wait for the communication to end */
    return i2c_wait_end(hi2c, timeout);
  }
}

/**
  * @brief  read data from eeprom.
  * @param  hi2c: the handle points to the operation information.
  * @param  mode: transfer mode.
  *         this parameter can be one of the following values:
  *         - EE_MODE_POLL: poll mode
  *         - EE_MODE_INT: interrupt mode
  *         - EE_MODE_DMA: dma mode
  * @param  mem_address_width: memory address width.
  *         this parameter can be one of the following values:
  *         - I2C_MEM_ADDR_WIDIH_8: memory address is 8 bit 
  *         - I2C_MEM_ADDR_WIDIH_16: memory address is 16 bit 
  * @param  address: eeprom address.
  * @param  mem_address: memory address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type eeprom_read_buffer(i2c_handle_type* hi2c, eeprom_mode_type mode, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  i2c_status_type status;
  
  /* write data to eeprom */
  if(mode == EE_MODE_POLL)     
  {
    return i2c_memory_read(hi2c, mem_address_width, address, mem_address, pdata, size, timeout);
  }
  else if(mode == EE_MODE_INT)
  {
    status = i2c_memory_read_int(hi2c, mem_address_width, address, mem_address, pdata, size, timeout);
    
    if(status != I2C_OK)
    {
      return status;
    }
    
    /* wait for the communication to end */
    return i2c_wait_end(hi2c, timeout);
  }  
  else
  {
    status = i2c_memory_read_dma(hi2c, mem_address_width, address, mem_address, pdata, size, timeout);
    
    if(status != I2C_OK)
    {
      return status;
    }
    
    /* wait for the communication to end */
    return i2c_wait_end(hi2c, timeout);
  }
}

/**
  * @brief  write data to the eeprom device.
  * @param  hi2c: the handle points to the operation information.
  * @param  mode: transfer mode.
  *         this parameter can be one of the following values:
  *         - EE_MODE_POLL: poll mode
  *         - EE_MODE_INT: interrupt mode
  *         - EE_MODE_DMA: dma mode
  * @param  mem_address_width: memory address width.
  *         this parameter can be one of the following values:
  *         - I2C_MEM_ADDR_WIDIH_8: memory address is 8 bit 
  *         - I2C_MEM_ADDR_WIDIH_16: memory address is 16 bit 
  * @param  address: eeprom address.
  * @param  mem_address: memory address.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
i2c_status_type eeprom_write_buffer(i2c_handle_type* hi2c, eeprom_mode_type mode, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout)
{
  uint16_t page_number = 0; 
  uint16_t page_last = 0; 
  uint16_t single_write = 0;    
  uint16_t write_address = 0;  
  i2c_status_type status;
  
  /* calculate the number of data programred for the first time, and the subsequent program address will be aligned */
  single_write = mem_address / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE + EEPROM_PAGE_SIZE - mem_address;
  
  /* first program address */
  write_address = mem_address;
  
  if (size < single_write)
  {
    single_write = size;
  }
  
  /* write data to eeprom */
  status = eeprom_write_page(hi2c, mode, mem_address_width, address, write_address, pdata, single_write, timeout);
  
  if(status != I2C_OK)
  {
    return status;
  }

  /* wait for eeprom device to be ready */
  if(eeprom_ready_wait(hi2c, address, timeout) != I2C_OK)
  {
    return I2C_ERR_TIMEOUT;
  }
  
  /* address increase */  
  write_address += single_write;
  
  /* subtract the number of data programed for the first time */
  size -= single_write;
  
  /* pointer increase */  
  pdata += single_write;
  
  /* if the remaining quantity is 0, it means the program is complete */
  if (size == 0)
  {
    return I2C_OK;
  }
  
  /* calculate the number of bytes in less than one page */
  page_last = size % EEPROM_PAGE_SIZE;
    
  /* calculate the number of full page */
  page_number = size / EEPROM_PAGE_SIZE;
  
  /* program an integer number of pages of data */
  while(page_number > 0)
  {
    /* write data to eeprom */
    status = eeprom_write_page(hi2c, mode, mem_address_width, address, write_address, pdata, EEPROM_PAGE_SIZE, timeout);
    
    if(status != I2C_OK)
    {
      return status;
    }
    
    /* wait for eeprom device to be ready */
    if(eeprom_ready_wait(hi2c, address, timeout) != I2C_OK)
    {
      return I2C_ERR_TIMEOUT;
    }
    
    /* page number subtract */  
    page_number--;
    
    /* address increase */  
    write_address += EEPROM_PAGE_SIZE;

    /* pointer increase */  
    pdata += EEPROM_PAGE_SIZE;
  }
  
  /* programring less than one page of data */
  if (page_last)
  {
    /* write data to eeprom */
    status = eeprom_write_page(hi2c, mode, mem_address_width, address, write_address, pdata, page_last, timeout);
    
    if(status != I2C_OK)
    {
      return status;
    }

    /* wait for eeprom device to be ready */
    if(eeprom_ready_wait(hi2c, address, timeout) != I2C_OK)
    {
      return I2C_ERR_TIMEOUT;
    }
  }
  
  return I2C_OK;
}

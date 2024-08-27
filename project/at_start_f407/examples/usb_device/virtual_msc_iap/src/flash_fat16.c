/**
  **************************************************************************
  * @file     flash_fat16.c
  * @brief    fat16 file system
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
#include "flash_fat16.h"
#include "usb_conf.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_USB_device_virtual_msc_iap
  * @{
  */
#define FAT16_SECTOR_SIZE                62
#define FAT16_CLUSTER_SIZE               (4 * 0x800)
#define FAT16_FIRST_CLUSTER              (8 * 0x800)
static uint8_t fat16_sector[FAT16_SECTOR_SIZE] = 
{
  0xEB,  /*0*/
  0x3C,  /*1*/
  0x90,  /*2*/
  0x4D,  /*3*/
  0x53,  /*4*/
  0x44,  /*5*/
  0x4F,  /*6*/
  0x53,  /*7*/
  0x35,  /*8*/
  0x2E,  /*9*/
  0x30,  /*10*/
  0x00,  /*11*/
  0x08,  /*12*/ //2K
  0x04,  /*13*/
  0x06,  /*14*/
  0x00,  /*15*/

  0x02,  /*16*/
  0x00,  /*17*/
  0x02,  /*18*/
  0xFF,  /*19*/
  0x0F,  /*20*/
  0xF8,  /*21*/
  0x01,  /*22*/
  0x00,  /*23*/
  0x01,  /*24*/
  0x00,  /*25*/
  0x01,  /*26*/
  0x00,  /*27*/
  0x00,  /*28*/
  0x00,  /*29*/
  0x00,  /*30*/
  0x00,  /*31*/

  0x00,  /*32*/
  0x00,  /*33*/
  0x00,  /*34*/
  0x00,  /*35*/
  0x00,  /*36*/
  0x00,  /*37*/
  0x29,  /*38*/
  0x96,  /*39*/
  0x16,  /*40*/
  0x66,  /*41*/
  0xD3,  /*42*/
  0x20,  /*43*/
  0x20,  /*44*/
  0x20,  /*45*/
  0x20,  /*46*/
  0x20,  /*47*/

  0x20,  /*48*/
  0x20,  /*49*/
  0x20,  /*50*/
  0x20,  /*51*/
  0x20,  /*52*/
  0x20,  /*53*/
  0x46,  /*54*/
  0x41,  /*55*/
  0x54,  /*56*/
  0x31,  /*57*/
  0x36,  /*58*/
  0x20,  /*59*/
  0x20,  /*60*/
  0x20   /*61*/
};



const uint8_t fat16_root_dir_sector[FAT16_DIR_SIZE]=
{
  0x20,                                  /*11 - Archive Attribute set */
  0x00,                                  /*12 - Reserved */
  0x4B,                                  /*13 - Create Time Tenth */
  0x9C,                                  /*14 - Create Time */
  0x42,                                  /*15 - Create Time */
  0x92,                                  /*16 - Create Date */
  0x38,                                  /*17 - Create Date */
  0x92,                                  /*18 - Last Access Date */
  0x38,                                  /*19 - Last Access Date */
  0x00,                                  /*20 - Not used in FAT16 */
  0x00,                                  /*21 - Not used in FAT16 */
  0x9D,                                  /*22 - Write Time */
  0x42,                                  /*23 - Write Time */
  0x92,                                  /*24 - Write Date */
  0x38,                                  /*25 - Write Date */
  0x00,                                  /*26 - First Cluster (none, because file is empty) */
  0x00,                                  /*27 - First Cluster (none, because file is empty) */
  0x00,                                  /*28 - File Size */
  0x00,                                  /*29 - File Size */
  0x00,                                  /*30 - File Size */
  0x00,                                  /*31 - File Size */
  'A','T','3','2',' ','I','A','P',' ',' ',' ',  /*32-42 - Volume label */
  0x08,                                  /*43 - File attribute = Volume label */
  0x00,                                  /*44 - Reserved */
  0x00,                                  /*45 - Create Time Tenth */
  0x00,                                  /*46 - Create Time */
  0x00,                                  /*47 - Create Time */
  0x00,                                  /*48 - Create Date */
  0x00,                                  /*49 - Create Date */
  0x00,                                  /*50 - Last Access Date */
  0x00,                                  /*51 - Last Access Date */
  0x00,                                  /*52 - Not used in FAT16 */
  0x00,                                  /*53 - Not used in FAT16 */
  0x9D,                                  /*54 - Write Time */
  0x42,                                  /*55 - Write Time */
  0x92,                                  /*56 - Write Date */
  0x38,                                  /*57 - Write Date */
};

const uint8_t fat16_table_sector0[FAT16_TABLE_SIZE] =
{
  0xF8,
  0xFF,
  0xFF,
  0xFF
};

uint8_t fat16_file_name[FAT16_FILENAME_SIZE] =
{
  'R',
  'e',
  'a',
  'd',
  'y',
  ' ',
  ' ',
  ' ',
  'T',
  'X',
  'T'
};

fat_dir_type g_file_attr;
flash_iap_type flash_iap;
uint32_t file_write_nr = 0;
uint8_t file_match = 0;

uint32_t flash_fat16_boot_dir_write(uint32_t fat_lbk, uint8_t *data, uint32_t len);
uint32_t flash_fat16_sector_write(uint32_t fat_lbk, uint8_t *data, uint32_t len);
uint32_t fat16_memory_copy(uint8_t *dst, const uint8_t *src, uint32_t len);
uint32_t fat16_memory_memset(uint8_t *dst, uint32_t set,  uint32_t len);
uint32_t flash_write_data(uint32_t address, uint8_t *data, uint32_t len);
uint32_t flash_read_data(uint32_t address, uint8_t *data, uint32_t len);
uint32_t fat16_memory_cmp(uint8_t *dst, uint8_t *src,  uint32_t len);
uint32_t flash_crc_check(uint32_t address, uint8_t *data, uint32_t len);
void flash_fat16_clear_upgrade_flag(void);
void flash_fat16_set_upgrade_flag(void);

/**
  * @brief  memory copy function
  * @param  dst: dest buffer pointer
  * @param  src: src buffer pointer
  * @param  len: copy length
  * @retval nubmer of data copy
  */
uint32_t fat16_memory_copy(uint8_t *dst, const uint8_t *src, uint32_t len)
{
  uint32_t i_index;
  for(i_index = 0; i_index < len; i_index ++)
  {
    dst[i_index] = src[i_index];
  }
  return i_index;
}

/**
  * @brief  memset function
  * @param  dst: dest buffer pointer
  * @param  set: set value
  * @param  len: set length
  * @retval nubmer of data set
  */
uint32_t fat16_memory_memset(uint8_t *dst, uint32_t set,  uint32_t len)
{
  uint32_t i_index;
  for(i_index = 0; i_index < len; i_index ++)
  {
    dst[i_index] = (uint8_t)set;
  }
  return i_index;
}

/**
  * @brief  memory compare function
  * @param  dst: dest buffer pointer
  * @param  src: src buffer pointer
  * @param  len: compare length
  * @retval compare result
  */
uint32_t fat16_memory_cmp(uint8_t *dst, uint8_t *src,  uint32_t len)
{
  uint32_t i_index;
  for(i_index = 0; i_index < len; i_index ++)
  {
    if(dst[i_index] != src[i_index])
      return 1;
  }
  return 0;
}

/**
  * @brief  clear iap upgrade flag
  * @param  none
  * @retval none
  */
void flash_fat16_clear_upgrade_flag(void)
{
  flash_unlock();
  flash_sector_erase(flash_iap.flash_app_addr - flash_iap.sector_size);
  flash_lock();
}

/**
  * @brief  set iap upgrade complete flag
  * @param  none
  * @retval none
  */
void flash_fat16_set_upgrade_flag(void)
{
  flash_unlock();
  flash_word_program(flash_iap.flash_app_addr - flash_iap.sector_size,
                      IAP_UPGRADE_COMPLETE_FLAG);
  flash_lock();
}

/**
  * @brief  get iap upgrade complete flag
  * @param  none
  * @retval the iap flag
  */
uint8_t flash_fat16_get_upgrade_flag(void)
{
  uint32_t flag_address = flash_iap.flash_app_addr - flash_iap.sector_size;
  if(*((uint32_t *)flag_address) == IAP_UPGRADE_COMPLETE_FLAG)
  {
    return IAP_SUCCESS;
  }
  else
  {
    return IAP_FAILED;
  }
}

/**
  * @brief  read a logical block address
  * @param  fat_lbk: logical block address to read
  * @param  data: pointer to array to store data read
  * @param  len: data length
  * @retval read length
  */
uint32_t flash_fat16_read(uint32_t fat_lbk, uint8_t *data, uint32_t len)
{
  uint32_t i_index = 0;
  switch(fat_lbk)
  {
    case FLASH_FAT16_BOOT_SECTOR_ADDR:
      i_index += fat16_memory_copy(data, fat16_sector, FAT16_SECTOR_SIZE);
      i_index += 2;

      fat16_memory_memset(data+i_index, 0, FAT16_BYTE_PER_SIZE-i_index);
      data[510] = 0x55;
      data[511] = 0xAA; 

      break;
    case FLASH_FAT16_1_ADDR:
    case FLASH_FAT16_2_ADDR:
      i_index += fat16_memory_copy(data, fat16_table_sector0, FAT16_TABLE_SIZE);

      fat16_memory_memset(data+i_index, 0, FAT16_BYTE_PER_SIZE-i_index);
      break;
    case FLASH_FAT16_ROOT_ADDR:
      i_index += fat16_memory_copy(data, fat16_file_name, FAT16_FILENAME_SIZE);
      i_index += fat16_memory_copy(data + i_index, fat16_root_dir_sector, FAT16_DIR_SIZE);

      fat16_memory_memset(data+i_index, 0, FAT16_BYTE_PER_SIZE-i_index);
      break;
    default:
      i_index = 0;
      fat16_memory_memset(data+i_index, 0, FAT16_BYTE_PER_SIZE-i_index);
      break;
  }
  return FAT16_BYTE_PER_SIZE;
}

/**
  * @brief  write a logical block address
  * @param  fat_lbk: logical block address to read
  * @param  data: pointer to array to store data read
  * @param  len: data length
  * @retval read length
  */
uint32_t flash_fat16_write(uint32_t fat_lbk, uint8_t *data, uint32_t len)
{
  switch(fat_lbk)
  {
    case FLASH_FAT16_BOOT_SECTOR_ADDR:
    case FLASH_FAT16_1_ADDR:
    case FLASH_FAT16_2_ADDR:
      break;
    case FLASH_FAT16_ROOT_ADDR:
      flash_fat16_boot_dir_write(fat_lbk, data, len);
      break;
    default:
      if(fat_lbk >= FLASH_FAT16_FILE_START_ADDR)
      {
        flash_fat16_sector_write(fat_lbk, data, len);
      }
      break;

  }
  return len;
}

/**
  * @brief  set file name
  * @param  file_name: file name
  * @param  len: filename length
  * @retval filename length
  */
uint32_t flash_fat16_set_name(const uint8_t *file_name, uint8_t len)
{
  uint8_t i_index = 0;
  for(i_index = 0; i_index < len && i_index < 8; i_index ++)
  {
    fat16_file_name[i_index] = file_name[i_index];
  }

  for(; i_index < 8; i_index ++)
  {
    fat16_file_name[i_index] = ' ';
  }
  return i_index;
}

/**
  * @brief  boot dir write
  * @param  fat_lbk: logical block address
  * @param  data: pointer to array to store data
  * @param  len: data length
  * @retval nubmer of data
  */
uint32_t flash_fat16_boot_dir_write(uint32_t fat_lbk, uint8_t *data, uint32_t len)
{
  fat_dir_type *pdir = (fat_dir_type *)data;
  uint32_t i_index = 2, loop_len = 0;

  pdir ++;
  pdir ++;
  loop_len += 64;

  while(i_index ++ < 512 && loop_len < len)
  {
    if((pdir->attr == 0x20) && ((pdir + 1)->attr == 0x00))
    {
      break;
    }
    else
    {
      pdir ++;
    }
    loop_len += 32;
  }

  if(i_index <= 512 && loop_len < len)
  {
    fat16_memory_copy((uint8_t *)&g_file_attr, (const uint8_t *)pdir, 32);
    
    if(file_match == 1)
    {
      file_match = 0;
      if(g_file_attr.file_size > 0)
      {
        if(flash_iap.file_write_nr >= g_file_attr.file_size)
        {
          /* upgrade finish */
          flash_iap.file_write_nr = 0;
          flash_iap.msc_up_status = UPGRADE_SUCCESS;
          
          /* set the upgrade done flag to flash */
          flash_fat16_set_upgrade_flag();
        }
      }
    }
    g_file_attr.write_data = 0;
    g_file_attr.write_time = 0;
    flash_iap.file_write_nr = 0;
  }
  else
  {
    fat16_memory_memset((uint8_t *)&g_file_attr, 0, 32);
  }
  return len;
}

/**
  * @brief  get offset address
  * @param  file_name: file name
  * @retval offset address
  */
uint32_t flash_fat16_addr_offset(uint8_t *file_name)
{
  uint8_t i_index = 0, offset_shift = 20;
  uint32_t offset = 0;
  uint8_t *pfile = file_name;

  if(*pfile != 'A')
  {
    return INVAILD_OFFSET_ADDR;
  }
  pfile += 1;

  for(i_index = 0; i_index < 6; i_index ++)
  {
    if(*pfile >= '0' && *pfile <= '9')
    {
      offset |= (*pfile - '0') << offset_shift;
    }
    else if(*pfile >= 'A' && *pfile <= 'F')
    {
      offset |= (*pfile - 'A' + 0xA) << offset_shift;
    }
    else if(*pfile >= 'a' && *pfile <= 'f')
    {
      offset |= (*pfile - 'a' + 0xA) << offset_shift;
    }
    else
    {
      return INVAILD_OFFSET_ADDR;
    }
    pfile ++;
    offset_shift -= 4;
  }

  if((offset < (flash_iap.flash_app_addr - flash_iap.flash_base_addr)) ||
      (offset > flash_iap.flash_size ))
  {
    return INVAILD_OFFSET_ADDR;
  }
  return offset;
}

/**
  * @brief  write sector
  * @param  fat_lbk: logical block address
  * @param  data: pointer to array to store data
  * @param  len: data length
  * @retval nubmer of data
  */
uint32_t flash_fat16_sector_write(uint32_t fat_lbk, uint8_t *data, uint32_t len)
{
  uint32_t file_size = g_file_attr.file_size;
  uint32_t flash_offset = 0;
  uint32_t status;
  static uint32_t s_bin_sp = 0;
  static uint32_t s_bin_pc = 0;
  static uint32_t file_offset_lbk = 0;
  
  if(s_bin_sp == 0 && s_bin_pc == 0)
  {
    s_bin_sp = *(uint32_t *)data;
    s_bin_pc = *(uint32_t *)(data+4);
  }

  /* check the suffix is .bin or .BIN*/
  if((fat16_memory_cmp((uint8_t *)&g_file_attr.file_name[8],
     (uint8_t *)FILE_SUFFIX1_NAME, FILE_SUFFIX1_LEN) == 0) ||
     (fat16_memory_cmp((uint8_t *)&g_file_attr.file_name[8],
     (uint8_t *)FILE_SUFFIX2_NAME, FILE_SUFFIX2_LEN) == 0)
      ||
      ((s_bin_sp & 0xF0000000) == 0x20000000 && (s_bin_pc & 0xFF000000) == 0x08000000))
  {
    /* check the upgrade status */
    if(flash_iap.msc_up_status == UPGRADE_DONE || flash_iap.msc_up_status == UPGRADE_SUCCESS)
    {
      return 0;
    }
    
    if(g_file_attr.file_size == 0)
    {
      g_file_attr.file_size = flash_iap.flash_app_size;
    }
    file_match = 1;
    
    file_size = g_file_attr.file_size;
    
    if(flash_iap.msc_up_status ==  UPGRADE_READY)
    {
      /* get offset address from filename */
      flash_offset = flash_fat16_addr_offset(g_file_attr.file_name);

      if(flash_offset == INVAILD_OFFSET_ADDR)
      {
        /* use default app address */
        flash_iap.write_addr = FLASH_APP_START_ADDR;
      }
      else
      {
        flash_iap.write_addr = flash_offset + flash_iap.flash_base_addr;
      }
      if( FAT16_FIRST_CLUSTER + (g_file_attr.clus_low * FAT16_CLUSTER_SIZE) == fat_lbk)
      {
        file_offset_lbk = FAT16_FIRST_CLUSTER + (g_file_attr.clus_low * FAT16_CLUSTER_SIZE);
      }
      else
      {
        file_offset_lbk = fat_lbk;
      }
      
      /* clear upgrade flag */
      flash_fat16_clear_upgrade_flag();

      flash_iap.msc_up_status = UPGRAGE_ONGOING;
    }

    if(flash_iap.flash_app_size >= g_file_attr.file_size)
    {
      if(fat_lbk >= file_offset_lbk)
      {
        /* write data to flash and check crc */
        status = flash_write_data(flash_iap.write_addr + fat_lbk - file_offset_lbk, data, len);      
        flash_iap.file_write_nr += len;
      }
      else
      {
        /* lbk address is invalid */
        status = 1;
      }

      if(status == 0)
      {
        if(flash_iap.file_write_nr >= file_size * 2)
        {
          /* upgrade finish */
          flash_iap.file_write_nr = 0;
          flash_iap.msc_up_status = UPGRADE_SUCCESS;
          s_bin_sp = 0;
          s_bin_pc = 0;
          /* set the upgrade done flag to flash */
          flash_fat16_set_upgrade_flag();
        }
      }
      else
      {
        /* upgrade failed */
        flash_iap.file_write_nr = 0;
        flash_iap.msc_up_status = UPGRADE_FAILED;
      }
    }
    else
    {
      /* upgrade file is large than flash size */
      flash_iap.msc_up_status = UPGRADE_LARGE;
    }

  }
  else if((fat16_memory_cmp((uint8_t *)&g_file_attr.file_name[0],
     (uint8_t *)"JUMP", 4) == 0))
  {
    /* receive jump command file */
    flash_iap.msc_up_status = UPGRADE_JUMP;
  }
  else
  {
    /* unkonw upgrade status */
  }

  return len;
}

/**
  * @brief  crc check
  * @param  address: flash address
  * @param  data: pointer to array to store data
  * @param  len: data length
  * @retval crc result
  */
uint32_t flash_crc_check(uint32_t address, uint8_t *data, uint32_t len)
{
  uint32_t flash_crc = 0;
  uint32_t data_crc = 0;
  uint32_t i_index = 0;
  uint32_t wlen = len / sizeof(uint32_t);
  uint32_t remain_len = len % sizeof(uint32_t);
  uint32_t *u32data = (uint32_t *)data;
  uint32_t *flash_addr = (uint32_t *)address;

  /* enable crc clock */
  crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);

  if(remain_len)
  {
    wlen += 1;
    for(i_index = 0; i_index < (4 - remain_len); i_index ++)
    {
      data[len+i_index] = 0xFF;
    }
  }

  /* calculate write buffer */
  crc_data_reset();
  for(i_index = 0; i_index < wlen; i_index ++)
  {
    data_crc = crc_one_word_calculate(u32data[i_index]);
  }

  /* calculate flash data */
  crc_data_reset();
  for(i_index = 0; i_index < wlen; i_index ++)
  {
    flash_crc = crc_one_word_calculate(*flash_addr++);
  }
  crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, FALSE);

  if(data_crc != flash_crc)
    return 1;

  return 0;

}

/**
  * @brief  read data from flash
  * @param  address: flash address
  * @param  data: pointer to array to store data
  * @param  len: data length
  * @retval read length
  */
uint32_t flash_read_data(uint32_t address, uint8_t *data, uint32_t len)
{
  uint32_t i_index;
  for(i_index = 0; i_index < len; i_index ++)
  {
    data[i_index] = *(uint8_t *)address;
    address += 1;
  }
  return i_index;
}


/**
  * @brief  write data to flash
  * @param  address: flash address
  * @param  data: pointer to array to store data
  * @param  len: data length
  * @retval write status
  */
uint32_t flash_write_data(uint32_t address, uint8_t *data, uint32_t len)
{
  uint32_t waddr = address;
  uint32_t wlength = len;
  uint32_t i_index = 0;
  if((address + len) > flash_iap.flash_base_addr + flash_iap.flash_size)
  {
    return 1;
  }

  if((waddr & flash_iap.sector_mask) == 0)
  {
    flash_unlock();
    if(len > flash_iap.sector_size)
    {
      for(; wlength >= flash_iap.sector_size; wlength -= flash_iap.sector_size)
      {
        flash_sector_erase(waddr);
        waddr += flash_iap.sector_size;
      }
      if(wlength > 0)
      {
        flash_sector_erase(waddr);
      }
    }
    else
    {
      flash_sector_erase(waddr);
    }

    flash_lock();
  }

  waddr = address;
  flash_unlock();
  for(i_index = 0; i_index < len; i_index ++)
  {
    flash_byte_program(waddr+i_index, data[i_index]);
  }
  flash_lock();

  return flash_crc_check(address, data, len);
}

/**
  * @brief  flash fat16 iap init
  * @param  none
  * @retval none
  */
void flash_fat16_init(void)
{
  uint32_t flash_size = *(uint32_t *)0x1FFFF7E0;
  flash_iap.flash_base_addr = FLASH_BASE;
  flash_iap.flash_app_addr = FLASH_APP_START_ADDR;
  flash_iap.write_addr = FLASH_APP_START_ADDR;
  flash_iap.flash_size = flash_size << 10;

  flash_iap.flash_app_size = flash_iap.flash_size -
                           (flash_iap.flash_app_addr -
                            flash_iap.flash_base_addr);

  if(flash_size < 0x100)
  {
    flash_iap.sector_size = FLASH_SECTOR_1K_SIZE;
    flash_iap.sector_mask = FLASH_SECTOR_1K_ALLGNED;
  }
  else
  {
    flash_iap.sector_size = FLASH_SECTOR_2K_SIZE;
    flash_iap.sector_mask = FLASH_SECTOR_2K_ALLGNED;
  }
  
}



/**
  * @}
  */

/**
  * @}
  */

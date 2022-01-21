/**
  **************************************************************************
  * @file     msc_bot_scsi.c
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    usb mass storage bulk-only transport and scsi command
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
#include "msc_bot_scsi.h"
#include "msc_diskio.h"

/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */

/** @defgroup USB_msc_bot_scsi
  * @brief usb device class mass storage demo
  * @{
  */  

/** @defgroup USB_msc_bot_functions
  * @{
  */

msc_type msc_struct;
cbw_type cbw_struct;
csw_type csw_struct = 
{
  CSW_DCSWSIGNATURE,
  0x00,
  0x00,
  CSW_BCSWSTATUS_PASS,
};

#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD uint8_t page00_inquiry_data[] ALIGNED_TAIL = {
	0x00,		
	0x00, 
	0x00, 
	0x00,
	0x00, 

};

#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD sense_type sense_data ALIGNED_TAIL =
{
  0x70,
  0x00,
  SENSE_KEY_ILLEGAL_REQUEST,
  0x00000000,
  0x0A,
  0x00000000,
  0x20,
  0x00,
  0x00000000
};

#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD uint8_t mode_sense6_data[8] ALIGNED_TAIL = 
{
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD uint8_t mode_sense10_data[8] ALIGNED_TAIL = 
{
  0x00,
  0x06,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};
/**
  * @brief  initialize bulk-only transport and scsi
  * @param  udev: to the structure of usbd_core_type
  * @retval none                            
  */
void bot_scsi_init(void *udev)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  msc_struct.msc_state = MSC_STATE_MACHINE_IDLE;
  msc_struct.bot_status = MSC_BOT_STATE_IDLE;
  msc_struct.max_lun = MSC_SUPPORT_MAX_LUN - 1;
  
  /* set out endpoint to receive status */
  usbd_ept_recv(pudev, USBD_MSC_BULK_OUT_EPT, (uint8_t *)&cbw_struct, CBW_CMD_LENGTH);
}

/**
  * @brief  reset bulk-only transport and scsi
  * @param  udev: to the structure of usbd_core_type
  * @retval none                            
  */
void bot_scsi_reset(void *udev)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  msc_struct.msc_state = MSC_STATE_MACHINE_IDLE;
  msc_struct.bot_status = MSC_BOT_STATE_RECOVERY;
  msc_struct.max_lun = MSC_SUPPORT_MAX_LUN - 1;
  
  /* set out endpoint to receive status */
  usbd_ept_recv(pudev, USBD_MSC_BULK_OUT_EPT, (uint8_t *)&cbw_struct, CBW_CMD_LENGTH);
}

/**
  * @brief  bulk-only transport data in handler
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval none                            
  */
void bot_scsi_datain_handler(void *udev, uint8_t ept_num)
{
  switch(msc_struct.msc_state)
  {
    case MSC_STATE_MACHINE_DATA_IN:
      if(bot_scsi_cmd_process(udev) != USB_OK)
      {
        bot_scsi_send_csw(udev, CSW_BCSWSTATUS_FAILED);
      }
      break;
    
    case MSC_STATE_MACHINE_LAST_DATA:
    case MSC_STATE_MACHINE_SEND_DATA:
      bot_scsi_send_csw(udev, CSW_BCSWSTATUS_PASS);
      break;
    
    default:
      break;
  }
}

/**
  * @brief  bulk-only transport data out handler
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval none                            
  */
void bot_scsi_dataout_handler(void *udev, uint8_t ept_num)
{
  switch(msc_struct.msc_state)
  {
    case MSC_STATE_MACHINE_IDLE:
      bot_cbw_decode(udev);
      break;
    
    case MSC_STATE_MACHINE_DATA_OUT:
      if(bot_scsi_cmd_process(udev) != USB_OK)
      {
        bot_scsi_send_csw(udev, CSW_BCSWSTATUS_FAILED);
      }
      break;
  }
}

/**
  * @brief  bulk-only cbw decode
  * @param  udev: to the structure of usbd_core_type
  * @retval none                            
  */
void bot_cbw_decode(void *udev)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  csw_struct.dCSWTag = cbw_struct.dCBWTage;
  csw_struct.dCSWDataResidue = cbw_struct.dCBWDataTransferLength;
  
  /* check param */
  if((cbw_struct.dCBWSignature != CBW_DCBWSIGNATURE) ||
    (usbd_get_recv_len(pudev, USBD_MSC_BULK_OUT_EPT) != CBW_CMD_LENGTH)
    || (cbw_struct.bCBWLUN > MSC_SUPPORT_MAX_LUN) ||
      (cbw_struct.bCBWCBLength < 1) || (cbw_struct.bCBWCBLength > 16))
  {
    bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
    msc_struct.bot_status = MSC_BOT_STATE_ERROR;
    bot_scsi_stall(udev);
  }
  else
  {
    if(bot_scsi_cmd_process(udev) != USB_OK)
    {
      bot_scsi_stall(udev);
    }
    else if((msc_struct.msc_state != MSC_STATE_MACHINE_DATA_IN) &&
            (msc_struct.msc_state != MSC_STATE_MACHINE_DATA_OUT) &&
            (msc_struct.msc_state != MSC_STATE_MACHINE_LAST_DATA))
    {
      if(msc_struct.data_len == 0)
      {
        bot_scsi_send_csw(udev, CSW_BCSWSTATUS_PASS);
      }
      else if(msc_struct.data_len > 0)
      {
        bot_scsi_send_data(udev, msc_struct.data, msc_struct.data_len);
      }
    }
  }
}

/**
  * @brief  send bot data
  * @param  udev: to the structure of usbd_core_type
  * @param  buffer: data buffer
  * @param  len: data len
  * @retval none                            
  */
void bot_scsi_send_data(void *udev, uint8_t *buffer, uint32_t len)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint32_t data_len = MIN(len, cbw_struct.dCBWDataTransferLength);
  
  csw_struct.dCSWDataResidue -= data_len;
  csw_struct.bCSWStatus = CSW_BCSWSTATUS_PASS;
  
  msc_struct.msc_state = MSC_STATE_MACHINE_SEND_DATA;
  
  usbd_ept_send(pudev, USBD_MSC_BULK_IN_EPT, 
                buffer, data_len);
}

/**
  * @brief  send command status
  * @param  udev: to the structure of usbd_core_type
  * @param  status: csw status
  * @retval none                            
  */
void bot_scsi_send_csw(void *udev, uint8_t status)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  csw_struct.bCSWStatus = status;
  csw_struct.dCSWSignature = CSW_DCSWSIGNATURE;
  msc_struct.msc_state = MSC_STATE_MACHINE_IDLE;
  
  usbd_ept_send(pudev, USBD_MSC_BULK_IN_EPT, 
                (uint8_t *)&csw_struct, CSW_CMD_LENGTH);
  
  usbd_ept_recv(pudev, USBD_MSC_BULK_OUT_EPT,
               (uint8_t *)&cbw_struct, CBW_CMD_LENGTH);
}


/**
  * @brief  send scsi sense code
  * @param  udev: to the structure of usbd_core_type
  * @param  sense_key: sense key
  * @param  asc: asc
  * @retval none                            
  */
void bot_scsi_sense_code(void *udev, uint8_t sense_key, uint8_t asc)
{
  sense_data.sense_key = sense_key;
  sense_data.asc = asc;
}


/**
  * @brief  check address
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @param  blk_offset: blk offset address
  * @param  blk_count: blk number
  * @retval usb_sts_type                            
  */
usb_sts_type bot_scsi_check_address(void *udev, uint8_t lun, uint32_t blk_offset, uint32_t blk_count)
{
  if((blk_offset + blk_count) > msc_struct.blk_nbr[lun])
  {
    bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
    return USB_FAIL;
  }
  return USB_OK;
}

/**
  * @brief  bot endpoint stall
  * @param  udev: to the structure of usbd_core_type
  * @retval none                            
  */
void bot_scsi_stall(void *udev)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if((cbw_struct.dCBWDataTransferLength != 0) &&
    (cbw_struct.bmCBWFlags == 0) &&
    msc_struct.bot_status == MSC_BOT_STATE_IDLE)
  {
    usbd_set_stall(pudev, USBD_MSC_BULK_OUT_EPT);
  }
  usbd_set_stall(pudev, USBD_MSC_BULK_IN_EPT);
  
  if(msc_struct.bot_status == MSC_BOT_STATE_ERROR)
  {
    usbd_ept_recv(pudev, USBD_MSC_BULK_OUT_EPT, 
                 (uint8_t *)&cbw_struct, CBW_CMD_LENGTH);
  }
}

/**
  * @brief  bulk-only transport scsi command test unit
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_test_unit(void *udev, uint8_t lun)
{
  usb_sts_type status = USB_OK;
  if(cbw_struct.dCBWDataTransferLength != 0)
  {
    bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
    return USB_FAIL;
  }
  
  msc_struct.data_len = 0;
  return status;
}

/**
  * @brief  bulk-only transport scsi command inquiry
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_inquiry(void *udev, uint8_t lun)
{
  uint8_t *pdata;
  uint32_t trans_len = 0;
  usb_sts_type status = USB_OK;
  
  if(cbw_struct.CBWCB[1] & 0x01)
  {
    pdata = page00_inquiry_data;
    trans_len = 5;
  }
  else
  {
    pdata = get_inquiry(lun);
    if(cbw_struct.dCBWDataTransferLength < SCSI_INQUIRY_DATA_LENGTH)
    {
      trans_len = cbw_struct.dCBWDataTransferLength;
    }
    else
    {
      trans_len = SCSI_INQUIRY_DATA_LENGTH;
    }
  }
  
  msc_struct.data_len = trans_len;
  while(trans_len)
  {
    trans_len --;
    msc_struct.data[trans_len] = pdata[trans_len];
  }
  return status;
}

/**
  * @brief  bulk-only transport scsi command start stop
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_start_stop(void *udev, uint8_t lun)
{
  msc_struct.data_len = 0;
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command meidum removal
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_allow_medium_removal(void *udev, uint8_t lun)
{
  msc_struct.data_len = 0;
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command mode sense6
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_mode_sense6(void *udev, uint8_t lun)
{
  uint8_t data_len = 8;
  msc_struct.data_len = 8;
  while(data_len)
  {
    data_len --;
    msc_struct.data[data_len] = mode_sense6_data[data_len];
  };
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command mode sense10
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_mode_sense10(void *udev, uint8_t lun)
{
  uint8_t data_len = 8;
  msc_struct.data_len = 8;
  while(data_len)
  {
    data_len --;
    msc_struct.data[data_len] = mode_sense10_data[data_len];
  };
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command capacity
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_capacity(void *udev, uint8_t lun)
{
  uint8_t *pdata = msc_struct.data;
  
  msc_disk_capacity(lun, &msc_struct.blk_nbr[lun], &msc_struct.blk_size[lun]);
    
  pdata[0] = (uint8_t)((msc_struct.blk_nbr[lun] - 1) >> 24);
  pdata[1] = (uint8_t)((msc_struct.blk_nbr[lun] - 1) >> 16);
  pdata[2] = (uint8_t)((msc_struct.blk_nbr[lun] - 1) >> 8);
  pdata[3] = (uint8_t)((msc_struct.blk_nbr[lun] - 1));
  
  pdata[4] = (uint8_t)((msc_struct.blk_size[lun]) >> 24);
  pdata[5] = (uint8_t)((msc_struct.blk_size[lun]) >> 16);
  pdata[6] = (uint8_t)((msc_struct.blk_size[lun]) >> 8);
  pdata[7] = (uint8_t)((msc_struct.blk_size[lun]));
  
  msc_struct.data_len = 8;
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command format capacity
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_format_capacity(void *udev, uint8_t lun)
{
  uint8_t *pdata = msc_struct.data;
  
  pdata[0] = 0;
  pdata[1] = 0;
  pdata[2] = 0;
  pdata[3] = 0x08;
  
  msc_disk_capacity(lun, &msc_struct.blk_nbr[lun], &msc_struct.blk_size[lun]);
  
  pdata[4] = (uint8_t)((msc_struct.blk_nbr[lun] - 1) >> 24);
  pdata[5] = (uint8_t)((msc_struct.blk_nbr[lun] - 1) >> 16);
  pdata[6] = (uint8_t)((msc_struct.blk_nbr[lun] - 1) >> 8);
  pdata[7] = (uint8_t)((msc_struct.blk_nbr[lun] - 1));
  
  pdata[8] = 0x02;
  
  pdata[9] = (uint8_t)((msc_struct.blk_size[lun]) >> 16);
  pdata[10] = (uint8_t)((msc_struct.blk_size[lun]) >> 8);
  pdata[11] = (uint8_t)((msc_struct.blk_size[lun]));
  
  msc_struct.data_len = 12;
  
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command request sense
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_request_sense(void *udev, uint8_t lun)
{
  uint32_t trans_len = 0x12;
  uint8_t *pdata = msc_struct.data;
  uint8_t *sdata = (uint8_t *)&sense_data;
 
  while(trans_len)
  {
    trans_len --;
    pdata[trans_len] = sdata[trans_len];
  }
  
  if(cbw_struct.dCBWDataTransferLength < REQ_SENSE_STANDARD_DATA_LEN)
  {
    msc_struct.data_len = cbw_struct.dCBWDataTransferLength;
  }
  else
  {
    msc_struct.data_len = REQ_SENSE_STANDARD_DATA_LEN;
  }
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command verify
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_verify(void *udev, uint8_t lun)
{
  uint8_t *cmd = cbw_struct.CBWCB;
  if((cbw_struct.CBWCB[1] & 0x02) == 0x02)
  {
    bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    return USB_FAIL;
  }
  
  msc_struct.blk_addr = cmd[2] << 24 | cmd[3] << 16 | cmd[4] << 8 | cmd[5];
  msc_struct.blk_len = cmd[7] << 8 | cmd[8];
  
  if(bot_scsi_check_address(udev, lun, msc_struct.blk_addr, msc_struct.blk_len) != USB_OK)
  {
    return USB_FAIL;
  }
  msc_struct.data_len = 0;
  return USB_OK;
}

/**
  * @brief  bulk-only transport scsi command read10
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_read10(void *udev, uint8_t lun)
{
  uint8_t *cmd = cbw_struct.CBWCB;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint32_t len;
  
  if(msc_struct.msc_state == MSC_STATE_MACHINE_IDLE)
  {
    if((cbw_struct.bmCBWFlags & 0x80) != 0x80)
    {
      bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
      return USB_FAIL;
    }
    
    msc_struct.blk_addr = cmd[2] << 24 | cmd[3] << 16 | cmd[4] << 8 | cmd[5];
    msc_struct.blk_len = cmd[7] << 8 | cmd[8];
    
    if(bot_scsi_check_address(udev, lun, msc_struct.blk_addr, msc_struct.blk_len) != USB_OK)
    {
      return USB_FAIL;
    }
    
    msc_struct.blk_addr *= msc_struct.blk_size[lun];
    msc_struct.blk_len *= msc_struct.blk_size[lun];
    
    if(cbw_struct.dCBWDataTransferLength != msc_struct.blk_len)
    {
      bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
      return USB_FAIL;
    }
    msc_struct.msc_state  = MSC_STATE_MACHINE_DATA_IN;
  }
  msc_struct.data_len = MSC_MAX_DATA_BUF_LEN;
  
  len = MIN(msc_struct.blk_len, MSC_MAX_DATA_BUF_LEN);
  if( msc_disk_read(lun, msc_struct.blk_addr, msc_struct.data, len) != USB_OK)
  {
    bot_scsi_sense_code(udev, SENSE_KEY_HARDWARE_ERROR, MEDIUM_NOT_PRESENT);
    return USB_FAIL;
  }
  usbd_ept_send(pudev, USBD_MSC_BULK_IN_EPT, msc_struct.data, len);
  msc_struct.blk_addr += len;
  msc_struct.blk_len -= len;
  
  csw_struct.dCSWDataResidue -= len;
  if(msc_struct.blk_len == 0)
  {
    msc_struct.msc_state = MSC_STATE_MACHINE_LAST_DATA;
  }
  
  return USB_OK;
}


/**
  * @brief  bulk-only transport scsi command write10
  * @param  udev: to the structure of usbd_core_type
  * @param  lun: logical units number
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_write10(void *udev, uint8_t lun)
{
  uint8_t *cmd = cbw_struct.CBWCB;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint32_t len;
  
  if(msc_struct.msc_state == MSC_STATE_MACHINE_IDLE)
  {
    if((cbw_struct.bmCBWFlags & 0x80) == 0x80)
    {
      bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
      return USB_FAIL;
    }
    
    msc_struct.blk_addr = cmd[2] << 24 | cmd[3] << 16 | cmd[4] << 8 | cmd[5];
    msc_struct.blk_len = cmd[7] << 8 | cmd[8];
    
    if(bot_scsi_check_address(udev, lun, msc_struct.blk_addr, msc_struct.blk_len) != USB_OK)
    {
      return USB_FAIL;
    }
    
    msc_struct.blk_addr *= msc_struct.blk_size[lun];
    msc_struct.blk_len *= msc_struct.blk_size[lun];
    
    if(cbw_struct.dCBWDataTransferLength != msc_struct.blk_len)
    {
      bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
      return USB_FAIL;
    }
    
    msc_struct.msc_state  = MSC_STATE_MACHINE_DATA_OUT;
    len = MIN(msc_struct.blk_len, MSC_MAX_DATA_BUF_LEN);
    usbd_ept_recv(pudev, USBD_MSC_BULK_OUT_EPT, (uint8_t *)msc_struct.data, len);
    
  }
  else
  {
    len = MIN(msc_struct.blk_len, MSC_MAX_DATA_BUF_LEN);
    if(msc_disk_write(lun, msc_struct.blk_addr, msc_struct.data, len) != USB_OK)
    {
      bot_scsi_sense_code(udev, SENSE_KEY_HARDWARE_ERROR, MEDIUM_NOT_PRESENT);
      return USB_FAIL;
    }
    
    msc_struct.blk_addr += len;
    msc_struct.blk_len -= len;
  
    csw_struct.dCSWDataResidue -= len;
    
    if(msc_struct.blk_len == 0)
    {
      bot_scsi_send_csw(udev, CSW_BCSWSTATUS_PASS);
    }
    else
    {
      len = MIN(msc_struct.blk_len, MSC_MAX_DATA_BUF_LEN);
      usbd_ept_recv(pudev, USBD_MSC_BULK_OUT_EPT, (uint8_t *)msc_struct.data, len);
    }
  }
  return USB_OK;
}

/**
  * @brief  clear feature
  * @param  udev: to the structure of usbd_core_type
  * @param  etp_num: endpoint number
  * @retval status of usb_sts_type                            
  */
void bot_scsi_clear_feature(void *udev, uint8_t ept_num)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  if(msc_struct.bot_status == MSC_BOT_STATE_ERROR)
  {
    usbd_set_stall(pudev, USBD_MSC_BULK_IN_EPT);
    msc_struct.bot_status = MSC_BOT_STATE_IDLE;
  }
  else if(((ept_num & 0x80) == 0x80) && (msc_struct.bot_status != MSC_BOT_STATE_RECOVERY))
  {
    bot_scsi_send_csw(udev, CSW_BCSWSTATUS_FAILED);
  }
}

/**
  * @brief  bulk-only transport scsi command process
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type bot_scsi_cmd_process(void *udev)
{
  usb_sts_type status = USB_FAIL;
  switch(cbw_struct.CBWCB[0])
  {
    case MSC_CMD_INQUIRY:
      status = bot_scsi_inquiry(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_START_STOP:
      status = bot_scsi_start_stop(udev, cbw_struct.bCBWLUN);
      break;  
    
    case MSC_CMD_MODE_SENSE6:
      status = bot_scsi_mode_sense6(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_MODE_SENSE10:
      status = bot_scsi_mode_sense10(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_ALLOW_MEDIUM_REMOVAL:
      status = bot_scsi_allow_medium_removal(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_READ_10:
      status = bot_scsi_read10(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_READ_CAPACITY:
      status = bot_scsi_capacity(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_REQUEST_SENSE:
      status = bot_scsi_request_sense(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_TEST_UNIT:
      status = bot_scsi_test_unit(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_VERIFY:
      status = bot_scsi_verify(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_WRITE_10:
      status = bot_scsi_write10(udev, cbw_struct.bCBWLUN);
      break;
    
    case MSC_CMD_READ_FORMAT_CAPACITY:
      status = bot_scsi_format_capacity(udev, cbw_struct.bCBWLUN);
      break;
    
    default:
      bot_scsi_sense_code(udev, SENSE_KEY_ILLEGAL_REQUEST, INVALID_COMMAND);
      status = USB_FAIL;
      break;
  }
  return status;
}

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

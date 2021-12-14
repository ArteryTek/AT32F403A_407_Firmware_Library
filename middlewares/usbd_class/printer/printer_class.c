/**
  **************************************************************************
  * @file     printer_class.c
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    usb printer class type
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
#include "usbd_core.h"
#include "printer_class.h"
#include "printer_desc.h"

/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */
  
/** @defgroup USB_printer_class
  * @brief usb device class printer demo
  * @{
  */  

/** @defgroup USB_printer_class_private_functions
  * @{
  */

usb_sts_type class_init_handler(void *udev);
usb_sts_type class_clear_handler(void *udev);
usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup);
usb_sts_type class_ept0_tx_handler(void *udev);
usb_sts_type class_ept0_rx_handler(void *udev);
usb_sts_type class_in_handler(void *udev, uint8_t ept_num);
usb_sts_type class_out_handler(void *udev, uint8_t ept_num);
usb_sts_type class_sof_handler(void *udev);
usb_sts_type class_event_handler(void *udev, usbd_event_type event);

/* usb rx and tx buffer */
static uint32_t alt_setting = 0;
static uint8_t g_rx_buff[USBD_OUT_MAXPACKET_SIZE];
static uint16_t g_rxlen;
__IO uint8_t g_tx_completed = 1, g_rx_completed = 0;

uint8_t PRINTER_DEVICE_ID[PRINTER_DEVICE_ID_LEN] =
{
  0x00, 0x16,
  'M', 'F', 'G',':','A','r','t','e', 'r', 'y' ,' ',
  'C','M', 'D', ':', 'E', 'S', 'C', 'P', 'O', 'S',' ',
};
static uint8_t g_printer_port_status = 0x18;
uint8_t g_printer_data[USBD_OUT_MAXPACKET_SIZE];


/* usb device class handler */
usbd_class_handler printer_class_handler = 
{
  class_init_handler,
  class_clear_handler,
  class_setup_handler,
  class_ept0_tx_handler,
  class_ept0_rx_handler,
  class_in_handler,
  class_out_handler,
  class_sof_handler,
  class_event_handler,
};

/**
  * @brief  initialize usb custom hid endpoint
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_init_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
    
  /* open in endpoint */
  usbd_ept_open(pudev, USBD_PRINTER_BULK_IN_EPT, EPT_BULK_TYPE, USBD_OUT_MAXPACKET_SIZE);
  
  /* open out endpoint */
  usbd_ept_open(pudev, USBD_PRINTER_BULK_OUT_EPT, EPT_BULK_TYPE, USBD_OUT_MAXPACKET_SIZE);
  
  /* set out endpoint to receive status */
  usbd_ept_recv(pudev, USBD_PRINTER_BULK_OUT_EPT, g_rx_buff, USBD_OUT_MAXPACKET_SIZE);
  
  g_tx_completed = 1;
  
  return status;
}

/**
  * @brief  clear endpoint or other state
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_clear_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  /* close in endpoint */
  usbd_ept_close(pudev, USBD_PRINTER_BULK_IN_EPT);
  
  /* close out endpoint */
  usbd_ept_close(pudev, USBD_PRINTER_BULK_OUT_EPT);
  
  return status;
}

/**
  * @brief  usb device class setup request handler
  * @param  udev: to the structure of usbd_core_type
  * @param  setup: setup packet
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;

  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)
  {
    /* class request */
    case USB_REQ_TYPE_CLASS:
      switch(setup->bRequest)
      {
        case PRINTER_REQ_GET_DEVICE_ID:
          usbd_ctrl_send(pudev, PRINTER_DEVICE_ID, PRINTER_DEVICE_ID_LEN);
          break;
        case PRINTER_REQ_GET_PORT_STATUS:
          usbd_ctrl_send(pudev, (uint8_t *)&g_printer_port_status, 1);
          break;
        case PRINTER_REQ_GET_SOFT_RESET:
          usbd_ctrl_recv(pudev, g_printer_data, USBD_OUT_MAXPACKET_SIZE);
          break;
        default:
          usbd_ctrl_unsupport(pudev);
          break;
      }
      break;
    /* standard request */
    case USB_REQ_TYPE_STANDARD:
      switch(setup->bRequest)
      {
        case USB_STD_REQ_GET_DESCRIPTOR:
          usbd_ctrl_unsupport(pudev);
          break;
        case USB_STD_REQ_GET_INTERFACE:
          usbd_ctrl_send(pudev, (uint8_t *)&alt_setting, 1);
          break;
        case USB_STD_REQ_SET_INTERFACE:
          alt_setting = setup->wValue;
          break;
        default:
          break;
      }
      break;
    default:
      usbd_ctrl_unsupport(pudev);
      break;
  }
  return status;
}

/**
  * @brief  usb device endpoint 0 in status stage complete
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_ept0_tx_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  
  /* ...user code... */
  
  return status;
}

/**
  * @brief  usb device endpoint 0 out status stage complete
  * @param  udev: usb device core handler type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_ept0_rx_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  
  /* ...user code... */
  
  return status;
}

/**
  * @brief  usb device transmision complete handler
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_in_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  
  /* ...user code...
    trans next packet data
  */
  g_tx_completed = 1;
  
  return status;
}

/**
  * @brief  usb device endpoint receive data
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_out_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  /* get endpoint receive data length  */
  g_rxlen = usbd_get_recv_len(pudev, ept_num);
  
  /*set recv flag*/
  g_rx_completed = 1;
  
  return status;
}

/**
  * @brief  usb device sof handler
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_sof_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  
  /* ...user code... */
  
  return status;
}

/**
  * @brief  usb device event handler
  * @param  udev: to the structure of usbd_core_type
  * @param  event: usb device event
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_event_handler(void *udev, usbd_event_type event)
{
  usb_sts_type status = USB_OK;
  switch(event)
  {
    case USBD_RESET_EVENT:
      
      /* ...user code... */
    
      break;
    case USBD_SUSPEND_EVENT:
      
      /* ...user code... */
    
      break;
    case USBD_WAKEUP_EVENT:
      /* ...user code... */
    
      break;
    
    default:
      break;
  }
  return status;
}

/**
  * @brief  usb device class rx data process
  * @param  udev: to the structure of usbd_core_type
  * @param  recv_data: receive buffer
  * @retval receive data len                            
  */
uint16_t usb_printer_get_rxdata(void *udev, uint8_t *recv_data)
{
  uint16_t i_index = 0;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint16_t tmp_len = g_rxlen;
  
  if(g_rx_completed == 0)
  {
    return 0;
  }
  g_rx_completed = 0;
  tmp_len = g_rxlen;
  for(i_index = 0; i_index < g_rxlen; i_index ++)
  {
    recv_data[i_index] = g_rx_buff[i_index];
  }
  
  usbd_ept_recv(pudev, USBD_PRINTER_BULK_OUT_EPT, g_rx_buff, USBD_OUT_MAXPACKET_SIZE);
  
  return tmp_len;
}

/**
  * @brief  usb device class send data
  * @param  udev: to the structure of usbd_core_type
  * @param  send_data: send data buffer
  * @param  len: send length
  * @retval error status                            
  */
error_status usb_printer_send_data(void *udev, uint8_t *send_data, uint16_t len)
{
  error_status status = SUCCESS;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(g_tx_completed)
  {
    g_tx_completed = 0;
    usbd_ept_send(pudev, USBD_PRINTER_BULK_IN_EPT, send_data, len);
  }
  else
  {
    status = ERROR;
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





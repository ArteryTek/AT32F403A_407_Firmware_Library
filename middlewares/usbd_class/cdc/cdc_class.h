/**
  **************************************************************************
  * @file     cdc_class.h
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    usb cdc class file
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
  
 /* define to prevent recursive inclusion -------------------------------------*/
#ifndef __CDC_CLASS_H
#define __CDC_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include "usb_std.h"
#include "usbd_core.h"

/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */
  
/** @addtogroup USB_cdc_class
  * @{
  */

/** @defgroup USB_cdc_class_definition 
  * @{
  */

/**
  * @brief usb cdc use endpoint define
  */
#define USBD_CDC_INT_EPT                 0x82
#define USBD_CDC_BULK_IN_EPT             0x81
#define USBD_CDC_BULK_OUT_EPT            0x01

/**
  * @brief usb cdc in and out max packet size define
  */
#define USBD_IN_MAXPACKET_SIZE           0x40
#define USBD_OUT_MAXPACKET_SIZE          0x40
#define USBD_CMD_MAXPACKET_SIZE          0x08

/**
  * @brief usb cdc class request code define
  */
#define SET_LINE_CODING                  0x20
#define GET_LINE_CODING                  0x21

/**
  * @}
  */

/** @defgroup USB_cdc_class_exported_types
  * @{
  */

/**
  * @brief usb cdc class set line coding struct
  */
typedef struct 
{
  uint32_t bitrate;                      /* line coding baud rate */
  uint8_t format;                        /* line coding foramt */
  uint8_t parity;                        /* line coding parity */
  uint8_t data;                          /* line coding data bit */
}linecoding_type;

/**
  * @}
  */

/** @defgroup USB_cdc_class_exported_functions
  * @{
  */
extern usbd_class_handler class_handler;
uint16_t usb_vcp_get_rxdata(void *udev, uint8_t *recv_data);
error_status usb_vcp_send_data(void *udev, uint8_t *send_data, uint16_t len);

/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif




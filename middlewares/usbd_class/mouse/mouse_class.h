/**
  **************************************************************************
  * @file     mouse_class.h
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    usb hid mouse header file
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
#ifndef __MOUSE_CLASS_H
#define __MOUSE_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include "usb_std.h"
#include "usbd_core.h"

/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */
  
/** @addtogroup USB_mouse_class
  * @{
  */

/** @defgroup USB_mouse_class_endpoint_definition 
  * @{
  */

/**
  * @brief usb hid use endpoint define
  */
#define USBD_HID_IN_EPT                  0x81

/**
  * @brief usb hid in and out max packet size define
  */
#define USBD_IN_MAXPACKET_SIZE           0x40

/**
  * @}
  */

/** @defgroup USB_mouse_class_request_code_definition 
  * @{
  */
  
/**
  * @brief usb hid class request code define
  */
#define HID_REQ_SET_PROTOCOL             0x0B
#define HID_REQ_GET_PROTOCOL             0x03
#define HID_REQ_SET_IDLE                 0x0A
#define HID_REQ_GET_IDLE                 0x02
#define HID_REQ_SET_REPORT               0x09
#define HID_REQ_GET_REPORT               0x01
#define HID_DESCRIPTOR_TYPE              0x21
#define HID_REPORT_DESC                  0x22

/**
  * @brief usb hid mouse operation
  */
#define MOVE_STEP                        20

#define BUTTON_RELEASE                   0
#define LEFT_BUTTON                      1
#define RIGHT_BUTTON                     2
#define LEFT_MOVE                        3
#define RIGHT_MOVE                       4
#define UP_MOVE                          5
#define DOWN_MOVE                        6

/**
  * @}
  */

/** @defgroup USB_hid_class_exported_functions
  * @{
  */
extern usbd_class_handler mouse_class_handler;
usb_sts_type class_send_report(void *udev, uint8_t *report, uint16_t len);
void usb_hid_mouse_send(void *udev, uint8_t op);
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

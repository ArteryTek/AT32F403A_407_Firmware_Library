/**
  **************************************************************************
  * @file     audio_class.c
  * @version  v2.0.6
  * @date     2021-12-31
  * @brief    usb audio class type
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
#include "audio_hid_class.h"
#include "audio_hid_desc.h"
#include "audio_codec.h"

/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */
  
/** @defgroup USB_audio_hid_class
  * @brief usb device class audio hid demo
  * @{
  */  

/** @defgroup USB_audio_hid_class_private_functions
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

void audio_req_get_cur(void *udev, usb_setup_type *setup);
void audio_req_set_cur(void *udev, usb_setup_type *setup);
void audio_req_get_min(void *udev, usb_setup_type *setup);
void audio_req_get_max(void *udev, usb_setup_type *setup);
void audio_req_get_res(void *udev, usb_setup_type *setup);
void audio_get_interface(void *udev, usb_setup_type *setup);
void audio_set_interface(void *udev, usb_setup_type *setup);
void usb_hid_buf_process(void *udev, uint8_t *report, uint16_t len);
/* usb hid rx and tx buffer */
static uint8_t g_rxhid_buff[USBD_OUT_MAXPACKET_SIZE];
static uint8_t g_txhid_buff[USBD_IN_MAXPACKET_SIZE];

usb_audio_type audio_struct = {0, 0, 0, 0, 0, 0x1400, 0, 0, 0, {0x0000, 0x1400, 0x33}, {0x0000, 0x1400, 0x33}};
/* custom hid static variable */
static uint32_t hid_protocol = 0;
static uint32_t hid_set_idle = 0;
static uint32_t alt_setting = 0;
static uint8_t hid_state;
uint8_t hid_set_report[64];
static __IO uint16_t audio_feedback_state = 0;
static __IO uint8_t audio_spk_out_stage = 0;
/* usb device class handler */
usbd_class_handler audio_hid_class_handler = 
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
  * @param  udev: usb device core handler type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_init_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  /* enable microphone in endpoint double buffer mode */
  usbd_ept_dbuffer_enable(pudev, USBD_AUDIO_MIC_IN_EPT);
  
  /* open microphone in endpoint */
  usbd_ept_open(pudev, USBD_AUDIO_MIC_IN_EPT, EPT_ISO_TYPE, AUDIO_MIC_IN_MAXPACKET_SIZE);
  
  /* enable speaker out endpoint double buffer mode */
  usbd_ept_dbuffer_enable(pudev, USBD_AUDIO_SPK_OUT_EPT);
  
  /* open speaker out endpoint */
  usbd_ept_open(pudev, USBD_AUDIO_SPK_OUT_EPT, EPT_ISO_TYPE, AUDIO_SPK_OUT_MAXPACKET_SIZE);
#if AUDIO_SUPPORT_FEEDBACK  
  /* enable speaker feedback endpoint double buffer mode */
  usbd_ept_dbuffer_enable(pudev, USBD_AUDIO_FEEDBACK_EPT);
  
  /* open speaker feedback endpoint */
  usbd_ept_open(pudev, USBD_AUDIO_FEEDBACK_EPT, EPT_ISO_TYPE, AUDIO_FEEDBACK_MAXPACKET_SIZE);
#endif 
  /* start receive speaker out data */
  usbd_ept_recv(pudev, USBD_AUDIO_SPK_OUT_EPT, audio_struct.audio_spk_data, AUDIO_SPK_OUT_MAXPACKET_SIZE);
  
  /*open hid endpoint */
  
  /* open custom hid in endpoint */
  usbd_ept_open(pudev, USBD_HID_IN_EPT, EPT_INT_TYPE, USBD_IN_MAXPACKET_SIZE);
  
  /* open custom hid out endpoint */
  usbd_ept_open(pudev, USBD_HID_OUT_EPT, EPT_INT_TYPE, USBD_OUT_MAXPACKET_SIZE);
  
  /* set out endpoint to receive status */
  usbd_ept_recv(pudev, USBD_HID_OUT_EPT, g_rxhid_buff, USBD_OUT_MAXPACKET_SIZE);
  
  return status;
}

/**
  * @brief  clear endpoint or other state
  * @param  udev: usb device core handler type
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_clear_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  /* close in endpoint */
  usbd_ept_close(pudev, USBD_AUDIO_MIC_IN_EPT);
  
#if AUDIO_SUPPORT_FEEDBACK  
  /* close in endpoint */
  usbd_ept_close(pudev, USBD_AUDIO_FEEDBACK_EPT);
#endif
  
  /* close out endpoint */
  usbd_ept_close(pudev, USBD_AUDIO_SPK_OUT_EPT);
  
  /* close custom hid in endpoint */
  usbd_ept_close(pudev, USBD_HID_IN_EPT);
  
  /* close custom hid out endpoint */
  usbd_ept_close(pudev, USBD_HID_OUT_EPT);
 
  return status;
}

/**
  * @brief  usb device class setup request handler
  * @param  udev: usb device core handler type
  * @param  setup: setup packet
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_audio_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint16_t len;

  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)
  {
    /* class request */
    case USB_REQ_TYPE_CLASS:
      switch(setup->bRequest)
      {
        case AUDIO_REQ_GET_CUR:
          audio_req_get_cur(pudev, setup);
          break;
        
        case AUDIO_REQ_SET_CUR:
          audio_req_set_cur(pudev, setup);
          break;
        
        case AUDIO_REQ_GET_MIN:
          audio_req_get_min(pudev, setup);
          break;
        
        case AUDIO_REQ_GET_MAX:
          audio_req_get_max(pudev, setup);
          break;
        
        case AUDIO_REQ_GET_RES:
          audio_req_get_res(pudev, setup);
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
          if((setup->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE)
          {
            len = MIN(AUDIO_DESCRIPTOR_SIZE, setup->wLength);
            usbd_ctrl_send(pudev, g_usbd_configuration+18, len);
          }
          else
          {
            usbd_ctrl_unsupport(pudev);
          }
          break;
        case USB_STD_REQ_GET_INTERFACE:
          audio_get_interface(udev, setup);
          break;
        
        case USB_STD_REQ_SET_INTERFACE:
          audio_set_interface(udev, setup);
          usbd_ctrl_send_status(pudev);
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
  * @brief  usb device class setup request handler
  * @param  udev: usb device core handler type
  * @param  setup: setup packet
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_hid_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint16_t len;
  uint8_t *buf;

  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)
  {
    /* class request */
    case USB_REQ_TYPE_CLASS:
      switch(setup->bRequest)
      {
        case HID_REQ_SET_PROTOCOL:
          hid_protocol = (uint8_t)setup->wValue;
          break;
        case HID_REQ_GET_PROTOCOL:
          usbd_ctrl_send(pudev, (uint8_t *)&hid_protocol, 1);
          break;
        case HID_REQ_SET_IDLE:
          hid_set_idle = (uint8_t)(setup->wValue >> 8);
          break;
        case HID_REQ_GET_IDLE:
          usbd_ctrl_send(pudev, (uint8_t *)&hid_set_idle, 1);
          break;
        case HID_REQ_SET_REPORT:
          hid_state = HID_REQ_SET_REPORT;
          usbd_ctrl_recv(pudev, hid_set_report, setup->wLength);
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
          if(setup->wValue >> 8 == HID_REPORT_DESC)
          {
            len = MIN(USBD_HID_SIZ_REPORT_DESC, setup->wLength);
            buf = (uint8_t *)g_usbd_hid_report;
          }
          else if(setup->wValue >> 8 == HID_DESCRIPTOR_TYPE)
          {
            len = MIN(9, setup->wLength);
            buf = (uint8_t *)g_hid_usb_desc;
          }
          usbd_ctrl_send(pudev, (uint8_t *)buf, len);
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
  * @brief  usb device class setup request handler
  * @param  udev: usb device core handler type
  * @param  setup: setup packet
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  switch(setup->bmRequestType & USB_REQ_RECIPIENT_MASK)
  {
    case USB_REQ_RECIPIENT_INTERFACE:
      if(setup->wIndex == HID_INTERFACE_NUMBER)
      {
        class_hid_setup_handler(udev, setup);
      }
      else
      {
        class_audio_setup_handler(pudev, setup);
      }
      break;
    case USB_REQ_RECIPIENT_ENDPOINT:
      class_audio_setup_handler(pudev, setup);
      break;
    
  }
  return status;
  
  
}

/**
  * @brief  usb device endpoint 0 in status stage complete
  * @param  udev: usb device core handler type
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
  usbd_core_type *pudev = (usbd_core_type *)udev;
  usb_setup_type *setup = &pudev->setup;
  uint32_t recv_len = usbd_get_recv_len(pudev, 0);
  
  /* ...user code... */
  if(setup->wIndex == HID_INTERFACE_NUMBER)
  {
    if( hid_state == HID_REQ_SET_REPORT)
    {
      /* hid buffer process */
      usb_hid_buf_process(udev, hid_set_report, recv_len);
      hid_state = 0;
    }
  }
  else
  {
    /* ...user code... */
    if( audio_struct.audio_cmd == AUDIO_REQ_SET_CUR)
    {
      /* class process */
      switch(audio_struct.request_no)
      {
        case AUDIO_VOLUME_CONTROL:
          if(audio_struct.interface == AUDIO_SPK_FEATURE_UNIT_ID)
          {
            audio_struct.spk_volume = (uint16_t)(audio_struct.g_audio_cur[0] | 
                                                 (audio_struct.g_audio_cur[1] << 8));
            /* set volume */
            audio_codec_set_spk_volume(audio_struct.spk_volume*100/audio_struct.spk_volume_limits[1]);
          }
          else
          {
            audio_struct.mic_volume = (uint16_t)(audio_struct.g_audio_cur[0] | 
                                                 (audio_struct.g_audio_cur[1] << 8));
            
            audio_codec_set_mic_volume(audio_struct.mic_volume*256/audio_struct.mic_volume_limits[1]);
          }
          break;
        case AUDIO_MUTE_CONTROL:
          if(audio_struct.interface == AUDIO_SPK_FEATURE_UNIT_ID)
          {
            audio_struct.spk_mute = audio_struct.g_audio_cur[0];
            audio_codec_set_spk_mute(audio_struct.spk_mute);
          }
          else
          {
            audio_struct.mic_mute = audio_struct.g_audio_cur[0];
            audio_codec_set_mic_mute(audio_struct.mic_mute);
          }
          
          break;
        case AUDIO_FREQ_SET_CONTROL:
          if(audio_struct.enpd == USBD_AUDIO_MIC_IN_EPT)
          {
            audio_struct.mic_freq = (audio_struct.g_audio_cur[0] | 
                              (audio_struct.g_audio_cur[1] << 8) |
                              (audio_struct.g_audio_cur[2] << 16));
            audio_codec_set_mic_freq(audio_struct.mic_freq);
          }
          else
          {
            audio_struct.spk_freq = (audio_struct.g_audio_cur[0] | 
                              (audio_struct.g_audio_cur[1] << 8) |
                              (audio_struct.g_audio_cur[2] << 16));
            audio_codec_set_spk_freq(audio_struct.spk_freq);
          }
          break;
        default:
          break;
      }
    }
  }
  
  return status;
}

/**
  * @brief  usb device transmision complete handler
  * @param  udev: usb device core handler type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_in_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  uint32_t len = 0;
  
  /* ...user code...
    trans next packet data
  */
  if((ept_num & 0x7F) == (USBD_AUDIO_MIC_IN_EPT & 0x7F))
  {
    len = audio_codec_mic_get_data(audio_struct.audio_mic_data);
    usbd_ept_send(udev, USBD_AUDIO_MIC_IN_EPT, audio_struct.audio_mic_data, len);
  }
#if AUDIO_SUPPORT_FEEDBACK  
  else if((ept_num & 0x7F) == (USBD_AUDIO_FEEDBACK_EPT & 0x7F))
  {
    len = audio_codec_spk_feedback(audio_struct.audio_feed_back);
    usbd_ept_send(udev, USBD_AUDIO_FEEDBACK_EPT, audio_struct.audio_feed_back, len);
  }
#endif  
  else if((ept_num & 0x7F) == (USBD_HID_IN_EPT & 0x7F))
  {
    
  }
  
  return status;
}

/**
  * @brief  usb device endpoint receive data
  * @param  udev: usb device core handler type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_out_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  uint16_t g_rxlen;
  
  /* get endpoint receive data length  */
  g_rxlen = usbd_get_recv_len(pudev, ept_num);
  
  if((ept_num & 0x7F) == (USBD_AUDIO_SPK_OUT_EPT & 0x7F))
  {
    /* speaker data*/
    audio_codec_spk_fifo_write(audio_struct.audio_spk_data, g_rxlen);
    
    /* get next data */
    usbd_ept_recv(pudev, USBD_AUDIO_SPK_OUT_EPT, audio_struct.audio_spk_data, AUDIO_SPK_OUT_MAXPACKET_SIZE);
  }
  else if((ept_num & 0x7F) == (USBD_HID_OUT_EPT & 0x7F))
  {
    /* hid buffer process */
    usb_hid_buf_process(udev, g_rxhid_buff, g_rxlen);
    
    /* start receive next packet */
    usbd_ept_recv(pudev, USBD_HID_OUT_EPT, g_rxhid_buff, USBD_OUT_MAXPACKET_SIZE);
  }
  return status;
}

/**
  * @brief  usb device sof handler
  * @param  udev: usb device core handler type
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
  * @param  udev: usb device core handler type
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
      audio_struct.spk_alt_setting = 0;
      audio_codec_spk_alt_setting(audio_struct.spk_alt_setting);
      audio_struct.mic_alt_setting = 0;
      audio_codec_mic_alt_setting(audio_struct.spk_alt_setting);
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
  * @brief  usb audio request get cur
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_req_get_cur(void *udev, usb_setup_type *setup)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(HBYTE(setup->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
  {
    if(HBYTE(setup->wValue) == AUDIO_MUTE_CONTROL)
    {
      audio_struct.g_audio_cur[0] = audio_struct.spk_mute;
      usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
    }
    else
    {
      *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.spk_volume;
      usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
    }
  }
  else
  {
    if(HBYTE(setup->wValue) == AUDIO_MUTE_CONTROL)
    {
      audio_struct.g_audio_cur[0] = audio_struct.mic_mute;
      usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
    }
    else
    {
      *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.mic_volume;
      usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
    }
    
  }
}

/**
  * @brief  usb audio request set cur
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_req_set_cur(void *udev, usb_setup_type *setup)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(setup->wLength > 0)
  {
    usbd_ctrl_recv(pudev, audio_struct.g_audio_cur, setup->wLength);
    
    audio_struct.audio_cmd = AUDIO_REQ_SET_CUR;
    audio_struct.audio_cmd_len = setup->wLength;
    
    switch(setup->bmRequestType & AUDIO_REQ_CONTROL_MASK)
    {
      case AUDIO_REQ_CONTROL_INTERFACE:
        audio_struct.interface = HBYTE(setup->wIndex);
        if(HBYTE(setup->wValue) == AUDIO_MUTE_CONTROL)
        {
          audio_struct.request_no = AUDIO_MUTE_CONTROL;
        }
        else
        {
          audio_struct.request_no = AUDIO_VOLUME_CONTROL;
        }
        break;
      case AUDIO_REQ_CONTROL_ENDPOINT:
        audio_struct.enpd = setup->wIndex;
        audio_struct.request_no = AUDIO_FREQ_SET_CONTROL;
        break;
      default:
        break;
    }
  }
}

/**
  * @brief  usb audio request get min
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_req_get_min(void *udev, usb_setup_type *setup)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(HBYTE(setup->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
  {
    *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.spk_volume_limits[0];
    usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
  }
  else
  {
    *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.mic_volume_limits[0];
    usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
  }
}

/**
  * @brief  usb audio request get max
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_req_get_max(void *udev, usb_setup_type *setup)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(HBYTE(setup->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
  {
    *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.spk_volume_limits[1];
    usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
  }
  else
  {
    *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.mic_volume_limits[1];
    usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
  }
}

/**
  * @brief  usb audio request get res
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_req_get_res(void *udev, usb_setup_type *setup)
{
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(HBYTE(setup->wIndex) == AUDIO_SPK_FEATURE_UNIT_ID)
  {
    *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.spk_volume_limits[2];
    usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
  }
  else
  {
    *((uint16_t *)audio_struct.g_audio_cur) = audio_struct.mic_volume_limits[2];
    usbd_ctrl_send(pudev, audio_struct.g_audio_cur, setup->wLength);
  }
}

/**
  * @brief  usb audio set interface
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_set_interface(void *udev, usb_setup_type *setup)
{
  uint32_t len;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  if(LBYTE(setup->wIndex) == AUDIO_SPK_INTERFACE_NUMBER)
  {
    audio_struct.spk_alt_setting = setup->wValue;
    audio_codec_spk_alt_setting(audio_struct.spk_alt_setting);
    if(audio_struct.spk_alt_setting )
    {
      int len = audio_codec_spk_feedback(audio_struct.audio_feed_back);
      usbd_ept_recv(pudev, USBD_AUDIO_SPK_OUT_EPT, audio_struct.audio_spk_data, AUDIO_SPK_OUT_MAXPACKET_SIZE);
      usbd_ept_send(pudev, USBD_AUDIO_FEEDBACK_EPT, audio_struct.audio_feed_back, len); 
    }
    
  }
  else if(LBYTE(setup->wIndex) == AUDIO_MIC_INTERFACE_NUMBER)
  {
    audio_struct.mic_alt_setting = setup->wValue;
    audio_codec_mic_alt_setting(audio_struct.mic_alt_setting);
    if(audio_struct.mic_alt_setting)
    {
      len = audio_codec_mic_get_data(audio_struct.audio_mic_data);
      usbd_ept_send(pudev, USBD_AUDIO_MIC_IN_EPT, audio_struct.audio_mic_data, len);
    }
  }
    
}


/**
  * @brief  usb audio get interface
  * @param  udev: usb device core handler type
  * @param  setup: setup class
  * @retval none                           
  */
void audio_get_interface(void *udev, usb_setup_type *setup)
{
  if(LBYTE(setup->wIndex) == AUDIO_SPK_INTERFACE_NUMBER)
  {
    usbd_ctrl_send((usbd_core_type *)udev, (uint8_t *)&audio_struct.spk_alt_setting, 1);
  }
  else if(LBYTE(setup->wIndex) == AUDIO_MIC_INTERFACE_NUMBER)
  {
    usbd_ctrl_send((usbd_core_type *)udev, (uint8_t *)&audio_struct.mic_alt_setting, 1);
  }
    
}

/**
  * @brief  usb device class send report
  * @param  udev: to the structure of usbd_core_type
  * @param  report: report buffer
  * @param  len: report length
  * @retval status of usb_sts_type                            
  */
usb_sts_type class_send_report(void *udev, uint8_t *report, uint16_t len)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;

  if(usbd_connect_state_get(pudev) == USB_CONN_STATE_CONFIGURED)
    usbd_ept_send(pudev, USBD_HID_IN_EPT, report, len);
  
  return status;
}


/**
  * @brief  usb device report function
  * @param  udev: to the structure of usbd_core_type
  * @param  report: report buffer
  * @param  len: report length
  * @retval none                            
  */
void usb_hid_buf_process(void *udev, uint8_t *report, uint16_t len)
{
  uint32_t i_index;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  
  switch(report[0])
  {
    case HID_REPORT_ID_2:
      if(g_rxhid_buff[1] == 0)
      {
        at32_led_off(LED2);
      }
      else
      {
        at32_led_on(LED2);
      }
      break;
    case HID_REPORT_ID_3:
      if(g_rxhid_buff[1] == 0)
      {
        at32_led_off(LED3);
      }
      else
      {
        at32_led_on(LED3);
      }
      break;
    case HID_REPORT_ID_4:
      if(g_rxhid_buff[1] == 0)
      {
        at32_led_off(LED4);
      }
      else
      {
        at32_led_on(LED4);
      }
      break;
    case HID_REPORT_ID_6:
      for(i_index = 0; i_index < len; i_index ++)
      {
        g_txhid_buff[i_index] = report[i_index];
      }
      usbd_ept_send(pudev, USBD_HID_IN_EPT, g_txhid_buff, len);
    default:
      break;   
  }
  
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



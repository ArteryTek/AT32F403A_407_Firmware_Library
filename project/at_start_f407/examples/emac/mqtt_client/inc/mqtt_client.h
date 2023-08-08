/**
  **************************************************************************
  * @file     mqtt_client.h
  * @brief    mqtt client header
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
#ifndef  _MQTT_CLIENT_H_
#define  _MQTT_CLIENT_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include "lwip/pbuf.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/netif.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */

/** @addtogroup 407_EMAC_mqtt_client EMAC_mqtt_client
  * @{
  */

/* mqtt server address and port configuration */
#define MQTT_SERVER_ADDRESS  "192.168.81.1"
#define MQTT_SERVER_PORT     1883

err_t mqtt_client_init(void);
static void at32_mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void at32_mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
static void at32_mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void mqtt_client_pub_request_cb(void *arg, err_t result);
err_t at32_mqtt_publish(mqtt_client_t *client, char *pub_topic, char *pub_buf, uint16_t data_len, uint8_t qos, uint8_t retain);
static void at32_mqtt_request_cb(void *arg, err_t err);
static err_t at32_mqtt_subscribe(mqtt_client_t *mqtt_client, char *sub_topic, uint8_t qos);
err_t mqtt_connect_routine(void);

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

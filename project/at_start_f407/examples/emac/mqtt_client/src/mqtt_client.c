/**
  **************************************************************************
  * @file     mqtt_client.c
  * @brief    mqtt client config program
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

/* includes ------------------------------------------------------------------*/
#include "mqtt_client.h"
#include "string.h"
#include "at32f403a_407_board.h"
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

mqtt_client_t *s__mqtt_client_instance;

/**
  * @brief  mqtt connection subscription process
  * @param  client: mqtt connection handle
  * @param  arg: mqtt connection parameter point
  * @retval none
  */
__WEAK void mqtt_conn_sub_proc(mqtt_client_t *client, void *arg)
{
  /* topic name */
  char test_sub_topic[] = "at_sub_topic";
  at32_mqtt_subscribe(client, test_sub_topic, 0);
}

/**
  * @brief  mqtt error process callback
  * @param  client: mqtt connection handle
  * @param  arg: mqtt connection parameter point
  * @retval none
  */
__WEAK void mqtt_error_process_callback(mqtt_client_t *client, void *arg)
{
  /* user code */
  mqtt_connect_routine();
}

/**
  * @brief  mqtt connection status callback
  * @param  client: mqtt connection handle
  * @param  arg: mqtt connection parameter point
  * @param  status: mqtt connection status
  * @retval none
  */
static void at32_mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  if (client == NULL)
  {
    printf("at32_mqtt_connection_cb: condition error @entry\r\n");
    return;
  }
  
  /* Successfully connected */
  if (status == MQTT_CONNECT_ACCEPTED)
  {
    printf("at32_mqtt_connection_cb: Successfully connected\r\n");

    mqtt_set_inpub_callback(client, at32_mqtt_incoming_publish_cb, at32_mqtt_incoming_data_cb, arg);

    mqtt_conn_sub_proc(client, arg);
  }
  else if (status == MQTT_CONNECT_DISCONNECTED)
  {
    printf("at32_mqtt_connection_cb: Fail connected, status = %s\r\n", lwip_strerr(status));

    mqtt_error_process_callback(client, arg);
  }
  else
  {
    printf("at32_mqtt_connection_cb: Fail connected, status = %s\r\n", lwip_strerr(status));
  }
}

/**
  * @brief  connect to mqtt server
  * @retval error status
  */
err_t mqtt_client_init(void)
{
  printf("at32_mqtt_connect: Enter!\r\n");

  if (s__mqtt_client_instance == NULL)
  {
    s__mqtt_client_instance = mqtt_client_new();
  }

  if (s__mqtt_client_instance == NULL)
  {
    printf("at32_mqtt_connect: s__mqtt_client_instance malloc fail @@!!!\r\n");
    return ERR_MEM;
  }
  
  return mqtt_connect_routine();
}

/**
  * @brief  mqtt received data callback
  * @param  arg: mqtt connection parameter point
  * @param  data: data
  * @param  len: data length
  * @param  flags: data flag
  * @retval none
  */
static void at32_mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  if ((data == NULL) || (len == 0))
  {
    printf("mqtt_client_incoming_data_cb: condition error @entry\r\n");
    return;
  }

  printf("recv_buffer = %s\r\n", data);

  printf("mqtt_client_incoming_data_cb:reveiving incomming data.\r\n");
}

/**
  * @brief  mqtt received publish callback
  * @param  arg: mqtt connection parameter point
  * @param  topic: mqtt topic
  * @param  tot_len: data total length
  * @retval none
  */
static void at32_mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
  if ((topic == NULL) || (tot_len == 0))
  {
    printf("at32_mqtt_incoming_publish_cb: condition error @entry\r\n");
    return;
  }

  printf("at32_mqtt_incoming_publish_cb: topic = %s.\r\n", topic);
  printf("at32_mqtt_incoming_publish_cb: tot_len = %d.\r\n", tot_len);
}

/**
  * @brief  mqtt send data callback
  * @param  arg: mqtt connection parameter point
  * @param  result: send data result
  * @retval none
  */
static void mqtt_client_pub_request_cb(void *arg, err_t result)
{
  mqtt_client_t *client = (mqtt_client_t *)arg;

  if (result != ERR_OK)
  {
    printf("mqtt_client_pub_request_cb: c002: Publish FAIL, result = %s\r\n", lwip_strerr(result));

    mqtt_error_process_callback(client, arg);
  }
  else
  {
    printf("mqtt_client_pub_request_cb: c005: Publish complete!\r\n");
  }
}

/**
  * @brief  mqtt send data to server
  * @param  client: mqtt connection handle
  * @param  pub_topic: mqtt publish topic
  * @param  pub_buf: mqtt publish buffer
  * @param  data_len: data length
  * @param  qos£ºquality of service, 0 1 or 2
  * @param  retain£ºmqtt retain flag
  * @retval send status
  */
err_t at32_mqtt_publish(mqtt_client_t *client, char *pub_topic, char *pub_buf, uint16_t data_len, uint8_t qos, uint8_t retain)
{
  err_t err;

  if ((client == NULL) || (pub_topic == NULL) || (pub_buf == NULL) || (data_len == 0) || (qos > 2) || (retain > 1))
  {
    printf("at32_mqtt_publish: input error@@");
    return ERR_VAL;
  }

#ifdef USE_MQTT_MUTEX

  if (s__mqtt_publish_mutex == NULL)
  {
    printf("at32_mqtt_publish: create mqtt mutex ! \r\n");
    s__mqtt_publish_mutex = xSemaphoreCreateMutex();
  }

  if (xSemaphoreTake(s__mqtt_publish_mutex, portMAX_DELAY) == pdPASS)
#endif /* USE_MQTT_MUTEX */
  {
    err = mqtt_publish(client, pub_topic, pub_buf, data_len, \
                       qos, retain, mqtt_client_pub_request_cb, (void *)client);
    if(err != ERR_OK)
    {
      printf("at32_mqtt_publish: mqtt_publish err = %s\r\n", lwip_strerr(err));
    }
#ifdef USE_MQTT_MUTEX
    printf("at32_mqtt_publish: mqtt_publish xSemaphoreTake\r\n");
    xSemaphoreGive(s__mqtt_publish_mutex);
#endif /* USE_MQTT_MUTEX */
  }

  return err;
}

/**
  * @brief  mqtt request callback
  * @param  arg: mqtt connection parameter point
  * @param  err: error status
  * @retval none
  */
static void at32_mqtt_request_cb(void *arg, err_t err)
{
  mqtt_client_t *client = (mqtt_client_t *)arg;

  if (arg == NULL)
  {
    printf("at32_mqtt_request_cb: input error@@\r\n");
    return;
  }

  if (err != ERR_OK)
  {
    printf("at32_mqtt_request_cb: FAIL sub, sub again, err = %s\r\n", lwip_strerr(err));

    mqtt_error_process_callback(client, arg);
  }
  else
  {
    printf("at32_mqtt_request_cb: sub SUCCESS!\r\n");
  }
}

/**
  * @brief  mqtt subscribe
  * @param  client: mqtt connection handle
  * @param  sub_topic: mqtt subscribe topic
  * @param  qos£ºquality of service, 0 1 or 2
  * @retval subscribe status
  */
static err_t at32_mqtt_subscribe(mqtt_client_t *mqtt_client, char *sub_topic, uint8_t qos)
{
  err_t err;
  printf("at32_mqtt_subscribe: Enter\r\n");

  if ((mqtt_client == NULL) || (sub_topic == NULL) || (qos > 2))
  {
    printf("at32_mqtt_subscribe: input error@@\r\n");
    return ERR_VAL;
  }

  if (mqtt_client_is_connected(mqtt_client) != 1)
  {
    printf("at32_mqtt_subscribe: mqtt is not connected, return ERR_CLSD.\r\n");
    return ERR_CLSD;
  }

  err = mqtt_subscribe(mqtt_client, sub_topic, qos, at32_mqtt_request_cb, (void *)mqtt_client);

  if (err != ERR_OK)
  {
    printf("at32_mqtt_subscribe: mqtt_subscribe Fail, return:%s \r\n", lwip_strerr(err));
  }
  else
  {
    printf("at32_mqtt_subscribe: mqtt_subscribe SUCCESS, reason: %s\r\n", lwip_strerr(err));
  }

  return err;
}

err_t mqtt_connect_routine(void)
{
  err_t ret;
  struct mqtt_connect_client_info_t mqtt_connect_info =
  {

    "AT_MQTT_Test",
    NULL,
    NULL,
    60,
    "at_pub_topic",
    "Offline_pls_check",
    0,
    0
  };

  ip_addr_t server_ip;
  /* mqtt server port */
  uint16_t server_port = MQTT_SERVER_PORT;
  /* mqtt server ip */
  ip4_addr_set_u32(&server_ip, ipaddr_addr(MQTT_SERVER_ADDRESS));
  ret = mqtt_client_connect(s__mqtt_client_instance, &server_ip, server_port, \
                            at32_mqtt_connection_cb, NULL, &mqtt_connect_info);

  printf("at32_mqtt_connect: connect to mqtt %s\r\n", lwip_strerr(ret));
  
  return ret;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  **************************************************************************
  * @file     netconf.c
  * @brief    network connection configuration
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

/* Includes ------------------------------------------------------------------*/
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "ethernetif.h"
#include "netconf.h"
#include "dns.h"
#include "stdio.h"
#include "at32_emac.h"
#include "timeouts.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define MAC_ADDR_LENGTH                  (6)
#define ADDR_LENGTH                      (4)
#define SYSTEMTICK_PERIOD_MS             10
/* Private variables ---------------------------------------------------------*/
extern volatile uint32_t local_time;
struct netif netif;
volatile uint32_t tcp_timer = 0;
volatile uint32_t arp_timer = 0;
volatile uint32_t link_timer = 0;
#if LWIP_DNS
volatile uint32_t dns_timer = 0;
#endif

static uint8_t mac_address[MAC_ADDR_LENGTH] = {0, 0, 0x44, 0x45, 0x56, 1};;
#if LWIP_DHCP
volatile uint32_t dhcp_fine_timer = 0;
volatile uint32_t dhcp_coarse_timer = 0;
#else
static uint8_t local_ip[ADDR_LENGTH]   = {192, 168, 81, 37};
static uint8_t local_gw[ADDR_LENGTH]   = {192, 168, 81, 187};
static uint8_t local_mask[ADDR_LENGTH] = {255, 255, 255, 0};
#endif
extern u32_t lwip_sys_now;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  initializes the lwip stack
  * @param  none
  * @retval none
  */
void tcpip_stack_init(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  /* Initialize the LwIP stack */
  lwip_init();

#if LWIP_DHCP  //need DHCP server
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;

#else
  IP4_ADDR(&ipaddr, local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
  IP4_ADDR(&netmask, local_mask[0], local_mask[1], local_mask[2], local_mask[3]);
  IP4_ADDR(&gw, local_gw[0], local_gw[1], local_gw[2], local_gw[3]);
#endif

  lwip_set_mac_address(mac_address);

  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
            struct ip_addr *netmask, struct ip_addr *gw,
            void *state, err_t (* init)(struct netif *netif),
            err_t (* input)(struct pbuf *p, struct netif *netif))

   Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/

  if(netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &netif_input) == NULL)
  {
    while(1);
  }

  /*  Registers the default network interface.*/
  netif_set_default(&netif);

  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&netif);
  
  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&netif, ethernetif_update_config);
}

/**
  * @brief  called when a frame is received
  * @param  none
  * @retval none
  */
void lwip_pkt_handle(void)
{
  /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
  ethernetif_input(&netif);
}

/**
  * @brief  this function is receive handler.
  * @param  none
  * @retval none
  */
void lwip_rx_loop_handler(void)
{
  /* handles all the received frames */
  lwip_pkt_handle();
}

/**
  * @brief  updates the system local time
  * @param  none
  * @retval none
  */
void time_update(void)
{
  local_time += SYSTEMTICK_PERIOD_MS;
  lwip_sys_now += SYSTEMTICK_PERIOD_MS;
}

/**
  * @brief  lwip periodic tasks
  * @param  localtime the current localtime value
  * @retval none
  */
void lwip_periodic_handle(volatile uint32_t localtime)
{
  /* lwip timeout */
  sys_check_timeouts();
  
#if (LINK_DETECTION > 0)
  /* link detection process every 500 ms */
  if (localtime - link_timer >= 500 || localtime < link_timer)
  {
    link_timer =  localtime;
    ethernetif_set_link(&netif);
  }
#endif
}


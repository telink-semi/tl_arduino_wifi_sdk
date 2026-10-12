/********************************************************************************************************
 * @file    port_network.c
 *
 * @brief   This is the source file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "port_config.h"
#if BASED_ON_TELINK_WIFI_SDK

#include "port_interface.h"
#include <sys/socket.h>
#include <netdb.h>
#include <lwip/tcp.h>
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include <lwip/tcpip.h>
#include <lwip/dns.h>
#include <arpa/inet.h>
#include "esp_transport_tcp.h"
#include "esp_transport_internal.h"
#include "sdk_common.h"

void* port_network_udp_begin(uint8_t type, ip_addr_t *ip, uint16_t port, uint32_t recvbufsize) {
  void *udp_server = NULL;
#ifdef IWIP_IPV6
  uint32_t udp_type = (type == IPv6) ? NETCONN_UDP_IPV6 : NETCONN_UDP;
#else
  uint32_t udp_type = NETCONN_UDP;
#endif
  struct netconn *conn = netconn_new(udp_type);

  do {
    if(!conn) break;
    if (netconn_bind(conn, ip, port) != ERR_OK) break;
    netconn_set_recvtimeout(conn, 0);
    netconn_set_recvbufsize(conn, recvbufsize);
    udp_server = conn;
  } while (0);

  if(!udp_server && conn) {
    netconn_delete(conn);
  }

  return udp_server;
}

void* port_network_udp_begin_packet(uint8_t type, uint32_t recvbufsize) {
  void *udp_server = NULL;
#ifdef IWIP_IPV6
  uint32_t udp_type = (type == IPv6) ? NETCONN_UDP_IPV6 : NETCONN_UDP;
#else
  uint32_t udp_type = NETCONN_UDP;
#endif
  struct netconn *conn = netconn_new(udp_type);

  do {
    if(!conn) break;
    netconn_set_recvtimeout(conn, 0);
    netconn_set_recvbufsize(conn, recvbufsize);
    udp_server = conn;
  } while (0);

  if(!udp_server && conn) {
    netconn_delete(conn);
  }

  return udp_server;
}

bool port_network_udp_end_packet(void *udp_server, ip_addr_t *ip, uint16_t port, uint8_t *data, uint32_t len) {
  bool ret = false;
  struct netconn *conn = (struct netconn *)udp_server;
  struct netbuf *buf = netbuf_new();
  void *payload = NULL;
  uint16_t payload_len;
  do {
    if(!conn || !buf) break;
    if(netbuf_alloc(buf, len) != ERR_OK) break;
    if(netbuf_data(buf, &payload, &payload_len) != ERR_OK) break;
    if(!payload) break;
    memcpy(payload, data, len);
    if(netconn_sendto(conn, buf, ip, port) != ERR_OK) break;
    ret = true;
  }while(0);

  if(!ret && buf) {
    netbuf_delete(buf);
  }

  return ret;
}

bool port_network_udp_add_multicast(void *udp_server, ip_addr_t* multicast_ip) {
  bool ret = false;
  struct netconn *conn = (struct netconn *)udp_server;
  if(conn) {
  #if 1
    if(netconn_join_leave_group(conn, multicast_ip, NULL, NETCONN_JOIN) == ERR_OK) {
      ret = true;
    }
  #endif
  }
  return ret;
}

int port_network_udp_parse_packet(void *udp_server, ip_addr_t *ip, uint16_t *port, uint8_t *data, uint32_t len) {
  int ret = -1;
  struct netconn *conn = (struct netconn *)udp_server;
  struct netbuf *buf = NULL;
  void *payload = NULL;
  uint16_t payload_len;
  do {
    if(!conn) break;
    if(netconn_recv(conn, &buf) != ERR_OK) break;
    if(!buf) break;
    if(netbuf_data(buf, &payload, &payload_len) != ERR_OK) break;
    if(!payload) break;
    if(payload_len > len) payload_len = len;
    memcpy(data, payload, payload_len);
    *ip = *netbuf_fromaddr(buf);
    *port = netbuf_fromport(buf);
    netbuf_delete(buf);
    ret = payload_len;
  }while(0);
  return ret;
}

void port_network_stop(void *server) {
  struct netconn *conn = (struct netconn *)server;
  if(conn) {
    netconn_close(conn);
    netconn_delete(conn);
  }
}

const uint8_t * port_network_get_host_by_name(const char *host) {
  struct hostent *server;
  server = gethostbyname(host);
  if (server == NULL) {
    log_e("could not get host from dns: %d", errno);
    return NULL;
  }
  return (const uint8_t *)(server->h_addr_list[0]);
}

void port_network_config_ip(ip_addr_t *ip, ip_addr_t *nm, ip_addr_t *gw, struct netif *netif) {
  netif_set_addr(netif, ip, nm, gw);
}

void port_network_get_ip_addr(ip_addr_t *ip, struct netif *netif) {
  if(netif && ip) {
  #if LWIP_IPV6
    if(!ip6_addr_isany_val(netif->ip6_addr)) {
      memcpy(ip, &netif->ip6_addr[0], sizeof(ip_addr_t));
    }
  #else
    if(!ip4_addr_isany_val(netif->ip_addr)) {
      memcpy(ip, &netif->ip_addr, sizeof(ip_addr_t));
    }
  #endif
  }
}

void port_network_get_dns_server(ip_addr_t *ip, uint8_t index) {
  const ip_addr_t *dns = dns_getserver(index & 1);
  if(dns) {
    memcpy(ip, dns, sizeof(ip_addr_t));
  }
}

void port_network_set_dns_server(ip_addr_t *ip, uint8_t index) {
  if(!ip) return;
  dns_setserver(index & 1, ip);
}

void port_network_set_host_name(const char *name, struct netif *netif) {
  if(!netif) return;
  netif->hostname = name;
}

const char* port_network_get_host_name(struct netif *netif) {
  if(!netif) return NULL;
  return netif->hostname;
}

void port_network_get_gateway(ip_addr_t *gw, struct netif *netif) {
  if(netif && gw) {
  #if LWIP_IPV6
    //TODO: get ipv6 gateway
  #else
    if(!ip4_addr_isany_val(netif->gw)) {
      memcpy(gw, &netif->gw, sizeof(ip_addr_t));
    }
  #endif
  }
}

void port_network_get_mac_addr(uint8_t mac[NETWORK_MAC_ADDR_LEN], struct netif *netif) {
  if(netif && mac) {
    memcpy(mac, netif->hwaddr, NETWORK_MAC_ADDR_LEN);
  }
}

void port_network_get_subnet_mask(ip_addr_t *mask, struct netif *netif) {
  if(netif && mask) {
  #if LWIP_IPV6
    //not support
  #else
    if(!ip4_addr_isany_val(netif->netmask)) {
      memcpy(mask, &netif->netmask, sizeof(ip_addr_t));
    }
  #endif
  }
}

void port_network_get_broadcast_addr(ip_addr_t *broadcast, struct netif *netif) {
  if(netif && broadcast) {
  #if LWIP_IPV6
    //not support
  #else
    if(!ip4_addr_isany_val(netif->ip_addr)) {
      broadcast->addr = netif->ip_addr.addr | ~netif->netmask.addr;
    }
  #endif
  }
}

void port_network_get_id(ip_addr_t *id, struct netif *netif) {
  if(netif && id) {
  #if LWIP_IPV6
    //not support
  #else
    if(!ip4_addr_isany_val(netif->ip_addr)) {
      id->addr = netif->ip_addr.addr & netif->netmask.addr;
    }
  #endif
  }
}

uint8_t port_network_get_subnet_cdir(struct netif *netif) {
#if LWIP_IPV6
  return 0;
#else
  ip_addr_t id;
  port_network_get_id(&id, netif);
  uint8_t cdir = 0;
  uint32_t m = ntohl(id.addr);
  while (m & 0x80000000) {
    cdir++;
    m <<= 1;
  }
  return cdir;
#endif
}

bool port_network_get_ctx(void *handle, ip_addr_t *ip, uint16_t *port, bool local) {
  struct netconn *conn = (struct netconn *)handle;
  bool ret = false;
  do {
    if(!conn || !ip || !port) break;
    if(netconn_getaddr(conn, ip, port, (local? 1: 0)) != ERR_OK) break;
    ret = true;
  } while (0);

  return ret;
}

enum {
    TCP_STATE_IDLE,
    TCP_STATE_BUSY,
    TCP_STATE_RECONNECT,
};

struct _port_tcp_ctx_t{
    void *handle;
    void *owner;
    uint32_t ref_time;
    uint8_t state;
    background_handle_t bgHandle;
    // configuration
    ip_addr_t ip;
    uint16_t port;
    uint32_t recon_time_ms;
    uint32_t timeout_ms;
};
typedef struct _port_tcp_ctx_t port_tcp_ctx_t, *port_tcp_ctx_t_ptr;

__attribute__((weak)) int tcp_task_read_callback(void *para) {
  (void)para;
  return 0;
}

__attribute__((weak)) int tcp_task_write_callback(void *para) {
  (void)para;
  return 0;
}

__attribute__((weak)) void tcp_task_stop_callback(void *para) {
  (void)para;
}

static bool tcp_check_connected(port_tcp_ctx_t_ptr tcpCtx) {
    if(esp_transport_poll_read(tcpCtx->handle, 0) < 0) {
        return false;
    }
    int error = esp_transport_get_errno(tcpCtx->handle);
    if(error != 0) log_e("tcp_check_connected last error: %d", error);
    return error == 0;
}

static bool tcp_connect__(port_tcp_ctx_t_ptr tcpCtx) {
    if(esp_transport_connect(tcpCtx->handle, ipaddr_ntoa(&tcpCtx->ip), tcpCtx->port, tcpCtx->timeout_ms) >= 0) {
        return true;
    }

    return false;
}

static void tcp_state_busy_process(port_tcp_ctx_t_ptr tcpCtx) {
    if(!tcp_check_connected(tcpCtx)) {
        log_e("tcp check connected failed");  
        tcpCtx->state = TCP_STATE_IDLE;  
        return;
    }

    if(tcp_task_write_callback(tcpCtx) < 0) {
        log_e("tcp task write callback error");
        tcpCtx->state = TCP_STATE_IDLE;  
        return;
    }

    if(tcp_task_read_callback(tcpCtx) < 0) {
        log_e("tcp task read callback error");
        tcpCtx->state = TCP_STATE_IDLE;  
        return;
    }   
}

static void tcp_state_reconnect_process(port_tcp_ctx_t_ptr tcpCtx) {
    if(millis() - tcpCtx->ref_time > tcpCtx->recon_time_ms) {
        if(tcp_connect__(tcpCtx)) {
            tcpCtx->state = TCP_STATE_BUSY;
            return;
        }

        tcpCtx->ref_time = millis();
    }
}

static void tcp_state_idle_process(port_tcp_ctx_t_ptr tcpCtx) {
    if(tcpCtx->recon_time_ms > 0) {
        esp_transport_close(tcpCtx->handle);
        tcpCtx->state = TCP_STATE_RECONNECT;
        tcpCtx->ref_time = millis();
    } else {
        tcp_task_stop_callback(tcpCtx);
    }
}

static void tcp_handler(void *ctx) {
    if(!port_tcp_is_opened(ctx)) return;
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;

    switch(tcpCtx->state) {
    case TCP_STATE_BUSY:
        tcp_state_busy_process(tcpCtx);
        break;
    case TCP_STATE_RECONNECT:
        tcp_state_reconnect_process(tcpCtx);
        break;
    case TCP_STATE_IDLE:
        tcp_state_idle_process(tcpCtx);
        break;
    default:
        break;
    }
}

void *port_tcp_create_context(void *owner) {
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)malloc(sizeof(port_tcp_ctx_t));
    if(tcpCtx) {
        memset(tcpCtx, 0, sizeof(port_tcp_ctx_t));
        tcpCtx->owner = owner;
    }
    return tcpCtx;
}

void port_tcp_destroy_context(void *ctx) {
    if(ctx) {
        free(ctx);
    }
}

bool port_tcp_set_configuration(void *ctx, port_tcp_configuration_t *config) {
    if(!ctx || !config) return false;
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;

    if(config->owner_valid) {
        tcpCtx->owner = config->owner;
    }

    if(config->ip_valid) {
        tcpCtx->ip = config->ip;
    }

    if(config->port_valid) {
        tcpCtx->port = config->port;
    }

    if(config->recon_time_ms_valid) {
        tcpCtx->recon_time_ms = config->recon_time_ms;
    }

    if(config->timeout_ms_valid) {
        tcpCtx->timeout_ms = config->timeout_ms;
    }

    return true;
}

bool port_tcp_get_configuration(void *ctx, port_tcp_configuration_t *config) {
    if(!ctx || !config) return false;
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;

    config->owner_valid = 1;
    config->ip_valid = 1;
    config->port_valid = 1;
    config->recon_time_ms_valid = 1;
    config->timeout_ms_valid = 1;

    config->owner = tcpCtx->owner;
    config->ip = tcpCtx->ip;
    config->port = tcpCtx->port;
    config->recon_time_ms = tcpCtx->recon_time_ms;
    config->timeout_ms = tcpCtx->timeout_ms;

    return true;
}

bool port_tcp_connect(void *ctx)
{
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;
    bool ret = false;
    do {
        if(!tcpCtx) break;
        if(port_tcp_is_opened(ctx)) break;
        tcpCtx->handle = esp_transport_tcp_init();
        if(!tcpCtx->handle) break;

        if(!tcp_connect__(tcpCtx)) break;

        tcpCtx->state = TCP_STATE_BUSY;
        tcpCtx->bgHandle = port_background_register_handler(tcp_handler, ctx, 0);
        if(!tcpCtx->bgHandle) break;

        ret = true;
    } while(0);

    if(!ret) {
        if(tcpCtx) {
            if(tcpCtx->handle) {
                esp_transport_close(tcpCtx->handle);
                esp_transport_destroy(tcpCtx->handle);
                tcpCtx->handle = NULL;
            }
        }
    }

    return ret;
}

int port_tcp_read(void *ctx, uint8_t *buffer, size_t len)
{
  if(!port_tcp_is_opened(ctx) || !buffer) return -1;
  port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;
  
  int ret = esp_transport_poll_read(tcpCtx->handle, 0);
  if( ret > 0) {
      ret = esp_transport_read(tcpCtx->handle, (char *)buffer, len, 0);
  } else {
      if(ret < 0) {
          int error = esp_transport_get_errno(tcpCtx->handle);
          if(error != 0) {
              log_e("port_tcp_read, last error: %d", error);
          }else {
              ret = 0;
          }      
      }
  }

  return ret;
}

int port_tcp_write(void *ctx, uint8_t *data, size_t len)
{
  port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;
  if(!port_tcp_is_opened(ctx) || !data) return -1;
  return esp_transport_write(tcpCtx->handle, (char *)data, len, -1);

}

bool port_tcp_close(void *ctx)
{
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;
    if(!tcpCtx) return false;
    port_background_unregister_handler(tcpCtx->bgHandle);
    tcpCtx->bgHandle = NULL;

    if(tcpCtx->handle) {
        esp_transport_close(tcpCtx->handle);
        esp_transport_destroy(tcpCtx->handle);
        tcpCtx->handle = NULL;
    }
   
    return true;
}

bool port_tcp_is_opened(void *ctx) {
    port_tcp_ctx_t_ptr tcpCtx = (port_tcp_ctx_t_ptr)ctx;
    return ((tcpCtx != NULL) && 
            (tcpCtx->handle != NULL));
}

#endif
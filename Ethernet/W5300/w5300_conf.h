/**
******************************************************************************
* @file         w5100s_conf.h                                                                                              
* @version      V1.0                          
* @date         2018-06-18                      
* @brief        w5100s_conf.c的头文件      
*        
* @company      深圳炜世科技有限公司
* @information  WIZnet W5100s官方代理商，全程技术支持，价格优势大！
* @website      www.wisioe.com  
* @forum        www.w5100s.com
* @qqGroup      579842114                                                     
******************************************************************************
*/
#ifndef _W5300_CONF_H_
#define _W5300_CONF_H_

#include "string.h"
#include "stdint.h"

typedef  void (*pFunction)(void);

extern uint16_t rxlen;
extern uint8_t  rxbuf[1024];

extern uint8_t  local_ip[4];                             // 定义W5100s默认IP地址
extern uint8_t  subnet[4];                               // 定义W5100s默认子网掩码
extern uint8_t  gateway[4];                              // 定义W5100s默认网关
extern uint8_t  dns_server[4];                           // 定义W5100s默认DNS服务器地址

extern uint8_t  remote_ip[4];                            // 远端IP地址
extern uint16_t remote_port;                             // 远端端口号
extern uint16_t local_port;                              // 定义本地端口

extern uint32_t dhcp_tick_1s;                            // DHCP运行计数
extern uint32_t ntptimer;                                // NTP运行计数
extern uint8_t  dhcp_state;

#define TX_RX_MAX_BUF_SIZE    2048

#define IP_FROM_DEFINE        0                          // 使用初始定义的IP信息
#define IP_FROM_DHCP          1                          // 使用DHCP获取IP信息
#define IP_FROM_FLASH         2                          // 从FLASH中读取IP信息
    
#define MAX_SOCK_NUM          8

#define SOCK_UDPS             1
#define SOCK_TCPC             1
#define SOCK_TCPS             2
#define SOCK_DHCP             0
#define SOCK_DNS              0
#define SOCK_HTTPC            4
#define SOCK_HTTPS            4
#define SOCK_NETBIOS          5
#define SOCK_NTP              6
#define SOCK_PING             7
#define SOCK_WEBSOCKET        4

#pragma pack(1)
/*此结构体定义了W5100S可供配置的主要参数*/
typedef struct _CONFIG_MSG                      
{
  uint8_t mac[6];            // MAC地址
  uint8_t lip[4];            // local IP本地IP地址
  uint8_t sub[4];            // 子网掩码
  uint8_t gw[4];             // 网关  
  uint8_t dns[4];            // DNS服务器地址
  uint8_t rip[4];            // remote IP远程IP地址
  uint8_t sw_ver[2];         // 软件版本号
}CONFIG_MSG;

#pragma pack()

extern CONFIG_MSG ConfigMsg;

/*MCU配置相关函数*/
void reboot(void);           

/*W5100S基本配置相关函数*/
void reset_break_gpio_init(void);
void reset_w5300(void);           
void set_w5300_netinfo(void);                 
void set_w5300_mac(void);
void dhcp_timer_init(void);
void ntp_timer_init(void);
void timer2_init(void);
void timer2_isr(void);
uint8_t getPHYStatus(void);
void PHY_check(void);
void sysinit(void);

#endif //_W5100S_CONF_H_

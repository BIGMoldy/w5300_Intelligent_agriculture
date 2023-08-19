/**
******************************************************************************
* @file         w5100s_conf.h                                                                                              
* @version      V1.0                          
* @date         2018-06-18                      
* @brief        w5100s_conf.c��ͷ�ļ�      
*        
* @company      ��������Ƽ����޹�˾
* @information  WIZnet W5100s�ٷ������̣�ȫ�̼���֧�֣��۸����ƴ�
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

extern uint8_t  local_ip[4];                             // ����W5100sĬ��IP��ַ
extern uint8_t  subnet[4];                               // ����W5100sĬ����������
extern uint8_t  gateway[4];                              // ����W5100sĬ������
extern uint8_t  dns_server[4];                           // ����W5100sĬ��DNS��������ַ

extern uint8_t  remote_ip[4];                            // Զ��IP��ַ
extern uint16_t remote_port;                             // Զ�˶˿ں�
extern uint16_t local_port;                              // ���屾�ض˿�

extern uint32_t dhcp_tick_1s;                            // DHCP���м���
extern uint32_t ntptimer;                                // NTP���м���
extern uint8_t  dhcp_state;

#define TX_RX_MAX_BUF_SIZE    2048

#define IP_FROM_DEFINE        0                          // ʹ�ó�ʼ�����IP��Ϣ
#define IP_FROM_DHCP          1                          // ʹ��DHCP��ȡIP��Ϣ
#define IP_FROM_FLASH         2                          // ��FLASH�ж�ȡIP��Ϣ
    
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
/*�˽ṹ�嶨����W5100S�ɹ����õ���Ҫ����*/
typedef struct _CONFIG_MSG                      
{
  uint8_t mac[6];            // MAC��ַ
  uint8_t lip[4];            // local IP����IP��ַ
  uint8_t sub[4];            // ��������
  uint8_t gw[4];             // ����  
  uint8_t dns[4];            // DNS��������ַ
  uint8_t rip[4];            // remote IPԶ��IP��ַ
  uint8_t sw_ver[2];         // ����汾��
}CONFIG_MSG;

#pragma pack()

extern CONFIG_MSG ConfigMsg;

/*MCU������غ���*/
void reboot(void);           

/*W5100S����������غ���*/
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

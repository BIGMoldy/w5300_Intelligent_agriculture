

#include "w5300_conf.h"

#include "usart.h"
#include "tim.h"
#include "bsp_flash.h"

#include "socket.h"
#include "wizchip_conf.h"


#include "dhcp.h"

#include "http_server.h"
#include "mqtt_client.h"
#include "tcp_client.h"
#include "MqttKit.h"

CONFIG_MSG  ConfigMsg, RecvMsg;                          

uint16_t rxlen = 0;
uint8_t  rxbuf[1024];


uint8_t mac[6]={0x00,0x08,0xdc,0x22,0x33,0x55};


uint8_t  local_ip[4]   = {192,168,124,203};                    
uint8_t  subnet[4]     = {255,255,255,0};                    
uint8_t  gateway[4]    = {192,168,124,1};                       
uint8_t  dns_server[4] = {114,114,114,114};                   

uint16_t local_port    = 8900;                               


uint8_t  remote_ip[4]  = {192,168,124,6};                    
uint16_t remote_port   = 9000;                               


uint8_t  ip_from = IP_FROM_DEFINE;      

uint8_t  s              = 0;                                  
uint32_t ms             = 0;                                
uint32_t ntptimer       = 0;                                 
uint8_t  MQTT_ping_time = 0;                                  

uint8_t txsize[MAX_SOCK_NUM] = {8,8,8,8,8,8,8,8}; // tx buffer set K bytes
uint8_t rxsize[MAX_SOCK_NUM] = {8,8,8,8,8,8,8,8}; // rx buffet set K bytes

void set_w5300_netinfo(void)
{
  
  memcpy(ConfigMsg.mac, mac, 6);
  memcpy(ConfigMsg.lip,local_ip,4);
  memcpy(ConfigMsg.sub,subnet,4);
  memcpy(ConfigMsg.gw,gateway,4);
  memcpy(ConfigMsg.dns,dns_server,4);
#ifdef  _HTTP_SERVER_H_

  ReadFlashNBtye(0,&ip_from,1);
	if(ip_from != 0xFF)
		ip_from = IP_FROM_DEFINE;
#endif
  switch(ip_from)
  {
    case IP_FROM_DEFINE:
     
      break;
		case IP_FROM_DHCP:
		
			s = 0;
			while(s < 15)
			{
				DHCP_run();
				if(dhcp_state == STATE_DHCP_LEASED)
					return;
			}
		
#ifdef  _HTTP_SERVER_H_      
    case IP_FROM_FLASH:
     
      ReadFlashNBtye(0,ConfigMsg.lip,12);
      break;
#endif
  }
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  getSIPR(local_ip);      
  printf("\r\n W5300 I P £º%d.%d.%d.%d",local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
  getSUBR(subnet);
  printf("\r\n W5300 £º%d.%d.%d.%d",subnet[0],subnet[1],subnet[2],subnet[3]);
  getGAR(gateway);
  printf("\r\n W5300 £º%d.%d.%d.%d",gateway[0],gateway[1],gateway[2],gateway[3]);
}


void set_w5300_mac(void)
{ 
  memcpy(ConfigMsg.mac, mac, 6);
  setSHAR(ConfigMsg.mac);
  getSHAR(mac);
  printf(" W5300 MAC£º%02x.%02x.%02x.%02x.%02x.%02x\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}
  


void reset_w5300(void)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_Delay(500);  
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_Delay(500);  
}


void reboot(void)
{
  printf("\r\n rest....Ð!\r\n");
	NVIC_SystemReset();
}


void dhcp_timer_init(void)
{
	MX_TIM6_Init();
	timer2_init();
}


void ntp_timer_init(void)
{
	MX_TIM6_Init();
  timer2_init();
}


void timer2_init(void)
{
	HAL_TIM_Base_Start_IT(&htim6);
}


void timer2_isr(void)
{
  ms++;  
  if(ms >= 1000)
  { 
		if(s < 20)
			s++;
    ms = 0;
    dhcp_tick_1s++;                   
#ifdef  _MQTT_CLIENT_H_		
		MQTT_ping_time++;                
		if(mqtt_connect == 1)
			MQTT_STATE = MQTT_PKT_PUBLISH;  
#endif		
#ifdef __NTP_H__
    ntptimer++;                       
#endif
  }
}


void PHY_check(void)
{
  uint8_t PHY_connect=0;
  PHY_connect = 0x01 & getPHYStatus();
  if(PHY_connect == 0)
  {
    
    PHY_connect = 0;
    do
    {
      close(PHY_connect++);
    }while(PHY_connect < 8);
    PHY_connect = 0;
    while(PHY_connect == 0)
		{ 
      PHY_connect = 0x01 & getPHYStatus();  
      printf(" .");
      HAL_Delay(500);
    }
    printf(" \r\n");  
  }
}

uint8_t getPHYStatus(void)
{
  
	return !HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_9);
}


void sysinit(void)
{
	uint8_t i = 0;
	for(i = 0;i < MAX_SOCK_NUM - 1;i++)
	{
		setTMSR(i,txsize[i]);
		setRMSR(i,rxsize[i]);	
	}
}

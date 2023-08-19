
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "tcp_client.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "w5300_conf.h"

#include "mqtt_client.h"
#include "MqttKit.h"


#include "dns.h"

#include "dht11.h"
#include "relay.h"
#include "yl_69.h"
#include "g5516.h"

#include "serial_studio.h"

#define MQTT_DNS_NAME "a1iMg8UonSl.iot-as-mqtt.cn-shanghai.aliyuncs.com"

#define D_temperature    "temperature"
#define D_Humidity       "Humidity"
#define D_soilHumidity   "soilHumidity"
#define D_AlarmSwitch    "AlarmSwitch"
#define D_LightLux       "LightLux"

uint8_t temperature;     
uint8_t humidity;        
uint8_t yl69_humidity;  
uint16_t g5516_LightLux;

uint8_t mqtt_connect = 0;

uint8_t tcp_client_buff[DATA_BUF_SIZE];	
uint8_t  BD_TG_server_ip[4] = {106,15,229,25};         
uint16_t BD_TG_server_port  = 1883;                   

int MQTT_STATE = MQTT_PKT_CONNECT;   

const char *device_set_topics[150] = {"/sys/hxrzyBEzRJz/MQTT1/thing/event/property/post"};
const char *device_set_topics1[150] = {"/sys/hxrzyBEzRJz/MQTT1/thing/service/property/set"};

char aly_sensor_arr[200] = "{\"id\":\"123\",\"version\":\"1.0\",\"sys\":{\"ack\":0},\"params\":{\"temperature\":22,\"Humidity\":34,\"soilHumidity\":56,\"AlarmSwitch\":1,\"LightLux\":1140},\"method\":\"thing.event.property.post\"}";

uint8_t BD_TG_ping_pak[2] = {0xC0,0x00};

unsigned char *data_ptr = NULL;

void do_mqtttcp_client(void)
{
	uint16_t len = 0;	

	switch(getSn_SR(SOCK_TCPC))                                       
	{
		case SOCK_CLOSED:                                            
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
			if(local_port > 8990)
				local_port = 8900;
				dhcp_timer_init();
			  mqtt_connect = 0;
		break;

		case SOCK_INIT:                                              
			if(DNS_run(gateway,MQTT_DNS_NAME,BD_TG_server_ip) == 1)
				connect(SOCK_TCPC,BD_TG_server_ip,BD_TG_server_port);    
			else
				printf("\r\n DNS Flash£¡");
		break;

		case SOCK_ESTABLISHED:                                      
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON);                      
			}

			len = getSn_RX_RSR(SOCK_TCPC);                             
			{
				recv(SOCK_TCPC,tcp_client_buff,len);                            
				data_ptr = tcp_client_buff;
				if(data_ptr != NULL)
					BD_TG_RevPro(data_ptr);
			}	
			switch(MQTT_STATE)
			{
				
				case MQTT_PKT_CONNECT:
					BD_TG_DevLink();
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;
			
				case MQTT_PKT_SUBSCRIBE:
					BD_TG_Subscribe(device_set_topics,1);
					BD_TG_Subscribe(device_set_topics1,1);
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;	
			
				case MQTT_PKT_PUBLISH:	
					aly_sensor();
					BD_TG_Publish(*device_set_topics, aly_sensor_arr);            
					HAL_Delay(300);                                           
				
					len=getSn_RX_RSR(SOCK_TCPC); 
					recv(SOCK_TCPC,tcp_client_buff,len); 
					data_ptr = tcp_client_buff;
					if(data_ptr != NULL)
					BD_TG_RevPro(data_ptr);
					HAL_Delay(100);                                      
					len=getSn_RX_RSR(SOCK_TCPC); 
					recv(SOCK_TCPC,tcp_client_buff,len); 
					data_ptr = tcp_client_buff;
					if(data_ptr != NULL)
						BD_TG_RevPro(data_ptr);
					
					MQTT_STATE = MQTT_PKT_PINGREQ;
			
				break;
				case MQTT_PKT_UNSUBSCRIBE:
					MQTT_UnSubscribe(device_set_topics,1);
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;
				case MQTT_PKT_PINGREQ:
					if(MQTT_ping_time > 100)
					{
						send(SOCK_TCPC,BD_TG_ping_pak,2);
						MQTT_ping_time = 0;
					}
				break;
			}	
    break;

	case SOCK_CLOSE_WAIT:                                                
		close(SOCK_TCPC);
		break;
	}
}
void do_tcp_client(void)
{
  uint16_t len = 0;	

  switch(getSn_SR(SOCK_TCPC))                           
  {
    case SOCK_CLOSED:                                 
      socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
      break;

    case SOCK_INIT:                                    
      connect(SOCK_TCPC,remote_ip,remote_port);       
      break;

    case SOCK_ESTABLISHED:                              
      if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
      {
        setSn_IR(SOCK_TCPC, Sn_IR_CON);                
      }

      len = getSn_RX_RSR(SOCK_TCPC);                  
      if(len > 0)
      {
        recv(SOCK_TCPC,tcp_client_buff,len);           
        tcp_client_buff[len] = 0x00;                   
        printf("%s\r\n",tcp_client_buff);
        send(SOCK_TCPC,tcp_client_buff,len);          
      }		  
      break;

    case SOCK_CLOSE_WAIT:                              
      disconnect(SOCK_TCPC);                           
      close(SOCK_TCPC);                                 
      break;
  }
}
/*"id":"123","version":"1.0","sys":{"ack":0},"params":{"temperature":00,"Humidity":00,"soilHumidity":00,"AlarmSwitch":0},"method":"thing.event.property.post"*/
void uart_insert(char * str1,char * temp,char * str2)
{	
	uint16_t i,j,k = 0;
	uint16_t str1_len,temp_len,str2_len;
	char arr[200] = {0};

	str1_len = strlen(str1);
	temp_len = strlen(temp);
	str2_len = strlen(str2);

	for(i = 0; i < str1_len;i++)                                        
	{
		if(str1[i] == temp[0])
		{
			if((memcmp(&str1[i],temp,temp_len)) == 0)
			{
				for(j = 0;(str1[i + temp_len + 2 + j] != ',') && (str1[i + temp_len + 2 + j] != '}');j++);              
				if(j == str2_len)
				{
					for(k = 0; k < j;k++)                                        
					{
						str1[i + temp_len + 2 + k] = str2[k];
					}
					return;
				}
				for(k = 0; k < (str1_len - i - temp_len - 2 - j);k++)         
					arr[k] = str1[i + temp_len + 2 + j + k];		
				for(k = 0;k < str2_len;k++)                                    
					str1[i + temp_len + 2 + k] = str2[k];
				for(j = 0;j < (str1_len - i - temp_len - 2 - k);j++)          
					str1[i + temp_len + 2 + k] = 0;
				strcat(str1,arr);                                             
				return;
			}
		}  		
	}
}

void aly_sensor(void)
{
	uint8_t i;
	
	char    temp[20] = {0}; 
	
	DHT11_Read_Data(&temperature,&humidity); 
	yl69_humidity = Yl69_Read();             
	g5516_LightLux=g5516_Read();
	
	uBinaryToAscii(temperature, temp);              
	uart_insert(aly_sensor_arr,D_temperature,temp);
	
	for(i = 0; i < 20;i++)
		temp[i] = 0;
	
  uBinaryToAscii(humidity, temp);                  
	uart_insert(aly_sensor_arr,D_Humidity,temp);
	
	for(i = 0; i < 20;i++)
		temp[i] = 0;
	
	uBinaryToAscii(yl69_humidity, temp);            
	uart_insert(aly_sensor_arr,D_soilHumidity,temp);
	
		for(i = 0; i < 20;i++)
		temp[i] = 0;
	
	uBinaryToAscii(g5516_LightLux, temp);           
	uart_insert(aly_sensor_arr,D_LightLux,temp);
	
	for(i = 0; i < 20;i++)
		temp[i] = 0;
	
	temp[0] = Fan_State() + '0';
	uart_insert(aly_sensor_arr,D_AlarmSwitch,temp);
}	


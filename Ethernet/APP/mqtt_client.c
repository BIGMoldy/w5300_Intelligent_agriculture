#include "stm32f4xx_hal.h"

#include "mqtt_client.h"
#include "mqttkit.h"
#include "socket.h"
#include "bsp_flash.h"
#include "w5300_conf.h"

#include "tcp_client.h"

#include <string.h>
#include <stdio.h>

#define PROID		      "MQTT1&hxrzyBEzRJz"                                                               // username

#define AUTH_INFO     "b56f7befc24925def97deeff54b4cd1cb0cb6c049528fe38fb9b4e37664b2c72"                // passwd

#define DEVID         "hxrzyBEzRJz.MQTT1|securemode=2,signmethod=hmacsha256,timestamp=1691646327182|"   // clientId

//#define PROID		      "W5300_ICA&a1iMg8UonSl"                                                               // username

//#define AUTH_INFO     "139c9373aaef81042fcd7d412f8c83a194c4a5b0cd90780c2f6e96ddcbe935fa"                // passwd

//#define DEVID         "a1iMg8UonSl.W5300_ICA|securemode=2,signmethod=hmacsha256,timestamp=1691461351506|"   // clientId

unsigned char w5500_buf[128];
unsigned char publish_buf[64];
int flag=0;



void BD_TG_DevLink(void)
{

    int len;
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};				
    unsigned char *dataPtr;

    printf( "\r\nMQTT_DevLink\r\nPROID：%s,\r\nAUIF：%s,\r\nDEVID：%s\r\n",PROID, AUTH_INFO, DEVID);

    if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
        do
        {
					send(SOCK_TCPC,mqttPacket._data,mqttPacket._len);
					HAL_Delay(500);
					len = getSn_RX_RSR(SOCK_TCPC);
        } while(len <= 0);

        recv(SOCK_TCPC,w5500_buf,len);

        len = getSn_RX_RSR(SOCK_TCPC);

        dataPtr = w5500_buf;
        if(dataPtr != NULL)
        {
            if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
            {
                switch(MQTT_UnPacketConnectAck(dataPtr))
                {
                case 0:
                    mqtt_connect = 1;
										printf("Tips:	连接成功\r\n");
										
                    break;
                case 1:
                    printf("WARN:	连接失败：协议错误\r\n");
                    break;
                case 2:
                    printf("WARN:	连接失败：非法的clientid\r\n");
                    break;
                case 3:
                    printf("WARN:	连接失败：服务器失败\r\n");
                    break;
                case 4:
                    printf("WARN:	连接失败：用户名或密码错误\r\n");
                    break;
                case 5:
                    printf("WARN:	连接失败：非法链接(比如token非法)\r\n");
                    break;
                default:
                    printf("ERR:	连接失败：未知错误\r\n");
                    break;
                }
            }
        }

        MQTT_DeleteBuffer(&mqttPacket);								
    }
    else
        printf( "WARN:	MQTT_PacketConnect Failed\r\n");
}


void MQTT_UnSubscribe(const char *topics[], uint8_t topics_cnt)
{

    int len;
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};				

    if(MQTT_PacketUnSubscribe(10, (const int8_t**)topics,topics_cnt, &mqttPacket) == 0)
    {
        send(SOCK_TCPC,mqttPacket._data,mqttPacket._len);
        HAL_Delay(100);  
        len=getSn_RX_RSR(SOCK_TCPC);
        recv(SOCK_TCPC,w5500_buf,len);
        if ((w5500_buf[0] >> 4) == MQTT_PKT_UNSUBACK)
            printf("成功取消订阅\r\n");
    }
}

void BD_TG_Subscribe(const char *topics[], unsigned char topic_cnt)
{

    unsigned char i = 0;
    uint16_t  len;
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};						

    for(; i < topic_cnt; i++)
        printf( "Subscribe Topic: %s\r\n", topics[i]);

    if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, (const int8_t**)topics, topic_cnt, &mqttPacket) == 0)
    {
        send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);				

        MQTT_DeleteBuffer(&mqttPacket);										
    }

    HAL_Delay(100); 
    len=getSn_RX_RSR(SOCK_TCPC);
    recv(SOCK_TCPC,w5500_buf,len);
    switch(w5500_buf[4])
    {
    case 0x00:
       
        break;
    case 0x01:
       
        break;
    case 0x02:
       
        break;
    case 0x80:
       
        break;
    }


}


void BD_TG_Publish(const char *topic, const char *msg)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							

    printf("Publish Topic: %s, Msg: %s\r\n", topic, msg);

    if(MQTT_PacketPublish(MQTT_PUBLISH_ID, (const int8_t*)topic, (const int8_t*)msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
    {
        send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);					

        MQTT_DeleteBuffer(&mqttPacket);										
    }

}


void BD_TG_RevPro(unsigned char *cmd)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							

    char *req_payload = NULL;
    char *cmdid_topic = NULL;
    unsigned char type = 0;
    unsigned char qos = 0;
		char *recv1,*recv2,*recv3;
    static unsigned short pkt_id = 0;
    short result = 0;
    char *dataPtr = NULL;
    char numBuf[10];
    int num = 0;
		uint8_t temp,sh,h;

    type = MQTT_UnPacketRecv(cmd);
    switch(type)
    {
    case MQTT_PKT_CMD:															

        result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload);				
        if(result == 0)
        {
            printf("cmdid: %s, req: %s\r\n", cmdid_topic, req_payload);

            if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	
            {
                printf( "Tips:	Send CmdResp\r\n");

                send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);			
                MQTT_DeleteBuffer(&mqttPacket);									
            }
        }

        break;

    case MQTT_PKT_PUBLISH:														

        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &req_payload, &qos, &pkt_id);
        if(result == 0)
        {
            printf("topic: %s\r\npayload: %s\r\n", cmdid_topic, req_payload);
 
					if(strstr(cmdid_topic,"set")!=NULL)
					{
						  recv1=strstr(req_payload,"soilHumidity");
						
							if(recv1!=NULL)
							{
							  recv1=recv1+12+2;
								sh=atoi(recv1);
								printf("soilHumidity=%d\r\n",sh);
								STMFLASH_Write(ADDR_FLASH_SECTOR_7,(uint32_t*)&sh,2);
							}
							
							
							recv2=strstr(req_payload,"temperature");
							if(recv2!=NULL)
							{
							  recv2=recv2+11+2;
								temp=atoi(recv2);
								printf("temperature=%d\r\n",temp);
								STMFLASH_Write(ADDR_FLASH_SECTOR_9,(uint32_t*)&temp,2);
							}			

							
							recv3=strstr(req_payload,"Humidity");
							if(recv3!=NULL)
							{
							  recv3=recv3+8+2;
								h=atoi(recv3);
								printf("Humidity=%d\r\n",h);
								STMFLASH_Write(ADDR_FLASH_SECTOR_8,(uint32_t*)&h,2);
							}												
							flag=1;
					}
						
            memcpy(publish_buf,req_payload,strlen(req_payload));
            switch(qos)
            {
            case 1:												

                if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
                {
                    printf( "Tips:	Send PublishAck\r\n");
                    send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
                    MQTT_DeleteBuffer(&mqttPacket);
                }

                break;

            case 2:													
               
                if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
                {
                    printf("Tips:	Send PublishRec\r\n");
                    send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
                    MQTT_DeleteBuffer(&mqttPacket);
                }
								
                break;

            default:
                break;
            }
        }

        break;

    case MQTT_PKT_PUBACK:														
        if(MQTT_UnPacketPublishAck(cmd) == 0)
            printf("Tips:	MQTT Publish Send OK\r\n");

        break;

    case MQTT_PKT_PUBREC:														

        if(MQTT_UnPacketPublishRec(cmd) == 0)
        {
            printf("Tips:	Rev PublishRec\r\n");
            if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
            {
                printf ("Tips:	Send PublishRel\r\n");
                send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
                MQTT_DeleteBuffer(&mqttPacket);
            }
        }

        break;

    case MQTT_PKT_PUBREL:													

        if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
        {
            printf("Tips:	Rev PublishRel\r\n");
            if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
            {
                printf( "Tips:	Send PublishComp\r\n");
                send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
                MQTT_DeleteBuffer(&mqttPacket);
            }
        }

        break;

    case MQTT_PKT_PUBCOMP:													

        if(MQTT_UnPacketPublishComp(cmd) == 0)
        {
            printf("Tips:	Rev PublishComp\r\n");
        }

        break;

    case MQTT_PKT_SUBACK:												

        if(MQTT_UnPacketSubscribe(cmd) == 0)
            printf("Tips:	MQTT Subscribe OK\r\n");
        else
            printf("Tips:	MQTT Subscribe Err\r\n");

        break;

    case MQTT_PKT_UNSUBACK:													

        if(MQTT_UnPacketUnSubscribe(cmd) == 0)
            printf( "Tips:	MQTT UnSubscribe OK\r\n");
        else
            printf("Tips:	MQTT UnSubscribe Err\r\n");

        break;

    default:
        result = -1;
        break;
    }

    //ESP8266_Clear();									

    if(result == -1)
        return;

    dataPtr = strchr(req_payload, '}');					

    if(dataPtr != NULL && result != -1)					
    {
        dataPtr++;

        while(*dataPtr >= '0' && *dataPtr <= '9')		
        {
            numBuf[num++] = *dataPtr++;
        }

        num = atoi((const char *)numBuf);				

    }

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
}

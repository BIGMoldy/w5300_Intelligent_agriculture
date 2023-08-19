
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE			2048
#endif

extern const char *device_set_topics[150];
extern const char *device_set_topics1[150];

extern int MQTT_STATE;
extern uint8_t MQTT_ping_time;
extern uint8_t mqtt_connect;
extern uint8_t temperature;    
extern uint8_t humidity;        
extern uint8_t yl69_humidity;   
extern uint16_t g5516_LightLux;
/* TCP client Loopback test example */
void do_mqtttcp_client(void);
void do_tcp_client(void);
void uart_insert(char * str1,char * temp,char * str2);
void aly_sensor(void);
void jansson_pack_test(void);
#endif

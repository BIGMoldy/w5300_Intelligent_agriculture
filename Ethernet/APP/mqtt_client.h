#ifndef _MQTT_CLIENT_H_
#define _MQTT_CLIENT_H_

#include <stdint.h>
extern int flag;
void  BD_TG_DevLink(void);

void BD_TG_Subscribe(const char *topics[], unsigned char topic_cnt);

void BD_TG_Publish(const char *topic, const char *msg);

void BD_TG_RevPro(unsigned char *cmd);
void MQTT_UnSubscribe(const char *topics[], uint8_t topic_cnt);

extern unsigned char publish_buf[64];
#endif

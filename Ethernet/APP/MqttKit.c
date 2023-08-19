#include "MqttKit.h"

#include <string.h>

#define CMD_TOPIC_PREFIX		"$creq"


void MQTT_NewBuffer(MQTT_PACKET_STRUCTURE *mqttPacket, uint32_t size)
{
	
	uint32_t i = 0;

	if(mqttPacket->_data == NULL)
	{
		mqttPacket->_memFlag = MEM_FLAG_ALLOC;
		
		mqttPacket->_data = (uint8_t *)MQTT_MallocBuffer(size);
		if(mqttPacket->_data != NULL)
		{
			mqttPacket->_len = 0;
			
			mqttPacket->_size = size;
			
			for(; i < mqttPacket->_size; i++)
				mqttPacket->_data[i] = 0;
		}
	}
	else
	{
		mqttPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < mqttPacket->_size; i++)
			mqttPacket->_data[i] = 0;
		
		mqttPacket->_len = 0;
		
		if(mqttPacket->_size < size)
			mqttPacket->_data = NULL;
	}

}


void MQTT_DeleteBuffer(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	if(mqttPacket->_memFlag == MEM_FLAG_ALLOC)
		MQTT_FreeBuffer(mqttPacket->_data);
	
	mqttPacket->_data = NULL;
	mqttPacket->_len = 0;
	mqttPacket->_size = 0;
	mqttPacket->_memFlag = MEM_FLAG_NULL;

}

int32_t MQTT_DumpLength(size_t len, uint8_t *buf)
{
	
	int32_t i = 0;
	
	for(i = 1; i <= 4; ++i)
	{
		*buf = len % 128;
		len >>= 7;
		if(len > 0)
		{
			*buf |= 128;
			++buf;
		}
		else
		{
			return i;
		}
	}

	return -1;
}

int32_t MQTT_ReadLength(const uint8_t *stream, int32_t size, uint32_t *len)
{
	
	int32_t i;
	const uint8_t *in = stream;
	uint32_t multiplier = 1;

	*len = 0;
	for(i = 0; i < size; ++i)
	{
		*len += (in[i] & 0x7f) * multiplier;

		if(!(in[i] & 0x80))
		{
			return i + 1;
		}

		multiplier <<= 7;
		if(multiplier >= 2097152)		//128 * *128 * *128
		{
			return -2;					// error, out of range
		}
	}

	return -1;							// not complete

}


uint8_t MQTT_UnPacketRecv(uint8_t *dataPtr)
{
	
	uint8_t status = 255;
	uint8_t type = dataPtr[0] >> 4;				
	
	if(type < 1 || type > 14)
		return status;
	
	if(type == MQTT_PKT_PUBLISH)
	{
		uint8_t *msgPtr;
		uint32_t remain_len = 0;
		
		msgPtr = dataPtr + MQTT_ReadLength(dataPtr + 1, 4, &remain_len) + 1;
		
		if(remain_len < 2 || dataPtr[0] & 0x01)					//retain
			return 255;
		
		if(remain_len < ((uint16_t)msgPtr[0] << 8 | msgPtr[1]) + 2)
			return 255;
		
		if(strstr((int8_t *)msgPtr + 2, CMD_TOPIC_PREFIX) != NULL)	
			status = MQTT_PKT_CMD;
		else
			status = MQTT_PKT_PUBLISH;
	}
	else
		status = type;
	
	return status;

}


uint8_t MQTT_PacketConnect(const int8_t *user, const int8_t *password, const int8_t *devid,
						uint16_t cTime, uint8_t clean_session, uint8_t qos,
						const int8_t *will_topic, const int8_t *will_msg, int32_t will_retain,
						MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint8_t flags = 0;
	uint8_t will_topic_len = 0;
	uint16_t total_len = 15;
	int16_t len = 0, devid_len = strlen(devid);
	
	if(!devid)
		return 1;
	
	total_len += devid_len + 2;
	

	if(clean_session)
	{
		flags |= MQTT_CONNECT_CLEAN_SESSION;
	}
	
	if(will_topic)
	{
		flags |= MQTT_CONNECT_WILL_FLAG;
		will_topic_len = strlen(will_topic);
		total_len += 4 + will_topic_len + strlen(will_msg);
	}
	
	
	switch((unsigned char)qos)
	{
		case MQTT_QOS_LEVEL0:
			flags |= MQTT_CONNECT_WILL_QOS0;							
		break;
		
		case MQTT_QOS_LEVEL1:
			flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS1);	
		break;
		
		case MQTT_QOS_LEVEL2:
			flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS2);	
		break;
		
		default:
		return 2;
	}
	
	
	if(will_retain)
	{
		flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_RETAIN);
	}
	
	
	if(!user || !password)
	{
		return 3;
	}
	flags |= MQTT_CONNECT_USER_NAME | MQTT_CONNECT_PASSORD;
	
	total_len += strlen(user) + strlen(password) + 4;
	
	
	MQTT_NewBuffer(mqttPacket, total_len);
	if(mqttPacket->_data == NULL)
		return 4;
	
	memset(mqttPacket->_data, 0, total_len);
	
	
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_CONNECT << 4;
	
	len = MQTT_DumpLength(total_len - 5, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 5;
	}
	else
		mqttPacket->_len += len;
	

	mqttPacket->_data[mqttPacket->_len++] = 0;
	mqttPacket->_data[mqttPacket->_len++] = 4;
	mqttPacket->_data[mqttPacket->_len++] = 'M';
	mqttPacket->_data[mqttPacket->_len++] = 'Q';
	mqttPacket->_data[mqttPacket->_len++] = 'T';
	mqttPacket->_data[mqttPacket->_len++] = 'T';
	
		mqttPacket->_data[mqttPacket->_len++] = 4;
   mqttPacket->_data[mqttPacket->_len++] = flags;
	
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(cTime);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(cTime);
	 
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(devid_len);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(devid_len);
	
	strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, devid, devid_len);
	mqttPacket->_len += devid_len;
	if(flags & MQTT_CONNECT_WILL_FLAG)
	{
		unsigned short mLen = 0;
		
		if(!will_msg)
			will_msg = "";
		
		mLen = strlen(will_topic);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(mLen);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(mLen);
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, will_topic, mLen);
		mqttPacket->_len += mLen;
		
		mLen = strlen(will_msg);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(mLen);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(mLen);
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, will_msg, mLen);
		mqttPacket->_len += mLen;
	}
	
		if(flags & MQTT_CONNECT_USER_NAME)
	{
		unsigned short user_len = strlen(user);
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(user_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(user_len);
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, user, user_len);
		mqttPacket->_len += user_len;
	}

	if(flags & MQTT_CONNECT_PASSORD)
	{
		unsigned short psw_len = strlen(password);
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(psw_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(psw_len);
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, password, psw_len);
		mqttPacket->_len += psw_len;
	}

	return 0;

}


uint8_t MQTT_PacketDisConnect(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 2);
	if(mqttPacket->_data == NULL)
		return 1;
	

	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_DISCONNECT << 4;
	

	mqttPacket->_data[mqttPacket->_len++] = 0;
	
	return 1;

}


uint8_t MQTT_UnPacketConnectAck(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;
	
	if(rev_data[2] == 0 || rev_data[2] == 1)
		return rev_data[3];
	else
		return 255;

}


uint8_t MQTT_PacketSaveData(const int8_t *devid, int16_t send_len, int8_t *type_bin_head, uint8_t type, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, "$dp", NULL, send_len + 3, MQTT_QOS_LEVEL1, 0, 1, mqttPacket) == 0)
	{
		mqttPacket->_data[mqttPacket->_len++] = type;					//ÀàÐÍ
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(send_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(send_len);
	}
	else
		return 1;
	
	return 0;

}


uint8_t MQTT_UnPacketCmd(uint8_t *rev_data, int8_t **cmdid, int8_t **req)
{

	int8_t *dataPtr = strchr((int8_t *)rev_data + 3, '/');	
	
	uint32_t remain_len = 0;
	uint16_t req_len = 0;
	
	if(dataPtr == NULL)									
		return 1;
	dataPtr++;											
	
	MQTT_ReadLength(rev_data + 1, 4, &remain_len);		
	
	*cmdid = (int8_t *)MQTT_MallocBuffer(37);				
	if(*cmdid == NULL)
		return 2;
	
	memset(*cmdid, 0, 37);								
	memcpy(*cmdid, (const int8_t *)dataPtr, 36);			
	dataPtr += 36;
	
	req_len = remain_len - 44;						
	*req = (int8_t *)MQTT_MallocBuffer(req_len + 1);		
	if(*req == NULL)
	{
		MQTT_FreeBuffer(*cmdid);
		return 3;
	}
	
	memset(*req, 0, req_len + 1);					
	memcpy(*req, (const int8_t *)dataPtr, req_len);	
	
	return 0;

}


uint8_t MQTT_PacketCmdResp(const int8_t *cmdid, const int8_t *req, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint16_t cmdid_len = strlen(cmdid);
	uint16_t req_len = strlen(req);
	_Bool status = 0;
	
	int8_t *payload = MQTT_MallocBuffer(cmdid_len + 6);
	if(payload == NULL)
		return 1;
	
	memset(payload, 0, cmdid_len + 6);
	memcpy(payload, "$crsp/", 6);
	strncat(payload, cmdid, cmdid_len);

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, payload, req, strlen(req), MQTT_QOS_LEVEL0, 0, 1, mqttPacket) == 0)
		status = 0;
	else
		status = 1;
	
	MQTT_FreeBuffer(payload);
	
	return status;

}


uint8_t MQTT_PacketSubscribe(uint16_t pkt_id, enum MqttQosLevel qos, const int8_t *topics[], uint8_t topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint32_t topic_len = 0, remain_len = 0;
	int16_t len = 0;
	uint8_t i = 0;
	
	if(pkt_id == 0)
		return 1;
	
	
	for(; i < topics_cnt; i++)
	{
		if(topics[i] == NULL)
			return 2;
		
		topic_len += strlen(topics[i]);
	}
	
	//2 bytes packet id + topic filter(2 bytes topic + topic length + 1 byte reserve)------
	remain_len = 2 + 3 * topics_cnt + topic_len;
	
	
	MQTT_NewBuffer(mqttPacket, remain_len + 5);
	if(mqttPacket->_data == NULL)
		return 3;
	
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_SUBSCRIBE << 4 | 0x02;
	
	len = MQTT_DumpLength(remain_len, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 4;
	}
	else
		mqttPacket->_len += len;
	
	
	//payload----------------------pkt_id---------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	
	//payload----------------------topic_name-----------------------------------------------
	for(i = 0; i < topics_cnt; i++)
	{
		topic_len = strlen(topics[i]);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
		
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, topics[i], topic_len);
		mqttPacket->_len += topic_len;
		
		mqttPacket->_data[mqttPacket->_len++] = qos & 0xFF;
	}

	return 0;

}


uint8_t MQTT_UnPacketSubscribe(uint8_t *rev_data)
{
	
	uint8_t result = 255;

	if(rev_data[2] == MOSQ_MSB(MQTT_SUBSCRIBE_ID) && rev_data[3] == MOSQ_LSB(MQTT_SUBSCRIBE_ID))
	{
		switch(rev_data[4])
		{
			case 0x00:
			case 0x01:
			case 0x02:
				//MQTT Subscribe OK
				result = 0;
			break;
			
			case 0x80:
				//MQTT Subscribe Failed
				result = 1;
			break;
			
			default:
				//MQTT Subscribe UnKnown Err
				result = 2;
			break;
		}
	}
	
	return result;

}


uint8_t MQTT_PacketUnSubscribe(uint16_t pkt_id, const int8_t *topics[], uint8_t topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint32_t topic_len = 0, remain_len = 0;
	int16_t len = 0;
	uint8_t i = 0;
	
	if(pkt_id == 0)
		return 1;
	
	
	for(; i < topics_cnt; i++)
	{
		if(topics[i] == NULL)
			return 2;
		
		topic_len += strlen(topics[i]);
	}
	
	//2 bytes packet id, 2 bytes topic length + topic + 1 byte reserve---------------------
	remain_len = 2 + (topics_cnt << 1) + topic_len;
	
	
	MQTT_NewBuffer(mqttPacket, remain_len + 5);
	if(mqttPacket->_data == NULL)
		return 3;
	
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_UNSUBSCRIBE << 4 | 0x02;
	

	len = MQTT_DumpLength(remain_len, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 4;
	}
	else
		mqttPacket->_len += len;
	
/*************************************payload***********************************************/
	
	//payload----------------------pkt_id---------------------------------------------------
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	
	//payload----------------------topic_name-----------------------------------------------
	for(i = 0; i < topics_cnt; i++)
	{
		topic_len = strlen(topics[i]);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
		
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, topics[i], topic_len);
		mqttPacket->_len += topic_len;
	}

	return 0;

}


uint8_t MQTT_UnPacketUnSubscribe(uint8_t *rev_data)
{
	
	uint8_t result = 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_UNSUBSCRIBE_ID) && rev_data[3] == MOSQ_LSB(MQTT_UNSUBSCRIBE_ID))
	{
		result = 0;
	}
	
	return result;

}


uint8_t MQTT_PacketPublish(uint16_t pkt_id, const int8_t *topic,
						const int8_t *payload, uint32_t payload_len,
						enum MqttQosLevel qos, int32_t retain, int32_t own,
						MQTT_PACKET_STRUCTURE *mqttPacket)
{

	uint32_t total_len = 0, topic_len = 0;
	int32_t len = 0;
	uint8_t flags = 0;
	
	
	if(pkt_id == 0)
		return 1;
	
	
	for(topic_len = 0; topic[topic_len] != '\0'; ++topic_len)
	{
		if((topic[topic_len] == '#') || (topic[topic_len] == '+'))
			return 2;
	}
	
	
	flags |= MQTT_PKT_PUBLISH << 4;
	
	
	if(retain)
		flags |= 0x01;
	
	
	total_len = topic_len + payload_len + 2;
	
	
	switch(qos)
	{
		case MQTT_QOS_LEVEL0:
			flags |= MQTT_CONNECT_WILL_QOS0;	
		break;
		
		case MQTT_QOS_LEVEL1:
			flags |= 0x02;						        
			total_len += 2;
		break;
		
		case MQTT_QOS_LEVEL2:
			flags |= 0x04;						       
			total_len += 2;
		break;
		
		default:
		return 3;
	}
	
	
	MQTT_NewBuffer(mqttPacket, total_len + 3);
	if(mqttPacket->_data == NULL)
		return 4;
		
	memset(mqttPacket->_data, 0, total_len + 3);
	
	mqttPacket->_data[mqttPacket->_len++] = flags;
	
	len = MQTT_DumpLength(total_len, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 5;
	}
	else
		mqttPacket->_len += len;
	
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
	
	strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, topic, topic_len);
	mqttPacket->_len += topic_len;
	if(qos != MQTT_QOS_LEVEL0)
	{
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	}
	

	if(payload != NULL)
	{
		strncat((int8_t *)mqttPacket->_data + mqttPacket->_len, payload, payload_len);
		mqttPacket->_len += payload_len;
	}
	
	return 0;

}

uint8_t MQTT_UnPacketPublish(uint8_t *rev_data, int8_t **topic, int8_t **payload, uint8_t *qos, uint16_t *pkt_id)
{
	
	const int8_t flags = rev_data[0] & 0x0F;
	uint8_t *msgPtr;
	uint32_t remain_len = 0;

	const int8_t dup = flags & 0x08;

	uint32_t payload_len = 0;
	uint16_t topic_len = 0;

	*qos = (flags & 0x06) >> 1;
	
	msgPtr = rev_data + MQTT_ReadLength(rev_data + 1, 4, &remain_len) + 1;
	
	if(remain_len < 2 || flags & 0x01)							//retain
		return 255;
	
	topic_len = (uint16_t)msgPtr[0] << 8 | msgPtr[1];
	if(remain_len < topic_len + 2)
		return 255;
	
	if(strstr((int8_t *)msgPtr + 2, CMD_TOPIC_PREFIX) != NULL)	
		return MQTT_PKT_CMD;
	
	switch(*qos)
	{
		case MQTT_QOS_LEVEL0:									// qos0 have no packet identifier
			
			if(0 != dup)
				return 255;

			*topic = MQTT_MallocBuffer(topic_len + 1);			
			if(*topic == NULL)
				return 255;
			
			memset(*topic, 0, topic_len + 1);
			memcpy(*topic, (int8_t *)msgPtr + 2, topic_len);	
			
			payload_len = remain_len - 2 - topic_len;			
			*payload = MQTT_MallocBuffer(payload_len + 1);
			if(*payload == NULL)								
			{
				MQTT_FreeBuffer(*topic);						
				return 255;
			}
			
			memset(*payload, 0, payload_len + 1);
			memcpy(*payload, (int8_t *)msgPtr + 2 + topic_len, payload_len);
			
		break;

		case MQTT_QOS_LEVEL1:
		case MQTT_QOS_LEVEL2:
			
			if(topic_len + 2 > remain_len)
				return 255;
			
			*pkt_id = (uint16_t)msgPtr[topic_len + 2] << 8 | msgPtr[topic_len + 3];
			if(pkt_id == 0)
				return 255;
			
			*topic = MQTT_MallocBuffer(topic_len + 1);			
			if(*topic == NULL)
				return 255;
			
			memset(*topic, 0, topic_len + 1);
			memcpy(*topic, (int8_t *)msgPtr + 2, topic_len);		
			
			payload_len = remain_len - 4 - topic_len;
			*payload = MQTT_MallocBuffer(payload_len + 1);		
			if(*payload == NULL)								
			{
				MQTT_FreeBuffer(*topic);					
				return 255;
			}
			
			memset(*payload, 0, payload_len + 1);
			memcpy(*payload, (int8_t *)msgPtr + 4 + topic_len, payload_len);
			
		break;

		default:
			return 255;
	}
	
	if(strchr((int8_t *)topic, '+') || strchr((int8_t *)topic, '#'))
		return 255;

	return 0;

}


uint8_t MQTT_PacketPublishAck(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	

	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREL << 4 | 0x02;
	

	mqttPacket->_data[mqttPacket->_len++] = 2;
	

	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}


uint8_t MQTT_UnPacketPublishAck(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}


uint8_t MQTT_PacketPublishRec(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	

	

	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREC << 4;
	
	
	mqttPacket->_data[mqttPacket->_len++] = 2;

	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}


uint8_t MQTT_UnPacketPublishRec(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}


uint8_t MQTT_PacketPublishRel(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	

	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREL << 4 | 0x02;
	

	mqttPacket->_data[mqttPacket->_len++] = 2;
	

	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}


uint8_t MQTT_UnPacketPublishRel(uint8_t *rev_data, uint16_t pkt_id)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(pkt_id) && rev_data[3] == MOSQ_LSB(pkt_id))
		return 0;
	else
		return 1;

}


uint8_t MQTT_PacketPublishComp(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREL << 4 | 0x02;
	
	
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}


uint8_t MQTT_UnPacketPublishComp(uint8_t *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}


uint8_t MQTT_PacketPing(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 2);
	if(mqttPacket->_data == NULL)
		return 1;
	

	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PINGREQ << 4;
	
	
	mqttPacket->_data[mqttPacket->_len++] = 0;
	
	return 0;

}

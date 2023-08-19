/**
******************************************************************************
* @file         httputil.h                                         
* @version      V1.0                          
* @date         2018-06-18                 
* @brief        httputil.c��ͷ�ļ�        
*
* @company      ��������Ƽ����޹�˾
* @information  WIZnet W5100S�ٷ������̣�ȫ�̼���֧�֣��۸����ƴ�
* @website      www.wisioe.com  
* @forum        www.w5100s.com
* @qqGroup      579842114                                                     
******************************************************************************
*/
#ifndef  __HTTPUTIL_H__
#define  __HTTPUTIL_H__

#include "http_server.h"

void proc_http(uint8_t s, uint8_t * buf);
void do_https(void);
void cgi_ipconfig(st_http_request *http_request);
//void trimp(uint8__t* src, uint8__t* dst, uint16_t len);
uint16_t make_msg_response(uint8_t* buf,int8_t* msg);

void make_cgi_response(uint16_t a,int8_t* b,int8_t* c);
void make_pwd_response(int8_t isRight,uint16_t delay,int8_t* cgi_response_content, int8_t isTimeout);

#endif

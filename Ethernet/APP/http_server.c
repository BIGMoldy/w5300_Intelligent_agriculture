/**
******************************************************************************
* @file         http_server.c                                         
* @version      V1.0                          
* @date         2018-06-18                 
* @brief        heeputil.c/.h webpge.h ���ʹ�ã����HTTP Serever����
*
* @company      ��������Ƽ����޹�˾
* @information  WIZnet W5100S�ٷ������̣�ȫ�̼���֧�֣��۸����ƴ�
* @website      www.wisioe.com   
* @forum        www.w5100s.com
* @qqGroup      579842114                                                     
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "http_server.h"

#include "stdint.h"

extern char  homepage_default[];
char tx_buf[MAX_URI_SIZE];
char rx_buf[MAX_URI_SIZE];
uint8_t pub_buf[1024];
  
/**
*@brief  ת��ת���ַ�Ϊascii charater 
*@param  url����Ҫת����ҳ��ַ
*@return ��
*/
void unescape_http_url(char * url)
{
  int x, y;
  
  for (x = 0, y = 0; url[y]; ++x, ++y) 
  {
    if ((url[x] = url[y]) == '%') 
    {
      url[x] = c2d(url[y+1])*0x10+c2d(url[y+2]);
      y+=2;
    }
  }
  url[x] = '\0';
}

/**
*@brief  ִ��һ���𸴣��� html, gif, jpeg,etc.
*@param  buf- ������
*@param  type- ����������
*@param  len-  �����ݳ���
*@return ��
*/
void make_http_response_head(unsigned char * buf,char type,uint32_t len)
{
  char * head;
  char tmp[10];
  memset(buf,0x00,MAX_URI_SIZE); 
  //  �ļ�����*/
  if   (type == PTYPE_HTML) head = RES_HTMLHEAD_OK;
  else if (type == PTYPE_GIF)    head = RES_GIFHEAD_OK;
  else if (type == PTYPE_TEXT)  head = RES_TEXTHEAD_OK;
  else if (type == PTYPE_JPEG)  head = RES_JPEGHEAD_OK;
  else if (type == PTYPE_FLASH)  head = RES_FLASHHEAD_OK;
  else if (type == PTYPE_MPEG)  head = RES_MPEGHEAD_OK;
  else if (type == PTYPE_PDF)    head = RES_PDFHEAD_OK;

  sprintf(tmp,"%ld", len);  
  strcpy((char*)buf, head);
  strcat((char*)buf, tmp);
  strcat((char*)buf, "\r\n\r\n");
  //printf("%s\r\n", buf);
}

/**
*@brief  Ѱ��һ��MIME�����ļ�
*@param  buf- MIME���ļ�
*@param  type-��������
*@return ��
*/
void find_http_uri_type(uint8_t * type, char * buf) 
{
  //  Decide type according to extention*/
  if   (strstr(buf, ".pl"))         *type = PTYPE_PL;
  else if (strstr(buf, ".html") || strstr(buf,".htm"))  *type = PTYPE_HTML;
  else if (strstr(buf, ".gif"))     *type = PTYPE_GIF;
  else if (strstr(buf, ".text") || strstr(buf,".txt"))  *type = PTYPE_TEXT;
  else if (strstr(buf, ".jpeg") || strstr(buf,".jpg"))  *type = PTYPE_JPEG;
  else if (strstr(buf, ".swf"))    *type = PTYPE_FLASH;
  else if (strstr(buf, ".mpeg") || strstr(buf,".mpg"))  *type = PTYPE_MPEG;
  else if (strstr(buf, ".pdf"))    *type = PTYPE_PDF;
  else if (strstr(buf, ".cgi")  || strstr(buf,".CGI"))  *type = PTYPE_CGI;
  else if (strstr(buf, ".js")   || strstr(buf,".JS"))    *type = PTYPE_TEXT;  
  else if (strstr(buf, ".xml")  || strstr(buf,".XML"))  *type = PTYPE_HTML;
  else               *type = PTYPE_ERR;
}

/**
*@brief  ����ÿһ��http��Ӧ
*@param  request�� ����һ��ָ��
*@return ��
*/
void parse_http_request(st_http_request * request,uint8_t * buf)
{
  char * nexttok;
  nexttok = strtok((char*)buf," ");
  if(!nexttok)
  {
    request->METHOD = METHOD_ERR;
    return;
  }
  if(!strcmp(nexttok, "GET") || !strcmp(nexttok,"get"))
  {
    request->METHOD = METHOD_GET;
    nexttok = strtok(NULL," ");
         
  }
  else if (!strcmp(nexttok, "HEAD") || !strcmp(nexttok,"head"))  
  {
    request->METHOD = METHOD_HEAD;
    nexttok = strtok(NULL," ");
      
  }
  else if (!strcmp(nexttok, "POST") || !strcmp(nexttok,"post"))
  {
    nexttok = strtok(NULL,"\0");//20120316
    //nexttok = strtok(NULL," ");
    request->METHOD = METHOD_POST;
     
  }
  else
  {
    request->METHOD = METHOD_ERR;
  }   
  if(!nexttok)
  {
    request->METHOD = METHOD_ERR;       
    return;
  }
  strcpy(request->URI,nexttok);           
}

/**
*@brief  �õ���Ӧ�����е���һ������
*@param  url ��Ҫת����ҳ��ַ
*@param  param_name�� 
*@return ����һ������
*/
uint8_t* get_http_param_value(char* uri, char* param_name)
{
  uint16_t len;
  uint8_t* pos2;
  uint8_t* name=0; 
  uint8_t *ret=pub_buf;
  uint16_t content_len=0;
  int8_t tmp_buf[10]={0x00,};
  if(!uri || !param_name) return 0;
  // **************/
  mid(uri,"Content-Length: ","\r\n",tmp_buf);
  content_len=atoi16(tmp_buf,10);
  //printf("content len=%d\r\n",content_len);
  uri = (int8_t*)strstr(uri,"\r\n\r\n");
  uri+=4;
  //printf("uri=%s\r\n",uri);
  uri[content_len]=0;
  // **************/   
  name= (uint8_t*)strstr(uri,param_name);
  if(name)
  {
    name += strlen(param_name) + 1; 
    pos2=(uint8_t*)strstr((char*)name,"&");
    if(!pos2) 
    {
      pos2=name+strlen((char*)name);
    }
    len=0;
    len = pos2-name;
 
    if(len)
    {
      ret[len]=0;
      strncpy((char*)ret,(char*)name,len);
      unescape_http_url((char *)ret);
      replacetochar((char *)ret,'+',' ');
    }
    else
      ret[0]=0;
  }
  else
    return 0;
  return ret;    
}

/**
*@brief  �ַ�תת��Ϊ8λ���ͺ���
*@param  str:Ҫת���ַ����� base:
*@return num:����ת�����������
*/
uint16_t atoi16(char* str,uint16_t base)
{
  unsigned int num = 0;
  while (*str !=0)
  num = num * base + c2d(*str++);
  return num;
}

/**
*@brief  ��ʮ������ת��Ϊ�ַ���
*@param  c:Ҫת��ʮ��������
*@return ����һ���ַ�������
*/
char c2d(uint8_t c  )
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return 10 + c -'a';
  if (c >= 'A' && c <= 'F')
    return 10 + c -'A';
  return (char)c;
}

/**
*@brief  �ַ�������
*@param  srcĿ���ַ��� s1 s2�����ַ���
*@return ��
*/
void mid(int8_t* src, int8_t* s1, int8_t* s2, int8_t* sub)
{
  int8_t* sub1;
  int8_t* sub2;
  uint16_t n;

  sub1=strstr(src,s1);
  sub1+=strlen(s1);
  sub2=strstr(sub1,s2);
  n=sub2-sub1;
  strncpy(sub,sub1,n);
  sub[n]=0;
}

/**
*@brief  ���µ��ַ�ȥ�滻�ַ�����������ַ�
*@param  str:�滻���ַ�����oldchar:������ַ���newchar���µ��ַ�  
*@return ��
*/
void replacetochar(char * str,  char oldchar,char newchar  )
{
  int x;
  for (x = 0; str[x]; x++) 
    if (str[x] == oldchar)
      str[x] = newchar;  
}

/**
*@brief  ip�����ַת��
*@param  adr����ַ ip��ip
*@return ��
*/
void inet_addr_(unsigned char* addr,unsigned char *ip)
{
  int i;
  char taddr[30];
  char * nexttok;
  char num;
  strcpy(taddr,(char *)addr);
  
  nexttok = taddr;
  for(i = 0; i < 4 ; i++)
  {
    nexttok = strtok(nexttok,".");
    if(nexttok[0] == '0' && nexttok[1] == 'x')
      num = atoi16(nexttok+2,0x10);
    else
      num = atoi16(nexttok,10);
  
  ip[i] = num;
  nexttok = NULL;
  }
} 

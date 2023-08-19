/**
******************************************************************************
* @file         http_server.c                                         
* @version      V1.0                          
* @date         2018-06-18                 
* @brief        heeputil.c/.h webpge.h 配合使用，完成HTTP Serever功能
*
* @company      深圳炜世科技有限公司
* @information  WIZnet W5100S官方代理商，全程技术支持，价格优势大！
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
*@brief  转化转义字符为ascii charater 
*@param  url：需要转化网页地址
*@return 无
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
*@brief  执行一个答复，如 html, gif, jpeg,etc.
*@param  buf- 答复数据
*@param  type- 答复数据类型
*@param  len-  答复数据长度
*@return 无
*/
void make_http_response_head(unsigned char * buf,char type,uint32_t len)
{
  char * head;
  char tmp[10];
  memset(buf,0x00,MAX_URI_SIZE); 
  //  文件类型*/
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
*@brief  寻找一个MIME类型文件
*@param  buf- MIME型文件
*@param  type-数据类型
*@return 无
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
*@brief  解析每一个http响应
*@param  request： 定义一个指针
*@return 无
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
*@brief  得到响应过程中的下一个参数
*@param  url 需要转化网页地址
*@param  param_name： 
*@return 返回一个数据
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
*@brief  字符转转化为8位整型函数
*@param  str:要转化字符串， base:
*@return num:返回转化后的整型数
*/
uint16_t atoi16(char* str,uint16_t base)
{
  unsigned int num = 0;
  while (*str !=0)
  num = num * base + c2d(*str++);
  return num;
}

/**
*@brief  把十进制数转化为字符型
*@param  c:要转化十进制数据
*@return 返回一个字符型数据
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
*@brief  字符串处理
*@param  src目标字符串 s1 s2操作字符串
*@return 无
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
*@brief  用新的字符去替换字符串中特殊的字符
*@param  str:替换后字符串，oldchar:特殊的字符，newchar：新的字符  
*@return 无
*/
void replacetochar(char * str,  char oldchar,char newchar  )
{
  int x;
  for (x = 0; str[x]; x++) 
    if (str[x] == oldchar)
      str[x] = newchar;  
}

/**
*@brief  ip网络地址转换
*@param  adr：地址 ip：ip
*@return 无
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

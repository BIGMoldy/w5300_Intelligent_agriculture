#include <stdio.h>
#include "tcp_server.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "w5300_conf.h"

uint8_t tcp_server_buff[DATA_BUF_SIZE];  

void do_tcp_server(void)
{  
  uint16_t len = 0;  
  switch(getSn_SR(SOCK_TCPS))                            
  {
    case SOCK_CLOSED:                                     
      socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);  
      break;     
    
    case SOCK_INIT:                                       
      listen(SOCK_TCPS);                                  
      break;
    
    case SOCK_ESTABLISHED:                                
    
      if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
      {
        setSn_IR(SOCK_TCPS, Sn_IR_CON);                   
      }
      len = getSn_RX_RSR(SOCK_TCPS);                     
      if(len > 0)
      {
        len = recv(SOCK_TCPS,tcp_server_buff,len);              
				tcp_server_buff[len] = 0x00;                     
        printf("%s\r\n",tcp_server_buff);
        send(SOCK_TCPS,tcp_server_buff,len);             
      }
      break;
    
    case SOCK_CLOSE_WAIT:                                
      disconnect(SOCK_TCPS);                             
      close(SOCK_TCPS);                                  
      break;
  }
}

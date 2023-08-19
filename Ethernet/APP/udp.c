
#include <stdio.h>
#include "udp.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "w5300_conf.h"
#include "tcp_client.h"
#include "mqttkit.h"

#define udp_port 9000

uint8_t udp_buff[DATA_BUF_SIZE];  
void do_udp(void)
{
  uint16_t len = 0;
  switch(getSn_SR(SOCK_UDPS))                                   
  {
    case SOCK_CLOSED:                                            
      socket(SOCK_UDPS,Sn_MR_UDP,udp_port,0);                 
      break;
    case SOCK_UDP:                                                
      if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
      {
        setSn_IR(SOCK_UDPS, Sn_IR_RECV);                          
      }
      len = getSn_RX_RSR(SOCK_UDPS);                              
      if(len > 0)
      {
				recvfrom(SOCK_UDPS,udp_buff, len, remote_ip,&remote_port);
				printf("len: %d\r\n", len);
				udp_buff[len - 8]=0x00;                                  
				sendto(SOCK_UDPS, udp_buff, len - 8, remote_ip, remote_port);

      }                                                                
      break;
  }
}


#ifndef __MYIIC_H
#define __MYIIC_H
#include "stm32f4xx_hal.h"
#include "sys.h" 
 

#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 
	 
#define IIC_SCL    PBout(8) 
#define IIC_SDA    PBout(9)  
#define READ_SDA   PBin(9)   


void IIC_Init(void);             	 
void IIC_Start(void);			
void IIC_Stop(void);	  			
void IIC_Send_Byte(uint8_t txd);			
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void); 				
void IIC_Ack(void);					
void IIC_NAck(void);				
void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif

















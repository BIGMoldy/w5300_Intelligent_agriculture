
#ifndef _BSP_FLASH_H  
#define _BSP_FLASH_H
  
#include "stm32f4xx_hal.h"
  
void WriteFlashData(uint32_t WriteAddress, uint8_t data[], uint32_t num);

void ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum);

#define BOARD_NUM_ADDR 0x0800C000

#define STM32_FLASH_BASE 0x08000000 
#define FLASH_WAITETIME 50000      

#define ADDR_FLASH_SECTOR_0  ((uint32_t)0x08000000)  
#define ADDR_FLASH_SECTOR_1  ((uint32_t)0x08004000) 
#define ADDR_FLASH_SECTOR_2  ((uint32_t)0x08008000)  
#define ADDR_FLASH_SECTOR_3  ((uint32_t)0x0800C000) 
#define ADDR_FLASH_SECTOR_4  ((uint32_t)0x08010000) 
#define ADDR_FLASH_SECTOR_5  ((uint32_t)0x08020000)  
#define ADDR_FLASH_SECTOR_6  ((uint32_t)0x08040000) 
#define ADDR_FLASH_SECTOR_7  ((uint32_t)0x08060000)  
#define ADDR_FLASH_SECTOR_8  ((uint32_t)0x08080000) 
#define ADDR_FLASH_SECTOR_9  ((uint32_t)0x080A0000)  
#define ADDR_FLASH_SECTOR_10 ((uint32_t)0x080C0000)  
#define ADDR_FLASH_SECTOR_11 ((uint32_t)0x080E0000)  

uint32_t STMFLASH_ReadWord(uint32_t faddr);                                     
void STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t NumToWrite);
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead);   
#endif /*__USER_FLASH_H*/

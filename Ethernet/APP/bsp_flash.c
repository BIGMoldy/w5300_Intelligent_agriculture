#include "bsp_flash.h"
#include <string.h>    

uint8_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if (addr < ADDR_FLASH_SECTOR_1)
		return FLASH_SECTOR_0;
	else if (addr < ADDR_FLASH_SECTOR_2)
		return FLASH_SECTOR_1;
	else if (addr < ADDR_FLASH_SECTOR_3)
		return FLASH_SECTOR_2;
	else if (addr < ADDR_FLASH_SECTOR_4)
		return FLASH_SECTOR_3;
	else if (addr < ADDR_FLASH_SECTOR_5)
		return FLASH_SECTOR_4;
	else if (addr < ADDR_FLASH_SECTOR_6)
		return FLASH_SECTOR_5;
	else if (addr < ADDR_FLASH_SECTOR_7)
		return FLASH_SECTOR_6;
	else if (addr < ADDR_FLASH_SECTOR_8)
		return FLASH_SECTOR_7;
	else if (addr < ADDR_FLASH_SECTOR_9)
		return FLASH_SECTOR_8;
	else if (addr < ADDR_FLASH_SECTOR_10)
		return FLASH_SECTOR_9;
	else if (addr < ADDR_FLASH_SECTOR_11)
		return FLASH_SECTOR_10;
	return FLASH_SECTOR_11;
}

uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(__IO uint32_t *)faddr;
}

void STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t Num)
{

	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint32_t SectorError = 0;
	uint32_t addrx = 0;
	uint32_t endaddr = 0;
	if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
		return; 

	HAL_FLASH_Unlock();         
	addrx = WriteAddr;             
	endaddr = WriteAddr + Num * 4;

	if (addrx < 0X080C1000)
	{
		while (addrx < endaddr)
		{
			if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF)
			{
				FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;     
				FlashEraseInit.Sector = STMFLASH_GetFlashSector(addrx);
				FlashEraseInit.NbSectors = 1;                          
				FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;   
				if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
				{
					break; 
				}
			}
			else
				addrx += 4;
			FLASH_WaitForLastOperation(FLASH_WAITETIME); 
		}
	}
	FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); 
	if (FlashStatus == HAL_OK)
	{
		while (WriteAddr < endaddr) 
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *pBuffer) != HAL_OK) 
			{
				break; 
			}
			WriteAddr += 4;
			pBuffer++;
		}
	}
	HAL_FLASH_Lock(); 
}

void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; i++)
	{
		pBuffer[i] = STMFLASH_ReadWord(ReadAddr);
		ReadAddr += 4;                           
	}
}


void WriteFlashData(uint32_t WriteAddress, uint8_t data[], uint32_t num)
{
	WriteAddress += ADDR_FLASH_SECTOR_10;
	STMFLASH_Write(WriteAddress,(uint32_t*)data,num);
} 


void ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum)
{  
	ReadAddress += ADDR_FLASH_SECTOR_10;
  STMFLASH_Read(ReadAddress,(uint32_t*)ReadBuf,ReadNum); 
}

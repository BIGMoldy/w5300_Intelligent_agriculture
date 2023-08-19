#include "DHT11.h"
#include "stm32f4xx.h"            
#include "stm32f4xx_hal.h"

void DHT11_IO_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pins : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}	

void DHT11_IO_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
	
  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


void DHT11_Reset(void)
{
	
	DHT11_IO_OUT();  
	DHT11_OUT_LOW;  
	HAL_Delay(20);  
	DHT11_OUT_HIG;  
	HAL_Delay_us(30);    
}

uint8_t DHT11_CheckExist(void)
{
	
	uint8_t Existence = 0;
	DHT11_IO_IN();  
	while((DHT11_IN_READ) && (Existence < 100)) 
	{
	
		Existence++;
		HAL_Delay_us(1);
	}
	if(Existence>=100)
	{
		return 1; 
	}
	else        
		Existence = 0;
	while((!DHT11_IN_READ) && (Existence < 100)) 
	{
		
		Existence++;
		HAL_Delay_us(1);
	}
	if(Existence>=100)
	{
		return 1;
	}
	return 0;
}

uint8_t DHT11_Read_Bit(void)
{
	
	uint8_t data = 0;
	while(DHT11_IN_READ&&data<100)
		{
		data++;
		HAL_Delay_us(1);
	}
	data = 0; 
	while(!DHT11_IN_READ&&data<100) 
	{
			data++;
		HAL_Delay_us(1);
	}
	
	HAL_Delay_us(40);
	if(DHT11_IN_READ) 
		return 1;
	else
		return 0;
}

uint8_t DHT11_Read_Byte(void)
{
	uint8_t i,data;
	data = 0;
	for(i = 0;i < 8;i++)
	{
		data = data << 1; 
		data = data | DHT11_Read_Bit();
	}
	return data;
}

uint8_t DHT11_Read_Data(uint8_t *temperature,uint8_t *humidity)
{
	
	uint8_t Buf[5]; 
	uint8_t i;
	DHT11_Reset();
	if(DHT11_CheckExist() == 0)  
	{
		for(i = 0;i < 5;i++) 
		{
			Buf[i] = DHT11_Read_Byte();
		}
		if((Buf[0] + Buf[1] + Buf[2] + Buf[3]) == Buf[4]) 
		{
			*humidity = Buf[0];  
			*temperature = Buf[2];
		}
	}
	else
		return 1;  
	return 0; 
}

uint8_t DHT11_Init(void)
{
	DHT11_Reset();
	return DHT11_CheckExist(); 
}

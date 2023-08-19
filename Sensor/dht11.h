#ifndef _DHT11__H_
#define _DHT11__H_
#include "stdint.h"

#define DHT11_OUT_HIG HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET)

#define DHT11_OUT_LOW HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)

#define DHT11_IN_READ HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_12)

void DHT11_IO_OUT(void);
void DHT11_IO_IN(void);
void DHT11_Reset(void);
uint8_t DHT11_CheckExist(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(uint8_t *temperature,uint8_t *humidity);
uint8_t DHT11_Init(void);
 
#endif


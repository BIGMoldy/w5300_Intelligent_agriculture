#include "RELAY.h"
#include "stm32f4xx.h"            
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "usart.h"
/*
  PG01
**/
void Fan1_Relay_Open(void)
{
	fan1_open;
}

void Fan1_Relay_Close(void)
{
	fan1_close;
}

void Fan2_Relay_Open(void)
{
	fan2_open;
}

void Fan2_Relay_Close(void)
{
	fan2_close;
}

void Fan3_Relay_Open(void)
{
	fan3_open;
}

void Fan3_Relay_Close(void)
{
	fan3_close;
}


char Fan_State(void)
{
	return !HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_1);
}
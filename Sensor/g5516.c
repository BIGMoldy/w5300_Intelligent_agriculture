#include "g5516.h"
#include "stm32f4xx.h"            
#include "stm32f4xx_hal.h"
#include "math.h"
#include "adc.h"
#include "usart.h"
/*
  PC03 AD
	PB00 µçÆ½
**/


uint16_t g5516_Read(void)
{
	
	uint16_t adcData_temp;
	uint16_t ADC_Sample = 0,ADC_Volt = 0;


	HAL_ADC_Start(&hadc2);
	if(HAL_ADC_PollForConversion(&hadc2,10) == HAL_OK)
	{
		ADC_Sample = HAL_ADC_GetValue(&hadc2);
		ADC_Volt = ADC_Sample * 330/4096;
	}
	HAL_ADC_Stop(&hadc2);
	printf("\r\n adcData = %d",ADC_Sample);
	printf("Voltage value:%d\r\n",ADC_Volt/100);
	
	return ADC_Sample;
}


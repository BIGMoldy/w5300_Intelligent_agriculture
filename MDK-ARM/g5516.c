#include "g5516.h"
#include "stm32f4xx.h"            
#include "stm32f4xx_hal.h"
#include "math.h"
#include "adc.h"
#include "usart.h"
/*
  PC03 AD
	PB00 µÁ∆Ω
**/


uint8_t g5516_Read(void)
{
	
	uint32_t adcData_temp;


 if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
    {
      adcData_temp = HAL_ADC_GetValue(&hadc2);
    }

	
//	printf("\r\n voltage = %F",voltage);
	
//	printf("\r\n adcData = %d",adcData);
	
	return adcData_temp;
}

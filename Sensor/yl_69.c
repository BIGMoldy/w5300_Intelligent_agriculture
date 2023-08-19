#include "yl_69.h"
#include "stm32f4xx.h"            
#include "stm32f4xx_hal.h"
#include "math.h"
#include "adc.h"
#include "usart.h"
/*
  PC04 AD
	PB00 
**/

#define Yl69_Adc_Max 2.516089
#define Yl69_Adc_Min 1.905555

uint8_t Yl69_Read(void)
{
	uint8_t i;
	uint32_t adcData = 0;
	uint32_t adcData_temp;
	float voltage;
	
	for(i = 0;i < 10;i++)
	{  
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,100);
		adcData_temp = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		adcData += adcData_temp;	
	}

	adcData /= 10;
	voltage = adcData * (3.3 / 4096);	
//	printf("\r\n voltage = %F",voltage);
	
	adcData = (Yl69_Adc_Max - voltage) / (Yl69_Adc_Max - Yl69_Adc_Min) * 100;
//	printf("\r\n adcData = %d",adcData);
	
	return adcData;
}

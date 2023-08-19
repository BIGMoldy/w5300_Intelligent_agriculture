#include "key.h"
#include "stm32f4xx_hal.h"

/*****************************************************************************
 * @name       :void KEY_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization key GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/ 
void KEY_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  
 
	
 GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
 
	  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
} 

/*****************************************************************************
 * @name       :uint8_t KEY_Scan(void)
 * @date       :2018-08-09 
 * @function   :Key processing,Response priority:KEY0>KEY1>WK_UP!!
 * @parameters :None
 * @retvalue   :0-No buttons were pressed.
								1-The KEY0 button is pressed
								2-The KEY1 button is pressed
								3-The KEY2 button is pressed
								4-The WK_UP button is pressed
******************************************************************************/ 
uint8_t KEY_Scan(uint8_t mode)
{	 
	static uint8_t key_up=1;
	if(mode)key_up=1;   
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		HAL_Delay(10);
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;
}

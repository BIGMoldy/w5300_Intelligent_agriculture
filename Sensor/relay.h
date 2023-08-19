#ifndef _RELAY__H_
#define _RELAY__H_
#include "stdint.h"

#define fan1_open HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET)
#define fan1_close HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET)

#define fan2_open HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET)
#define fan2_close HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET)

#define fan3_open HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET)
#define fan3_close HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET)

#define buzzer_open HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define buzzer_close HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)

void Fan1_Relay_Open(void);
void Fan1_Relay_Close(void);
void Fan2_Relay_Open(void);
void Fan2_Relay_Close(void);
void Fan3_Relay_Open(void);
void Fan3_Relay_Close(void);
char Fan_State(void);

#endif

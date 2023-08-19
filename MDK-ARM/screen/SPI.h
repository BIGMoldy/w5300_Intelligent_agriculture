#ifndef _SPI_H_
#define _SPI_H_

#include "sys.h"
#include "stm32f4xx_hal.h"

#define SCLK        3	//PB13--->>TFT --SCL/SCK
#define MISO        4	
#define MOSI        5	//PB15 MOSI--->>TFT --SDA/DIN

#define SPI_MOSI  PBout(MOSI)
#define SPI_SCLK  PBout(SCLK)
#define SPI_MISO  PBin(MISO)


//Òº¾§¿ØÖÆ¿ÚÖÃ1²Ù×÷Óï¾äºê¶¨Òå

#define	SPI_MOSI_SET  	SPI_MOSI=1 //LCD_CTRL->BSRR=SPI_MOSI    
#define	SPI_SCLK_SET  	SPI_SCLK=1 //LCD_CTRL->BSRR=SPI_SCLK    


//Òº¾§¿ØÖÆ¿ÚÖÃ0²Ù×÷Óï¾äºê¶¨Òå

#define	SPI_MOSI_CLR  	SPI_MOSI=0 //LCD_CTRL->BRR=SPI_MOSI    
#define	SPI_SCLK_CLR  	SPI_SCLK=0 //LCD_CTRL->BRR=SPI_SCLK    

void  SPIv_WriteData(uint8_t Data);


#endif

#include "show_lcd.h"
#include "string.h"
#include "usart.h"
#include "bsp_flash.h"
#include "DHT11.h"
#include "yl_69.h"
#include "tcp_client.h"
#include "relay.h"
#include "mqtt_client.h"
uint16_t ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};

typedef struct _CONFIG_MSG                      
{
  uint8_t SH_val;            
  uint8_t h_val;           
  uint8_t temp_val;    
  uint8_t mode;	
}Module;


void DrawTestPage(uint8_t *str)
{

LCD_Fill(0,0,lcddev.width,20,BLUE);

LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);
Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"http://www.wiznet.com",16,1);

//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
}


void English_Font_test(void)
{
	
	Gui_StrCenter(0,2,WHITE,BLUE,"INTELLIGEN AGRICULTURE",16,0);
	Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"http://www.wiznet.com",16,0);
	Show_Str(10,30,BLUE,YELLOW,"SH:",12,0);
	Show_Str(80,30,BLUE,YELLOW,"Val1:",12,0);
	Show_Str(110,30,RED,BLUE,"-",16,0);
	Show_Str(165,30,RED,BLUE,"+",16,0);
	Show_Str(180,30,BLUE,YELLOW,"fan1:",12,0);

	
	Show_Str(10,80,BLUE,YELLOW,"H:",12,0);
	Show_Str(80,80,BLUE,YELLOW,"Val2:",12,0);
	Show_Str(110,80,RED,BLUE,"-",16,0);
	Show_Str(165,80,RED,BLUE,"+",16,0);
	Show_Str(180,80,BLUE,YELLOW,"fan2:",12,0);

	
	Show_Str(10,130,BLUE,YELLOW,"T:",12,0);
	Show_Str(80,130,BLUE,YELLOW,"Val3:",12,0);
	Show_Str(110,130,RED,BLUE,"-",16,0);
	Show_Str(165,130,RED,BLUE,"+",16,0);
	Show_Str(180,130,BLUE,YELLOW,"fan3:",12,0);
	
	Show_Str(10,180,BLUE,YELLOW,"L:",16,0);
	
	
}

//void LCD_ShowFloat(uint16_t x,uint16_t y,uint8_t size,float num,uint8_t Int,uint8_t Float)
//{
//        int i,n,a;
//		double	t;  
//        n=(int)num;
//		t=(num-n);
//        for(i=0;i<Float;i++)
//        {
//					a=t*10; 
//					t=(t*10)-a;
//          LCD_ShowNum(x+i*8+Int*8,y,a,4,size);
//        }
//        LCD_ShowNum(x,y,num,4,size);
//				Show_Str(x+Int*16,y,0,0,".",16,1);
//}



void Touch_Test(void)
{
		uint32_t SH_val,h_val,temp_val;
		Module module = {0};	
		STMFLASH_Read(ADDR_FLASH_SECTOR_6,(uint32_t*)&module.mode,2);
    if(module.mode==1)
		{
			module.SH_val=68;
			module.h_val=85;
			module.temp_val=25;
			STMFLASH_Write(ADDR_FLASH_SECTOR_7,(uint32_t*)&module.SH_val,2);
			STMFLASH_Write(ADDR_FLASH_SECTOR_8,(uint32_t*)&module.h_val,2);
			STMFLASH_Write(ADDR_FLASH_SECTOR_9,(uint32_t*)&module.temp_val,2);
			h_val=module.h_val;
			SH_val=module.SH_val;
			temp_val=module.temp_val;
			printf("write success\r\n");
    }else
		{
			STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t*)&module.SH_val,8);
			STMFLASH_Read(ADDR_FLASH_SECTOR_8,(uint32_t*)&module.h_val,8);
			STMFLASH_Read(ADDR_FLASH_SECTOR_9,(uint32_t*)&module.temp_val,8);
			h_val=module.h_val;
			SH_val=module.SH_val;
			temp_val=module.temp_val;
		}
					
	TP_Init();
	
//LCD_Clear(WHITE);
	while(1)
	{
		if(flag==1)
		{
		STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t*)&module.SH_val,8);
		STMFLASH_Read(ADDR_FLASH_SECTOR_8,(uint32_t*)&module.h_val,8);
		STMFLASH_Read(ADDR_FLASH_SECTOR_9,(uint32_t*)&module.temp_val,8);
		h_val=module.h_val;
		SH_val=module.SH_val;
		temp_val=module.temp_val;
			flag=0;
		}
		
		English_Font_test();
		LCD_ShowNum(40,130,temperature,3,16);
		LCD_ShowNum(40,30,yl69_humidity,3,16);
		LCD_ShowNum(40,80,humidity,3,16);
		LCD_ShowNum(120,30,module.SH_val,3,16);
		LCD_ShowNum(120,80,module.h_val,3,16);
		LCD_ShowNum(120,130,module.temp_val,3,16);
		LCD_ShowNum(40,180,g5516_LightLux,4,16);
		Show_Str(30,250,BLUE,YELLOW,"RES",16,1);
		Show_Str(150,250,BLUE,YELLOW,"SAVE",16,1);
		
			if(0<yl69_humidity&&yl69_humidity<module.SH_val)
			{
				LCD_ShowNum(210,30,1,1,16);
				Fan1_Relay_Open();
				

			}
			else
			{
			  LCD_ShowNum(210,30,0,1,16);
				Fan1_Relay_Close();
				
			}			
			if(0<humidity&&humidity<module.h_val)
			{
			 	LCD_ShowNum(210,80,1,1,16);
				Fan2_Relay_Open();
				
			}
			else
			{
			 	LCD_ShowNum(210,80,0,1,16);
				Fan2_Relay_Close();
			}								
			
			if(0<temperature&&temperature>module.temp_val)
			{
			  LCD_ShowNum(210,130,1,1,16);
				Fan3_Relay_Open();
			}
			else
			{
			  LCD_ShowNum(210 ,130,0,1,16);
				Fan3_Relay_Close();
			}		
			
			if(temperature>module.temp_val||(0<humidity&&humidity<module.h_val)||(0<yl69_humidity&&yl69_humidity<module.SH_val))
			{
			  buzzer_open;
			}
			else
			{
				buzzer_close;
			}
	
		if(!tp_dev.scan(0))
		{			
		do_mqtttcp_client();
		HAL_Delay(500);
		}
		
		if(tp_dev.sta&TP_PRES_DOWN)			
		{	
				if(150<tp_dev.x&&tp_dev.x<210&&0<tp_dev.y&&tp_dev.y<40)
				{									
					module.SH_val=module.SH_val+1;					
					SH_val=module.SH_val;
					LCD_ShowNum(120,30,module.SH_val,3,16);
					HAL_Delay(500);
				}
				
        if(60<tp_dev.x&&tp_dev.x<120&&0<tp_dev.y&&tp_dev.y<40)
				{				
					module.SH_val=module.SH_val-1;				
					SH_val=module.SH_val;
					LCD_ShowNum(120,30,module.SH_val,3,16);
						HAL_Delay(500);
				}
								
				if(150<tp_dev.x&&tp_dev.x<210&&50<tp_dev.y&&tp_dev.y<90)
				{									
					module.h_val=module.h_val+1;				
					h_val=module.h_val;
					LCD_ShowNum(120,80,module.h_val,3,16);
					HAL_Delay(500);
				}
				
        if(60<tp_dev.x&&tp_dev.x<120&&50<tp_dev.y&&tp_dev.y<90)
				{				
					module.h_val=module.h_val-1;					
					h_val=module.h_val;
					LCD_ShowNum(120,80,module.h_val,3,16);
					HAL_Delay(500);
				}	
				
				if(150<tp_dev.x&&tp_dev.x<210&&100<tp_dev.y&&tp_dev.y<140)
				{									
					module.temp_val=module.temp_val+1;
				  temp_val=module.temp_val;
					LCD_ShowNum(120,130,module.temp_val,3,16);
					HAL_Delay(500);
				}
				
        if(60<tp_dev.x&&tp_dev.x<120&&100<tp_dev.y&&tp_dev.y<140)
				{				
					module.temp_val=module.temp_val-1;			
					LCD_ShowNum(120,130,module.temp_val,3,16);
					HAL_Delay(500);
				}
				
				if(30<tp_dev.x&&tp_dev.x<80&&240<tp_dev.y&&tp_dev.y<260)
				{
					printf("rest....\r\n");
				  module.mode=1;
					temp_val=module.temp_val;
						STMFLASH_Write(ADDR_FLASH_SECTOR_6,(uint32_t*)&module.mode,2);
					Show_Str(30,250,RED,YELLOW,"RES",16,0);
					printf("rest success\r\n");
				}
				
				if(150<tp_dev.x&&tp_dev.x<240&&240<tp_dev.y&&tp_dev.y<270)
				{
					module.h_val=h_val;
					module.SH_val=SH_val;
					module.temp_val=temp_val;
					module.mode=0;
					printf("save....\r\n");

					STMFLASH_Write(ADDR_FLASH_SECTOR_6,(uint32_t*)&module.mode,2);
				 	STMFLASH_Write(ADDR_FLASH_SECTOR_7,(uint32_t*)&module.SH_val,2);
					STMFLASH_Write(ADDR_FLASH_SECTOR_8,(uint32_t*)&module.h_val,2);
					STMFLASH_Write(ADDR_FLASH_SECTOR_9,(uint32_t*)&module.temp_val,2);
				
					flag=1;
					Show_Str(150,250,RED,YELLOW,"SAVE",16,0);
					printf("save success\r\n");
				}
		}
	}   
}

#include "serial_studio.h"

#include "string.h"

#include "usart.h"
#include "dht11.h"
#include "relay.h"
#include "yl_69.h"
/*

**/

char uart_data[20] = {0};

#define Frame_start_sequence "/*"    
#define Frame_end_sequence   "*/"    
#define Data_separator       ","    


void Serial_Studio(void)
{
	uint8_t i;
	uint8_t temperature;     
	uint8_t humidity;      
	uint8_t yl69_humidity;   
	char    temp[20] = {0};  
	
	for(i = 0;i < 20;i++)
		uart_data[i] = 0;                       
	
	DHT11_Read_Data(&temperature,&humidity);  
	yl69_humidity = Yl69_Read();             

	
	strcat(uart_data,Frame_start_sequence);   
	uBinaryToAscii(temperature, temp);
	strcat(uart_data,temp);                   
	strcat(uart_data,Data_separator);         
	
	for(i = 0; i < 20;i++)
		temp[i] = 0;
	
	uBinaryToAscii(humidity, temp);
	strcat(uart_data,temp);                  
	strcat(uart_data,Data_separator);         

	for(i = 0; i < 20;i++)
		temp[i] = 0;
		
	uBinaryToAscii(yl69_humidity, temp);
	strcat(uart_data,temp);                   
	strcat(uart_data,Data_separator);       
	
	for(i = 0; i < 20;i++)
		temp[i] = 0;

	temp[0] = Fan_State() + '0';
	strcat(uart_data,temp);                   
	strcat(uart_data,Frame_end_sequence);     
	printf("\r\n%s",uart_data);
}


char uBinaryToAscii(unsigned int value, char *str)
{
    char i;
    char num_str[10] = { 0 };
    unsigned int quotient;
    unsigned char len;

    len = 0;

    quotient = value / 10;
    while (quotient != 0)
    {
        num_str[len] = value % 10 + '0';
        len++;
        value = quotient;
        quotient = value / 10;
    }

//    if (value != 0)
    {
        num_str[len] = value + '0';
        len++;
    }

    for (i = 0; i < len; i++)
    {
        *str = num_str[len-i-1];
        str++;
    }

    return len;
}

char sBinaryToAscii(int value, char *str)
{
    char len;

    if (value < 0)
    {
        *str = '-';
        str++;
        len = uBinaryToAscii(-value, str);
        return len + 1;
    }
    else
    {
        len = uBinaryToAscii(value, str);
        return len;
    }
}

char binaryToAscii(int data, char *str)
{
	char len = 0;
	int value;
	char i;
	char num_str[10] = { 0 };
	int quotient;

	if (data < 0)
	{
		*str = '-';
		str++;
		value = -data;
		len = 1;
	}
	else
	{
		value = data;
		len = 0;
	}

	quotient = value / 10;
	while (quotient != 0)
	{
		num_str[len] = value % 10 + '0';
		len++;
		value = quotient;
		quotient = value / 10;
	}

	if (value != 0)
	{
		num_str[len] = value + '0';
		len++;
	}

	for (i = 0; i < len; i++)
	{
		*str = num_str[len - i - 1];
		str++;
	}

	return len;
}




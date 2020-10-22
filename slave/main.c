#include <asf.h>
#include "main.h"
#include "SPI.h"
#include "modbus_func.h"
#include "string.h"
#include "SPI.h"
#include "timer.h"

uint16_t MB_holding_reg[16] = {0x1122,0x1223,0x1324,0x1425,0x1526,0x1627, 0x1728, 0x1829, 0x1930, 0x1221, 0x1331, 0x1441, 0x1551, 0x1661, 0x1771, 0x1881};
uint16_t MB_input_reg[16] = {0x0102,0x0304,0x0506,0x0708,0x0910,0x1112, 0x1314, 0x1516, 0x1718, 0x1920, 0x2122, 0x2324, 0x2526, 0x2728, 0x2930, 0x3132};
uint8_t global_count = 0;
volatile uint8_t Massiv_output[15]; //вывод(ответ)
volatile uint8_t Massiv_input[15];  //вывод(ответ)
unsigned int RCR = 0;
unsigned char adress = 0x02; // адрес 0x02

int main (void)
{
	cli();
	ptr_mas_input = &Massiv_input[0];
	ptr_mas_output = &Massiv_output[0];
	timeSpiInterrupt = 0xFFFF;
	SPI_SlaveInit();
	Timer_ini();
	sei();
	while(1)
	{	
		if ( (timeSpiInterrupt != 0xFFFF) && (timeSpiInterrupt > 10) )
		{
			if (!check_crc16( (void *) &Massiv_input[1], Massiv_input[0]) )
			{
				ptr_mas_input = &Massiv_input[0];
				ptr_mas_output = &Massiv_output[0];
				
				if ( (*(ptr_mas_input + 1) >= 0x60) && (*(ptr_mas_input + 1) <= 0x69) ) // отбросить адрес, пересчитать crc
				{
					*(ptr_mas_input + 1) = *(ptr_mas_input + 1) - 0x60 ;
				}
				
				spi_mode = 2;
				timeSpiInterrupt = 0xFFFF;
/*==============================================обработка команды====================================================*/
				if ( *(ptr_mas_input + 1) == adress)
				{
					*ptr_mas_output = adress;
					global_count++;
/*=====================================================0x03==========================================================*/		
					if ( *(ptr_mas_input + 2) == 0x03)
					{
						*(ptr_mas_output + 1) = 0x03;
						global_count++;
					
						//проверки
						if ((*(ptr_mas_input + 6) <= 0x01) || (*(ptr_mas_input + 6) >= 0x7D ))
						{
							*ptr_mas_output = 0x83;
							*(ptr_mas_output + 1) = 0x03;											//Exception code = 0x03
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else if ((*(ptr_mas_input + 4) >= 99) || ((*(ptr_mas_input + 4) + (*(ptr_mas_input + 6) - 0x01)) >= 99 ))
						{
							*ptr_mas_output = 0x83;
							*(ptr_mas_output + 1) = 0x02;											//Exception code = 0x02
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else
						{
							Read_Holding_Reg((void *) &Massiv_input[0], (void *) &Massiv_output[0]);	
						}	
					}
/*===================================================0x06============================================================*/
					else if (*(ptr_mas_input + 2) == 0x06)
					{
						*(ptr_mas_output + 1) = 0x06;
						global_count++;
						//проверки
						if (((*(ptr_mas_input + 6) >= 0xFF) && (*(ptr_mas_input + 5) >= 0xFF)) || ((*(ptr_mas_input + 6) <= 0x00) && (*(ptr_mas_input + 5) <= 0x00)))
						{
							*ptr_mas_output = 0x86;
							*(ptr_mas_output + 1) = 0x03;										   //Exception code = 0x03
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else if (*(ptr_mas_input + 4) <= 0 || *(ptr_mas_input + 4) >= 100)
						{
							*ptr_mas_output = 0x86;
							*(ptr_mas_output + 1) = 0x02;										   //Exception code = 0x02
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else
						{
							Write_single_register((void *) &Massiv_input[0], (void *) &Massiv_output[0]);	
						}
					}
/*==================================================0x04=============================================================*/
					else if (*(ptr_mas_input + 2) == 0x04)
					{
						*(ptr_mas_output + 1) = 0x04;
						global_count++;
						//проверки
						if ( (*(ptr_mas_input + 6) <= 0x01) || (*(ptr_mas_input + 6) >= 0x7D) )
						{
							*ptr_mas_output = 0x84;
							*(ptr_mas_output + 1) = 0x03;										//Exception code = 0x03
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else if ((*(ptr_mas_input + 4) >= 99) || ((*(ptr_mas_input + 4) + (*(ptr_mas_input + 6) - 0x01)) >= 99 ))
						{
							*ptr_mas_output = 0x84;
							*(ptr_mas_output + 1) = 0x02;										//Exception code = 0x02
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else
						{
							Read_Input_Registers((void *) &Massiv_input[0], (void *) &Massiv_output[0]);
						}
				
					}
/*===================================================0x10==========================================================*/
					else if (*(ptr_mas_input + 2) == 0x10)
					{
						*(ptr_mas_output + 1) = 0x10;
						global_count++;
						//проверки
						if ( ((*(ptr_mas_input + 6) <= 0x01) || (*(ptr_mas_input + 6) >= 0x7B)) || (*(ptr_mas_input + 7) != (*(ptr_mas_input + 6)*2) ) )
						{
							*ptr_mas_output = 0x90;
							*(ptr_mas_output + 1) = 0x03;										//Exception code = 0x03
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else if ((*(ptr_mas_input + 4) >= 99) || ((*(ptr_mas_input + 4) + (*(ptr_mas_input + 6) - 0x01)) >= 99 ))
						{
							*ptr_mas_input = 0x90;
							*(ptr_mas_input + 1) = 0x02;										//Exception code = 0x02
							spi_mode = 3;
							global_count = 2;
							CRC_result();
						}
						else
						{
							Write_Multiple_Registers((void *) &Massiv_input[0], (void *) &Massiv_output[0]);	
						}
					}
/*===================================если функция отсутствует================================================*/
					else
					{
						*ptr_mas_input = 0x80 + *(ptr_mas_input + 2);							//Exception code = 0x01
						*(ptr_mas_input + 1) = 0x01;
						global_count++;
						CRC_result();
					}
/*==========================================================================================================*/			
					spi_mode = 3;
					SPDR = global_count; // отправил байт длины ответа
				}
				else
				{
					spi_mode = 4;
				}
			}
		}
	}
}
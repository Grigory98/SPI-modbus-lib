#include <asf.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "modbus_func.h"
#include "SPI_master.h"

uint8_t *ptr_mas_input;
uint8_t *ptr_mas_output;
uint8_t *ptr_masout_tmp;

volatile uint16_t timeSpiInterrupt = 0;
volatile uint16_t timeSpiError = 0;

volatile uint8_t spi_mode = 0;		//статус
uint8_t	g_count_receive_SPI = 0;
uint8_t	g_count_receive_SPI_tmp = 0;
uint8_t	SPI_transmit_len = 0;
uint8_t global_count = 0;

void SPI_MasterInit(void)
{
	DDRB = (1<<MOSI_BIT)|(1<<SCK_BIT)|(1<<SS_BIT)|(0<<MISO_BIT);
	PORTB = (0<<MOSI_BIT)|(0<<SCK_BIT)|(0<<SS_BIT)|(0<<MISO_BIT);
	SPCR = (1<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA);
	SPI_SetRate_del_32;
}

void spi_transmit(unsigned char *loc_ptrmas_inp, unsigned char *loc_ptrmas_out, unsigned char loc_uart_len)
{
	ptr_masout_tmp = loc_ptrmas_out;		//Massiv_output[0]
	ptr_mas_input = loc_ptrmas_inp;			//Massiv_input[0]
	ptr_mas_output = loc_ptrmas_out;		//Massiv_output[0]
	memcpy((void *) (ptr_mas_output + 1), (void *) ptr_mas_input, loc_uart_len); //копируем весь Massiv_input[0] в Massiv_output[1]
	*ptr_mas_output = loc_uart_len;			//первый элемент массива - длина посылки отправляемая на слэйв
	SPI_transmit_len = loc_uart_len + 1;	//длина данных для передачи по SPI
	spi_mode = 1;
	SPDR = *ptr_mas_output;					//передаем первый байт длины команды принятой с uart, срабатывают прерывания
}

void spi_timeout(void)
{
	if ((timeSpiInterrupt != 0xFFFF) && (timeSpiInterrupt > 50))	// таймаут для начала приема
	{
		timeSpiInterrupt = 0xFFFF;
		if (SPDR == 0xFF && SPDR == 0x00)
		{
			g_count_receive_SPI = 0;
			spi_mode = 4;
		}
		else
		{
			timeSpiError = 0;
			SPDR = 0xFF;		//срабатывает прерывание с spi_mode = 2
		}
	}
}

void intbus_error_timeout(volatile unsigned char *loc_ptrmas_inp, volatile unsigned char *loc_ptrmas_out)
{
	if ((spi_mode == 2) && (timeSpiError != 0xFFFF) && (timeSpiError >= 100)) //ошибка intbus
	{
		timeSpiError = 0xFFFF;
		*loc_ptrmas_out = 0x00;
		*(loc_ptrmas_out + 1) = 0x02/**loc_ptrmas_inp - 0x60*/;
		global_count = 2;
		CRC_result();
		USART_Transmit_data((void *) loc_ptrmas_out, global_count);
		spi_mode = 0;
		global_count = 0;
		ptr_mas_input = loc_ptrmas_inp;
		ptr_mas_output = loc_ptrmas_out;
	}
	else if (spi_mode != 2)
	{
		timeSpiError = 0xFFFF;
	}
}

void send_slaveAnswer_to_pc(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output)
{
	if (spi_mode == 4)
	{
		ptr_mas_input = loc_ptrmas_input;
		ptr_mas_output = loc_ptrmas_output;
		
		if (!check_crc16( (void *) (ptr_mas_output + 1), *ptr_mas_output) )
		{
			USART_Transmit_data((void *) (ptr_mas_output + 1), g_count_receive_SPI);
		}
		spi_mode = 0;
	}
}

send_masterAnswer_to_pc(unsigned char adr, unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output)
{
	if (spi_mode == 5)
	{
		ptr_mas_input = loc_ptrmas_input;
		ptr_mas_output = loc_ptrmas_output;
		
		if (!check_crc16( (void *) ptr_mas_input, UART_len) )
		{
			
/*=====================================================================обработка команды========================================================================*/
			if ( *(ptr_mas_input) == adr)
			{
				*ptr_mas_output = adr;
				global_count++;
/*===========================================================================0x03===============================================================================*/
				if ( *(ptr_mas_input + 1) == 0x03)
				{
					*(ptr_mas_output + 1) = 0x03;
					global_count++;
					
					//проверки
					if ((*(ptr_mas_input + 5) <= 0x01) || (*(ptr_mas_input + 5) >= 0x7D ))
					{
						*ptr_mas_output = 0x83;
						*(ptr_mas_output + 1) = 0x03;											//Exception code = 0x03
						global_count = 2;
						CRC_result();
					}
					else if ((*(ptr_mas_input + 3) >= 99) || ((*(ptr_mas_input + 3) + (*(ptr_mas_input + 5) - 0x01)) >= 99 ))
					{
						*ptr_mas_output = 0x83;
						*(ptr_mas_output + 1) = 0x02;											//Exception code = 0x02
						global_count = 2;
						CRC_result();
					}
					else
					{
						Read_Holding_Reg((void *) ptr_mas_input, (void *) ptr_mas_output);
					}
				}
/*=========================================================================0x06===============================================================================*/
				else if (*(ptr_mas_input + 1) == 0x06)
				{
					*(ptr_mas_output + 1) = 0x06;
					global_count++;
					//проверки
					if (((*(ptr_mas_input + 5) >= 0xFF) && (*(ptr_mas_input + 4) >= 0xFF)) || ((*(ptr_mas_input + 5) <= 0x00) && (*(ptr_mas_input + 4) <= 0x00)))
					{
						*ptr_mas_output = 0x86;
						*(ptr_mas_output + 1) = 0x03;										   //Exception code = 0x03
						global_count = 2;
						CRC_result();
					}
					else if (*(ptr_mas_input + 3) <= 0 || *(ptr_mas_input + 3) >= 100)
					{
						*ptr_mas_output = 0x86;
						*(ptr_mas_output + 1) = 0x02;										   //Exception code = 0x02
						global_count = 2;
						CRC_result();
					}
					else
					{
						Write_single_register((void *) ptr_mas_input, (void *) ptr_mas_output);
					}
				}
/*========================================================================0x04==============================================================================*/
				else if (*(ptr_mas_input + 1) == 0x04)
				{
					*(ptr_mas_output + 1) = 0x04;
					global_count++;
					//проверки
					if ( (*(ptr_mas_input + 5) <= 0x01) || (*(ptr_mas_input + 5) >= 0x7D) )
					{
						*ptr_mas_output = 0x84;
						*(ptr_mas_output + 1) = 0x03;										//Exception code = 0x03
						global_count = 2;
						CRC_result();
					}
					else if ((*(ptr_mas_input + 3) >= 99) || ((*(ptr_mas_input + 3) + (*(ptr_mas_input + 5) - 0x01)) >= 99 ))
					{
						*ptr_mas_output = 0x84;
						*(ptr_mas_output + 1) = 0x02;										//Exception code = 0x02
						global_count = 2;
						CRC_result();
					}
					else
					{
						Read_Input_Registers((void *) ptr_mas_input, (void *) ptr_mas_output);
					}
					
				}
/*======================================================================0x10=============================================================================*/
				else if (*(ptr_mas_input + 1) == 0x10)
				{
					*(ptr_mas_output + 1) = 0x10;
					global_count++;
					//проверки
					if ( ((*(ptr_mas_input + 5) <= 0x01) || (*(ptr_mas_input + 5) >= 0x7B)) || (*(ptr_mas_input + 6) != (*(ptr_mas_input + 5)*2) ) )
					{
						*ptr_mas_output = 0x90;
						*(ptr_mas_output + 1) = 0x03;										//Exception code = 0x03
						global_count = 2;
						CRC_result();
					}
					else if ((*(ptr_mas_input + 3) >= 99) || ((*(ptr_mas_input + 3) + (*(ptr_mas_input + 5) - 0x01)) >= 99 ))
					{
						*ptr_mas_output = 0x90;
						*(ptr_mas_output + 1) = 0x02;										//Exception code = 0x02
						global_count = 2;
						CRC_result();
					}
					else
					{
						Write_Multiple_Registers((void *) ptr_mas_input, (void *) ptr_mas_output);
					}
				}
/*========================================================если функция отсутствует=======================================================================*/
				else
				{
					*ptr_mas_output = 0x80 + *(ptr_mas_input + 1);							//Exception code = 0x01
					*(ptr_mas_output + 1) = 0x01;
					global_count++;
					CRC_result();
				}
			}
		}
/*========================================================конец обработки команды=======================================================================*/
		USART_Transmit_data((void *) ptr_mas_output, global_count); // отправка ответа
		UART_len = 0;
		global_count = 0;
		spi_mode = 0;
	}
}

ISR(SPI_STC_vect)
{
	uint8_t loc_tmp = 0;
	loc_tmp = SPDR;
	
	switch (spi_mode)
	{
		case 1:	//посылает остальные байты пакета
			if (SPI_transmit_len)		// если есть еще данные для переачи
			{
				SPDR = *(ptr_mas_output + 1); //Massiv_output[1]
				ptr_mas_output++;		//Massiv_output[2..n]
				SPI_transmit_len--;
			}
			else //пакет отправлен, выполняется таймаут для начала приема ответа
			{
				spi_mode = 2;
				timeSpiInterrupt = 0;	// таймаут для начала приема ответа
			}
		break;
		
		case 2:	//принимается первый байт с количеством байтов ответного пакета
			if ((loc_tmp != 0xFF) && (loc_tmp != 0x00))
			{
				g_count_receive_SPI = loc_tmp;
				g_count_receive_SPI_tmp = g_count_receive_SPI;
				ptr_mas_output = ptr_masout_tmp;
				*ptr_mas_output = loc_tmp;
				spi_mode = 3;			// пришел байт длины ответа
			}
			SPDR = 0xFF;
		break;
		
		case 3: //принимаются остальные байты ответного пакета
			if (g_count_receive_SPI_tmp--)
			{
				*(ptr_mas_output + 1) = loc_tmp;
				ptr_mas_output++;
				SPDR = 0xFF;
			}
			else //прием окончен, принятый ответ отправляется по уарту
			{
				spi_mode = 4;
			}
		break;
		
		default:
		break;
	}
}
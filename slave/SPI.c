#include <asf.h>
#include "SPI.h"
#include "main.h"

volatile uint16_t timeSpiInterrupt = 0;
uint8_t *ptr_mas_input = 0;
uint8_t *ptr_mas_output = 0;
uint8_t g_len_output = 0;
uint8_t g_len_input = 0;
uint8_t spi_mode = 0;

void SPI_SlaveInit(void)
{
	DDRB = (1<<MISO_BIT);
	SPCR = (1<<SPIE)|(1<<SPE)|(0 << DORD)|(0 << MSTR);
	SPSR = (0 << SPI2X);
}

ISR(SPI_STC_vect)
{
	uint8_t loc_tmp = 0;
	loc_tmp = SPDR;
	
	if (spi_mode != 3) SPDR = 0x00;
	
	switch (spi_mode)
	{
		case 0:		// ждали команду, пришел первый байт команды
		if ((loc_tmp != 0xFF) && (loc_tmp != 0x00))
		{
			*ptr_mas_input = loc_tmp;
			ptr_mas_input++;
			spi_mode = 1;
			timeSpiInterrupt = 0;	// ждем окончания команды
		}
		break;
		
		case 1:				  // продолжаем прием команды
		*ptr_mas_input = loc_tmp;
		ptr_mas_input++;
		timeSpiInterrupt = 0; // ждем окончания команды
		break;
		
		case 2:				// пришел запрос во время обработки принятой команды
		break;
		
		case 3:				// обработали команду, выдаем ответ
		SPDR = *ptr_mas_output;
		ptr_mas_output++;
		global_count--;
		if (!global_count) // отправляем последний байт
		{
			spi_mode = 4;
		}
		break;
		
		case 4:			   // передали последний байт, больше передавать нечего
		global_count = 0;
		spi_mode = 0;
		break;
		
		default:
		break;
	}
}


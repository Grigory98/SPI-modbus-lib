#define F_CPU 8000000
#include <asf.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "SPI_master.h"
#include "main.h"

volatile unsigned char UART_len = 0;
volatile uint16_t timeRxInterrupt = 0;

ISR(USART_RX_vect)
{
	Massiv_input[UART_len] = UDR0;
	UART_len++;
	timeRxInterrupt = 0;
}

void USART_init()
{
	USART_SetRate_19200;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1 << RXCIE0)|(0 << TXCIE0);
	UCSR0A = (0 << U2X0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit_data(unsigned char *data, unsigned char loc_len)
{
	while (loc_len--)
	{
		while (!(UCSR0A&(1<<UDRE0)));
		UDR0 = *data;
		data++;
	}
}

void usart_timeout()
{
	if ((timeRxInterrupt != 0xFFFF) && (timeRxInterrupt > 10))	/// приняли данные с UART
	{
		timeRxInterrupt = 0xFFFF;
		if ( (Massiv_input[0] >= 0x60) && (Massiv_input[0] <= 0x69) )
		{
			spi_transmit((void *) &Massiv_input[0], (void *) &Massiv_output[0], UART_len);
			UART_len = 0;
		}
		else if (Massiv_input[0] == adress)
		{
			spi_mode = 5;
		}
	}
}
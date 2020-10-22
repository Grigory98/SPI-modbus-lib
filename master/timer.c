#define F_CPU 8000000
#include <asf.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "SPI_master.h"

void Timer_ini() // T=2мс
{
	TCNT2 = 0;
	OCR2A = 248;
	TCCR2A = 0b00000010;
	TCCR2B = 0b00000011;
	TIMSK2 = (1<<OCIE2A);
}

ISR(TIMER2_COMPA_vect)
{
	if (timeRxInterrupt != 0xFFFF)  timeRxInterrupt++;
	if (timeSpiInterrupt != 0xFFFF)  timeSpiInterrupt++;
	if (timeSpiError != 0xFFFF)  timeSpiError++;
	
	spi_timeout();
	usart_timeout();
	//intbus_error_timeout((void *) &Massiv_input[0], (void *) &Massiv_output[0], 4);
}
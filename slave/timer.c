#include <asf.h>
#include "timer.h"
#include "SPI.h"

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
	if (timeSpiInterrupt != 0xFFFF)  timeSpiInterrupt++;
}
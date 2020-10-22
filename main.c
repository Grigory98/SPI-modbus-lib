/*master*/
#define F_CPU 8000000
#include <asf.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "SPI_master.h"
#include "usart.h"
#include "timer.h"
#include "modbus_func.h"

volatile unsigned char Massiv_input[20] = {}; //ввод(запрос)
volatile unsigned char Massiv_output[20] = {}; //вывод(ответ)
uint16_t MB_holding_reg[16] = {0x1456,0x9998,0x9796,0x9594,0x9392,0x9190, 0x8988, 0x8786, 0x8584, 0x8382, 0x8180, 0x7978, 0x7776, 0x7574, 0x7372, 0x7170};
uint16_t MB_input_reg[16] = {0x1337,0x1234,0x1235,0x1236,0x1237,0x1238, 0x1239, 0x1230, 0x1231, 0x1232, 0x1233, 0x1234, 0x1235, 0x1236, 0x1237, 0x1238};
unsigned char adress = 0x02;

int main (void)
{
	cli();
	USART_init();
	SPI_MasterInit();
	Timer_ini();
	timeSpiInterrupt = 0xFFFF;
	timeRxInterrupt = 0xFFFF;
	ptr_mas_input = &Massiv_input[0];
	ptr_mas_output = &Massiv_output[0];
	sei();
	
	while(1)
	{	
		send_slaveAnswer_to_pc((void *) &Massiv_input[0], (void *) &Massiv_output[0]);
		send_masterAnswer_to_pc(adress, &Massiv_input[0], &Massiv_output[0]);
		intbus_error_timeout((void *) &Massiv_input[0], (void *) &Massiv_output[0]);
	}
}




/*
#ifndef MACROS_H_
#define MACROS_H_

volatile unsigned char fc = 0;
volatile unsigned char num = 0;


// установка бита
#define _SetBit(ADDRESS, BIT) (ADDRESS |= (1<<BIT))

// очистка бита
#define _ClearBit(ADDRESS, BIT) (ADDRESS &= ~(1<<BIT))


void Modbus_func_0x03()
{
	if (Massiv_input[1] == 0x03) //0x03
	{
		fc = Massiv_input[5];
		num = 3;
		while(fc--)
		{
			num++;
			num++;
		}
		num++;
		num++;
	}
}

void Modbus_func_0x04()
{
	if (Massiv_input[1] == 0x04) //0x04
	{
		fc = Massiv_input[5];
		num = 3;
		while(fc--)
		{
			num++;
			num++;
		}
		num++;
		num++;
	}
}

void Modbus_func_0x06()
{
	if (Massiv_input[1] == 0x06) //0x06
	{
		num = 8;
	}
}

void Modbus_func_0x10()
{
	if (Massiv_input[1] == 0x10) //0x10
	{
		num = 8;
	}
}


#endif /* MACROS_H_ */
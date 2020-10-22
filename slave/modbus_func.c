#include <asf.h>
#include "modbus_func.h"
#include "main.h"
#include "SPI.h"
#include "string.h"
/*===============================================================CRC===============================================================================*/
int crc_chk (unsigned char* data_loc, unsigned char length) //контрольная сумма
{
	register int j;
	register unsigned int reg_crc = 0xFFFF;
	while(length--)
	{
		reg_crc ^= *data_loc++;
		for(j=0;j<8;j++)
		{
			if(reg_crc & 0x01)
			{
				reg_crc = (reg_crc >> 1) ^ 0xA001;
			}
			else
			{
				reg_crc = reg_crc >> 1;
			}
		}
	}
	return reg_crc;
}
/*=================================================================================================================================================*/
unsigned short crc16_update(unsigned short crc, unsigned char data_byte) {
	unsigned short i_loc = 0;

	crc ^= data_byte;

	for (i_loc = 0; i_loc < 8; ++i_loc) {
		if (crc & 1) {
			crc = (crc >> 1) ^ 0xA001;
			} else {
			crc = (crc >> 1);
		}
	}

	return crc;
}
/*=================================================================================================================================================*/
unsigned short check_crc16 (unsigned char *tmp_buf, unsigned short len_tmp_buf)		// проверка контрольной суммы
{       
	unsigned short i_f_sum = 0;
	unsigned short result = 0xFFFF;
	unsigned short result_crc_in = 0;

	if (len_tmp_buf < 3) return 0xFF;

	for(i_f_sum = 0; i_f_sum < (len_tmp_buf - 2); i_f_sum++) {
		result = crc16_update(result, *tmp_buf);
		tmp_buf++;
	}

	result_crc_in = (*tmp_buf & 0xFF);
	tmp_buf++;
	result_crc_in |= (*tmp_buf << 8);

	return (result ^ result_crc_in);
}
/*=================================================================================================================================================*/
void CRC_result()
{
	RCR = crc_chk(Massiv_output, global_count);
	Massiv_output[global_count++] = RCR;
	Massiv_output[global_count++] = (RCR >> 8);
}
/*=================================================================================================================================================*/
/*=================================================================================================================================================*/
void Read_Holding_Reg(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output) //x03
{
	unsigned char adr = 0;
	unsigned char f = 0;
	
	//Адрес регистра модбас
	*(loc_ptrmas_output + 2) = *(loc_ptrmas_input + 6)*2;
	global_count++;
	
	//Значение регистра модбас
	adr = *(loc_ptrmas_input + 4);
	f = *(loc_ptrmas_input + 6);
	
	loc_ptrmas_output = loc_ptrmas_output + 3;
	while(f--)
	{
		//начинает с Massiv_input[3]
		*loc_ptrmas_output = MB_holding_reg[adr] >> 8;
		loc_ptrmas_output++;
		*loc_ptrmas_output = MB_holding_reg[adr];
		loc_ptrmas_output++;
		adr++;
		global_count++;
		global_count++;
	}
	CRC_result();
}
/*=================================================================================================================================================*/
void Write_single_register(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output) //x06
{
	unsigned char adr = 0;
	
	//Адрес регистра modbus
	*(loc_ptrmas_output + 2) = *(loc_ptrmas_input + 3);
	global_count++;
	*(loc_ptrmas_output + 3) = *(loc_ptrmas_input + 4);
	global_count++;
	
	//Значение регистра modbus
	*(loc_ptrmas_output + 4) = *(loc_ptrmas_input + 5);
	global_count++;
	*(loc_ptrmas_output + 5) = *(loc_ptrmas_input + 6);
	global_count++;
	
	adr = *(loc_ptrmas_output + 3);
	MB_input_reg[adr] = ((uint16_t) *(loc_ptrmas_output + 4) << 8) | *(loc_ptrmas_output + 5);
	CRC_result();
}
/*=================================================================================================================================================*/
void Read_Input_Registers(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output) //x04
{
	unsigned char adr = 0;
	unsigned char f = 0;
	
	//Адрес регистра модбас
	*(loc_ptrmas_output + 2) = *(loc_ptrmas_input + 6) * 2;
	global_count++;
	
	//Значение регистра модбас
	adr = *(loc_ptrmas_input + 4);
	f = *(loc_ptrmas_input + 6);
	
	while(f--)
	{
		*(loc_ptrmas_output + global_count) = MB_input_reg[adr] >> 8;
		global_count++;
		*(loc_ptrmas_output + global_count) = MB_input_reg[adr];
		adr++;
		global_count++;
	}
	CRC_result();
}
/*=================================================================================================================================================*/
void Write_Multiple_Registers(volatile uint8_t *loc_ptrmas_input, volatile uint8_t *loc_ptrmas_output) //x10
{
	unsigned char adr = 0;
	unsigned char f = 0;
	unsigned char count = 0;
	
	//адрес регистра
	*(loc_ptrmas_output + 2) = *(loc_ptrmas_input + 3);
	global_count++;
	*(loc_ptrmas_output + 3) = *(loc_ptrmas_input + 4);
	global_count++;
	
	//кол-во регистров
	*(loc_ptrmas_output + 4) = *(loc_ptrmas_input + 5);
	global_count++;
	*(loc_ptrmas_output + 5) = *(loc_ptrmas_input + 6);
	global_count++;
	
	adr = *(loc_ptrmas_input + 4);
	//Кол-во байт
	count = *(loc_ptrmas_input + 6);
	f = 8;
	loc_ptrmas_input += 8;
	while(count--)
	{
		MB_input_reg[adr] = ( (uint16_t) *(loc_ptrmas_input)  << 8) | (uint8_t)*(loc_ptrmas_input + 1);
		loc_ptrmas_input += 2;
		adr++;
	}
	CRC_result();
}
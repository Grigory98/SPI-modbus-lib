void Read_Holding_Reg(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output);
void Read_Input_Registers(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output);
void Write_single_register(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output);
void Write_Multiple_Registers(volatile uint8_t *loc_ptrmas_input, volatile uint8_t *loc_ptrmas_output);
int crc_chk (unsigned char* data_loc, unsigned char length);
unsigned short crc16_update(unsigned short crc, unsigned char data_byte);
unsigned char calc_crc16 (unsigned char *tmp_buf, unsigned short len_tmp_buf);
unsigned short check_crc16 (unsigned char *tmp_buf, unsigned short len_tmp_buf);
extern void CRC_result();

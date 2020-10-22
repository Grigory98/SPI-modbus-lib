extern uint16_t MB_holding_reg[16];
extern uint16_t MB_input_reg[16];
extern volatile unsigned char Massiv_input[20]; //ввод(запрос)
extern volatile unsigned char Massiv_output[20]; //вывод(ответ)
unsigned int RCR;
void Timer_ini();
unsigned char adress;
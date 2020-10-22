/*------------------------------------- f = 8MHz --------------------------------------------*/
#define USART_SetRate_9600		UBRR0H = (unsigned char) (51>>8); UBRR0L = (unsigned char) 51;
#define USART_SetRate_19200		UBRR0H = (unsigned char) (25>>8); UBRR0L = (unsigned char) 25;
#define USART_SetRate_28800		UBRR0H = (unsigned char) (16>>8); UBRR0L = (unsigned char) 16;
#define USART_SetRate_38400		UBRR0H = (unsigned char) (12>>8); UBRR0L = (unsigned char) 12;
#define USART_SetRate_57600		UBRR0H = (unsigned char) (8>>8);  UBRR0L = (unsigned char) 8;
#define USART_SetRate_115200	UBRR0H = (unsigned char) (3>>8);  UBRR0L = (unsigned char) 3;
/*-------------------------------------------------------------------------------------------*/
volatile uint16_t timeRxInterrupt;
volatile unsigned char UART_len;
volatile unsigned char Massiv_input[20]; //ввод(запрос)
volatile unsigned char Massiv_output[20]; //вывод(ответ)
/*-------------------------------------------------------------------------------------------*/
void USART_init();
void USART_Transmit_data(unsigned char *data, unsigned char loc_len);
void usart_timeout();







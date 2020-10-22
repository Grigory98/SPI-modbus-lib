#define SS_BIT 2
#define MOSI_BIT 3
#define MISO_BIT 4
#define SCK_BIT 5
/*---------------------------------------------------------Скорости--------------------------------------------------------------------------*/
#define SPI_SetRate_del_2		SPCR |= (0<<SPR1)|(0<<SPR0); SPSR |= (1<<SPI2X); //f/2
#define SPI_SetRate_del_4		SPCR |= (0<<SPR1)|(0<<SPR0);					 //f/4
#define SPI_SetRate_del_8		SPCR |= (0<<SPR1)|(1<<SPR0); SPSR |= (1<<SPI2X); //f/8
#define SPI_SetRate_del_16		SPCR |= (0<<SPR1)|(1<<SPR0);					 //f/16
#define SPI_SetRate_del_32		SPCR |= (1<<SPR1)|(0<<SPR0); SPSR |= (1<<SPI2X); //f/32
#define SPI_SetRate_del_64		SPCR |= (1<<SPR1)|(0<<SPR0);					 //f/64
#define SPI_SetRate_del_128		SPCR |= (1<<SPR1)|(1<<SPR0);					 //f/128
/*---------------------------------------------------------Указатели-------------------------------------------------------------------------*/
uint8_t *ptr_mas_input;
uint8_t *ptr_mas_output;
uint8_t *ptr_masout_tmp;
/*---------------------------------------------------------Таймауты--------------------------------------------------------------------------*/
volatile uint16_t timeSpiInterrupt;
volatile uint16_t timeSpiError;
/*------------------------------------------------------Статусы/счетчики---------------------------------------------------------------------*/
volatile uint8_t spi_mode;
uint8_t global_count;
uint8_t	g_count_receive_SPI;
uint8_t	g_count_receive_SPI_tmp;
uint8_t	SPI_transmit_len;
/*---------------------------------------------------------Функции---------------------------------------------------------------------------*/
void SPI_MasterInit(void);
void spi_transmit(unsigned char *loc_ptrmas_inp, unsigned char *loc_ptrmas_out, unsigned char loc_uart_len);
void intbus_error_timeout(volatile unsigned char *loc_ptrmas_inp, volatile unsigned char *loc_ptrmas_out);
void spi_timeout(void);
void send_slaveAnswer_to_pc(unsigned char *loc_ptrmas_input, unsigned char *loc_ptrmas_output);
void send_masterAnswer_to_pc(unsigned char adr, volatile unsigned char *loc_ptrmas_input, volatile unsigned char *loc_ptrmas_output);
/*-------------------------------------------------------------------------------------------------------------------------------------------*/


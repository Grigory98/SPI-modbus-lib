#define SCK_BIT 5
#define MISO_BIT 4
#define MOSI_BIT 3
#define SS_BIT 2
/*================================================================*/
extern volatile uint16_t timeSpiInterrupt;
extern uint8_t *ptr_mas_input;
extern uint8_t *ptr_mas_output;
extern uint8_t spi_mode;
/*================================================================*/
void SPI_SlaveInit(void);
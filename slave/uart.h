void USART_ini( unsigned int speed) //инициализация USART
{
	UBRR0H = (unsigned char) (speed>>8);
	UBRR0L = (unsigned char) speed;
	UCSR0B = (0<<RXEN0)|(1<<TXEN0)|(0 << RXCIE0)|(0 << TXCIE0);
	UCSR0A = (0 << U2X0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit_data(unsigned char *data, unsigned char loc_len)   //передача с мк на пк. data - массив с байтами, len - длина массива
{
	while (loc_len--)
	{
		while (!(UCSR0A&(1<<UDRE0)));
		UDR0 = *data;
		data++;
	}
}


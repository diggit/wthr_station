#include "uart_lib.h"


// get char
unsigned char uart_getc( void )
{
	while ( !(UCSRA & (1<<RXC)) )
		;
	return UDR;
}

// send char
void uart_putc(char data)
{
	while ( !( UCSRA & (1<<UDRE)) )
		;
	UDR = data;
}

//send string, (BTW string is array of chars, if you don't know...)
void uart_puts(char *str)//accepts address on array beginning
{
	for(; *str != '\0'; str++)// while character on given address is not EQ \0, send it, increase address and iterate!
	{
		uart_putc(*str);//send character which is located on address called str here, *str gives valu (char) stored there
	}
}


void uart_init(void)
{
	
	//set prescaler
	UBRRL = (uint8_t)ubrr_val;
	UBRRH = (uint8_t)ubrr_val>>8;
	//enable RX nad TX
	UCSRB = (1<<TXEN) | (1<<RXEN);
	if(UA_RX_INT_EN)
	{
		UCSRB |= (1<<RXCIE); //enable RX interrupt
	}
	//set frame format 8data bits, 1 stop bit, no parity
	UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1); //but it is default...
	//URSEL MUST! by written when we really want to write into UCSRC and no into UBRRH (they are shared, URSEL selects in 1 UCSRC)

}

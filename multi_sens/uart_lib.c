#include "uart_lib.h"

#define __NOP asm volatile("NOP")

// get char
unsigned char uart_getc( void )
{
	while ( !(UCSRA & (1<<RXC)) )
		__NOP;
	return UDR;
}

// send char
void uart_putc(char data)
{
	while ( !( UCSRA & (1<<UDRE)) )
		__NOP;
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


void uart_num(int32_t num,uint8_t min)
{
	if(num<0)//sign
	{
		uart_putc('-');
		num=-num;
	}
	if(num>1000000000 || min>10)
		uart_putc('0'+num/1000000000);
	if(num>100000000 || min>9)
		uart_putc('0'+num/100000000%10);
	if(num>10000000 || min>8)
		uart_putc('0'+num/10000000%10);
	if(num>1000000 || min>7)
		uart_putc('0'+num/1000000%10);
	if(num>100000 || min>6)
		uart_putc('0'+num/100000%10);
	if(num>10000 || min>5)
		uart_putc('0'+num/10000%10);
	if(num>1000 || min>3)
		uart_putc('0'+num/1000%10);
	if(num>100 || min>2)
		uart_putc('0'+num/100%10);
	if(num>10 || min>1)
		uart_putc('0'+num/10%10);
	uart_putc('0'+num%10);
	//done	
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
void uart_flush()
{
	/*
	char dummy;
	while( UCSRA & (1<<RXC))
	{
		dummy=UDR;
		//dummy=dummy;
	}*/
	UCSRB &= ~(1<<RXEN);
	UCSRB |= (1<<RXEN);
	
}

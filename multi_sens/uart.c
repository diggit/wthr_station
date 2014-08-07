// Copyright (C) 2014  xorly

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#include "uart.h"


//functions available for receiving
#ifdef UART_RX
	unsigned char uart_getc( void )
	{
		while ( !(UCSRA & (1<<RXC)) )
			__NOP;
		return UDR;
	}

	//flush input buffer
	void uart_flush()
	{
		UCSRB &= ~(1<<RXEN);
		UCSRB |= (1<<RXEN);
	}
#endif

//functions available for transmitting
#ifdef UART_TX
	// send char
	void uart_putc(char data)
	{
		while ( !( UCSRA & (1<<UDRE)) )
			__NOP;
		UDR = data;
	}

	//send string, (BTW string is array of chars, if you don't know...)
	

	void uart_puts_endchar(char *str,char endchar)
	{
		for(; *str != '\0'; str++)// while character on given address is not EQ \0, send it, increase address and iterate!
		{
			uart_putc(*str);//send character which is located on address called str here, *str gives value (char) stored there
		}
		if(endchar)
			uart_putc(endchar);
	}

	void uart_peint(char *str)//accepts address on array beginning
	{
		uart_puts_endchar( str,0);
	}
	void uart_println(char *str)//accepts address on array beginning
	{
		uart_puts_endchar( str,'\n');
	}

#endif

void uart_init(void)
{
	
	//set prescaler
	UBRRL = (uint8_t)ubrr_val;
	UBRRH = (uint8_t)ubrr_val>>8;

	//enable RX nad TX, conditionaly
#if (defined UART_RX && defined UART_TX)
	UCSRB = (1<<TXEN) | (1<<RXEN);
#elif defined UART_TX
	UCSRB = (1<<TXEN);
#elif defined UART_RX
	UCSRB = (1<<RXEN);
	#ifdef UA_RX_INT_EN
		UCSRB |= (1<<RXCIE); //enable RX interrupt
	#endif
#endif

	//set frame format 8data bits, 1 stop bit, no parity
	UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1); //but it is default...
	//URSEL MUST! by written when we really want to write into UCSRC and no into UBRRH (they are shared, URSEL selects in 1 UCSRC)
}



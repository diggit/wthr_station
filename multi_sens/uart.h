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


#ifndef __UART_LIB  //not yet included
	#define __UART_LIB

	#include <avr/io.h>
	#include <inttypes.h>
	#include "config.h"

	#ifndef FOSC
	#	error No CPU frequency defined (FOSC)
	#endif

	#ifndef BAUD
	#	error UART baudrate not defined (BAUD) 
	#endif

	#define ubrr_val (FOSC/(16*BAUD)-1)

	#define __NOP __asm__ volatile("NOP")



	#if (defined UART_RX || defined UART_TX)
	#	ifdef UART_RX
			unsigned char uart_getc( void );
			void uart_flush(void);
	#		ifndef UA_RX_INT_EN
	#			define UA_RX_INT_EN 0
	#		endif
	#	endif

	#	ifdef UART_TX
	#		include "misc.h" //tools for number conversion to array of chars
			void uart_putc(char data );
			void uart_print(char *str);
			void uart_println(char *str);
			void uart_puts_endchar(char *str,char endchar);
	#		define uart_num(V,M) uart_print(itoa(V,M))
	#	endif
	#else
	#	warning "not RX nor TX enabled! Do not import or define at least one of: UART_RX, UART_RX."
	#endif
	
	void uart_init(void);

#endif

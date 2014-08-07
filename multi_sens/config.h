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


//main configuration

#ifndef _CONFIG
	#define _CONFIG
	#include <avr/io.h>

	//#define debug

	#define STRING_VERSION __DATE__ " " __TIME__ // build date and time

	#define FOSC 16000000UL //MCU crystal freq.
	#define F_CPU FOSC
	//UART-config--------------------------------------------------------------------------------------------------------------------------------
	#define BAUD 19200UL //speed of UART, for short distances could be increased, bud don't forget to change value in grabber script on server...
	#define UA_RX_INT_EN 1 //enable UART recieve INT
	#define UART_RX
	#define UART_TX

	//DHT11-config--------------------------------------------------------------------------------------------------------------------------------
	#define DHTP PORTB //PORT where DHT11 data pin is attached
	#define DHTD DDRB  //...it's configuration register (I/O selection)
	#define DHTI PINB  //register where to get actual status of input
	
	//I2C-config--------------------------------------------------------------------------------------------------------------------------------
	#define I2C_CLOCK  100000L
	
	//ADT-config--------------------------------------------------------------------------------------------------------------------------------	
	#define ADT_addr_base 0b1001000
	#define ADT_samples 5
	#define ADT_pause	500 //ms

#endif
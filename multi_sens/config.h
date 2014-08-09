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
	#define UART_RX_INT //enable UART recieve INT
	#define UART_RX
	#define UART_TX

	//DHT11-config--------------------------------------------------------------------------------------------------------------------------------
	#define DHTP PORTB //PORT where DHT11 data pin is attached
	#define DHTD DDRB  //...it's configuration register (I/O selection)
	#define DHTI PINB  //register where to get actual status of input
	#define DHT_sensor_pin 2
	#define DHT_retry_count 5
	
	//I2C-config--------------------------------------------------------------------------------------------------------------------------------
	#define I2C_CLOCK  100000L
	
	//ADT-config--------------------------------------------------------------------------------------------------------------------------------	
	#define ADT_sensor_count 3
	#define ADT_minimum_working 3
	#define ADT_addr_base 0b01001000 //aligned to left
	#define ADT_samples 5
	#define ADT_pause	500 //ms

	//BMP085-config-------------------------------------------------------------------------------------------------------------------------------
	//addresses of BMP085 i2c sensor
	#define BMP_ADDR 0b01110111
	
	//resolution of BMP085 pressure measurement
	#define BMP_resolution 3 //0 fastest - worst, 3 slowest - best
	#define BMP_max_pressure_noise 10 //pressure treshold
	#define BMP_max_temperature_noise 5 //temperature treshold
	#define BMP_samples 3 //samples count which value shouldn't be too far from it's avg. value eg AVG:200 val1:201 val2:198 val3:203 -worst distance is 3 (=|200-203|), values above are tresholds 
	#define BMP_max_tries 5 //if measured values were too far from their AVG value, try it until OK or max_tries is reached
	#define BMP_int_reg PINC
	#define BMP_int_pin	1
	#define BMP_pause 50 //millisecond pause between samples
	//BMP085-config-------------------------------------------------------------------------------------------------------------------------------

#endif
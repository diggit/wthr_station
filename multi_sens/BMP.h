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

#ifndef _BMP_LIB
	#define _BMP_LIB
	
	#include <inttypes.h>
	#include "i2cmaster.h"
	#include "misc.h"
	#include "uart.h"
	#include "calibrated_loop.h"
	#include "config.h"
	#include "state.h"
	
	//waiting for signal that indicates End Of Conversion on pin PB0
	#define WFC() while( !(bit_get(BMP_int_reg,BIT(BMP_int_pin))) )NOP; //Wait For Conversion
	
	void		BMP_get_calibration();//downloads Calibration Values from sensor
	uint32_t	BMP_read_bytes(uint8_t reg_addr, uint8_t bytes);//reads 'bytes' bytes (up to 4) from address defined by RA/WA beginning on given register address 
	int32_t		BMP_read_temperature();//measures raw value, converts it into human readable value and returns it
	long		BMP_read_pressure();//similar to BMP_calc_temp, but works with pressure
	uint8_t 	BMP_check();
	uint8_t		BMP_measure(uint32_t *pressure, int16_t *temperature);
#endif
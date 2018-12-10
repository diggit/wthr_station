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

#ifndef _STATE
	#define _STATE
	
	#include <inttypes.h>
	
	//TODO: creater err.h and move there
	//error codes definitions

	struct errors
	{
		uint8_t general:1;
		uint8_t i2c:1;
		uint8_t DHT:1;
		uint8_t ADT:1;
		uint8_t	BMP:1;
		uint8_t	BH:1;
		uint8_t timeout:1;
	};

	extern struct errors error_flags;

#endif
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

//library for communication with DHT sensors
#ifndef _DHT
	#define _DHT
	
	
	#include <inttypes.h>
	#include "misc.h"
	#include "calibrated_loop.h"
	#include "config.h"

	#ifndef DHTP
		#error no DHT output register defined (DHTP)
	#endif

	#ifndef DHTI
		#error no DHT input register defined (DHTI)
	#endif
	
	#ifndef DHTD
		#error no DHT direction register defined (DHTD)
	#endif

	extern uint8_t DHT_response[5];

	#define DHT_START(b) bit_set(DHTD,b); delay_us(1000*30); bit_clr(DHTD,b); delay_us(4*10)//; bit_clr(DHTD,b)//; bit_clr(DHTP,b) //shouldn't be DHTP (pull-up) bit 2 cleared too?

	void DHT(uint8_t bit);

	
#endif
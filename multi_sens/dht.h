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
	
	#ifdef debug
	#include "uart.h"
	#endif
	
	#ifndef DHTP
		#error no DHT output register defined (DHTP)
	#endif

	#ifndef DHTI
		#error no DHT input register defined (DHTI)
	#endif
	
	#ifndef DHTD
		#error no DHT direction register defined (DHTD)
	#endif

	#ifndef DHT_sensor_pin
		#error DHT_sensor_pin not define, where is sensor connected?
	#endif

	#ifndef DHT_retry_count
		#error DHT_retry_count not define, how many times try to get valid data?
	#endif

	extern uint8_t DHT_response[4];

	void DHT_start(uint8_t bit);
	uint8_t DHT(uint8_t bit);

	
#endif
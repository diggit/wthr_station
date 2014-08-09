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
 #ifndef _ADT
 	#define _ADT
	
	#include "config.h"
	#include <inttypes.h>
	#include "i2cmaster.h"
	#include "calibrated_loop.h"
	#include "state.h"
	//#include "misc.h"
	
	extern uint8_t ADT_status;

	#ifndef ADT_sensor_count
		#error ADT_sensor_count not defined
	#endif

	#ifndef ADT_addr_base
		#error ADT_addr_base not define (address with all address pins grounded)
	#endif

	#ifndef ADT_samples
		#error ADT_samples not defined, how many times sample every sensor?
	#endif

	#ifndef ADT_pause
		#error ADT_pause not defined, how long delay between sampling?
	#endif

	#ifndef ADT_minimum_working
		#error ADT_minimum_working not defined, how many sensors are needed to realiable rowking?
	#else
		#if ADT_minimum_working > ADT_sensor_count
			#error ADT_minimum_working is greater than ADT_sensor_count, nonsense huh?
		#endif
	#endif

	extern uint8_t ADT_status;

	void	ADT_wake();
	void	ADT_shutdown();
	uint8_t ADT_measure(int16_t *storage, uint8_t samples,uint16_t sample_pause_ms);
	uint8_t ADT_get_working_count();
	uint8_t ADT_check();
 #endif
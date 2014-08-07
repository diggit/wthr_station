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

	uint8_t	ADT_config();
	void	ADT_wake();
	void	ADT_shutdown();
	int32_t ADT_measure(uint8_t samples,uint16_t sample_pause_ms)	;
 #endif
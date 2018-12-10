// Copyright (C) 2018  xorly

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


/* Patrik Bachan
 * few routines for operations with ambient light sensor
 * R/W^inv
 */

#include "BH1750.h"

uint8_t BH_cmd(uint8_t cmd)
{
	if(i2c_start(BH1750_ADDR,I2C_WRITE))
	{
		i2c_stop();
		return 1;
	}
	uint8_t err = 0;
	err += i2c_write(cmd);
	i2c_stop();

	return err;
}

uint8_t BH_measure(void)
{
	if(BH_cmd(BH_CMD_POWER_ON))
		return 1;
	
	if(BH_cmd(BH_CMD_SINGLE_HRES_MODE_2))
		return 1;
	
	return 0;
}

uint8_t BH_read(uint16_t *light_value)
{
	if(i2c_start(BH1750_ADDR, I2C_READ))
	{
		i2c_stop();
		return 1;
	}
	
	union
	{
		uint16_t u16;
		uint8_t u8[2];
	}buffer;

	buffer.u8[1]=i2c_read(1);
	buffer.u8[0]=i2c_read(0);
	i2c_stop();

	*light_value = buffer.u16;
	return 0;
}
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


/* Patrik Bachan
 * few routines for operations with temperature sensor board
 * R/W^inv
 * Sensors are (up to 3) AD7410 - 16bit i2c tempterature sensor from Analog Devices (free samples FTW!)
 */

#include "ADT.h"
#include "calibrated_loop.h"
#include "inttypes.h"
#include "misc.h"
#include "config.h"

const uint8_t TADDR[]={((ADT_addr_base|0)<<1) , ((ADT_addr_base|1)<<1) , ((ADT_addr_base|2)<<1)}; // 3 sensors

uint8_t ADT_config()
{
	uint8_t i;
	for(i=0;i<len(TADDR);i++)
	{
		#ifdef debug
			uart_print("cfg ADT no:");
			uart_num(i,1);
		#endif
		if(i2c_start(TADDR[i]))
			return (i+1);//return number of not responding sensor
		i2c_write(0x03);//config reg
		i2c_write(0b10000000);//set into high res. mode and shutdown
		i2c_stop();
	}
	return 0;
}

void ADT_wake()
{
	uint8_t i;
	//power on sensors
	for(i=0;i<len(TADDR);i++)
	{
		i2c_start(TADDR[i]);
		i2c_write(0x03);//config reg
		i2c_write(0b10000000);
		i2c_stop();
	}
	
}

void ADT_shutdown()
{
	uint8_t i;
	//shutdown
	for(i=0;i<len(TADDR);i++)
	{
		i2c_start(TADDR[i]);
		i2c_write(0x03);//config reg
		i2c_write(0b11100000);//start measurement
		i2c_stop();
	}
}


int32_t ADT_measure(uint8_t samples,uint16_t sample_pause_ms)
{
	int32_t output;
	int32_t temp=0;
	
	uint8_t i,j;
	int8_t Htemp;
	int16_t Ltemp;

	
	//delay fow proper wakeup
	
	delay_us(300000);//decrease?
	
	for(j=0;samples>j;j++)//take N samples
	{
	
		
		for(i=0;i<len(TADDR);i++)
		{
			i2c_start(TADDR[i]|1);//start reading
			Htemp=i2c_readAck();//recieve degrees
			Ltemp=i2c_readNak();//recieve decimals (hexadecimals correctly), align them on lower bits
			i2c_stop();
			temp+=(Htemp<<8)+Ltemp;
#ifdef debug
			uart_num((Htemp<<8)+Ltemp,4);
			uart_putc('\n');
#endif

		}
		delay_us((uint32_t)1000*sample_pause_ms);
	}
	
	

	output=temp/(samples*len(TADDR));//calculate average value (still not converted)
	//there is now sth. like 3089 (24.1°C)

	output*=10;//without this, we will lose decimal information in next step
	output/=128;//convert into value
	//now it is 241
	
	return output;
}

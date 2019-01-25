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


uint8_t ADT_status=0xff;
//BIT 0 ~ ADT0 status
//BIT 1 ~ ADT1 status
//...
//1 ~ ERROR, NOT WORKING
//0 ~ OK, WORKING

void ADT_wake()
{
	uint8_t i;
	//power on sensors
	for(i=0;i<ADT_sensor_count;i++)
	{
		#ifdef debug
		uart_print("ADT wake:");
		uart_num(i,1);
		uart_nl();
		#endif		
		if(bit_get(ADT_status,BIT(i)))
		{
			#ifdef debug
			uart_println("SKIP");
			#endif		
			continue;//jump to next sensor, this one is broken
		}

		i2c_start(ADT_addr_base|i,I2C_WRITE);
		i2c_write(0x03);//config reg
		i2c_write(0b10000000);
		i2c_stop();
	}
	
}

void ADT_shutdown()
{
	uint8_t i;
	//shutdown
	for(i=0;i<ADT_sensor_count;i++)
	{
		#ifdef debug
		uart_print("ADT shutdown:");
		uart_num(i,1);
		uart_nl();
		#endif		
		if(bit_get(ADT_status,BIT(i)))
		{
			#ifdef debug
			uart_println("SKIP");
			#endif
			continue;
		}

		i2c_start(ADT_addr_base|i,I2C_WRITE);
		i2c_write(0x03);//config reg
		i2c_write(0b11100000);//7: 16bit res, 6~5: shutdown mode
		i2c_stop();
	}
		#ifdef debug
		uart_println("ADTs off");
		#endif

}


uint8_t ADT_measure(int16_t *storage, uint8_t samples,uint16_t sample_pause_ms)
{
	ADT_wake();

	int32_t temp=0;
	
	uint8_t sensor_num,sample_num;
	
	union
	{
		int16_t value;
		struct{
			uint8_t H;
			uint8_t L;
		}bytes;
	}sample;
	
	//delay fow proper wakeup
	
	delay_ms(300);//because 1st conversion after wake is fast and inaccurate,
	//nomal conversion tooks 240ms
	
	for(sample_num=0;sample_num<samples;sample_num++)//take N samples
	{
	
		
		for(sensor_num=0;sensor_num<ADT_sensor_count;sensor_num++)
		{

			#ifdef debug
			uart_print("ADT measure:");
			uart_num(sensor_num,1);
			uart_nl();
			#endif	

			if(!bit_get(ADT_status,BIT(sensor_num)))
			{
				if(i2c_start(ADT_addr_base|sensor_num,I2C_READ))//start reading
					return 1;
				sample.bytes.H=i2c_readAck();//recieve degrees
				sample.bytes.L=i2c_readNak();//recieve decimals (hexadecimals correctly), align them on lower bits
				i2c_stop();

				temp+=sample.value;
				
				#ifdef debug
				uart_num(sample.value,4);
				uart_putc('\n');
				#endif
			}
			#ifdef debug
			else
				uart_println("SKIP");
			#endif

		}
		delay_ms(sample_pause_ms); //wait for another conversion, tooks 240ms at minimum
	}
	
	

	temp=temp/(samples*ADT_get_working_count());//calculate average value (still not converted)
	//there is now sth. like 3089 (24.1°C)

	temp*=10;//without this, we will lose decimal information in next step
	temp/=128;//convert into value
	//now it is 241

	ADT_shutdown();
	
	*storage=temp;

	//all measurements done
	
	return 0;
}

//return count of working sensors
uint8_t ADT_get_working_count()
{
	uint8_t sum=0;
	uint8_t index;
	for(index=0;index<ADT_sensor_count;index++)
	{		
		if(!bit_get(ADT_status,BIT(index)))
		{
			sum++;
		}
	}

	return sum;
}

uint8_t ADT_check()
{
	uint8_t i;
	for(i=0;i<ADT_sensor_count;i++)
	{
		#ifdef debug
		uart_print("ADT chk: ");
		uart_num(i,1);
		#endif	
		if(i2c_start(ADT_addr_base|i,I2C_READ))
		{
			bit_set(ADT_status,BIT(i));//set Ith bit as sign of sensor error
			#ifdef debug
				uart_println(" FAIL");
			#endif
		}
		else
		{
			i2c_read(0);
			bit_clr(ADT_status,BIT(i));//set as working
			#ifdef debug
			uart_println(" OK");
			#endif
		}
		i2c_stop();
		
	}

	if(ADT_minimum_working > ADT_get_working_count())
	{
		error_flags.ADT=1;
		return 1;
	}
	else
	{	
		error_flags.ADT=0;
		return 0;
	}
}

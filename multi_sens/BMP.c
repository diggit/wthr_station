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

#include "BMP.h"

struct { //downloaded calibration values
	int16_t ac1;
	int16_t ac2; 
	int16_t ac3; 
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1; 
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
}CV;

struct{//calibration values calculated
	uint32_t b7;
	int32_t b6;
	int32_t b5;
	uint32_t b4;
	int32_t b3;
} CVC;


void BMP_get_calibration()//downloads Calibration Values from sensor
{
	CV.ac1 = BMP_read_bytes(0xAA,2);
	CV.ac2 = BMP_read_bytes(0xAC,2);
	CV.ac3 = BMP_read_bytes(0xAE,2);
	CV.ac4 = BMP_read_bytes(0xB0,2);
	CV.ac5 = BMP_read_bytes(0xB2,2);
	CV.ac6 = BMP_read_bytes(0xB4,2);
	CV.b1 = BMP_read_bytes(0xB6,2);
	CV.b2 = BMP_read_bytes(0xB8,2);
	CV.mb = BMP_read_bytes(0xBA,2);
	CV.mc = BMP_read_bytes(0xBC,2);
	CV.md = BMP_read_bytes(0xBE,2);
}

uint32_t BMP_read_bytes(uint8_t reg_addr, uint8_t bytes)//reads 'bytes' bytes (up to 4) from address defined by RA/WA beginning on given register address 
{
	uint8_t raw[]={0,0,0,0};//maximal 4 bytes to read
	uint32_t sum=0;
	uint8_t i;//some index var.
	if(bytes>4){bytes=4;} //higher vals. shrink to 4
	bytes--;//now, our values are 0-3, better suits here
	if(i2c_start(BMP_ADDR,I2C_WRITE))//we wanna say what to read, that is writing...
	{
		error_flags.BMP=1;
	}
	i2c_write(reg_addr);//read from given addr
	if(i2c_rep_start(BMP_ADDR,I2C_READ))//restart i2c comm. for reading now
	{
		error_flags.BMP=1;
	}
	
	for(i=0;i<bytes;i++)
	{
		raw[i]=i2c_read(1);
	}
	raw[i]=i2c_read(0);//this is out last byte
	i2c_stop();
	
	for(i=0;i<=bytes;i++)//merge all recieved bytes into single 4byte datatype
	{
		sum|=((uint32_t)raw[i])<<(8*(bytes-i));
	}
	return sum;
}


int32_t BMP_read_temperature()//measures raw value, converts it into human readable value and returns it
{
	uint8_t err=0;//err test, only for debug purposes, won't be used
	int32_t X1,X2;//buffers for translate vals into human readable
	
	//proceed temp mesurement 
	if(i2c_start(BMP_ADDR,I2C_WRITE))//we wanna say what to read, that is writing...
	{
		error_flags.BMP=1;
	}
	//uart_print("chat opened\n");
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x2E);//and initiate temp mesurement
	i2c_stop();
	
	delay_ms(20);
	WFC();//wait for EOC
	delay_ms(80);//maybe can prevent of reading random O temp.
	
	//read raw temp value
	uint16_t UT=(uint16_t)BMP_read_bytes(0xF6,2);
	
	//converting, this magic formula was copied from sensors datasheet
	X1=(((int32_t)UT-(int32_t)CV.ac6)*(int32_t)CV.ac5)>>15;
	X2=((int32_t)CV.mc<<11)/(X1+(int32_t)CV.md);
	CVC.b5=X1+X2;
	return((CVC.b5+8)>>4);
}

long BMP_read_pressure()//similar to BMP_calc_temp, but works with pressure
{
	uint8_t err=0;
	int32_t X1,X2,X3,p;
	uint32_t raw_p;
	
	if(i2c_start(BMP_ADDR,I2C_WRITE))//we wanna say what to read, that is writing...
	{
		error_flags.BMP=1;
	}
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x34+(BMP_resolution<<6));//and measure pressure
	i2c_stop();
	
	WFC();//wait for EOC
	
	raw_p=(BMP_read_bytes(0xF6,3))>>(8-BMP_resolution);

	//main calculation, formula also copied from sensors datasheet
	CVC.b6=CVC.b5-4000; //this affects temp. dependency
	X1=(CV.b2 * ((CVC.b6*CVC.b6)>>12))>>16;
	X2=(CV.ac2 * CVC.b6)>>11;
	X3=X1 + X2;
	CVC.b3=(((((uint32_t) CV.ac1) * 4 +X3)<<BMP_resolution) + 2)>>2;
	
	X1=(CV.ac3 * CVC.b6)>>13;
	X2=(CV.b1 * ((CVC.b6*CVC.b6)>>12))>>16;
	X3=((X1 + X2) + 2)>>2;
	CVC.b4=(CV.ac4 * (uint32_t)(X3 + 32768))>>15;
	
	CVC.b7=((uint32_t)(raw_p - CVC.b3) * (50000>>BMP_resolution));
	if (CVC.b7 < 0x80000000)	{p=(CVC.b7<<1) / CVC.b4;}
	else						{p=(CVC.b7 / CVC.b4)<<1;}
	
	X1=(p>>8)*(p>>8);
	X1=(X1 * 3038)>>16;
	X2=(-7357 * p)>>16;
	p+=(X1 + X2 + 3791)>>4;//original
	//final value must be calculated, it is now done in python script, look into BMP085 datasheet for more info...
	return p;
}

uint8_t BMP_check()
{
	if(i2c_start(BMP_ADDR,I2C_READ))
		return 1;
	i2c_read(0);
	i2c_stop();
	return 0;
}

uint8_t BMP_measure(uint32_t *pressure, int16_t *temperature)
{
	uint8_t sample_number,try_number;
	uint8_t valid_data=0;
	int16_t Tbuf [BMP_samples];
	int32_t Pbuf [BMP_samples];
	int32_t bmp_pressure_avg=0;
	int16_t bmp_temperature_avg=0;

	//BMP-------------------------		
		for(try_number=0;try_number<BMP_max_tries && !(valid_data);try_number++)
		{
			#ifdef debug
			uart_print("try nuber: ");
			uart_num(try_number,1);
			#endif
			//single try
			
			bmp_temperature_avg=0;
			bmp_pressure_avg=0;
			
			//gather all data Ntimes sampled
			for(sample_number=0;sample_number<BMP_samples;sample_number++)//load "BMP_samples" samples
			{
				Tbuf[sample_number]=BMP_read_temperature();//store measured data
				bmp_temperature_avg+=Tbuf[sample_number];//add to average
				
				Pbuf[sample_number]=BMP_read_pressure();
				bmp_pressure_avg+=Pbuf[sample_number];
				delay_ms(BMP_pause);
			}
			bmp_temperature_avg/=BMP_samples;//calculate average value (for comparison)
			bmp_pressure_avg/=BMP_samples;
			
			//check values
			valid_data=1;
			//discard whole measurement in case, some of N samples,
			//is too divergent from average value (too high noise)
			for(sample_number=0;sample_number<BMP_samples;sample_number++)
			{
				
				if( uabs(bmp_temperature_avg-Tbuf[sample_number])>BMP_max_temperature_noise)
				{
					valid_data=0;
					#ifdef debug					
					uart_print("BMP temp too noisy: ");
					uart_num(Tbuf[sample_number],1);
					uart_print("\n avg: ");
					uart_num(bmp_temperature_avg,1);
					uart_print("\ndivergence:");
					uart_num(uabs(Tbuf[sample_number]-bmp_temperature_avg),1);
					uart_nl();
					#endif					
				}
				
				if( uabs(bmp_pressure_avg-Pbuf[sample_number])>BMP_max_pressure_noise)//if out of tollerance
				{
					valid_data=0;
					#ifdef debug					
					uart_print("BMP press too noisy: ");
					uart_num(Pbuf[sample_number],1);
					uart_print("\n avg: ");
					uart_num(bmp_pressure_avg,1);
					uart_print("\ndivergence:");
					uart_num(uabs(Pbuf[sample_number]-bmp_pressure_avg),1);
					uart_nl();
					#endif					
				}
			}
		}
		#ifdef debug
		uart_println("BMP085 - done");
		#endif
		
		if(valid_data)
		{
			*temperature=bmp_temperature_avg;
			*pressure=bmp_pressure_avg;
			return 0;
		}
		else
		{
			*temperature=0;//will cause reset below
			*pressure=0;
			return 1;
		}
		
		// if(temp<-500 || temp>500 || press<90000 || press>150000)//awww somethi'n screwed up, reset is most effective
		// {
		// 	uart_println(" wrong value returned, resetting... ");
		// 	error_flags.BMP=1;

		// }
		
}
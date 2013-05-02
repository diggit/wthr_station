/* Patrik Bachan
 * few routines for operations with temperature sensor board
 * R/W^inv
 * Sensors are (up to 3) AD7410 - 16bit i2c tempterature sensor from Analog Devices (free samples FTW!)
 */

#include "calibrated_loop.c"

#define len(x)  (sizeof(x) / sizeof(x[0]))

#define addr_base 0b1001000
const uint8_t TADDR[]={((addr_base|0)<<1) , ((addr_base|1)<<1) , ((addr_base|2)<<1)}; // 3 sensors

int ADTconfig()
{
	uint8_t i;
	for(i=0;i<len(TADDR);i++)
	{
		if(i2c_start(TADDR[i]))
			return (i+1);//return number of not responding sensor
		i2c_write(0x03);//config reg
		i2c_write(0b10000000);//set into high res. mode and shutdown
		i2c_stop();
	}
	return 0;
}

void ADTwake()
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

void ADTshutdown()
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


int32_t ADTmeasure(uint8_t samples,uint16_t sample_pause_ms)
{
	int32_t output;
	int16_t temp=0;
	
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

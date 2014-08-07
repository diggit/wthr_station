/***************************************************************************
 * Desc: Program for controller Atmega8, it will be placed near sensors,
 * reads raw values, converts them and on masters request, sends them back over UART
 * USED sensors are: temperature and pressure digital i2c sensor BMP085
 * humidity digital sensor DHT11
 * 
 * Author: Patrik Bachan
 * Contact:patrikbachan@gmail.com (email), diggit@jabbim.cz (jabber)
 * 
 * License: GNU GPL v3
 ***************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "i2cmaster.h"
#include "uart.h"//useful UART functions, init, send, receive and so on
#include "calibrated_loop.h"
#include "misc.h"
#include "config.h"
#include "dht.h"
#include "ADT.h"//library for extended temperature board



//TODO: separate DHT to library
//BMP085-config-------------------------------------------------------------------------------------------------------------------------------
//addresses of BMP085 i2c sensor
#define BMP_ADDR 0b11101110
#define READ	 1 //lowest bit in addr selects READ/write
//waiting for signal that indicates End Of Conversion on pin PB0
#define WFC() while( !(PINC & 2) )NOP; //Wait For Conversion
//resolution of BMP085 pressure measurement
#define resolution 3 //0 fastest - worst, 3 slowest - best
#define pressure_noise 10 //pressure treshold
#define temperature_noise 5 //temperature treshold
#define matching_min 3 //samples count which value shouldn't be too far from it's avg. value eg AVG:200 val1:201 val2:198 val3:203 -worst distance is 3 (=|200-203|), values above are tresholds 
#define max_tries 5 //if measured values were too far from their AVG value, try it until OK or max_tries is reached
//BMP085-config-------------------------------------------------------------------------------------------------------------------------------




//TODO: creater err.h and move there
//error codes definitions
#define I2C_BMP_start_err	1
#define I2C_EXT_start_err	2
#define DHT_err				3

//TODO: move to header file
int32_t BMP_calc_temp();
void BMP_get_CalibV();
long BMP_calc_pressure();
uint32_t BMP_read_NB(uint8_t address, uint8_t bytes);
void DHT(uint8_t bit);
void handleERROR(uint8_t ERROR_code);


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



int main (void)
{
	uart_init();//init UART
	uart_print("booted...\ncompiled: ");
	uart_print(STRING_VERSION);
	uart_putc('\n');
	i2c_init();//bring up I2C
	#ifdef debug
	uart_println("i2c bus ready...");
	#endif
	
	BMP_get_CalibV();//download callibration values from BMP085
	#ifdef debug
	uart_println("BMP085 i2c comm. success...");
	#endif
	BMP_calc_temp();//initial reading
	BMP_calc_pressure();//same here

	#ifdef debug
		int ret;
		ret=ADT_config();
		if(ret)
		{
			uart_print("ADT init fail, sns: ");
			uart_num(ret,1);
			uart_putc('\n');
		}
		else
			uart_println("ADT ini OK");
	#else
		if(ADT_config())
			handleERROR(I2C_EXT_start_err);
	#endif

	ADT_shutdown();//put sensors into shutdown mode (low self heating)

	#ifdef debug
	uart_println("ADTs are off NOW");
	#endif
	
	set_sleep_mode(SLEEP_MODE_IDLE); //choose most suitable sleep mode - IDLE (still running UART)
	sleep_enable();//enable sleeping
	
	
	sei(); //enable interrupt, mainly for UART RX

	#ifdef debug
	uart_println("entering infinite sleep loop...");
	#endif
	while(1)//sleep it after interrupt again and again
	{
	#ifdef debug
		uart_println("sleeping");//debug
	#endif
		sleep_enable();
		sleep_cpu();//MCU is awaken on every interrupt, this will sleep it back again...
	}	
	
	return 0;
}



ISR(USART_RXC_vect)//return measured values on request, otherwivise, node will be in idle mode, (maybe sleeping - should be implemented)
{
	cli();
	//TODO: rename/clarifi/reduce variables
	uint8_t cnt,cnt2;
	uint8_t OK=0;
	char reply=UDR; //latch incomming data byte
	int16_t temp=0;
	int16_t Tavg;
	int16_t Tbuf [matching_min];
	
	int32_t tempEX=0;
	
	uint32_t press=0;
	uint32_t Pavg;
	uint32_t Pbuf [matching_min];
	if(reply=='Q')//send data as response of Query
	{
		
		
		for(cnt2=0;cnt2<max_tries && !(OK);cnt2++)
		{
#ifdef debug
			uart_print("try nuber: ");
			uart_num(cnt2,1);
#endif
			//single try
			OK=1;
			Tavg=0;
			Pavg=0;
			for(cnt=0;cnt<matching_min;cnt++)//load "matching_min" samples
			{
				Tbuf[cnt]=BMP_calc_temp();//store measured data
				Tavg+=Tbuf[cnt];//add to average
				
				Pbuf[cnt]=BMP_calc_pressure();
				Pavg+=Pbuf[cnt];
			}
			Tavg/=matching_min;//calculate average value (for comparison)
			Pavg/=matching_min;
			
			//check values
			for(cnt=0;cnt<matching_min;cnt++)
			{
				
				if( uabs(Tavg-Tbuf[cnt])>temperature_noise)
				{
					OK=0;
#ifdef debug					
					uart_print("Temperature from BMP085 is out of tollerance: ");
					uart_num(Tbuf[cnt],1);
					uart_print("\n avg: ");
					uart_num(Tavg,1);
#endif					
					break;
				}
				
				if( uabs(Pavg-Pbuf[cnt])>pressure_noise)//if out of tollerance
				{
					OK=0;
#ifdef debug					
					uart_print("Pressure from BMP085 is out of tollerance, value: \n");
					uart_num(Pbuf[cnt],1);
					uart_putc('\n');
					uart_num(uabs(Pbuf[cnt]-Pavg),1);
					uart_putc('\n');
					uart_num(uabs(Pavg-Pbuf[cnt]),1);
					uart_print("\n avg: ");
					uart_num(Pavg,1);
					uart_putc('\n');
#endif					
					break;
				}
			}
		}
#ifdef debug
			uart_println("BMP085 - done");
#endif
		
		if(OK)
		{
			temp=Tavg;
			press=Pavg;
		}
		else
		{
			temp=0;//will cause reset below
			press=0;
		}
		
		if(temp<-500 || temp>500 || press<90000 || press>150000)//awww somethi'n screwed up, reset is most effective
		{
			uart_println(" wrong value returned, resetting... ");
			Reset_AVR();

		}
		
		//power on sensors on EXT board
		ADT_wake();
#ifdef debug
			uart_println("ADTs on");
#endif
		tempEX=ADT_measure(ADT_samples,ADT_pause);//get value from EX board
		ADT_shutdown();
#ifdef debug
			uart_println("ADTs off");
#endif
		
		uart_print("!,MULTISNS1,T0,");//some ID
		uart_num((int32_t)tempEX,3);//temp from ex
		
		uart_print(",T1,");
		uart_num(temp,1);//temp from BMP085
		
		uart_print(",P0,");//pressure from BMP085
		uart_num(press,1);
		uart_print(",H0,");//himudity from DHT11
		DHT(2);//start measurement
		uart_num(DHT_response[0],1);
		uart_print(",$\n");
		
	}
	else if (reply=='R')
	{
		uart_println("Resetting...");
		Reset_AVR();
	}
	else uart_println("send Q to get data or R for reset");
	delay_us(500000);//DHT needs some refresh, just for case of too often reading
	uart_flush();/*something weird, if Q was incomming without pause, interrupt occured one by one. 
	* No MCU sleep was done. 24th reading from sensors returned wrong values and next would block MCU. 
	* Solution was total MCU Reset (see above). So, multiple requests (too short pause between them) are ignored for now...*/
	sei();
}

void BMP_get_CalibV()//downloads Calibration Values from sensor
{
	CV.ac1 = BMP_read_NB(0xAA,2);
	CV.ac2 = BMP_read_NB(0xAC,2);
	CV.ac3 = BMP_read_NB(0xAE,2);
	CV.ac4 = BMP_read_NB(0xB0,2);
	CV.ac5 = BMP_read_NB(0xB2,2);
	CV.ac6 = BMP_read_NB(0xB4,2);
	CV.b1 = BMP_read_NB(0xB6,2);
	CV.b2 = BMP_read_NB(0xB8,2);
	CV.mb = BMP_read_NB(0xBA,2);
	CV.mc = BMP_read_NB(0xBC,2);
	CV.md = BMP_read_NB(0xBE,2);
}

uint32_t BMP_read_NB(uint8_t reg_addr, uint8_t bytes)//reads 'bytes' bytes (up to 4) from address defined by RA/WA beginning on given register address 
{
	uint8_t raw[]={0,0,0,0};//maximal 4 bytes to read
	uint32_t sum=0;
	uint8_t i;//some index var.
	if(bytes>4){bytes=4;} //higher vals. shrink to 4
	bytes--;//now, our values are 0-3, better suits here
#ifdef debug
	uart_println("BMP:dialing");
#endif	
	if(i2c_start(BMP_ADDR))//we wanna say what to read, that is writing...
	{
		handleERROR(I2C_BMP_start_err);
	}
#ifdef debug
	uart_println("BMP:reg. choose");
#endif
	i2c_write(reg_addr);//read from given addr
#ifdef debug
	uart_println("BMP:reading");
#endif
	if(i2c_rep_start(BMP_ADDR|1))//restart i2c comm. for reading now
	{
		handleERROR(I2C_BMP_start_err);
	}
	
	for(i=0;i<bytes;i++)
	{
		raw[i]=i2c_read(1);
	}
	raw[i]=i2c_read(0);//this is out last byte
	i2c_stop();//end i2c communication
	
	for(i=0;i<=bytes;i++)//merge all recieved data into single value
	{
		sum|=((uint32_t)raw[i])<<(8*(bytes-i));
	}
	return sum;
}


int32_t BMP_calc_temp()//measures raw value, converts it into human readable value and returns it
{
	uint8_t err=0;//err test, only for debug purposes, won't be used
	int32_t X1,X2;//buffers for translate vals into human readable
	
	//proceed temp mesurement 
	if(i2c_start(BMP_ADDR))//we wanna say what to read, that is writing...
	{
		handleERROR(I2C_BMP_start_err);
	}
	//uart_print("chat opened\n");
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x2E);//and initiate temp mesurement
	i2c_stop();
	
	delay_us(20000);
	WFC();//wait for EOC
	delay_us(80000);//maybe can prevent of reading random O temp.
	
	//read raw temp value
	uint16_t UT=(uint16_t)BMP_read_NB(0xF6,2);
	
	//converting, this magic formula was copied from sensors datasheet
	X1=(((int32_t)UT-(int32_t)CV.ac6)*(int32_t)CV.ac5)>>15;
	X2=((int32_t)CV.mc<<11)/(X1+(int32_t)CV.md);
	CVC.b5=X1+X2;
	return((CVC.b5+8)>>4);
}

long BMP_calc_pressure()//similar to BMP_calc_temp, but works with pressure
{
	uint8_t err=0;
	int32_t X1,X2,X3,p;
	uint32_t raw_p;
	
	if(i2c_start(BMP_ADDR))//we wanna say what to read, that is writing...
	{
		handleERROR(I2C_BMP_start_err);
	}
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x34+(resolution<<6));//and measure pressure
	i2c_stop();
	
	WFC();//wait for EOC
	
	raw_p=(BMP_read_NB(0xF6,3))>>(8-resolution);

	//main calculation, formula also copied from sensors datasheet
	CVC.b6=CVC.b5-4000; //this affects temp. dependency
	X1=(CV.b2 * ((CVC.b6*CVC.b6)>>12))>>16;
	X2=(CV.ac2 * CVC.b6)>>11;
	X3=X1 + X2;
	CVC.b3=(((((uint32_t) CV.ac1) * 4 +X3)<<resolution) + 2)>>2;
	
	X1=(CV.ac3 * CVC.b6)>>13;
	X2=(CV.b1 * ((CVC.b6*CVC.b6)>>12))>>16;
	X3=((X1 + X2) + 2)>>2;
	CVC.b4=(CV.ac4 * (uint32_t)(X3 + 32768))>>15;
	
	CVC.b7=((uint32_t)(raw_p - CVC.b3) * (50000>>resolution));
	if (CVC.b7 < 0x80000000)	{p=(CVC.b7<<1) / CVC.b4;}
	else						{p=(CVC.b7 / CVC.b4)<<1;}
	
	X1=(p>>8)*(p>>8);
	X1=(X1 * 3038)>>16;
	X2=(-7357 * p)>>16;
	p+=(X1 + X2 + 3791)>>4;//original
	//final value must be calculated, it is now done in python script, look into BMP085 datasheet for more info...
	return p;
}

void handleERROR(uint8_t ERROR_code)//when something goes wrong, this func. is caled, but what to do next ?
{
	switch(ERROR_code)
	{
		case I2C_BMP_start_err:
		{
			uart_println("BMP085 error (I2C)");
			i2c_stop();
			break;
		}
		case I2C_EXT_start_err:
		{
			uart_println("ADT error (I2C)!");
			i2c_stop();
			break;
		}
		case DHT_err:
		{
			uart_println("DTH error!");
			break;
		}
		
		
	}

	//give it a chance, don't be so strict
	//delay(10000);
	//Reset_AVR();
	//while(1)asm volatile("NOP");//wait forever
}

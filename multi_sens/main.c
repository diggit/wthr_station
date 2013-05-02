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
//#include "display_ex.c" //functions for more elegant display output
#include <avr/wdt.h>

//#define debug

//i2c library for communication
#include "i2cmaster.c"

#include "calibrated_loop.c"


#ifndef _NOP
	#define _NOP asm volatile("NOP")//we have optimizations, empty while(1) loops are not ok, _NOP fixes this
#endif

#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {_NOP;}//little workaround, atmega has no software reset
#define FOSC 16000000UL //MCU crystal freq.
#define BAUD 19200UL //speed of UART, for short distances could be increased, bud don't forget to change value in grabber script on server...
#define UA_RX_INT_EN 1 //enable UART recieve INT

#include "uart_lib.c"//useful UART functions, init, send, receive and so on
#include "temp.c"

//basic macros for simple bit writing
#define bit_set(p,m) ((p) |= (m))
#define bit_clr(p,m) ((p) &= ~(m))


//addresses of BMP085 i2c sensor
#define BMP_ADDR 0b11101110
#define READ	 1 //lowest bit in addr selects READ/write

//waiting for signal that indicates End Of Conversion on pin PB0
#define WFC() while( !(PINC & 2) )_NOP; //Wait For Conversion
//DHT start sequence


#define DHTP PORTB //PORT where DHT11 data pin is attached
#define DHTD DDRB  //...it's configuration register (I/O selection)
#define DHTI PINB  //register where to get actual status of input
#define DHT_START(b) bit_set(DHTD,b); delay_us(1000*30); bit_clr(DHTD,b); delay_us(4*10)//; bit_clr(DHTD,b)//; bit_clr(DHTP,b) //shouldn't be DHTP (pull-up) bit 2 cleared too?

//resolution of BMP085 pressure measurement
#define resolution 3 //0 fastest - worst, 3 slowest - best

#define pressure_noise 10 //pressure treshold
#define temperature_noise 5 //temperature treshold
#define matching_min 3 //samples count which value shouldn't be too far from it's avg. value eg AVG:200 val1:201 val2:198 val3:203 -worst distance is 3 (=|200-203|), values above are tresholds 
#define max_tries 5 //if measured values were too far from their AVG value, try it until OK or max_tries is reached

//options for extended temperature sensor board
#define temp_ex_samples 5
#define temp_ex_pause	500 //ms


//char str_buffer[]="XXXXXXXXXXX\0"; //remove?

//error codes definition
#define I2C_BMP_start_err	1
#define I2C_EXT_start_err	2
#define DHT_err				3

#define STRING_VERSION __DATE__ " " __TIME__ // build date and time


int8_t response[5]={0,0,0,0,0};//init with zeros, buffer for DHT11

void delay(uint16_t num) ;
int32_t calc_temp();
void getCV();
long calc_pressure();
uint32_t readNB(uint8_t address, uint8_t bytes);
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


inline int32_t sabs( int32_t num)
{
	if(num<0)
		return -num;
	else 
		return num;
}


int main (void)
{
	uart_init();//init UART
	uart_puts("booted...\ncompiled: ");
	uart_puts(STRING_VERSION);
	uart_putc('\n');
	//while(1)NOP;
	i2c_init();//bring up I2C
#ifdef debug
	uart_puts("i2c bus ready...\n");
#endif
	
	getCV();//download callibration values from BMP085
#ifdef debug
	uart_puts("i2c communication success...\n");
#endif
	calc_temp();//initial reading
	calc_pressure();//same here

#ifdef debug
	int ret;
	ret=ADTconfig();
	if(ret)
	{
		uart_puts("failed init with EX temp. sensor: ");
		uart_num(ret,1);
		uart_putc('\n');
	}
	else
		uart_puts("extended temp. board init success...\n");
#else
	if(ADTconfig())
		handleERROR(I2C_EXT_start_err);
#endif
	
	set_sleep_mode(SLEEP_MODE_IDLE); //choose most suitable sleep mode - IDLE (still running UART)
	sleep_enable();//enable sleeping
	
	
	sei(); //enable interrupt, mainly for UART RX

#ifdef debug
	uart_puts("entering infinite sleep loop...\n");
#endif
	while(1)//sleep it after interrupt again and again
	{
#ifdef debug
		uart_puts("sleeping\n");//debug
#endif
		sleep_enable();
		sleep_cpu();//MCU is awaken on every interrupt, this will sleep it back again...
		//_NOP;
	}	
	
	return 0;
}

void DHT(uint8_t bit)
{
	
	uint8_t i,j;

	//"say hello"
	DHT_START(1<<bit);
	//expect ACK
	while(DHTI&(1<<bit));//wait for pull down
	
	while(!(DHTI&(1<<bit)));//wait for pull up
	while(DHTI&(1<<bit));//wait for pull down

	
	//recieve 40(1<<bit)s
	for(j=0;j<5;j++)
	{
		response[j]=0;//erase old value
		for(i=0;i<8;i++)
		{
			response[j]=response[j]<<1;//shift left - prepare for new (1<<bit)
			while(!(DHTI&(1<<bit)));//wait for pull up
			delay_us(50);
			if(DHTI&(1<<bit))//i still pulled up>longer pulse>LOG1
			{
				response[j]|=1;//set lowest (1<<bit)
				while(DHTI&(1<<bit));//wait for pull down
			}
			//else //already pulled down > shorter pulse > LOG0 , zero is hopefuly there (default)

			
		}
	}
	//link is down
	//end event check
	while(!(DHTI&(1<<bit)));//wait for pull up

	
}

ISR(USART_RXC_vect)//return measured values on request, otherwivise, node will be in idle mode, (maybe sleeping - should be implemented)
{
	cli();
	
	uint8_t cnt,cnt2;
	uint8_t OK=0;
	char reply=UDR; 
	int16_t temp=0;
	int16_t Tavg;
	int16_t Tbuf [matching_min];
	
	int32_t tempEX=0;
	uint32_t press=0;
	uint32_t Pavg;
	uint32_t Pbuf [matching_min];
	if(reply=='Q')//send data as response fo Query
	{
		
		
		for(cnt2=0;cnt2<max_tries && !(OK);cnt2++)
		{
#ifdef debug
			uart_puts("try nuber: ");
			uart_num(cnt2,1);
#endif
			//single try
			OK=1;
			Tavg=0;
			Pavg=0;
			for(cnt=0;cnt<matching_min;cnt++)//load "matching_min" samples
			{
				Tbuf[cnt]=calc_temp();//store measured data
				Tavg+=Tbuf[cnt];//add to average
				
				Pbuf[cnt]=calc_pressure();
				Pavg+=Pbuf[cnt];
			}
			Tavg/=matching_min;//calculate average value (for comparison)
			Pavg/=matching_min;
			
			//check values
			for(cnt=0;cnt<matching_min;cnt++)
			{
				
				if( sabs(Tavg-Tbuf[cnt])>temperature_noise)
				{
					OK=0;
#ifdef debug					
					uart_puts("Temperature from BMP085 is out of tollerance: ");
					uart_num(Tbuf[cnt],1);
					uart_puts("\n avg: ");
					uart_num(Tavg,1);
#endif					
					break;
				}
				
				if( sabs(Pavg-Pbuf[cnt])>pressure_noise)//if out of tollerance
				{
					OK=0;
#ifdef debug					
					uart_puts("Pressure from BMP085 is out of tollerance, value: \n");
					uart_num(Pbuf[cnt],1);
					uart_putc('\n');
					uart_num(sabs(Pbuf[cnt]-Pavg),1);
					uart_putc('\n');
					uart_num(sabs(Pavg-Pbuf[cnt]),1);
					uart_puts("\n avg: ");
					uart_num(Pavg,1);
					uart_putc('\n');
#endif					
					break;
				}
			}
		}
		
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
			uart_puts(" wrong value returned, resetting... ");
			Reset_AVR();

		}
		
		//power on sensors on EXT board
		//ADTwake();
		tempEX=ADTmeasure(temp_ex_samples,temp_ex_pause);//get value from EX board
		//ADTshutdown();
		
		uart_puts("!,MULTISNS1,T0,");//some ID
		uart_num((int32_t)tempEX,3);//temp from ex
		
		uart_puts(",T1,");
		uart_num(temp,1);//temp from BMP085
		
		uart_puts(",P0,");//pressure from BMP085
		uart_num(press,1);
		uart_puts(",H0,");//himudity from DHT11
		DHT(2);//start measurement
		uart_num(response[0],1);
		uart_puts(",$\n");
		
	}
	else if (reply=='R')
	{
		uart_puts("Resetting...");
		Reset_AVR();
	}
	else uart_puts("send Q to get data or R for reset\n");
	delay(600);//DHT needs some refresh, just for case of too often reading
	uart_flush();/*something weird, if Q was incomming without pause, interrupt occured one by one. 
	* No MCU sleep was done. 24th reading from sensors returned wrong values and next would block MCU. 
	* Solution was total MCU Reset (see above). So, multiple requests (too short pause between them) are ignored for now...*/
	sei();
}

void getCV()//downloads Calibration Values from sensor
{
	CV.ac1 = readNB(0xAA,2);
	CV.ac2 = readNB(0xAC,2);
	CV.ac3 = readNB(0xAE,2);
	CV.ac4 = readNB(0xB0,2);
	CV.ac5 = readNB(0xB2,2);
	CV.ac6 = readNB(0xB4,2);
	CV.b1 = readNB(0xB6,2);
	CV.b2 = readNB(0xB8,2);
	CV.mb = readNB(0xBA,2);
	CV.mc = readNB(0xBC,2);
	CV.md = readNB(0xBE,2);
}

uint32_t readNB(uint8_t reg_addr, uint8_t bytes)//reads 'bytes' bytes (up to 4) from address defined by RA/WA beginning on given register address 
{
	uint8_t raw[]={0,0,0,0};//maximal 4 bytes to read
	uint32_t sum=0;
	uint8_t i;//some index var.
	if(bytes>4){bytes=4;} //higher vals. shrink to 4
	bytes--;//now, our values are 0-3, better suits here
	
	//uart_puts("reading BMP\n");
	if(i2c_start(BMP_ADDR))//we wanna say what to read, that is writing...
	{
		handleERROR(I2C_BMP_start_err);
		//uart_puts("track point 2\n");
	}
	//uart_puts("track point 1\n");
	i2c_write(reg_addr);//read from given addr
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


int32_t calc_temp()//measures raw value, converts it into human readable value and returns it
{
	uint8_t err=0;//err test, only for debug purposes, won't be used
	int32_t X1,X2;//buffers for translate vals into human readable
	
	//proceed temp mesurement 
	//uart_puts("calc temp\n");
	if(i2c_start(BMP_ADDR))//we wanna say what to read, that is writing...
	{
		handleERROR(I2C_BMP_start_err);
#ifdef debug
		uart_puts("track point 2\n");
#endif
	}
	//uart_puts("chat opened\n");
	err+=i2c_write(0xF4);//we wanna write to control reg.
	err+=i2c_write(0x2E);//and initiate temp mesurement
	i2c_stop();
	
	delay(20);
	WFC();//wait for EOC
	delay(80);//maybe can prevent of reading random O temp.
	
	//read raw temp value
	uint16_t UT=(uint16_t)readNB(0xF6,2);
	
	//converting, this magic formula was copied from sensors datasheet
	X1=(((int32_t)UT-(int32_t)CV.ac6)*(int32_t)CV.ac5)>>15;
	X2=((int32_t)CV.mc<<11)/(X1+(int32_t)CV.md);
	CVC.b5=X1+X2;
	return((CVC.b5+8)>>4);
}

long calc_pressure()//similar to calc_temp, but works with pressure
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
	
	raw_p=(readNB(0xF6,3))>>(8-resolution);

	//example_run();
	//raw_p=23843;
	
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
	//p+=3400;//my sensor has wrong value at output, offset added to fix it, I hope, It'll work ;-)
	//haaa mistake, final value must be calculated, it is now done in python script, look onto BMP085 datasheet for more info...
	return p;
}

void delay(uint16_t num) //simple delay loop, really stupid but works
{
	uint16_t i,j;
	for (i = 0; i < num; i++)
		for (j=0;j<1000;j++)
			_NOP;
}

void handleERROR(uint8_t ERROR_code)//when something goes wrong, this func. is caled, but what to do next ?
{
	switch(ERROR_code)
	{
		case I2C_BMP_start_err:
		{
			uart_puts("cant access to BMP085 sensor on I2C bus!");
			i2c_stop();
			break;
		}
		case I2C_EXT_start_err:
		{
			uart_puts("cant access to one or more sensors on extension temperature board on I2C bus!");
			i2c_stop();
			break;
		}
		case DHT_err:
		{
			uart_puts("communication with DTH sensor failed!");
			break;
		}
		
		
	}
	//give it a chance, don't be so strict
	//uart_puts("due to error, performing device reset!");
	//delay(10000);
	//Reset_AVR();
	//while(1)asm volatile("NOP");//wait forever
}

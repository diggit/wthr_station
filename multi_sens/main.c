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
#include "BMP.h" 

#define LED_CFG() DDRC|=1
#define LED_ON() PORTC|=1
#define LED_OFF() PORTC &=~1


//TODO: move to header file
void handleERROR(uint8_t ERROR_code);

uint8_t hw_check()
{
	#ifdef debug
	uart_println("HW check...");
	uart_print("i2c ");
	#endif

	if(i2c_check())
	{
		error_flags.i2c=1;
		#ifdef debug
			uart_println("FAIL, skipping all i2c sensors!");
		#endif
	}
	else
	{
		error_flags.i2c=0;
		#ifdef debug
		uart_println("OK");
		#endif
	}

	if(error_flags.i2c==0)
	{


		#ifdef debug
			uart_print("BMP085 ");
		#endif
		if(BMP_check())
		{
			#ifdef debug
				uart_println("FAIL");
			#endif
			error_flags.BMP=1;
		}
		#ifdef debug
			else
				uart_println("OK");
		#endif



		if(ADT_check())
		{
			#ifdef debug
			uart_print("ADT fail, sns: ");
			for(int i=0;i<ADT_sensor_count;i++)
			{
				if(ADT_status&(1<<i))
				{
					uart_num(i,1);
					uart_putc(',');
				}

			}
			uart_println(" unresponsive");
			#endif
			error_flags.ADT=1;
		}
		#ifdef debug
		else
			uart_println("ADT OK");
		#endif
		ADT_shutdown();//put sensors into shutdown mode (low self heating)
	}

	

	#ifdef debug
		uart_print("DHT ");
	#endif
	if(DHT(DHT_sensor_pin))
	{
		error_flags.DHT=1;
		#ifdef debug
			uart_println("FAIL");
		#endif
	}
	#ifdef debug
	else
		uart_println("OK");
	#endif


	#ifdef debug
		uart_println("hw check DONE!");
	#endif
	return 0;
}

int main (void)
{
	DDRB=0;
	DDRD=0;
	DDRC=0;
	LED_CFG();
	LED_ON();
	uart_init();//init UART
	uart_print("booted...\ncompiled: ");
	uart_println(STRING_VERSION);
	i2c_init();//bring up I2C

	hw_check();
	

	if(error_flags.i2c==0 && error_flags.BMP==0)
	{
		//bmp init()
		BMP_get_calibration();//download callibration values from BMP085
		BMP_read_temperature();//initial reading
		BMP_read_pressure();//same here	
	}
	
	set_sleep_mode(SLEEP_MODE_IDLE); //choose most suitable sleep mode - IDLE (still running UART)
	sleep_enable();//enable sleeping
	
	
	sei(); //enable interrupt, mainly for UART RX

	#ifdef debug
	uart_println("entering infinite sleep loop...");
	#endif
	while(1)//sleep it after interrupt again and again
	{
		LED_OFF();
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
	LED_ON();
	#ifdef debug
		uart_println("woken");
	#endif
	cli();

	char reply=UDR; //latch incomming data byte
	
	//BMP085 output
	int16_t bmp_temperature=0;
	uint32_t bmp_pressure=0;
	//ADT7410 output
	int16_t adt_temperature=0;
	//DHT humidity
	uint8_t dht_humidity=0;


	if(reply=='Q')//send data as response of Query
	{
		uart_println("measuring...");
		hw_check();

		if(error_flags.i2c==0 && error_flags.BMP==0 && BMP_measure(&bmp_pressure,&bmp_temperature)==0)
		{
			NOP;
		}
		else
		{
			error_flags.BMP=1;
			bmp_pressure=0;
			bmp_temperature=0;
		}

		if(error_flags.i2c==0 && error_flags.ADT==0 && ADT_measure(&adt_temperature,ADT_samples,ADT_pause)==0)//get value from EX board
		{
			NOP;
		}
		else
		{
			error_flags.ADT=1;
			adt_temperature=0;
		}

		if(error_flags.DHT==0 && DHT(DHT_sensor_pin)==0)
			dht_humidity=DHT_response[0];
		else
		{
			error_flags.DHT=1;	
			dht_humidity=0;
		}
//ADT------------------------------------------		
		//power on sensors on EXT board
		
		
		uart_print("!,MULTISNS1,T0,");//some ID
		uart_num((int32_t)adt_temperature,3);//temp from ex
		
		uart_print(",T1,");
		uart_num(bmp_temperature,1);//bmp_temperature from BMP085
		
		uart_print(",P0,");//bmp_pressure from BMP085
		uart_num(bmp_pressure,1);

		uart_print(",H0,");//himudity from DHT11
		uart_num(dht_humidity,1);

		//errors
		uart_print(",GEN,");
		uart_num(error_flags.general,1);

		uart_print(",I2C,");
		uart_num(error_flags.i2c,1);

		//global ADT status
		uart_print(",ADT,");
		uart_num(error_flags.ADT,1);

		//each sensor status
		for (int i = 0; i < ADT_sensor_count; ++i)
		{
			uart_print(",ADT");
			uart_num(i,1);
			uart_putc(',');
			uart_num( (ADT_status&(1<<i)) ? 1 : 0,1);	
		}
		

		uart_print(",BMP,");
		uart_num(error_flags.BMP,1);

		uart_print(",DHT,");
		uart_num(error_flags.DHT,1);


		uart_print(",$\n");
		
	}
	else if (reply=='R')
	{
		uart_println("Resetting...");
		Reset_AVR();
	}
	else uart_println("send Q to get data or R for reset");
	
	#ifdef debug
		uart_println("ERR dump: ");
		uart_print("GEN:");
		uart_num(error_flags.general,2);
		uart_nl();

		uart_print("i2c:");
		uart_num(error_flags.i2c,2);
		uart_nl();

		uart_print("ADT:");
		uart_num(error_flags.ADT,2);
		uart_nl();

		uart_print("BMP:");
		uart_num(error_flags.BMP,2);
		uart_nl();
	#endif

	delay_us(500000);//DHT needs some refresh, just for case of too often reading
	uart_flush();/*something weird, if Q was incomming without pause, interrupt occured one by one. 
	* No MCU sleep was done. 24th reading from sensors returned wrong values and next would block MCU. 
	* Solution was total MCU Reset (see above). So, multiple requests (too short pause between them) are ignored for now...*/
	sei();
}

#ifndef _BH_LIB
	#define _BH_LIB
	
	#include "config.h"
	#include <inttypes.h>
	#include "i2cmaster.h"
	#include "calibrated_loop.h"
	#include "state.h"

	#define BH_CMD_POWER_DOWN			0b00000000 //No active state. 
	#define BH_CMD_POWER_ON				0b00000001 //Waiting for measurement command.
	#define BH_CMD_RESET				0b00000111 //Reset Data register value. Reset command is not acceptable in Power Down mode.
	#define BH_CMD_CONT_HRES_MODE		0b00010000 //Start measurement at 1lx resolution. Measurement Time is typically 120ms.
	#define BH_CMD_CONT_HRES_MODE_2		0b00010001 //Start measurement at 0.5lx resolution. Measurement Time is typically 120ms.
	#define BH_CMD_CONT_LRES_MODE		0b00010011 //Start measurement at 4lx resolution. Measurement Time is typically 16ms.
	#define BH_CMD_SINGLE_HRES_MODE		0b00100000 //Start measurement at 1lx resolution. Measurement Time is typically 120ms. It is automatically set to Power Down mode after measurement.
	#define BH_CMD_SINGLE_HRES_MODE_2	0b00100001 //Start measurement at 0.5lx resolution. Measurement Time is typically 120ms. It is automatically set to Power Down mode after measurement.
	#define BH_CMD_SINGLE_LRES_MODE		0b00100011 //Start measurement at 4lx resolution. Measurement Time is typically 16ms. It is automatically set to Power Down mode after measurement

	uint8_t BH_measure(void);
	uint8_t BH_read(uint16_t *light_value);
	
#endif
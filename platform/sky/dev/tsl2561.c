/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/i2c.h"
#include "dev/tsl2561.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

// Initialize the sensor and return coefficients
void tsl2561_poweron(){
	write_(TSL2561_ADDR, TSL2561_REG_CONTROL, 0x03);
}

void tsl2561_powerdonw(){
	write_(TSL2561_ADDR, TSL2561_REG_CONTROL, 0x00);
}


void tsl2561_settiming(bool gain, unsigned char t){
	unsigned timing = read_(TSL2561_ADDR, TSL2561_REG_TIMING, 1);
	
	if (gain)
		timing |= 0x10;
	else
		timing &= ~0x10;

	timing &= ~0x03;
	timing |= (t & 0x03);

	write_(TSL2561_ADDR, TSL2561_REG_TIMING, timing);

}

tsl2561_data tsl2561_sample(){
	unsigned char rv[2];
	read_multibyte(TSL2561_ADDR, TSL2561_REG_DATA_0 | TSL2561_CMD, 2, rv);
	tsl2561_data rlt;
	rlt.ch0.h = rv[1];
	rlt.ch0.l = rv[0];

	read_multibyte(TSL2561_ADDR, TSL2561_REG_DATA_1 | TSL2561_CMD, 2, rv);
	tsl2561_data rlt;
	rlt.ch1.h = rv[1];
	rlt.ch1.l = rv[0];
	return rlt;

}


void print_tsl2561_sample(tsl2561_data samples){
	uart1_writeb((unsigned char) samples.ch0.h);
	uart1_writeb((unsigned char) samples.ch0.l);

	uart1_writeb((unsigned char) samples.ch1.h);
	uart1_writeb((unsigned char) samples.ch1.l);
}



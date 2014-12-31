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
#include "dev/htu21d.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void htu21d_init(){
	htu21d_set_res(0); // resolution for hum is 12 bits
					   // resolution for tmp is 14 bits
}


void htu21d_set_res(unsigned resolution){
  unsigned userRegister = i2c.read_(HTDU21D_ADDRESS, READ_USER_REG,1); //Go get the current register state
  userRegister &= 0b01111110; //Turn off the resolution bits
  resolution &= 0b10000001; //Turn off all other bits but resolution bits
  userRegister |= resolution; //Mask in the requested resolution bits
  
  //Request a write to user register
  write_(HTDU21D_ADDRESS, WRITE_USER_REG, userRegister)
}

data htu21d_sample_hum(){
	unsigned rv[3];
	read_multibyte(HTDU21D_ADDRESS, TRIGGER_HUMD_MEASURE_HOLD, 3, rv);
	data humd;
	humd.h = rv[0];
	humd.l = rv[1];
	humd.crc = rv[2];

	return humd;
}

data htu21d_sample_tmp(){
	unsigned rv[3];
	read_multibyte(HTDU21D_ADDRESS, TRIGGER_TEMP_MEASURE_HOLD, 3, rv);
	data temp;
	temp.h = rv[0];
	temp.l = rv[1];
	temp.crc = rv[2];

	return temp;
}


void print_htu21d_sample(data samples){
	uart1_writeb((unsigned char) samples.h);
	uart1_writeb((unsigned char) samples.l);
	uart1_writeb((unsigned char) samples.crc);

}


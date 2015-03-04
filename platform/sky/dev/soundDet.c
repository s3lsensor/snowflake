/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/adc.h"
#include "dev/soundDet.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define audio 0
#define envlp 1
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

int soundDet_enable(){
	adc_on();
	return adc_configure(audio);
}
void soundDet_disable(){
	adc_off();
}

unsigned short soundDet_sample_audio(){
	if(adc_configure(audio)){
		return adc_sample();
	}else{
		printf("Audio sample failed");
	}
	return 0;
}

unsigned short soundDet_sample_envlp(){
	if(adc_configure(envlp)){
		return adc_sample();
	}else{
		printf("Envelope sample failed");
	}
	return 0;
}


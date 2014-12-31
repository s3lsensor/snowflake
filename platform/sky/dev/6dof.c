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
#include "dev/6dof.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

// Initialize the sensor and return coefficients
void adxl345_init(){
	write_(ADXL_ADDR, POWER_CTL, MEASURE);
	write_(ADXL_ADDR, DATA_FORMAT, 1<<0);
}

adxl345_union adxl345_sample(){
	data tmp;
	adxl345_union rlt;

	tmp.h = read_(ADXL_ADDR, DATAX0, 1);
	tmp.l = read_(ADXL_ADDR, DATAX1, 1);
	rlt.x = tmp;

	tmp.h = read_(ADXL_ADDR, DATAY0, 1);
	tmp.l = read_(ADXL_ADDR, DATAY1, 1);
	rlt.y = tmp;

	tmp.h = read_(ADXL_ADDR, DATAZ0, 1);
	tmp.l = read_(ADXL_ADDR, DATAZ1, 1);
	rlt.z = tmp;

	return rlt;

}

void print_adxl345_sample(adxl345_union samples){

	uart1_writeb((unsigned char) samples.x.h);
	uart1_writeb((unsigned char) samples.x.l);

	uart1_writeb((unsigned char) samples.y.h);
	uart1_writeb((unsigned char) samples.y.l);

	uart1_writeb((unsigned char) samples.z.h);
	uart1_writeb((unsigned char) samples.z.l);

}

void itg3200_init(){
	//Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
	write_(DLPF_FS, DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0);
	
	//Set sample rate divider for 100 Hz operation
	write_(SMPLRT_DIV, 9);	//Fsample = Fint / (divider + 1) where Fint is 1kHz
	
	//Setup the interrupt to trigger when new data is ready.
	write_(INT_CFG, INT_CFG_RAW_RDY_EN | INT_CFG_ITG_RDY_EN);
	
	//Select X gyro PLL for clock source
	write_(PWR_MGM, PWR_MGM_CLK_SEL_0);
}


itg3200_union itg3200_sample(){
	data tmp;
	itg3200_union rlt;

	tmp.h = read_(ITG_ADDR, GYRO_XOUT_H, 1);
	tmp.l = read_(ITG_ADDR, GYRO_XOUT_L, 1);
	rlt.x = tmp;

	tmp.h = read_(ITG_ADDR, GYRO_YOUT_H, 1);
	tmp.l = read_(ITG_ADDR, GYRO_YOUT_L, 1);
	rlt.y = tmp;

	tmp.h = read_(ITG_ADDR, GYRO_ZOUT_H, 1);
	tmp.l = read_(ITG_ADDR, GYRO_ZOUT_L, 1);
	rlt.z = tmp;


	tmp.h = read_(ITG_ADDR, TEMP_OUT_H, 1);
	tmp.l = read_(ITG_ADDR, TEMP_OUT_L, 1);
	rlt.temp = tmp;
	return rlt;
}

void print_itg3200_sample(itg3200_union samples){
	uart1_writeb((unsigned char) samples.x.h);
	uart1_writeb((unsigned char) samples.x.l);

	uart1_writeb((unsigned char) samples.y.h);
	uart1_writeb((unsigned char) samples.y.l);

	uart1_writeb((unsigned char) samples.z.h);
	uart1_writeb((unsigned char) samples.z.l);

	uart1_writeb((unsigned char) samples.temp.h);
	uart1_writeb((unsigned char) samples.temp.l);
}


/*
 * This file defines the driver for ms5803
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */

/*
 * Pin connection: VCC -> U2-1
		   GND -> U2-9
		   SCL -> U28-3
		   SDA -> U28-4
		   PS -> VCC
		   CS -> GND  : I2C address will be 0x77 

 * usage: init before use to get the coefficients. reset everytime the sensor restarts. 
 */
#ifndef MS5803_H
#define MS5803_H

#define delay_1ms()   do{ _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); }while(250)

#define CMD_RESET 0x1E // reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command
#define CMD_PROM 0xA0 // Coefficient location


#define PRESSURE 0x00
#define TEMPERATURE 0x10

// precisions
#define ADC_256  0x00
#define ADC_512  0x02
#define ADC_1024  0x04
#define ADC_2048  0x06
#define ADC_4096  0x08


#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1




#define MS5803_ADDR 0x77

typedef struct
{
    int8_t hByte;
    int8_t mByte;
    int8_t lByte;
}ms5803_data;

typedef struct
{
	int32_t pressure;
	int32_t temperature;
}ms5803_union

#define MS5803_DATA_SIZE (sizeof(ms5803_data)/sizeof(uint8_t))

int16_t* ms5803_init();
int ms5803_reset();
int ms5803_send(int8_t cmd);
int ms5803_sample(int8_t precision, int16_t *coeff);


void print_ms5803_sample(ms5803_union samples);

#endif /*MPU_6050_H*/

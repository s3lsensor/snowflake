#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma 
#include "sys/etimer.h"
#include "appconn/app_conn.h"
#include "node-id.h"

#include "app_util.h"
#include "dev/adc.h"
#include "dev/leds.h"
#include "dev/i2c.h"
#include "dev/mpu-6050.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SIN_TAB_LEN 120
#define RESOLUTION 7


#define ADC_SAMPLING_FREQ 64 //use power of 2 in Hz (tested 1, 2, 4...32)
#define ADC_SAMPLES_PER_FRAME (ADC_SAMPLING_FREQ/FRAMES_PER_SEC)

#define MPU_SAMPLING_FREQ 16 //tested 1, 2, and 4
#define MPU_SAMPLES_PER_FRAME (MPU_SAMPLING_FREQ/FRAMES_PER_SEC)

#define I2C_SENSOR
//#define ADC_SENSOR

#ifdef I2C_SENSOR
#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ MPU_SAMPLING_FREQ
#endif /*I2C_SENSOR*/

#ifdef ADC_SENSOR
#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ ADC_SAMPLES_PER_FRAME
#endif /*ADC_SENSOR*/

#ifndef DATA_SIZE
#define DATA_SIZE 1 //size of uint8_t (1 bit)
#endif /*DATA_SIZE*/




static const int8_t SIN_TAB[] =
{
		0,6,13,20,26,33,39,45,52,58,63,69,75,80,
		85,90,95,99,103,107,110,114,116,119,121,
		123,125,126,127,127,127,127,127,126,125,
		123,121,119,116,114,110,107,103,99,95,90,
		85,80,75,69,63,58,52,45,39,33,26,20,13,6,
		0,-6,-13,-20,-26,-33,-39,-45,-52,-58,-63,
		-69,-75,-80,-85,-90,-95,-99,-103,-107,-110,
		-114,-116,-119,-121,-123,-125,-126,-127,-127,
		-127,-127,-127,-126,-125,-123,-121,-119,-116,
		-114,-110,-107,-103,-99,-95,-90,-85,-80,-75,
		-69,-63,-58,-52,-45,-39,-33,-26,-20,-13,-6
};

static int8_t sinI(uint16_t angleMilli)
{
	uint16_t pos;
	pos = (uint16_t) ((SIN_TAB_LEN * (uint32_t) angleMilli)/1000);
	return SIN_TAB[pos%SIN_TAB_LEN];
}

static int8_t sin(uint16_t angleMilli)
{
	return SIN_TAB[angleMilli%SIN_TAB_LEN];
}

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Hello world Process");
//PROCESS(sensor_sampling_process, "Sensor Sampling Process");
//AUTOSTART_PROCESSES(&null_app_process, &sensor_sampling_process);
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
//APP Callback function
static void app_recv(void)
{
	//printf("Received from RDC\n");
	PROCESS_CONTEXT_BEGIN(&null_app_process);
	
	uint16_t *data = (uint16_t*)packetbuf_dataptr();

	uint8_t flag = 0;


	int i;
	rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
	uint8_t rx_sn_id = sent_sn_addr->u8[0];

	uint8_t pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	uint8_t payload_len = packetbuf_datalen()/DATA_SIZE;


	printf("%u,%u,%u,",rx_sn_id,pkt_seq,payload_len);
	/*
	for(i=0;i<payload_len;i++){
		printf("%u,",data[i]);
	}
	printf("\n");
	*/

	//app_output(data,rx_sn_id,pkt_seq,payload_len);

	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};


/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{

	static struct etimer rxtimer;

	PROCESS_BEGIN();


	printf("Hello world Started.\n");

#ifdef SF_FEATURE_SHELL_OPT
	serial_shell_init();
	remote_shell_init();
	shell_reboot_init();
	shell_blink_init();
	shell_sky_init();
#endif

	app_conn_open(&nullApp_callback);


#ifdef ADC_SENSOR
	static uint16_t samples[ADC_SAMPLES_PER_FRAME]={0};
	uint8_t i;
//	static uint8_t samples_sorted_bytes[2*ADC_SAMPLES_PER_FRAME];
	static uint8_t sample_num = 0; //increments from 0 to samples_per_frame-1


	if (node_id != 0){
		adc_on();
		//adc_configure(0); //to sample reference voltage (Vref/2), ~2048.
		etimer_set( &rxtimer, (unsigned long)(CLOCK_SECOND/(ADC_SAMPLING_FREQ)));
	}
	else
		etimer_set(&rxtimer,CLOCK_SECOND/20);

	if(node_id != 0)
	{

	  while(1)
	  {

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
	    etimer_reset(&rxtimer);

	    samples[sample_num]=adc_sample();
	    sample_num++;
	    if(sample_num == ADC_SAMPLES_PER_FRAME){
	    	sample_num=0;
	    	/*
	    	 * Byte order needs to be reversed because of low-endian system.
	    	 * Can be done at AP level too, if needed.
	    	 */
	    	// for(i=0;i<ADC_SAMPLES_PER_FRAME;i++){
	    	// 	samples_sorted_bytes[2*i]=(samples[i]>>8);
	    	// 	samples_sorted_bytes[2*i+1]= (samples[i]& 0xff);
	    	// }


	    	//app_conn_send(samples_sorted_bytes,sizeof(uint8_t)*ADC_SAMPLES_PER_FRAME*2);
	    	 tdma_rdc_buf_ptr = 0;
	    	 tdma_rdc_buf_send_ptr = 0;
	    	 tdma_rdc_buf_full_flg = 0;
	    	 app_conn_send(samples,sizeof(uint16_t)*ADC_SAMPLES_PER_FRAME/sizeof(uint8_t));
	    }

	  }
	}
#endif


#ifdef I2C_SENSOR
	//static rtimer_clock_t rt, del;
	static mpu_data samples;
	static int i;
	static uint8_t MPU_status = 0;
/*
	static uint8_t samples_sorted_bytes[14*MPU_SAMPLES_PER_FRAME],comp_samples_sorted_bytes[14*MPU_SAMPLES_PER_FRAME];
	static uint8_t sample_num=0, uncomp_data_len=14*MPU_SAMPLES_PER_FRAME,comp_data_len;
	static uint8_t *st;
*/

	if (node_id != 0){

		MPU_status = 0;
		for(i = 0; i < 100 & ~MPU_status;i++)
		{
			MPU_status = mpu_enable();
		}

		if (MPU_status == 0)
			printf("MPU could not be enabled.\n");


		MPU_status = 0;
		for(i = 0; i < 100 & ~MPU_status;i++)
		{
			MPU_status = mpu_wakeup();
		}

		if (MPU_status == 0)
			printf("MPU could not be awakened.\n");

		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/MPU_SAMPLING_FREQ));
		}
	else
		etimer_set(&rxtimer,CLOCK_SECOND/20);

	if(node_id != 0)
	{

		while(1){
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
			etimer_reset(&rxtimer);

			int m=mpu_sample_all(&samples);
/*
			st = &samples;
			for(i=0;i<7;i++){
				samples_sorted_bytes[2*i+14*sample_num]=*(st+2*i+1);
				samples_sorted_bytes[2*i+1+14*sample_num]= *(st+2*i);
			}
			sample_num++;

			if(sample_num==MPU_SAMPLES_PER_FRAME){
				sample_num=0;

				app_conn_send(samples_sorted_bytes,sizeof(uint8_t)*14*MPU_SAMPLES_PER_FRAME);
			}
*/
			app_conn_send((uint8_t*)&samples,sizeof(mpu_data)/sizeof(uint8_t));
		}
	}

#endif

	PROCESS_END();

}

/*
PROCESS_THREAD(sensor_sampling_process, ev, data)
{
  PROCESS_BEGIN();
  if (SN_ID != 0){
	printf("Sensor Sampling begun\n");

 	 static struct etimer sensetimer;
  	  etimer_set(&sensetimer,CLOCK_SECOND);
	  init_mpu6050();  
	  uint8_t rv;
	  rv = read_(MPU_ADDRESS, 0x75, 0);
	  printf("%d \n", rv);


	  while(1)
	  {

//	    if(SN_ID != 0)
//    {
  	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sensetimer));

  	    etimer_reset(&sensetimer);

  	    measure_mpu();
	    #if DEBUG
  	    printf("Accel value: %d\tY value: %d\tZ value: %d\n",accx,accy,accz);
	    #endif

	    packetbuf_copyfrom(measurevector,sizeof(int8_t)*10);
	    NETSTACK_RDC.send(NULL,NULL);

  	  }
  	}
  PROCESS_END();
}
 */
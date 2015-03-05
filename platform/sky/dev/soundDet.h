/*
 * This file defines the driver for ms5803
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */

/*
 * Pin connection: analog output, need a adc converter
 * audio -- U2_3
 * envolope -- U2_5
 * usage:  
 */
#ifndef SOUNDDET_H
#define SOUNDDET_H

int soundDet_enable();
void soundDet_disable();

unsigned short soundDet_sample_audio();
unsigned short soundDet_sample_envlp();
#endif /*ML8511_H*/

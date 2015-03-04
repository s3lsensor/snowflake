/*
 * This file defines the driver for ms5803
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */

/*
 * Pin connection: 
	U2_1	ACC
	U2_9	GND
	U28_3	 EN
	U2_10	OUT

 * usage: the functions are self-explantory
 */
#ifndef ML8511_H
#define ML8511_H

int ml8511_enable();
void ml8511_disable();

unsigned short ml8511_sample();

#endif /*ML8511_H*/

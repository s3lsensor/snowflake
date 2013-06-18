/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: hello-world.c,v 1.1 2006/10/02 21:46:46 adamdunkels Exp $
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "i2c.h"
#include "spi.h"
#include <stdio.h> /* For printf() */


/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
    WDTCTL = WDTPW+WDTHOLD;
 unsigned rv;
// unsigned id;
 printf("process starts\n");

//check and receive

/*-------I2C:	read data for X1----------------------------------------------*/
i2c_enable();
printf("i2c enabled\n");

//id = 0xA6;

//start
i2c_start();
//printf("i2c started\n");
//slave address-write
//if(!i2c_write(0xA6))
//	printf("write slave address transmit fail!\n");
i2c_write(0xA6);//0x3A);//0xA6);
//printf("write slave address transmit fail!\n");
//register address
//if(!i2c_write(0x33))
//	printf("register address transmit fail\n");
i2c_write(0x33);
//printf("register address transmit fail\n");

//stop-start
i2c_stop();
i2c_start();
//slave address-read
//if(!i2c_write(0xA7))
//	printf("read slave address transmit fail!\n");
i2c_write(0xA7);//0x3B);//0xA7);
//printf("read slave address transmit fail!\n");
//read data
rv = i2c_read(0);
//NACK

//stop
i2c_stop();

//disable
i2c_disable();
printf("recieved: %d", rv);
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
#ifndef _TEMP_H
#define _TEMP_H

#ifndef F_CPU
	#define F_CPU				1000000UL
#endif

#define MAXTEMP					500U						// in 0.1 degrees C

#define NTC_SENSOR_ON			PORTF |= (1<<PF3)
#define NTC_SENSOR_OFF			PORTF &= ~(1<<PF3)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

extern unsigned int targetTemp;

void initTemp( void );
unsigned char findNTCTemp( unsigned int r );

#endif
#ifndef _VALVE_H  
#define _VALVE_H

#ifndef F_CPU
	#define F_CPU		1000000UL
#endif

#define RUN_MOTOR_CW			PORTE |= (1<<PE7)&~(1<<PE6)
#define RUN_MOTOR_CCW			PORTE |= (1<<PE6)&~(1<<PE7)
#define STOP_MOTOR				PORTE &= ~((1<<PE6)|(1<<PE7))

#define OPTO_SENSOR_ON			PORTE |= (1<<PE2);
#define OPTO_SENSOR_OFF			PORTE &= ~(1<<PE2);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "../Controls/Controls.h"
#include "../Main.h"

void initValve( void );
void doProbe( void );

#endif
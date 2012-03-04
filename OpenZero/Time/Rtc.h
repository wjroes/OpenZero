#ifndef _RTC_H  
#define _RTC_H

#ifndef F_CPU
	#define F_CPU					1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "../Main.h"

typedef struct { 
	unsigned char second;   //enter the current time, date, month, and year
	unsigned char minute;
	unsigned char hour;                                     
	unsigned char date;       
	unsigned char month;
	unsigned int year;    
	unsigned char dow; 
} _RTC;

extern volatile _RTC rtc;

void initRTC(void);

void increaseClock( TIMESETPHASE timesetphase );
void decreaseClock( TIMESETPHASE timesetphase );

#endif 


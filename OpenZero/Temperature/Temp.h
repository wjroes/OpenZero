#ifndef _TEMP_H
#define _TEMP_H

#define NTC_SENSOR_ON			PORTF |= (1<<PINF3)
#define NTC_SENSOR_OFF			PORTF &= ~(1<<PINF3)

void initTemp( void );
unsigned char findNTCTemp( unsigned int r );

#endif
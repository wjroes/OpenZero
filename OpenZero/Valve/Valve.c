/*
 * Valve.c
 *
 * Created: 3-3-2012 14:05:45
 *  Author: Willem
 */ 
#ifndef F_CPU
	#define F_CPU		1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "../Controls/Controls.h"
#include "../Main.h"

void doProbe( void )
{
	runstate = PROBING_STATE;
	
	ADMUX |= (1<<REFS0)|(1<<MUX1);					// ref. voltage = AVcc, channel = ADC2
	ADCSRA |= (1<<ADPS2)|(1<<ADIE);					// 1:16 prescaler
	
	_delay_ms( 300 );
	
	ADCSRA |= (1<<ADEN);							// enable ADC
	ADCSRA |= (1<<ADSC);							// start first conversion
	
	probingphase = PROBING_START;
	
	// take some time to take a measurement without the motor running, go to sleep and wake up if the measurements are done
	set_sleep_mode( SLEEP_MODE_ADC );
	sleep_mode();
	
	probingphase = PROBING_RUNNING_CCW;
	RUN_MOTOR_CCW;
	while( probingphase == PROBING_RUNNING_CCW ) ;

	STOP_MOTOR;
	
	while( probingphase == PROBING_END_CCW) 
	{
		if( BUTTON_OK_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_OK_PRESSED )
			{
				// wait until button is released
				while( BUTTON_OK_PRESSED ) ;						
				// continue here if button is still depressed
				
				probingphase = PROBING_RUNNING_CW;
			}
		}
	}		
	
	RUN_MOTOR_CW;
	while( probingphase == PROBING_RUNNING_CW ) ;
	
	STOP_MOTOR;
	
	while( probingphase == PROBING_END_CW )
	{
		if( BUTTON_OK_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_OK_PRESSED )
			{
				// wait until button is released
				while( BUTTON_OK_PRESSED ) ;						
				// continue here if button is still depressed
				
				probingphase = PROBING_RUNNING_CCW;
			}
		}
	}	
	
	RUN_MOTOR_CCW;
	while( probingphase == PROBING_RUNNING_CCW ) ;

	STOP_MOTOR;
	
	probingphase = PROBING_END;

	ADCSRA &= ~((1<<ADEN)|(1<<ADIE));					// disable ADC
	
	_delay_ms( 300 );

	runstate = NORMAL_STATE;
}	


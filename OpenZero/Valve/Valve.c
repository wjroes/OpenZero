/*
 * Valve.c
 *
 * Created: 3-3-2012 14:05:45
 *  Author: Willem
 */ 
#include "Valve.h"

volatile VALVESTATE valvestate = VALVE_UNKNOWN;

void initValve( void )
{
	DDRE |= (1<<DDE6)|(1<<DDE7);	// Pin E6 / Pin E7 provide power to the motor and are both outputs
	DDRE |= (1<<DDE2);				// Pin E2 provides power to the opto-sensor LED and is output

	DDRE &= ~(1<<DDE1);				// PINE1 = PCINT1 attached to opto-sensor collector is input
	PORTE |= (1<<PE1);				// PINE1 = PCINT1 attached to opto-sensor collector, enable internal pull-up
	
	EIMSK |= (1<<PCIE0);			// Enable interrupt-on-change interrupts for PCINT0-PCINT7
	PCMSK0 |= (1<<PCINT1);			// But only mask PCINT1
}

void doProbe( void )
{
	runstate = PROBING_STATE;
	
	ADMUX = (1<<REFS0)|(1<<MUX1);					// ref. voltage = AVcc, channel = ADC2
	ADCSRA = (1<<ADPS2)|(1<<ADIE);					// 1:16 prescaler
	
	_delay_ms( 300 );
	
	ADCSRA |= (1<<ADEN);							// enable ADC
	ADCSRA |= (1<<ADSC);							// start first conversion
	
	probingphase = PROBING_START;
	
	// take some time to take a measurement without the motor running, go to sleep and wake up if the measurements are done
	set_sleep_mode( SLEEP_MODE_ADC );
	sleep_mode();
	
	OPTO_SENSOR_ON;
	
	openValve();
	
	while( !okButtonPressed() ) ;
	
	closeValve();
	
	while( !okButtonPressed() ) ;

	openValve();
	
	probingphase = PROBING_END;

	ADCSRA &= ~((1<<ADEN)|(1<<ADIE));				// disable ADC

	OPTO_SENSOR_OFF;
	
	_delay_ms( 300 );

	runstate = NORMAL_STATE;
}	

void openValve( void )
{
	valvestate = VALVE_OPENING;
	
	probingphase = PROBING_RUNNING_CW;
	RUN_MOTOR_CW;
	while( probingphase == PROBING_RUNNING_CW ) ;

	valvestate = VALVE_OPEN;
	
	STOP_MOTOR;
}	

void closeValve( void )
{
	valvestate = VALVE_CLOSING;
	
	probingphase = PROBING_RUNNING_CCW;
	RUN_MOTOR_CCW;
	while( probingphase == PROBING_RUNNING_CCW ) ;
	
	valvestate = VALVE_CLOSED;

	STOP_MOTOR;
}
	
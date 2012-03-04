/*
 * Controls.c
 *
 * Created: 3-3-2012 18:11:20
 *  Author: Willem
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "Controls.h"

unsigned char previousRotaryState = 0;
	
void initControls( void )
{
	DDRB &= (1<<PB0);
	DDRB &= (1<<PB7);
	
	// all pins are input by default, so there's no need to set the direction
	PORTB |= (1<<PB0);				// PINB0 = "+" button on wheel, setting to 1 enables the internal pull-up
	PORTB |= (1<<PB4);				// PINB4 = "MENU" button
	PORTB |= (1<<PB5);				// PINB5 = "TIME" button
	PORTB |= (1<<PB6);				// PINB6 = "OK" button
	PORTB |= (1<<PB7);				// PINB7 = "-" button on wheel
	
// Why can I not get PCINT1_vecto to work???
//	EIMSK |= (1<<PCIE1);				// Enable interrupt-on-change interrupts for PCINT8-PCINT15 which includes the push buttons
//	PCMSK1 |= (1<<PCINT8)|(1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14)|(1<<PCINT15);
	
	DDRE  = (1<<DDE6)|(1<<DDE7);	// Pin E6 / Pin E7 provide power to the motor and are both outputs
	DDRE |= (1<<DDE2);				// Pin E2 provides power to the opto-sensor and is output
	DDRF  = (1<<DDF3);				// Pin F3 providers power to the NTC and is output
}

ROTARYBUTTON readRotaryButton( void )
{
	unsigned char rotaryState = (BUTTON_UP_PRESSED<<1)|BUTTON_DOWN_PRESSED;
	if( rotaryState != previousRotaryState )
	{
		_delay_ms( DEBOUNCE_TIME );
		if( rotaryState == (BUTTON_UP_PRESSED<<1)|BUTTON_DOWN_PRESSED )
		{
			switch( (previousRotaryState<<2)|rotaryState )
			{
				case 1:
				case 7:
				case 8:
				case 14:
					// turning CW
					previousRotaryState = rotaryState;
					return ROTARY_UP;
				break;

				case 2:
				case 4:
				case 11:
				case 13:
					// turning CCW
					previousRotaryState = rotaryState;
					return ROTARY_DOWN;
				break;
			}
		}			
	}	
	
	return ROTARY_STILL;		
}


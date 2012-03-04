#ifndef _CONTROLS_H
#define _CONTROLS_H

#ifndef F_CPU
	#define F_CPU					1000000UL
#endif

#define MAX_MENU_ITEMS			1

#define DEBOUNCE_TIME			10			// msec

#define ADC_THRESHOLD_CLOSE		965			// adc value cut off point while closing
#define ADC_THRESHOLD_OPEN		965			// adc value cut off point while opening

#define BUTTON_TIME_PRESSED		bit_is_clear( PINB, PB5 )
#define BUTTON_MENU_PRESSED		bit_is_clear( PINB, PB4 )
#define BUTTON_OK_PRESSED		bit_is_clear( PINB, PB6 )
#define BUTTON_UP_PRESSED		bit_is_clear( PINB, PB0 )
#define BUTTON_DOWN_PRESSED		bit_is_clear( PINB, PB7 )

#define RUN_MOTOR_CW			PORTE |= (1<<PE7)&~(1<<PE6)
#define RUN_MOTOR_CCW			PORTE |= (1<<PE6)&~(1<<PE7)
#define STOP_MOTOR				PORTE &= ~((1<<PE6)|(1<<PE7))

#define OPTO_SENSOR_ON			PORTE |= (1<<PE2);
#define OPTO_SENSOR_OFF			PORTE &= ~(1<<PE2);

typedef enum
{
	ROTARY_STILL,
	ROTARY_UP,
	ROTARY_DOWN
} ROTARYBUTTON;

void initControls( void );

ROTARYBUTTON readRotaryButton( void );

#endif
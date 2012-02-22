/*
 * ZeroAbove.c
 *
 * Created: 15-2-2012 21:16:33
 *  Author: Willem
 */ 
#define F_CPU					1000000UL

#define DEBOUNCE_TIME			50			// msec

#define ADC_THRESHOLD_CLOSE		915			// adc value cut off point while closing
#define ADC_THRESHOLD_OPEN		915			// adc value cut off point while opening

#define BUTTON_TIME_PRESSED		bit_is_clear( PINB, PINB4 )
#define BUTTON_MENU_PRESSED		bit_is_clear( PINB, PINB5 )
#define BUTTON_OK_PRESSED		bit_is_clear( PINB, PINB6 )
#define BUTTON_UP_PRESSED		bit_is_clear( PINB, PINB0 )
#define BUTTON_DOWN_PRESSED		bit_is_clear( PINB, PINB7 )

#define RUN_MOTOR_CW			PORTE |= (1<<PINE7)&~(1<<PINE6)
#define RUN_MOTOR_CCW			PORTE |= (1<<PINE6)&~(1<<PINE7)
#define STOP_MOTOR				PORTE &= ~((1<<PINE6)|(1<<PINE7))

#define NTC_SENSOR_ON			PORTF |= (1<<PINF3)
#define NTC_SENSOR_OFF			PORTF &= ~(1<<PINF3)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "Rtc.h"
#include "ZeroAbove.h"
#include "Lcd.h"

typedef enum 
{
	NORMAL_STATE,
	PROBING_STATE,
	TIMESET_STATE,
} RUNSTATE;
	
typedef enum 
{
	TIMESET_UNKNOWN,
	TIMESET_START,
	TIMESET_HOURS,
	TIMESET_MINUTES,
	TIMESET_DATE,
	TIMESET_MONTH,
	TIMESET_YEAR,
	TIMESET_END
} TIMESETPHASE;

typedef enum 
{
	PROBING_UNKNOWN,
	PROBING_START,
	PROBING_RUNNING_CCW,
	PROBING_END_CCW,
	PROBING_RUNNING_CW,
	PROBING_END_CW,
	PROBING_END
} PROBINGPHASE;

volatile RUNSTATE runstate = NORMAL_STATE;	
volatile TIMESETPHASE timesetphase = TIMESET_UNKNOWN;
volatile PROBINGPHASE probingphase = PROBING_UNKNOWN;

volatile unsigned int adcValue = 0;						// in V x 1000
volatile unsigned int adcVref = 3000;					// in V x 1000
volatile unsigned int adcTemp = 0;

unsigned char findNTCTemp( unsigned int r )
{
	// finds the best matching item in a sorted array and returns the index in the array using binary search
	// TO DO: move this table to EEPROM
	unsigned int ntcTable[50] = {335,317,301,285,270,256,243,231,219,208,198,188,179,170,162,154,146,139,133,126,120,115,109,104,100,95,91,86,82,79,75,72,68,65,62,60,57,55,52,50,48,46,44,42,40,39,37,35,34,33}; // NTC R values in k Ohm from 0 to 49 in steps of 1 degree
 	unsigned char low = 0;
	unsigned char mid;
    unsigned char high = (sizeof ntcTable) - 1;	

 	while (low < high) 
	{
		mid = (high + low)>>1;

		if (ntcTable[mid]>r)
	    	low = mid + 1; 
		else
			high = mid; 
	}

	if (ntcTable[mid] == r)
		return mid;
	else
		return high+1; 
}

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

void doTemp( void )
{
	NTC_SENSOR_ON;
	
	ADMUX |= (1<<REFS0)|(1<<MUX0);					// ref. voltage = AVcc, channel = ADC1
	ADCSRA |= (1<<ADPS2)|(1<<ADIE);					// 1:16 prescaler
	ADCSRA |= (1<<ADEN);							// enable ADC
	ADCSRA |= (1<<ADSC);							// start first conversion
}
	
int main(void)
{	
	unsigned long temp;
	
	// all pins are input by default, so there's no need to set the direction
	PORTB |= (1<<PINB0);			// PINB3 = "+" button on wheel, setting PINB3 to 1 enables the internal pull-up
	PORTB |= (1<<PINB4);			// PINB4 = "TIME" button
	PORTB |= (1<<PINB5);			// PINB5 = "MENU" button
	PORTB |= (1<<PINB6);			// PINB6 = "OK" button
	PORTB |= (1<<PINB7);			// PINB7 = "-" button on wheel
	
	DDRE = (1<<PINE6)|(1<<PINE7);	// Pin E6 / Pin E7 provide power to the motor and are both outputs
	DDRF = (1<<PINF3);				// NTC Pin (PF3) is output
	
	sei();							// Enable Global Interrupts
	
	EIMSK |= (1<<PCIE1);			// Enable interrupt-on-change interrupts for PCINT8-PCINT15 which includes the push buttons
	PCMSK1 |= (1<<PCINT8)|(1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14)|(1<<PCINT15);
	
	LCD_Init();
	Rtc_init();
	
	// start a probe run to find the "full open" and "full close" positions
	doProbe();

	runstate = NORMAL_STATE;

	// now go and measure current temperature using the NTC on ADC1
	doTemp();
	
	while (1)
	{
		if( BUTTON_TIME_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_TIME_PRESSED )
			{
				// wait until button is released
				while( BUTTON_TIME_PRESSED ) ;						
				// continue here if button is still depressed
				
				switch( runstate )
				{
					case NORMAL_STATE :
						// show "TIME" in display
						runstate = TIMESET_STATE;
						timesetphase = TIMESET_START;
						
						_delay_ms( 500 );
						
						// show time with hours blinking
						timesetphase = TIMESET_YEAR;
						break;
						
					default :
						runstate = NORMAL_STATE;
						break;
				}
				
			}			
		} // end if( BUTTON_TIME_PRESSED )
		
		if( BUTTON_OK_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_OK_PRESSED )
			{
				// wait until button is released
				while( BUTTON_OK_PRESSED ) ;						
				// continue here if button is still depressed
				
				switch( runstate )
				{
					case TIMESET_STATE :
						switch( timesetphase )
						{
							case TIMESET_YEAR :
								timesetphase = TIMESET_MONTH;
								break;
						
							case TIMESET_MONTH :
								timesetphase = TIMESET_DATE;
								break;
						
							case TIMESET_DATE :
								timesetphase = TIMESET_HOURS;
								break;
						
							case TIMESET_HOURS :
								timesetphase = TIMESET_MINUTES;
								break;
						
							case TIMESET_MINUTES :
								timesetphase = TIMESET_YEAR;
								break;
								
							default :
								break;
						}
						break;
													
					default :
						break;
				}
			}
		} // end if( BUTTON_OK_PRESSED )
		
		if( BUTTON_UP_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_UP_PRESSED )
			{
				// wait until button is released
				while( BUTTON_UP_PRESSED ) ;						
				// continue here if button is still depressed
				
				switch( runstate )
				{
					case TIMESET_STATE :
						switch( timesetphase )
						{
							case TIMESET_YEAR :
								rtc.year++;
								if( rtc.year == 9999 )
									rtc.year = 2012;
								break;
						
							case TIMESET_MONTH :
								rtc.month++;
								if( rtc.month == 13 )
									rtc.month = 1;
								break;
						
							case TIMESET_DATE :
								rtc.date++;
								if (rtc.date==32)
								{
									rtc.date=1;
								}
								else if (rtc.date==31) 
								{                    
									if ((rtc.month==4) || (rtc.month==6) || (rtc.month==9) || (rtc.month==11)) 
									{
										rtc.date=1;
									}
								}
								else if (rtc.date==30)
								{
									if(rtc.month==2)
									{
									   rtc.date=1;
									}
								}              
								else if (rtc.date==29) 
								{
									if((rtc.month==2) && (is_not_leapyear()))
									{
										rtc.date=1;
									}                
								}                          
								break;
						
							case TIMESET_HOURS :
								rtc.hour++;
								if( rtc.hour == 24 )
									rtc.hour = 0;
								break;
						
							case TIMESET_MINUTES :
								rtc.minute++;
								if( rtc.minute == 60 )
									rtc.minute = 0;
								break;
								
							default :
								break;
						}
						break;
						
					default :
						break;
				}
			}
		} // end if( BUTTON_UP_PRESSED )

		if( BUTTON_DOWN_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_DOWN_PRESSED )
			{
				// wait until button is released
				while( BUTTON_DOWN_PRESSED ) ;						
				// continue here if button is still depressed
				
				switch( runstate )
				{
					case TIMESET_STATE :
						switch( timesetphase )
						{
							case TIMESET_YEAR :
								if( rtc.year > 2012 )
									rtc.year--;
								break;
						
							case TIMESET_MONTH :
								rtc.month--;
								if( rtc.month == 0 )
									rtc.month = 12;
								break;
						
							case TIMESET_DATE :
								rtc.date--;
								if (rtc.date==0) 
								{                    
									if ((rtc.month==4) || (rtc.month==6) || (rtc.month==9) || (rtc.month==11)) 
									{
										rtc.date=30;
									}
									else if((rtc.month==2) && (is_not_leapyear()))
									{
										rtc.date=28;
									}                
									else if(rtc.month==2)
									{
									   rtc.date=29;
									}
									else
									{
										rtc.date = 31;
									}									
								}                          
								break;
						
							case TIMESET_HOURS :
								if( rtc.hour == 0 )
									rtc.hour = 23;
								else
									rtc.hour--;
								break;
						
							case TIMESET_MINUTES :
								if( rtc.minute == 0 )
									rtc.minute = 59;
								else
									rtc.minute--;
								break;
								
							default :
								break;
						}
						break;
						
					default :
						break;
				}
			}
		} // end if( BUTTON_DOWN_PRESSED )

		// go to sleep but wake up if any button is pressed
		set_sleep_mode( SLEEP_MODE_ADC );
		sleep_mode();
	} // end while forever

}

ISR(LCD_vect) 
{ 
	switch( runstate )
	{
		case PROBING_STATE :
		
			switch( probingphase )
			{
				case PROBING_UNKNOWN :
					LCD_writeText( "INIT" );
					break;

				case PROBING_END_CCW :
					LCD_writeText( "OPEN" );
					break;

				case PROBING_END_CW :
					LCD_writeText( "CLSE" );
					break;

				case PROBING_START :
					LCD_writeNum( adcVref );
					break;

				case PROBING_END :
					LCD_writeText( "DONE" );
					break;

				default :
					LCD_writeNum( adcValue );
					break;
			}
			break;
						
		case TIMESET_STATE :
		
			switch( timesetphase )
			{
				case TIMESET_START :
					LCD_writeText( "0IME" );
					break;
		
				case TIMESET_YEAR :
					LCD_blinkYears();
					break;
		
				case TIMESET_MONTH :
					LCD_blinkMonths();
					break;
		
				case TIMESET_DATE :
					LCD_blinkDate();
					break;
		
				case TIMESET_HOURS :
					LCD_blinkHours();
					break;
		
				case TIMESET_MINUTES :
					LCD_blinkMinutes();
					break;
					
				default:
					break;
			}			
		
		default:
			LCD_writeNum( adcTemp );

		break;
	}				
}

ISR( ADC_vect )
{
	unsigned long temp;
	
	adcValue = ADCL;   
	adcValue += (ADCH<<8);

	switch( runstate )
	{
		case NORMAL_STATE :
			temp = adcValue;
			temp *= adcVref;
			temp /= 1024;			// this is the measured voltage x 1000
			
			temp *= 120;
			temp /= adcVref;		// in k Ohms, this is the calculated R of the NTC based on the Vadc and Vref measured earlier
			
			adcTemp = findNTCTemp( (unsigned int)temp );
			break;
			
		case PROBING_STATE :
			
			if( probingphase == PROBING_START )
			{
				temp = adcValue;
				temp *= 3000;
				temp /= 1024;
				adcVref = temp;		// in V x 1000
			}

			if( probingphase == PROBING_RUNNING_CW )
			{
				if( adcValue < ADC_THRESHOLD_CLOSE )
					probingphase = 	PROBING_END_CW;
			}
			else if( probingphase == PROBING_RUNNING_CCW )
			{
				if( adcValue < ADC_THRESHOLD_CLOSE )
					probingphase = PROBING_END_CCW;
			}
			break;
			
		default:
			break;
	}	
			
	ADCSRA |= (1<<ADSC);							// restart conversion
}

ISR( PCINT1_vect )
{
}	
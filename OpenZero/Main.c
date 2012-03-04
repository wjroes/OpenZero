/*
 * Main.c
 *
 * Created: 15-2-2012 21:16:33
 *  Author: Willem
 */ 
#ifndef F_CPU
	#define F_CPU					1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "Main.h"
#include "Time/Rtc.h"
#include "Lcd/Lcd.h"
#include "Lcd/ZeroLcd.h"
#include "Valve/Valve.h"
#include "Temperature/Temp.h"
#include "Controls/Controls.h"

volatile RUNSTATE runstate = NORMAL_STATE;	
volatile TIMESETPHASE timesetphase = TIMESET_UNKNOWN;
volatile PROBINGPHASE probingphase = PROBING_UNKNOWN;
volatile MAINMENU mainmenu = TEMP;

volatile unsigned int adcValue = 0;						// in V x 1000
volatile unsigned int adcVref = 3000;					// in V x 1000
volatile unsigned int adcTemp = 0;

volatile unsigned int revCounter = 0;

int main(void)
{
	initRTC();
	initControls();
	initLCD();
	initValve();

	// timer0 
	TCCR0A = (1<<CS02)|(1<<CS00);	// timer clock = system clock / 1024
	TIFR0 = (1<<TOV0);				// clear pending interrupts
	TIMSK0 = (1<<TOIE0);			// enable timer0 overflow Interrupt
	
	sei();							// Enable Global Interrupts
	
	// start a probe run to find the "fully open" and "fully closed" positions
	doProbe();

	// initialize the NTC sensor and start the 1st measurement
	// consequent measurements will be done every tick
	initTemp();
	
	runstate = NORMAL_STATE;
	
	while (1)
	{
		if( menuButtonPressed() )
		{
			switch( runstate )
			{
				case NORMAL_STATE :
					runstate = MENU_STATE;
					break;
						
				default :
					runstate = NORMAL_STATE;
					break;
			}
		} // end if( menuButtonPressed )
		
		if( timeButtonPressed() )
		{
			switch( runstate )
			{
				case NORMAL_STATE :
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
		} // end if( timeButtonPressed )

		if( okButtonPressed() )
		{
			switch( runstate )
			{
				case MENU_STATE :
					switch( mainmenu )
					{
						case TEMP :
							runstate = TEMPSET_STATE;
							break;
							
						case TIME :
							runstate = TIMESET_STATE;
							timesetphase = TIMESET_START;
						
							_delay_ms( 500 );
						
							// show time with hours blinking
							timesetphase = TIMESET_YEAR;
							break;
							
						default:
							break;
					}
					break;					

				case TEMPSET_STATE :
					runstate = MENU_STATE;
					break;
				
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
		} // end if( okButtonPressed )
		
		ROTARYBUTTON rotaryButton = readRotaryButton();

		if( rotaryButton == ROTARY_UP )
		{
			switch( runstate )
			{
				case NORMAL_STATE :
				case MENU_STATE :
					mainmenu++;
					if( mainmenu == LAST_ITEM )
						mainmenu = 0;
				break;
						
				case TIMESET_STATE :
					increaseClock( timesetphase );
					break;
						
				case TEMPSET_STATE :
					if( targetTemp >= 500 )
						targetTemp = 0;
					else
						targetTemp += 5;
					break;
				
				default :
					break;
			}
		} // end if( BUTTON_UP_PRESSED )

		if( rotaryButton == ROTARY_DOWN )
		{
			switch( runstate )
			{
				case NORMAL_STATE :
				case MENU_STATE :
					if( mainmenu == 0 )
						mainmenu = LAST_ITEM;
					mainmenu--;
				break;
						
				case TIMESET_STATE :
					decreaseClock( timesetphase );
					break;
						
				case TEMPSET_STATE :
					if( targetTemp == 0 )
						targetTemp = MAXTEMP;
					else
						targetTemp -= 5;
					break;
				
				default :
					break;
			}
		} // end if( BUTTON_DOWN_PRESSED )

		// go to sleep but wake up if any button is pressed
		set_sleep_mode( SLEEP_MODE_ADC );
		sleep_mode();

	} // end while forever

}

ISR(LCD_vect) 
{ 
	Lcd_SymbolsOff();	
	Lcd_DaysOff();
	Lcd_FillBar( 0 );
	
	if( ADC_CONVERSION_BUSY )
		Lcd_Symbol( LOCK, 1 );
	else
		Lcd_Symbol( LOCK, 0 );
	
	switch( runstate )
	{
		case TEMPSET_STATE :
			Lcd_Symbol( ICE, 1 );
			LCD_showTemp( targetTemp );
			break;

		case MENU_STATE :
			switch( mainmenu )
			{
				case TEMP :
				LCD_writeText((unsigned char *)"TEMP");
				break;
				
				case TIME :
				LCD_writeText((unsigned char *)"TIME");
				break;
				
				default :
				LCD_writeText((unsigned char *)"MENU");
				break;
			}			
			break;
			
		case TIMESET_STATE :
			LCD_showDay();
			switch( timesetphase )
			{
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
					
				case TIMESET_START :
				default:
					LCD_writeText( (unsigned char *)"TIME" );
					break;
			}			
			break;
			
		case PROBING_STATE :
			switch( probingphase )
			{
				case PROBING_UNKNOWN :
					LCD_writeText( (unsigned char *)"INIT" );
					break;

				case PROBING_START :
					LCD_writeNum( adcVref );
					break;

				case PROBING_RUNNING_CCW :
					Lcd_Symbol( INSIDE, 1);
					LCD_writeNum( adcValue );
					break;
					
				case PROBING_END_CCW :
					LCD_writeText( (unsigned char *)"CLSD" );
					break;

				case PROBING_RUNNING_CW :
					Lcd_Symbol( OUTSIDE, 1);
					LCD_writeNum( adcValue );
					break;
				
				case PROBING_END_CW :
					LCD_writeText( (unsigned char *)"OPEN" );
					break;

				case PROBING_END :
					LCD_writeText( (unsigned char *)"DONE" );
					break;

				default :
					LCD_writeNum( revCounter );
					break;
			}
			break;
						
		default:
			switch( mainmenu )
			{
				case TEMP :
					LCD_showTemp( adcTemp );
					break;
					
				case TIME :
					LCD_showDay();
					LCD_showTime();
					LCD_showSecondsBar();
					break;
					
				default:
					Lcd_Symbol(BAG, 1 );
					LCD_writeNum( adcValue );
					break;
			}
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
			// Rt = 120k * adc / (1024 - adc )
			temp = 120 * adcValue;
			temp /= (1024 - adcValue );			// in k Ohms, this is the calculated R of the NTC based on Vcc
			
			adcTemp = findNTCTemp( (unsigned int)temp );

			ADCSRA &= ~(1<<ADEN)|(1<<ADIE);		// disable ADC
			NTC_SENSOR_OFF;
		
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
				if( adcValue < ADC_THRESHOLD_OPEN )
				{
					LCD_writeText( (unsigned char *)"REVS");
					_delay_ms( 1000 );
					LCD_writeNum( revCounter );
					revCounter = 0;
					_delay_ms( 3000 );
					
					probingphase = 	PROBING_END_CW;
				}					
			}
			else if( probingphase == PROBING_RUNNING_CCW )
			{
				if( adcValue < ADC_THRESHOLD_CLOSE )
				{
					LCD_writeText( (unsigned char *)"REVS");
					_delay_ms( 1000 );
					LCD_writeNum( revCounter );
					revCounter = 0;
					_delay_ms( 3000 );
					
					probingphase = PROBING_END_CCW;
				}				
			}
			
			ADCSRA |= (1<<ADSC);							// restart conversion
			break;
			
		default:
			break;
	}	
}

ISR(TIMER0_OVF_vect) 
{ 
	LCD_tick();
	
	NTC_SENSOR_ON;
	ADCSRA |= (1<<ADSC);							// start conversion

}

ISR( PCINT0_vect )
{
	// opto-sensor pulse
	revCounter++;
}	

ISR( PCINT1_vect )
{
}	


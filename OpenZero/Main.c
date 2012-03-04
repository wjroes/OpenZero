/*
 * Main.c
 *
 * Created: 15-2-2012 21:16:33
 *  Author: Willem
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "Main.h"
#include "Time/Rtc.h"
#include "Lcd/Lcd.h"
#include "Lcd/ZeroLcd.h"
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

volatile unsigned char menuItem = 0;

volatile unsigned char *lcd_text = "    ";

int main(void)
{
	initLCD();
	initRTC();
	initControls();
	
	// timer0 
	TCCR0A = (1<<CS02)|(1<<CS00);	// timer clock = system clock / 1024
	TIFR0 = (1<<TOV0);				// clear pending interrupts
	TIMSK0 = (1<<TOIE0);			// enable timer0 overflow Interrupt
	
	sei();							// Enable Global Interrupts
	
	// start a probe run to find the "fully open" and "fully closed" positions
//	doProbe();

	initTemp();
	
	runstate = NORMAL_STATE;
	
	while (1)
	{
		if( BUTTON_MENU_PRESSED )
		{
			_delay_ms( DEBOUNCE_TIME );
			
			// poll again after a debounce period
			if( BUTTON_MENU_PRESSED )
			{
				// wait until button is released
				while( BUTTON_MENU_PRESSED ) ;						
				// continue here if button is still depressed
				
				switch( runstate )
				{
					case NORMAL_STATE :
						runstate = MENU_STATE;
						break;
						
					default :
						runstate = NORMAL_STATE;
						break;
				}
				
			}			
		} // end if( BUTTON_MENU_PRESSED )
		
		
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
		
		ROTARYBUTTON rotaryButton = readRotaryButton();

		if( rotaryButton == ROTARY_UP )
		{
			switch( runstate )
			{
				case MENU_STATE :
					if( mainmenu < MAX_MENU_ITEMS )
						mainmenu++;
					else
						mainmenu = 0;
				break;
						
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
		} // end if( BUTTON_UP_PRESSED )

		if( rotaryButton == ROTARY_DOWN )
		{
			switch( runstate )
			{
				case MENU_STATE :
					if( mainmenu > 0 )
						mainmenu--;
					else
						mainmenu = MAX_MENU_ITEMS;
				break;
						
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
		} // end if( BUTTON_DOWN_PRESSED )

		// go to sleep but wake up if any button is pressed
		set_sleep_mode( SLEEP_MODE_ADC );
		sleep_mode();

	} // end while forever

}

ISR(LCD_vect) 
{ 
	if( bit_is_set( ADCSRA, ADSC ) )
	{
		Lcd_Symbol( LOCK, 1 );
	}
	else
	{
		Lcd_Symbol( LOCK, 0 );
	}				
	
	switch( runstate )
	{
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
					LCD_writeNum( adcValue );
					break;
					
				case PROBING_END_CCW :
					LCD_writeText( (unsigned char *)"OPEN" );
					break;

				case PROBING_RUNNING_CW :
					LCD_writeNum( adcValue );
					break;
				
				case PROBING_END_CW :
					LCD_writeText( (unsigned char *)"CLSE" );
					break;

				case PROBING_END :
					LCD_writeText( (unsigned char *)"DONE" );
					break;

				default :
					LCD_writeNum( adcValue );
					break;
			}
			break;
						
		case TIMESET_STATE :
		
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
		
		default:
//			LCD_writeNum( adcValue );
			LCD_showTemp( adcTemp );
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
				if( adcValue < ADC_THRESHOLD_CLOSE )
				{
					probingphase = 	PROBING_END_CW;
				}					
			}
			else if( probingphase == PROBING_RUNNING_CCW )
			{
				if( adcValue < ADC_THRESHOLD_OPEN )
				{
					probingphase = PROBING_END_CCW;
					
					// reset the revcounter
					revCounter = 0;
				}				
			}
			break;
			
		default:
			break;
	}	
			
	ADCSRA |= (1<<ADSC);							// restart conversion
}

ISR(TIMER0_OVF_vect) 
{ 
	switch( runstate )
	{
		case NORMAL_STATE :
			// if NTC sensor is off
			// start a temp check
			if( bit_is_clear( PINF, PF3 ) )
			{
				NTC_SENSOR_ON;
				ADCSRA |= (1<<ADSC);							// start conversion
			}
			break;
			
		default:
			break;
	}	
}

ISR( PCINT0_vect )
{
	// opto-sensor pulse
	revCounter++;
}	

ISR( PCINT1_vect )
{
}	


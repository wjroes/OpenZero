/**** A V R  A P P L I C A T I O N  NOTE 1 3 4 ************************** 
 *
 * Title:           Real Time Clock
 * Version:         1.01
 * Last Updated:    12.10.98
 * Target:          ATmega103 (All AVR Devices with secondary external oscillator)
 *
 * Support E-mail:  avr@atmel.com
 *
 * Description      
 * This application note shows how to implement a Real Time Clock utilizing a secondary 
 * external oscilator. Included a test program that performs this function, which keeps
 * track of time, date, month, and year with auto leap-year configuration. 8 LEDs are used
 * to display the RTC. The 1st LED flashes every second, the next six represents the
 * minute, and the 8th LED represents the hour.
 *
 ******************************************************************************************/ 
#include "Rtc.h"

// these EEMEM variable declarations are just used as 'labels' into the EEPROM space
// we do not have to keep track ourselves where certain values are stored, note: seconds are not stored
uint8_t	EEMEM	EE_hours;
uint8_t EEMEM	EE_minutes;
uint8_t EEMEM	EE_days;
uint8_t EEMEM	EE_months;
uint16_t EEMEM	EE_years;

volatile _RTC rtc;

unsigned char calcDOW( void )
{
    unsigned char t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	unsigned int y = rtc.year;
	unsigned int result;
	
	y -= (rtc.month < 3);
    result = (y + y/4 - y/100 + y/400 + t[rtc.month-1] + rtc.date - 1) % 7;
	return (unsigned char)result;
}
   
void initRTC(void)  
{         
	rtc.second = 0;
	rtc.minute = (unsigned int)eeprom_read_byte(&EE_minutes);
	rtc.hour = (unsigned int)eeprom_read_byte(&EE_hours);
	rtc.date = (unsigned int)eeprom_read_byte(&EE_days);
	rtc.month = (unsigned int)eeprom_read_byte(&EE_months);
	rtc.year = (unsigned int)eeprom_read_word(&EE_years);
	
	rtc.dow = calcDOW();
	
	TIMSK2 = ~((1<<TOIE2)|(1<<OCIE2A));		// disable TC2 interrupt
	
    ASSR = (1<<AS2);						// set Timer/Counter2 to be asynchronous from the CPU clock 
											// with a second external clock(32,768kHz)driving it.  
    TCNT2 = 0x00;
    TCCR2A = (1<<CS22)|(1<<CS20);			// prescale the timer to be clock source / 128 to make it exactly 1 second for every overflow to occur

    while(ASSR&0b0000111);					// wait until TC2 is updated

	TIFR2 = (1<<TOV2);						// clear pending interrupts
    TIMSK2 = (1<<TOIE2);					// enable Timer/Counter2 Overflow Interrupt                             
}

void save_clock( void )
{
	if( rtc.minute!=(unsigned int)eeprom_read_byte(&EE_minutes) )
		eeprom_write_byte( &EE_minutes, (uint8_t)rtc.minute);
	if( rtc.hour!=(unsigned int)eeprom_read_byte(&EE_hours) )
		eeprom_write_byte( &EE_hours, (uint8_t)rtc.hour);
	if( rtc.date!=(unsigned int)eeprom_read_byte(&EE_days) )
		eeprom_write_byte( &EE_days, (uint8_t)rtc.date);
	if( rtc.month!=(unsigned int)eeprom_read_byte(&EE_months) )
		eeprom_write_byte( &EE_months, (uint8_t)rtc.month);
	if( rtc.year!=(unsigned int)eeprom_read_word(&EE_years) )
		eeprom_write_word( &EE_years, (uint16_t)rtc.year);
}
	
unsigned char is_not_leapyear( void )      
{
    if (!(rtc.year%100))
        return (unsigned char)(rtc.year%400);
    else
        return (unsigned char)(rtc.year%4);
}         

void update_clock( void )
{
    if (rtc.second==60)        
    {
        rtc.second=0;
        if (++rtc.minute==60) 
        {
            rtc.minute=0;
            if (++rtc.hour==24)
            {
				// update dow
				rtc.dow = calcDOW();
				
				// save the clock to eeprom every hour on the hour in case battery runs out or is replaced
				save_clock();
				
                rtc.hour=0;
                if (++rtc.date==32)
                {
                    rtc.month++;
                    rtc.date=1;
                }
                else if (rtc.date==31) 
                {                    
                    if ((rtc.month==4) || (rtc.month==6) || (rtc.month==9) || (rtc.month==11)) 
                    {
                        rtc.month++;
                        rtc.date=1;
                    }
                }
                else if (rtc.date==30)
                {
                    if(rtc.month==2)
                    {
                       rtc.month++;
                       rtc.date=1;
                    }
                }              
                else if (rtc.date==29) 
                {
                    if((rtc.month==2) && (is_not_leapyear()))
                    {
                        rtc.month++;
                        rtc.date=1;
                    }                
                }                          
                if (rtc.month==13)
                {
                    rtc.month=1;
                    rtc.year++;
                }
            }
        }
    }  
}
	
void increaseClock( TIMESETPHASE timesetphase )
{
	switch( timesetphase )
	{
		case TIMESET_YEAR :
			rtc.year++;
			if( rtc.year >= 2100 )
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
			if( rtc.hour >= 24 )
				rtc.hour = 0;
			break;
						
		case TIMESET_MINUTES :
			rtc.minute++;
			if( rtc.minute >= 60 )
				rtc.minute = 0;
			break;
								
		default :
			break;
	}
	
	rtc.dow = calcDOW();
	save_clock();
}		

void decreaseClock( TIMESETPHASE timesetphase )
{
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
	
	rtc.dow = calcDOW();
	save_clock();
}
	
void tick( void )
{
	rtc.second++;
	update_clock();
}        
 
ISR(TIMER2_OVF_vect)
{
	tick();
}  

        

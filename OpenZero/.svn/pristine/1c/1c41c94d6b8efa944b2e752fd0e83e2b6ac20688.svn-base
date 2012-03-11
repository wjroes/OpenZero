/*
 * Temp.c
 *
 * Created: 3-3-2012 14:07:33
 *  Author: Willem
 */ 
#include "Temp.h"

unsigned int targetTemp = 200;

void initTemp( void )
{
	DDRF  = (1<<DDF3);								// Pin F3 providers power to the NTC and is output
	NTC_SENSOR_ON;
	
	ADMUX = (1<<REFS0)|(1<<MUX0);					// ref. voltage = AVcc, channel = ADC1
	ADCSRA = (1<<ADPS2)|(1<<ADPS2)|(1<<ADPS2);		// 1:128 prescaler
	ADCSRA |= (1<<ADEN)|(1<<ADIE);					// enable ADC
	ADCSRA |= (1<<ADSC);							// start first conversion
}
	
unsigned char findNTCTemp( unsigned int r )
{
	// finds the best matching item in a sorted array and returns the index in the array using binary search
	unsigned int ntcTable[50] = {335,317,301,285,270,256,243,231,219,208,198,188,179,170,162,154,146,139,133,126,120,115,109,104,100,95,91,86,82,79,75,72,68,65,62,60,57,55,52,50,48,46,44,42,40,39,37,35,34,33}; // NTC R values in k Ohm from 0 to 49 in steps of 1 degree
 
 	unsigned char low = 0;
	unsigned char mid = 0;
    unsigned char high = (sizeof ntcTable / 2) - 1;	

 	while (low < high) 
	{
		mid = (high + low)>>1;

		if (ntcTable[mid]>r)
	    	low = mid + 1; 
		else
			high = mid; 
	}

	// the temperature is the index in ntcTable
	// the output is multiplied by 10 to get a more accurate reading
	if (ntcTable[mid] == r)
		return mid * 10;
	else
	{
		// interpolate between the immediate upper and lower boundaries to 
		// get to 1-decimal accuracy assuming a linear curve for the NTC between 1 degree values
		unsigned int interpolate = 10 * (r - ntcTable[high]);
		interpolate /= (ntcTable[high-1] - ntcTable[high]);
		
		return high * 10 + interpolate; 
	}	
}


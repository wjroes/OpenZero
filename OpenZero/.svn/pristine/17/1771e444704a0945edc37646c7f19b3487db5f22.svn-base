#ifndef _SEGMENT_H  
#define _SEGMENT_H

#ifndef F_CPU
	#define F_CPU					1000000UL
#endif

#define MAXBARS		24
#define MAXSYMBOLS	13

#include <avr/io.h>

typedef enum {
	LOCK,
	COLON,
	BAT,
	DOT,
	RADIO,
	ICE,
	MANU,
	MOON,
	AUTO,
	OUTSIDE,
	BAG,
	INSIDE,
	SCALE
} LCDSYMBOL;

void Lcd_Map(unsigned char position, unsigned char character);
void Lcd_Symbol( LCDSYMBOL symbol, unsigned char on );
void Lcd_Bar( unsigned char index, unsigned char on );
void Lcd_FillBar( unsigned char index );
void Lcd_Day( unsigned char dayofweek );
void Lcd_DaysOn( void );
void Lcd_DaysOff( void );
void Lcd_SymbolsOff( void );

#endif 


#ifndef _SEGMENT_H  
#define _SEGMENT_H

#define MAXBARS		24

typedef enum 
{
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

#endif 


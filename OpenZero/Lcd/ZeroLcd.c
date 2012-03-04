/*
 * 14Segment.c
 *
 * Created: 15-2-2012 21:16:33
 *  Author: Willem
 */ 

#include "ZeroLcd.h"

int lcdmap[] = 
{
//	  ABCDEFGHIJKLMN
	0b00000011111111,	// *
	0b00000011010010,	// +
	0b00000000000100,	// ,
	0b00000011000000,	// -
	0b11000111000000,	// . -> degree symbol
	0b00000000001100,	// /
	0b11111100001100,	// 0
	0b01100000000000,	// 1
	0b11011011000000,	// 2
	0b11110001000000,	// 3
	0b01100111000000,	// 4
	0b10010110000001,	// 5
	0b10111111000000,	// 6
	0b11100000000000,	// 7
	0b11111111000000,	// 8
	0b11110111000000,	// 9
	0b00000000001001,	// :
	0b00000000010010,	// ; -> |
	0b00000000001001,	// <
	0b00010011000000,	// =
	0b00000000100100,	// >
	0b11000001000010,	// ?
	0b11011101010000,	// @
	0b11101111000000,	// A
	0b11110001010010,	// B
	0b10011100000000,	// C
	0b11110000010010,	// D
	0b10011111000000,	// E
	0b10001110000000,	// F
	0b10111101000000,	// G
	0b01101111000000,	// H
	0b00000000010010,	// I
	0b01111000000000,	// J
	0b00001110001001,	// K
	0b00011100000000,	// L
	0b01101100101000,	// M
	0b01101100100001,	// N
	0b11111100000000,	// O
	0b11001111000000,	// P
	0b11111100000001,	// Q
	0b11001111000001,	// R
	0b10110111000000,	// S
	0b10000000010010,	// T
	0b01111100000000,	// U
	0b00001100001100,	// V
	0b01101100000101,	// W
	0b00000000101101,	// X
	0b00000000101010,	// Y
	0b10010000001100,	// Z
	0
};

int segmentmap[][14][2] = 
{
	// mapped out by position: 1 - 7
	// and then for every segment from A - N
	// the coordinates in the form {REG.NR,BIT.NR}, e.g. {0,2} = {LCDDR0,bit 2}
	// bits are numbered from right to left, i.e. bit 0 is rightmost bit
	{ {15,6},{15,4},{5,4},{0,5},{0,7},{15,7},{5,7},{10,5},{10,7},{10,6},{15,5},{0,6},{5,6},{5,5} }, // 1
	{ {15,3},{15,1},{0,1},{0,2},{0,4},{10,4},{5,3},{10,1},{10,3},{10,2},{15,2},{0,3},{5,2},{5,1} }, // 2
	{ {17,1},{17,3},{7,3},{2,2},{2,0},{17,0},{7,0},{12,2},{12,0},{12,1},{17,2},{2,1},{7,1},{7,2} }, // 3
	{ {17,4},{17,6},{2,6},{2,5},{2,3},{12,3},{7,4},{12,6},{12,4},{12,5},{17,5},{2,4},{7,5},{7,6} } // 4
};

int barmap[24][2] = 
{ 
	// all the 24 dots on the scale
	{11,0},{6,0},{1,0},{1,1},{6,1},{11,1},{11,2},{6,2},{1,2},{1,3},{6,3},{11,3},{11,4},{6,4},{1,4},{1,5},{6,5},{11,5},{16,6},{11,6},{6,6},{1,6},{1,7},{6,7} // BAR1-BAR24
};
	
int daymap[7][2] =
{
	{16,0},{16,1},{16,2},{16,3},{16,4},{16,5},{11,7}
};
	
void Lcd_Day( unsigned char dayofweek )
{
	for( unsigned char i = 0; i<7; i++ )
	{
		if( daymap[i][0] == 11 )
		{
			if( dayofweek == i )
				LCDDR11|=(1<<daymap[i][1]);
			else
				LCDDR11&=~(1<<daymap[i][1]);
		}
		else
		{
			if( dayofweek == i )
				LCDDR16|=(1<<daymap[i][1]);
			else
				LCDDR16&=~(1<<daymap[i][1]);
		}		
	}		
}
	
void Lcd_DaysOn( void )
{
	for( unsigned char i = 0; i<7; i++ )
	{
		if( daymap[i][0] == 11 )
			LCDDR11|=(1<<daymap[i][1]);
		else
			LCDDR16|=(1<<daymap[i][1]);
	}		
}	

void Lcd_DaysOff( void )
{
	for( unsigned char i = 0; i<7; i++ )
	{
		if( daymap[i][0] == 11 )
			LCDDR11&=~(1<<daymap[i][1]);
		else
			LCDDR16&=~(1<<daymap[i][1]);
	}		
}	

void Lcd_Bar( unsigned char index, unsigned char on )
{
	// index is 0 based position
	unsigned char lcd_register = barmap[index][0];
	unsigned char segment_number = barmap[index][1];
	unsigned char segment_bits = (1<<segment_number);
	
	switch( lcd_register )
	{
		case 1:
			if( on )
				LCDDR1|=segment_bits;
			else
				LCDDR1&=~segment_bits;
			break;
		case 6:
			if( on )
				LCDDR6|=segment_bits;
			else
				LCDDR6&=~segment_bits;
			break;
		case 11:
			if( on )
				LCDDR11|=segment_bits;
			else
				LCDDR11&=~segment_bits;
			break;
		case 16:
			if( on )
				LCDDR16|=segment_bits;
			else
				LCDDR16&=~segment_bits;
			break;
		default :
			break;
	}
		
}	

void Lcd_FillBar( unsigned char index )
{
	for( unsigned char i = 0; i<MAXBARS; i++ )
	{
		Lcd_Bar( i, i<index );
	}	
}
	
void Lcd_Symbol( LCDSYMBOL symbol, unsigned char on )
{
	switch( symbol )
	{
		case LOCK :
			if( on )
				LCDDR17|=(1<<7);
			else
				LCDDR17&=~(1<<7);
			break;
		case COLON :
			if( on )
				LCDDR16|=(1<<7);
			else
				LCDDR16&=~(1<<7);
			break;
		case BAT :
			if( on )
				LCDDR12|=(1<<7);
			else
				LCDDR12&=~(1<<7);
			break;
		case DOT :
			if( on )
				LCDDR7|=(1<<7);
			else
				LCDDR7&=~(1<<7);
			break;
		case RADIO :
			if( on )
				LCDDR2|=(1<<7);
			else
				LCDDR2&=~(1<<7);
			break;
		case ICE :
			if( on )
				LCDDR18|=(1<<0);
			else
				LCDDR18&=~(1<<0);
			break;
		case MANU :
			if( on )
				LCDDR15|=(1<<0);
			else
				LCDDR15&=~(1<<0);
			break;
		case MOON :
			if( on )
				LCDDR13|=(1<<0);
			else
				LCDDR13&=~(1<<0);
			break;
		case AUTO :
			if( on )
				LCDDR10|=(1<<0);
			else
				LCDDR10&=~(1<<0);
			break;
		case OUTSIDE :
			if( on )
				LCDDR8|=(1<<0);
			else
				LCDDR8&=~(1<<0);
			break;
		case INSIDE :
			if( on )
				LCDDR3|=(1<<0);
			else
				LCDDR3&=~(1<<0);
			break;
		case BAG :
			if( on )
				LCDDR5|=(1<<0);
			else
				LCDDR5&=~(1<<0);
			break;
		case SCALE :
			if( on )
				LCDDR0|=(1<<0);
			else
				LCDDR0&=~(1<<0);
			break;
		default:
			break;	
	}	
}	

void Lcd_SymbolsOff( void )
{
	for( unsigned char i = 0; i<MAXSYMBOLS; i++ )
	{
		Lcd_Symbol( i, 0 );
	}		
}
	
void Lcd_Map(unsigned char position, unsigned char character)
{
	unsigned char lcd_register;
	unsigned char segment_number;
	unsigned char segment_bits;
	unsigned char index = character - '*';
	unsigned int mapping = lcdmap[index];

	if( character == ' ' )
		mapping = 0;
	
	for(int i=0;i<14;i++)
	{
		lcd_register = segmentmap[position][i][0];
		segment_number = segmentmap[position][i][1];
		segment_bits = (1<<segment_number);
		
		switch( lcd_register )
		{
			case 0:
				if( mapping & (1<<(13-i)) )
					LCDDR0|=segment_bits;
				else
					LCDDR0&=~segment_bits;
				break;
			case 1:
				if( mapping & (1<<(13-i)) )
					LCDDR1|=segment_bits;
				else
					LCDDR1&=~segment_bits;
				break;
			case 2:
				if( mapping & (1<<(13-i)) )
					LCDDR2|=segment_bits;
				else
					LCDDR2&=~segment_bits;
				break;
			case 3:
				if( mapping & (1<<(13-i)) )
					LCDDR3|=segment_bits;
				else
					LCDDR3&=~segment_bits;
				break;
			case 5:
				if( mapping & (1<<(13-i)) )
					LCDDR5|=segment_bits;
				else
					LCDDR5&=~segment_bits;
				break;
			case 6:
				if( mapping & (1<<(13-i)) )
					LCDDR6|=segment_bits;
				else
					LCDDR6&=~segment_bits;
				break;
			case 7:
				if( mapping & (1<<(13-i)) )
					LCDDR7|=segment_bits;
				else
					LCDDR7&=~segment_bits;
				break;
			case 8:
				if( mapping & (1<<(13-i)) )
					LCDDR8|=segment_bits;
				else
					LCDDR8&=~segment_bits;
				break;
			case 10:
				if( mapping & (1<<(13-i)) )
					LCDDR10|=segment_bits;
				else
					LCDDR10&=~segment_bits;
				break;
			case 11:
				if( mapping & (1<<(13-i)) )
					LCDDR11|=segment_bits;
				else
					LCDDR11&=~segment_bits;
				break;
			case 12:
				if( mapping & (1<<(13-i)) )
					LCDDR12|=segment_bits;
				else
					LCDDR12&=~segment_bits;
				break;
			case 13:
				if( mapping & (1<<(13-i)) )
					LCDDR13|=segment_bits;
				else
					LCDDR13&=~segment_bits;
				break;
			case 15:
				if( mapping & (1<<(13-i)) )
					LCDDR15|=segment_bits;
				else
					LCDDR15&=~segment_bits;
				break;
			case 16:
				if( mapping & (1<<(13-i)) )
					LCDDR16|=segment_bits;
				else
					LCDDR16&=~segment_bits;
				break;
			case 17:
				if( mapping & (1<<(13-i)) )
					LCDDR17|=segment_bits;
				else
					LCDDR17&=~segment_bits;
				break;
			case 18:
				if( mapping & (1<<(13-i)) )
					LCDDR18|=segment_bits;
				else
					LCDDR18&=~segment_bits;
				break;
		} // switch			
	}
}
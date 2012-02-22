/*
 * 14Segment.c
 *
 * Created: 15-2-2012 21:16:33
 *  Author: Willem
 */ 

#include <avr/io.h>

#include "14Segment.h"

int lcdmap_stk504[] = 
{
//	  ABCDEFGHIJKLMN
	0b00000011111111,	// *
	0b00000011010010,	// +
	0b00000000000100,	// ,
	0b00000011000000,	// -
	0,	// .
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

int segmentmap_stk504[][14][2] = 
{
	// mapped out by position: 1 - 7
	// and then for every segment from A - N
	// the coordinates in the form {REG.NR,BIT.NR}, e.g. {0,2} = {LCDDR0,bit 2}
	// bits are numbered from right to left, i.e. bit 0 is rightmost bit
	{ {0,2},{5,3},{15,3},{15,0},{10,0},{0,0},{5,0},{10,3},{0,1},{5,1},{5,2},{10,1},{15,1},{10,2} }, // 1
	{ {0,6},{5,7},{15,7},{15,4},{10,4},{0,4},{5,4},{10,7},{0,5},{5,5},{5,6},{10,5},{15,5},{10,6} }, // 2
	{ {1,2},{6,3},{16,3},{16,0},{11,0},{1,0},{6,0},{11,3},{1,1},{6,1},{6,2},{11,1},{16,1},{11,2} }, // 3
	{ {1,6},{6,7},{16,7},{16,4},{11,4},{1,4},{6,4},{11,7},{1,5},{6,5},{6,6},{11,5},{16,5},{11,6} }, // 4
	{ {2,2},{7,3},{17,3},{17,0},{12,0},{2,0},{7,0},{12,3},{2,1},{7,1},{7,2},{12,1},{17,1},{12,2} }, // 5
	{ {2,6},{7,7},{17,7},{17,4},{12,4},{2,4},{7,4},{12,7},{2,5},{7,5},{7,6},{12,5},{17,5},{12,6} }, // 6
	{ {3,2},{8,3},{18,3},{18,0},{13,0},{3,0},{8,0},{13,3},{3,1},{8,1},{8,2},{13,1},{18,1},{13,2} }, // 7

};

void Lcd_Map_Strk504(int position, unsigned char character)
{
	int lcd_register;
	int segment_number;
	unsigned char segment_bits;
	unsigned char index = character - '*';
	unsigned int mapping = lcdmap_stk504[index];

	if( character == ' ' )
		mapping = 0;
	
	for(int i=0;i<14;i++)
	{
		lcd_register = segmentmap_stk504[position][i][0];
		segment_number = segmentmap_stk504[position][i][1];
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
//				case 4:
//				if( mapping & (1<<(13-i)) )
//					LCDDR4|=segment_bits;
//				else
//					LCDDR4&=~segment_bits;
//					break;
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
//				case 9:
//					if( mapping & (1<<(13-i)) )
//						LCDDR9|=segment_bits;
//					else
//						LCDDR9&=~segment_bits;
//					break;
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
//				case 14:
//					if( mapping & (1<<(13-i)) )
//						LCDDR14|=segment_bits;
//					else
//						LCDDR14&=~segment_bits;
//					break;
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
//				case 19:
//					if( mapping & (1<<(13-i)) )
//						LCDDR19|=segment_bits;
//					else
//						LCDDR19&=~segment_bits;
//					break;
		} // switch			
	}
}
#ifndef _LCD_H
#define _LCD_H

void LCD_Init(void);
void LCD_ticker( unsigned char *text );
void LCD_blinkYears( void );
void LCD_blinkMonths( void );
void LCD_blinkDate( void );
void LCD_blinkHours( void );
void LCD_blinkMinutes( void );
void LCD_showTime( void );
void LCD_writeText( unsigned char *text );
void LCD_writeNum( unsigned int num );

#endif
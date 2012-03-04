#ifndef _LCD_H
#define _LCD_H

#define LCD_MAX_CHARS	4

void initLCD(void);

void LCD_tick( void );

void LCD_blinkYears( void );
void LCD_blinkMonths( void );
void LCD_blinkDate( void );
void LCD_blinkHours( void );
void LCD_blinkMinutes( void );
void LCD_showDay( void );
void LCD_showSecondsBar( void );
void LCD_showTime( void );
void LCD_tickertape( unsigned char *text, unsigned char len );
void LCD_writeText( unsigned char *text );
void LCD_writeNum( unsigned int num );
void LCD_progressbar(uint16_t value, uint16_t max);
void LCD_showTemp( uint8_t temp );

#endif
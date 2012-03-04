#ifndef _RTC_H  
#define _RTC_H

typedef struct 
{ 
	unsigned char second;   //enter the current time, date, month, and year
	unsigned char minute;
	unsigned char hour;                                     
	unsigned char date;       
	unsigned char month;
	unsigned int year;      
} time;

extern volatile time rtc;

void initRTC(void);

void set_clock( void );
char is_not_leapyear( void );  

#endif 


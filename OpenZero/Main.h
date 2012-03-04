#ifndef _MAIN_H
#define _MAIN_H

#ifndef F_CPU
	#define F_CPU					1000000UL
#endif

typedef enum 
{
	NORMAL_STATE,
	MENU_STATE,
	PROBING_STATE,
	TIMESET_STATE,
} RUNSTATE;
	
typedef enum 
{
	TEMP,
	TIME
} MAINMENU;

typedef enum 
{
	TIMESET_UNKNOWN,
	TIMESET_START,
	TIMESET_HOURS,
	TIMESET_MINUTES,
	TIMESET_DATE,
	TIMESET_MONTH,
	TIMESET_YEAR,
	TIMESET_END
} TIMESETPHASE;

typedef enum 
{
	PROBING_UNKNOWN,
	PROBING_START,
	PROBING_RUNNING_CCW,
	PROBING_END_CCW,
	PROBING_RUNNING_CW,
	PROBING_END_CW,
	PROBING_END
} PROBINGPHASE;

extern volatile RUNSTATE runstate;	
extern volatile TIMESETPHASE timesetphase;
extern volatile PROBINGPHASE probingphase;
extern volatile MAINMENU mainmenu;


#endif
#ifndef _PRESS_KEY_H
#define _PRESS_KEY_H

#define SHORT_KEY_COUNTER 			10
#define SPEED_UP_KEY_COUNTER 		260
#define SPEED_UP_INTERVAL_TIME  50
#define LONG_KEY_COUNTER   			240

#define PRESSED  							1
#define RELEASE	 							0

#define NO_KEY              	0

#define BACK_KEY							6
#define ENTER_KEY							5
#define RIGHT_KEY							4
#define LEFT_KEY							3
#define DOWN_KEY    					2
#define UP_KEY								1
#define POWER_KEY             0x10
#define BACK_LONG_KEY					0x86
#define ENTER_LONG_KEY				0x85
#define RIGHT_LONG_KEY				0x84
#define LEFT_LONG_KEY					0x83
#define DOWN_LONG_KEY					0x82  
#define UP_LONG_KEY						0x81

typedef struct
{
	volatile unsigned char raw_value;
	volatile unsigned char code_value;
	unsigned char state;
	unsigned char flag;
	volatile unsigned int counter;
	unsigned char attr;
}key_manage;

extern key_manage key;

extern void key_init(void);
extern void key_value_affirm(void);
extern void key_clear(void);
extern unsigned char key_speed_up_attr(void);

#endif


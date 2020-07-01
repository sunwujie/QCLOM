#ifndef _MISC_H
#define _MISC_H

#define GREEN_LED    	0
#define YELL_LED			1
#define RED_LED				2

#define ON		1
#define OFF		0
#define ENABLE 1
#define DISABLE 0

#define  PUMP_OFF_STATE			0
#define  PUMP_ON_STATE   		1
#define  PUMP_FAULT_STATE  	2
//#define  PUMP_EXIT_STATE    3

extern signed char sys_temperatue;


typedef struct
{
	unsigned char state;
	unsigned char counter;
	unsigned int  on_time;
	//unsigned int  off_time;
	unsigned char flag;
}miscStr;

//extern void beep_init(void);

//extern unsigned char beep_set_state(unsigned char state);
//extern void beep_off(void);
//extern unsigned char beep_on(unsigned char on_time,unsigned char off_time);
//extern void beep_driver(void);

//extern void alarm_motor_init(void);
//extern unsigned char alarm_motor_set_state(unsigned char state);
//extern void alarm_motor_off(void);
//extern unsigned char alarm_motor_on(unsigned char on_time,unsigned char off_time);
//extern void alarm_motor_driver(void);

//extern void led_init(void);
//extern void led_off(unsigned char led_num);
//extern unsigned char led_on(unsigned char led_num,unsigned char on_time,
//																								unsigned char off_time);
//extern void led_driver(void);

extern void pump_init(void);
extern void pump_change_state(void);
extern unsigned char pump_set_state(unsigned char state);
extern unsigned char pump_get_state(void);
extern unsigned char pump_get_flag(void);
extern unsigned char pump_get_current_value(void);
extern unsigned char pump_get_sample_value(void);
extern void pump_clear_alarm_info(void);
extern unsigned char pump_state_check(void);

extern unsigned char temperatue_get_value(void);

#endif


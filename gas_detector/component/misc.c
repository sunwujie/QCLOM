#include "board.h"
#include "misc.h"

//////泵电流值范围.///////////////////
#define  PUMP_CURR_HIGH_LIMIT   	190 
#define  PUMP_CURR_LOW_LIMIT    	50  
#define  PUMP_FAULT_COUNTER   		20

//miscStr beep;
miscStr pump;
//miscStr alarm_motor;
//miscStr led[3];
static unsigned char pump_current=0;
signed char sys_temperatue;

#if 0
void beep_init(void)
{
	beep.counter=0;
	beep.off_time=0;
	beep.on_time =0;
	beep.state = OFF;
	beep.flag = ENABLE;
}


unsigned char beep_set_state(unsigned char state)
{
	if((state != DISABLE)&&(state != ENABLE))
		return 1;
	beep.flag = state;
	if(state == DISABLE)
	{
		BEEP_OFF;
	}
	return 0;
}

void beep_off(void)
{
	BEEP_OFF;
	beep.counter=0;
	beep.off_time=0;
	beep.on_time=0;
	beep.state=OFF;
}

unsigned char beep_on(unsigned char on_time,unsigned char off_time)
{
	if(beep.flag == DISABLE)
	{
		return 1;
	}
	if(beep.on_time == 0)
	{
		return 2;
	}
	beep.on_time=on_time;
	beep.off_time=off_time;
	beep.state =ON;
	BEEP_ON;
	
	return 0; 
}

void beep_driver(void)
{
	if(beep.flag == DISABLE)
	{
		return;
	}
	if(beep.state == ON)
	{
		if(beep.on_time > 0)
		{
			beep.on_time--;
			if(beep.on_time == 0)
			{
				BEEP_OFF;
			}
		}
		else
		{
			if(beep.off_time > 0)
			{
				beep.off_time--;
			}
		}
	}
}

void alarm_motor_init(void)
{
	alarm_motor.counter=0;
	alarm_motor.off_time=0;
	alarm_motor.on_time =0;
	alarm_motor.state = OFF;
	alarm_motor.flag = ENABLE;
}

unsigned char alarm_motor_set_state(unsigned char state)
{
	if((state != DISABLE)&&(state != ENABLE))
		return 1;
	alarm_motor.flag = state;
	if(state == DISABLE)
	{
		MOTOR_OFF;
	}
	return 0;
}

void alarm_motor_off(void)
{
	MOTOR_OFF;
	alarm_motor.counter=0;
	alarm_motor.off_time=0;
	alarm_motor.on_time=0;
	alarm_motor.state=OFF;
}


unsigned char alarm_motor_on(unsigned char on_time,unsigned char off_time)
{
	if(alarm_motor.flag == DISABLE)
	{
		return 1;
	}
	if(alarm_motor.on_time == 0)
	{
		return 2;
	}
	alarm_motor.on_time=on_time;
	alarm_motor.off_time=off_time;
	alarm_motor.state =ON;
	MOTOR_ON;
	
	return 0; 
}

void alarm_motor_driver(void)
{
	if(alarm_motor.flag == DISABLE)
	{
		return;
	}
	if(alarm_motor.state == ON)
	{
		if(alarm_motor.on_time > 0)
		{
			alarm_motor.on_time--;
			if(alarm_motor.on_time == 0)
			{
				BEEP_OFF;
			}
		}
		else
		{
			if(alarm_motor.off_time > 0)
			{
				alarm_motor.off_time--;
			}
		}
	}
}

void led_init(void)
{
	unsigned char i;

	for(i=0;i<3;i++)
	{
		led[i].counter=0;
		led[i].flag =0;
		led[i].off_time=0;
		led[i].on_time=0;
		led[i].state=OFF;
	}
}


void led_off(unsigned char led_num)
{
	if(led_num >2)
	{
		return;
	}
	if(led_num == 0)
	{
		GREEN_LED_OFF;
	}
	else if(led_num == 1)
	{
		YELL_LED_OFF;
	}
	else
	{
		RED_LED_OFF;
	}
	led[led_num].counter=0;
	led[led_num].flag=0;
	led[led_num].off_time=0;
	led[led_num].on_time =0;
	led[led_num].state=OFF;
}

unsigned char led_on(unsigned char led_num,unsigned char on_time,
																								unsigned char off_time)
{
	if(led[led_num].on_time == 0)
	{
		return 2;
	}
	led[led_num].on_time=on_time;
	led[led_num].off_time=off_time;
	led[led_num].state =ON;
	if(led_num == 0)
	{
		GREEN_LED_ON;
	}
	else if(led_num == 1)
	{
		YELL_LED_ON;
	}
	else
	{
		RED_LED_ON;
	}
	
	return 0; 	
}


void led_driver(void)
{
	unsigned char i;

	for(i=0;i<3;i++)
	{
		if(led[i].state == ON)
		{
			if(led[i].on_time > 0)
			{
				led[i].on_time--;
				if(led[i].on_time == 0)
				{
					if(i == 0)
					{
						GREEN_LED_ON;
					}
					else if(i == 1)
					{
						YELL_LED_ON;
					}
					else
					{
						RED_LED_ON;
					}	
				}
			}
			else
			{
				if(led[i].off_time > 0)
				{
					led[i].off_time--;
				}
			}
		}
	}
}
#endif

void pump_init(void)
{
	pump.counter=0;
	pump.flag=0;
//	pump.off_time=0;
	pump.on_time=0;
	pump.state=PUMP_ON_STATE;
}

void pump_change_state(void)
{
//	if(pump.flag == 0)
//		return;
	
	if(pump.state == PUMP_ON_STATE)
	{
		pump.state = PUMP_OFF_STATE;
		PUMP_OFF;
		////关泵的时候把故障状态清除.
		pump.counter=0;
		pump.flag = 0;
	}
	else
	{
		pump.state = PUMP_ON_STATE;
		PUMP_ON;
	}
}

unsigned char pump_set_state(unsigned char state)
{
	pump.state=state;
	if(pump.state == PUMP_OFF_STATE)
	{
		////关泵的时候把故障状态清除.
		pump.counter=0;
		pump.flag = 0;
	}
//	if(pump.state == PUMP_OFF_STATE)
//	{
//		PUMP_OFF;
//	}
//	else
//	{
//		PUMP_ON;
//	}
	return 0;
}

unsigned char pump_get_state(void)
{
	return pump.state;
}

unsigned char pump_get_flag(void)
{
	return pump.flag;
}


unsigned char pump_get_current_value(void)
{
	if(pump_current_check()> 0)
	{
		pump_current= get_pumpcurrent_value();
		return 0;
	}
	else
		return 1;
}

unsigned char pump_get_sample_value(void)
{
	return pump_current;
}

void pump_clear_alarm_info(void)
{
	pump.counter=0;
	pump.flag = 0;
}

unsigned char pump_state_check(void)
{
	if((pump_current>PUMP_CURR_HIGH_LIMIT)
	||(pump_current<PUMP_CURR_LOW_LIMIT))
	{
		pump.counter++;
		if(pump.counter >= PUMP_FAULT_COUNTER)
		{
			pump.counter=PUMP_FAULT_COUNTER;
			pump.flag=PUMP_FAULT_STATE; 
			return 1;
		}
	}
	else
	{
		pump.counter=0;
		pump.flag = 0;
	}
	return 0;
}

unsigned char temperatue_get_value(void)
{	
	static signed int temp_sum=0;
	static unsigned char counter=0;
	signed char temp_value;
	
	if(Temp_value_check()> 0)
	{
		temp_value=((long int)get_Temp_value()*61-98600)/355;
		temp_sum+=temp_value;
		counter++;
		if(counter>9)
		{
			sys_temperatue = temp_sum/counter;	
			temp_sum=0;
			counter=0;
		}
		return 0;
	}
	return 1;
}


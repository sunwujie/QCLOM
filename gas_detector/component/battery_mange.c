#include "board.h"
#include "battery_mange.h"

extern volatile unsigned int sys_tick;

#define BATTERY_ONE 		0
#define BATTERY_TWO 		1
#define SELECT_NULL   	2

#define BATTERY_LEVEL_ZERO		0
#define BATTERY_LEVEL_ONE			1
#define BATTERY_LEVEL_TWO			2
#define BATTERY_LEVEL_THREE		3
#define BATTERY_LEVEL_FOUR		4

#define BAT_CONNET_LIMIT_LOW 300

battery bat[2];
//电池欠压值
unsigned int	sleep_voltage=VOLTAGE_HALF_AD;
//电池状态
unsigned char voltage_status;

unsigned char battery_init(void)
{
	bat[0].charge_time=0;
	bat[0].private_data = (void *) 0;
	bat[0].status = IDLE;
	bat[0].vol_level=BATTERY_LEVEL_ONE;
	bat[0].vol_value=VOLTAGE_HALF_AD;
	
	bat[1].charge_time=0;
	bat[1].private_data = (void *) 0;
	bat[1].status = IDLE;	
  bat[1].vol_level=BATTERY_LEVEL_ONE;
	bat[1].vol_value=VOLTAGE_HALF_AD;
	
  return 0;
}

#define BAT_BUF_SIZE 18

//////读信号值.//////
unsigned char battery_get_voltage(void)
{
	static unsigned int BAT1_buf[BAT_BUF_SIZE];
	static unsigned int BAT2_buf[BAT_BUF_SIZE];
	static unsigned char BAT_counter=0;
	unsigned char i;
	unsigned int sum1=0,sum2=0;
	
	if(BAT_one_value_check()&& BAT_two_value_check())
	{
		for(i=0;i<(BAT_BUF_SIZE-1);i++)
		{
			BAT1_buf[i]=BAT1_buf[i+1];
			BAT2_buf[i]=BAT2_buf[i+1];	
		}
		BAT1_buf[BAT_BUF_SIZE-1]=get_BAT_one_value();
		BAT2_buf[BAT_BUF_SIZE-1]=get_BAT_two_value();
		if(BAT_counter < BAT_BUF_SIZE)
			BAT_counter++;
		for(i=0;i < BAT_BUF_SIZE;i++)
		{
			sum1+=BAT1_buf[i];
			sum2+=BAT2_buf[i];
		}
		bat[0].vol_value=sum1/BAT_counter;
		bat[1].vol_value=sum2/BAT_counter;
		return 0;
	}
	else
		return 1;
}

static void battery_voltage_update(void)
{
	unsigned char i;
	
	for(i=0;i<2;i++)
	{
		if(bat[i].status == DISCHARGE_DONE)
		{
			bat[i].vol_level=BATTERY_LEVEL_ZERO;
			continue;
		}
		
		if(bat[i].vol_value >= VOLTAGE_FULL_AD)
		{
			bat[i].vol_level = BATTERY_LEVEL_FOUR;	
		}
		else if(bat[i].vol_value >= VOLTAGE_HALF_AD)
		{
			bat[i].vol_level = BATTERY_LEVEL_THREE;
		}
		else
		{
			bat[i].vol_level = BATTERY_LEVEL_ZERO;
		}
	}

	if((bat[0].status==DISCHARGE_DONE)
	&&(bat[1].vol_value<START_CHECK_LOW_VOLTAGE))
	{
		voltage_status = BATTERY_LEVEL_ZERO;
		return;
	}
	if((bat[1].status==DISCHARGE_DONE)
	&&(bat[0].vol_value<START_CHECK_LOW_VOLTAGE))
	{
		voltage_status = BATTERY_LEVEL_ZERO;
		return;
	}
	
	if((bat[0].vol_level + bat[1].vol_level)>= 2*BATTERY_LEVEL_FOUR)
	{
		voltage_status = BATTERY_LEVEL_FOUR;
	}
	else if((bat[0].vol_level + bat[1].vol_level)
	>= (BATTERY_LEVEL_THREE+BATTERY_LEVEL_FOUR))
	{
		voltage_status = BATTERY_LEVEL_THREE;
	}
	else if((bat[0].vol_level + bat[1].vol_level)>= 2*BATTERY_LEVEL_TWO)
	{
		voltage_status = BATTERY_LEVEL_TWO;
	}
	else if((bat[0].vol_level + bat[1].vol_level)>= 2*BATTERY_LEVEL_ONE)
	{
		voltage_status = BATTERY_LEVEL_ONE;	
	}
	else
	{
		voltage_status = BATTERY_LEVEL_ZERO;
	}
}

unsigned char battery_check_connect_state(void)
{
	static unsigned char connect_counter=0;
	unsigned char ret_value=0;
	
	if((EXTTERN_POWER != 0)
	&&((bat[0].vol_value<BAT_CONNET_LIMIT_LOW)
	||(bat[1].vol_value<BAT_CONNET_LIMIT_LOW)))
	{
		if(connect_counter<60)
		{
			connect_counter++;
		}
		else
		{
			ret_value=1;		
		}
	}
	else
	{
		connect_counter=0;
	}
	
	return ret_value;
}


static unsigned char battery_select_charge(void)
{
	unsigned char temp_bat = SELECT_NULL;

	if((bat[0].status == CHARGING)
	||(bat[1].status == CHARGING))
	{
		if(bat[0].status == CHARGING)
		{
			return BATTERY_ONE;
		}
		return BATTERY_TWO;
	}
	
	if((bat[0].status == CHARGE_DONE)
	&&(bat[1].status == CHARGE_DONE))
	{
		voltage_status = BATTERY_LEVEL_FOUR;
		if(bat[0].vol_value <= VOLTAGE_FULL_AD)
		{
			bat[0].status=IDLE;	
		}
		if(bat[1].vol_value <= VOLTAGE_FULL_AD)
		{
			bat[1].status=IDLE;	
		}
		return SELECT_NULL;
	}
	
	if(bat[0].vol_value < bat[1].vol_value)
	{
		if(bat[BATTERY_ONE].status == CHARGE_DONE)
		{
			temp_bat = BATTERY_TWO;
		}
		else
		{
			temp_bat = BATTERY_ONE;	
		}	
	}
	else
	{
		if(bat[BATTERY_TWO].status == CHARGE_DONE)
		{
			temp_bat = BATTERY_ONE;
		}
		else
		{
			temp_bat = BATTERY_TWO;	
		}			
	}
	return temp_bat;
}

static unsigned char do_battery_charge(unsigned char which_bat)
{
//	unsigned char temp;
	static unsigned char bat_charge_state=0;
	unsigned int bat_delay_check_timer=0;
	
	if(which_bat == SELECT_NULL)
	{
		return 1;
	}
	
	if(bat[which_bat].status == CHARGING)
	{
		if(bat[which_bat].charge_time >= CFG_CHARGE_FULL_TIME)
		{
			BAT_STOP_CHARGE;
			bat[which_bat].status = CHARGE_DONE;			
			bat[which_bat].charge_time = 0;
			return 0;
		}		
		if(bat[which_bat].vol_value >= STOP_CHARG_VOLTAGE)
		{
			if(bat_charge_state==0)
			{
				BAT_STOP_CHARGE;
				bat_delay_check_timer=sys_tick;
				bat_charge_state=1;
			}
			else 
			{
				if((sys_tick-bat_delay_check_timer)>=2000)
				{
					bat_charge_state=0;	
					bat[which_bat].status = CHARGE_DONE;			
					bat[which_bat].charge_time = 0;
					return 0;
				}
			}
		}
		else
		{
			if(bat_charge_state==1)
			{
				if(which_bat == BATTERY_ONE)
				{
					BAT_ONE_CHARGE;
					BAT_ONE_DISCHARGE;
				}
				else
				{
					BAT_TWO_CHARGE;
					BAT_TWO_DISCHARGE;
				}	
				bat_charge_state=0;
			}
		}
	}
	else
	{
		bat[which_bat].status = CHARGING;
		if(which_bat == BATTERY_ONE)
		{
			////充电的同时,选择放电.
			BAT_ONE_CHARGE;
			BAT_ONE_DISCHARGE;
			if(bat[BATTERY_TWO].status == DISCHARGING)
			{
				bat[BATTERY_TWO].status = IDLE;
			}
		}
		else
		{
			////充电的同时,选择放电.
			BAT_TWO_CHARGE;
			BAT_TWO_DISCHARGE;
			if(bat[BATTERY_ONE].status == DISCHARGING)
			{
				bat[BATTERY_ONE].status = IDLE;
			}
		}
	}
	return 0;
}

static unsigned char battery_select_discharge(void)
{
	unsigned char temp_bat = SELECT_NULL;

	if((bat[0].status == DISCHARGE_DONE)
	&&(bat[1].status == DISCHARGE_DONE))
	{
		return SELECT_NULL;
	}	

	if((bat[0].status == DISCHARGING)
	||(bat[1].status == DISCHARGING))
	{
		if(bat[0].status == DISCHARGING)
		{
			return BATTERY_ONE;
		}
		return BATTERY_TWO;
	}	
	
	if(bat[0].vol_value < bat[1].vol_value)
	{
		if(bat[0].status == DISCHARGE_DONE)
		{
			temp_bat = BATTERY_TWO;
		}
		else
		{
			temp_bat = BATTERY_ONE;	
		}
	}
	else
	{
		if(bat[1].status == DISCHARGE_DONE)
		{
			temp_bat = BATTERY_ONE;
		}
		else
		{
			temp_bat = BATTERY_TWO;	
		}
	}
	return temp_bat;
}

static unsigned char check_battery_low_voltage(unsigned int *pdata)
{
#define BUF_LEN  3
#define SAMPLE_INTERVAL 20*200  //采样间隔为20s.

	static unsigned int data_buf[BUF_LEN];
	static unsigned char data_counter=0;
	static unsigned int get_data_timer=0;
	unsigned char i;
	unsigned char ret=0;
	unsigned char tender_counter=0;
	
	if((sys_tick-get_data_timer)<SAMPLE_INTERVAL)
		return 0;

	get_data_timer=sys_tick;
	if(data_counter<BUF_LEN)
	{
		data_buf[data_counter]=*pdata;
		data_counter++;
	}
	else
	{
		for(i=0;i<(BUF_LEN-1);i++)
		{
			if(data_buf[i]<data_buf[i+1])
			{
				tender_counter++;
			}
			data_buf[i]=data_buf[i+1];
		}
		data_buf[BUF_LEN-1]=*pdata;
		if((data_buf[0]>(data_buf[BUF_LEN-1]+7))
		&&(tender_counter>=(BUF_LEN+1)/2))
		{
			ret=1;
		}
	}
	return ret;
}

static unsigned char do_battery_discharge(unsigned char which_bat)
{
	static unsigned char real_discharge=0;
	unsigned char temp;
	unsigned char ret=0;
	
	if(which_bat == SELECT_NULL)
	{
		if(((bat[0].vol_value < POWER_OFF_VOLTAGE)&&(real_discharge==1))
		||((bat[1].vol_value < POWER_OFF_VOLTAGE)&&(real_discharge==2)))
			return 3;
		else
			return SELECT_NULL;
	}
	
	if(bat[which_bat].status == DISCHARGING)
	{
		temp = (which_bat ? 0: 1);
		if(bat[temp].status != DISCHARGE_DONE)
		{
			if(bat[which_bat].vol_value < sleep_voltage)
			{
				bat[which_bat].status = DISCHARGE_DONE;	
				if(temp == BATTERY_ONE)
				{
					BAT_ONE_DISCHARGE;
					real_discharge=1;
				}
				else
				{
					BAT_TWO_DISCHARGE;
					real_discharge=2;
				}
			}
		}
		else
		{
			if((bat[which_bat].vol_value < START_CHECK_LOW_VOLTAGE)
			||(check_battery_low_voltage(&bat[which_bat].vol_value)))
			{
				bat[which_bat].status = DISCHARGE_DONE;
				ret=2;
			}
		}
	}
	else
	{
		bat[which_bat].status = DISCHARGING;
		if(which_bat == BATTERY_ONE)
		{
			BAT_ONE_DISCHARGE;
			real_discharge=1;
		}
		else
		{
			BAT_TWO_DISCHARGE;
			real_discharge=2;
		}
	}
	return ret;
}

void battery_charge_timer(void)
{
	unsigned char i;
	static volatile unsigned char charge_counter=0;
	
	if(EXTTERN_POWER != 0)
	{
		for(i=0; i < 2;i++)
		{
			if(bat[i].status == CHARGING)
			{
				charge_counter++;
				if(charge_counter>=200)
				{
					if(bat[i].charge_time <= CFG_CHARGE_FULL_TIME)
					{
						bat[i].charge_time++;
					}
					charge_counter=0;
				}
				return;
			}
		}			
	}
}

unsigned char battery_voltage_level_get(void)
{	
	return voltage_status; 
}

unsigned int battery_low_voltage_get(void)
{
	return sleep_voltage;
}

unsigned char battery_low_voltage_set(unsigned int voltage)
{
	if(voltage < VOLTAGE_LOW_AD)
	{
		voltage = VOLTAGE_LOW_AD;
	}
	sleep_voltage = voltage;
	return 0;
}

unsigned char battery_manage(void)
{
	unsigned char ret=0;
	unsigned char which_battery;
	unsigned char i;
	
	battery_voltage_update();	
	if(EXTTERN_POWER == 0)
	{
		for(i=0; i < 2;i++)
		{
			if(bat[i].status == IDLE)
			{
				if(bat[i].vol_value < sleep_voltage)
				{
					bat[i].status = DISCHARGE_DONE;	
				}	
			}
			else if(bat[i].status == CHARGING)
			{
				BAT_STOP_CHARGE;
				bat[i].charge_time = 0;
				bat[i].status = IDLE;
			}
		}
		which_battery = battery_select_discharge();
		ret=do_battery_discharge(which_battery);
	}
	else
	{
		which_battery = battery_select_charge();
		ret=do_battery_charge(which_battery);
	}
	return ret;
}

unsigned int battery_get_sample_value(unsigned char which_bat)
{
	if(which_bat>1)
		return 0;
	
	return bat[which_bat].vol_value;
}

unsigned char battery_get_state(unsigned char which_bat)
{
	if(which_bat>1)
		return 0;
	
	return bat[which_bat].status;	
}

unsigned char battery_get_level(unsigned char which_bat)
{
	if(which_bat>1)
		return 0;
	
	return bat[which_bat].vol_level;	

}


#ifndef _BATTERY_MANAGE_H
#define _BATTERY_MANAGE_H

typedef struct 
{ 
	//电池状态
	unsigned char    status;
	//电量的等级值
	unsigned char    vol_level;
	//电量AD值
  unsigned int     vol_value;  
	//充电时间
  unsigned int     charge_time; 
	void *					 private_data;  
}battery;

/////电池报欠压后需要工作的时间(单位为分钟),之后系统进入睡眠状态.
#define CFG_LAST_WORK_TIME          		20

//////电池充满电的时间为4小时.////////////////
#define CFG_CHARGE_FULL_TIME      			4*3600  

///////停止充电电压阈值:
//     1)4.55v ADC:2484 
//     2)4.45v  ADC:2429 
#define STOP_CHARG_VOLTAGE             2429

//////开始检测低电压阈值:
//      3.48v  ADC:1900  
#define START_CHECK_LOW_VOLTAGE      	1910

/////////满电量时电压值要求大于等于3.7V对应的AD值为2020 ///////
/////4.15V电压值对应的AD值为2266.
#define VOLTAGE_FULL_AD                	2020 

////////三格电量时电压值要求大于等于3.2V对应的AD值为1748 /////
//////3.53V电压值对应的AD值为1928.//////
#define VOLTAGE_HALF_AD          				1748

////////一格电量时电压值:3.16V////////////////
#define VOLTAGE_LOW_AD          				1728

///////关机电压值:3.0V///////////////////
#define POWER_OFF_VOLTAGE          				1628

///////////电池状态描述/////////
#define IDLE 						0
#define CHARGING  			2
#define CHARGE_DONE   	3
#define DISCHARGING 		4
#define DISCHARGE_DONE 	5


extern unsigned char battery_init(void);
extern unsigned char battery_get_voltage(void);
extern unsigned char battery_check_connect_state(void);
extern void battery_charge_timer(void);
extern unsigned char battery_voltage_level_get(void);
extern unsigned int battery_low_voltage_get(void);
extern unsigned char battery_low_voltage_set(unsigned int voltage);
extern unsigned char battery_manage(void);
extern unsigned int battery_get_sample_value(unsigned char which_bat);
extern unsigned char battery_get_state(unsigned char which_bat);
extern unsigned char battery_get_level(unsigned char which_bat);

#endif

#ifndef _BATTERY_MANAGE_H
#define _BATTERY_MANAGE_H

typedef struct 
{ 
	//���״̬
	unsigned char    status;
	//�����ĵȼ�ֵ
	unsigned char    vol_level;
	//����ADֵ
  unsigned int     vol_value;  
	//���ʱ��
  unsigned int     charge_time; 
	void *					 private_data;  
}battery;

/////��ر�Ƿѹ����Ҫ������ʱ��(��λΪ����),֮��ϵͳ����˯��״̬.
#define CFG_LAST_WORK_TIME          		20

//////��س������ʱ��Ϊ4Сʱ.////////////////
#define CFG_CHARGE_FULL_TIME      			4*3600  

///////ֹͣ����ѹ��ֵ:
//     1)4.55v ADC:2484 
//     2)4.45v  ADC:2429 
#define STOP_CHARG_VOLTAGE             2429

//////��ʼ���͵�ѹ��ֵ:
//      3.48v  ADC:1900  
#define START_CHECK_LOW_VOLTAGE      	1910

/////////������ʱ��ѹֵҪ����ڵ���3.7V��Ӧ��ADֵΪ2020 ///////
/////4.15V��ѹֵ��Ӧ��ADֵΪ2266.
#define VOLTAGE_FULL_AD                	2020 

////////�������ʱ��ѹֵҪ����ڵ���3.2V��Ӧ��ADֵΪ1748 /////
//////3.53V��ѹֵ��Ӧ��ADֵΪ1928.//////
#define VOLTAGE_HALF_AD          				1748

////////һ�����ʱ��ѹֵ:3.16V////////////////
#define VOLTAGE_LOW_AD          				1728

///////�ػ���ѹֵ:3.0V///////////////////
#define POWER_OFF_VOLTAGE          				1628

///////////���״̬����/////////
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

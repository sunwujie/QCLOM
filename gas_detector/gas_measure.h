#ifndef _GAS_MEASURE_H
#define _GAS_MEASURE_H

#define SMOOTH_BUFFER_SIZE  16

//高浓度淹没发生或恢复时浓度持续的个数 (<SMOOTH_BUFFER_SIZE)
#define OVERFLOW_CONTINUE_COUNT       8

//高浓度淹没持续报警时间(50ms计时) 400*50=20s 
#define PUREGAS_OVERFLOW_DELAY_TIME         200

#define STEL_BUF_SIZE				15

#define TWA_BUF_SIZE				32

#define SIGNAL_CUT_NUM      4

#define NUM_SIGNAL_OF_ONE_SECOD  		10

#define NUM_SIGNAL_OF_ONE_MINITE 		60

#define  FLA_HIGH_LIMIT       			26000
#define  FLA_LOW_LIMIT        			-13100

#define CONCN_MIN_INCREASE          50
#define CONCN_MIN_INCREASE_L       	25
#define CONCN_MIN_INCREASE_H       	75
#define CONCN_MIN_INCREASE_OVER    	150

///指定低电量标志的EEPROM写入地址.
#define CFG_LOW_POWER_ADDR          0X03FF
#define LOW_POWER_FLAG              0X5A

#define DETEC_CONFIG_ADDR						0X0400				
#define DETEC_CLIB_ADDR							0X0480
#define DETEC_PUMP_INFO_ADDR				0X0560
#define DETEC_BAT_INFO_ADDR					0X0600
#define DETEC_OTHE_INFO_ADDR        0X0620
#define DETEC_TOXIC_ADDR         		0X0640

#define DETEC_OFF           0           
#define DETEC_OK            1
#define DETEC_FAULT         2
#define DETEC_ALARM         3

#define L_ALARM						1
#define H_ALARM						2
#define OVRANG_ALARM			3
#define TWA_ALARM					4
#define STEL_ALARM				5


typedef struct
{
	unsigned char decimal;
	unsigned char unit;
	////气体量程,单位为浓度.
	unsigned short conce_FS;
	///气体高限报警点,单位为相对量程百分比.
	unsigned short alarm_high;
	///气体底限报警点,单位为相对量程百分比.
	unsigned short alarm_low;
	////气体高限报警点的报警方向.
	unsigned char alarm_high_dir;
	///气体底限报警点的报警方向.
	unsigned char alarm_low_dir;
	unsigned char flag;
	unsigned int crc_value;
}cfginfo;

typedef struct
{
	unsigned char pga;
	////气体浓度为0时的ad值.
	signed int conc_zero_AD;
	////气体浓度为满量程时的ad值.
	signed int conce_FS_AD;
	////气体标定浓度.
	unsigned int span_conc;
	////
	signed int zero_offset;
	unsigned int bias_vol;	
	unsigned int crc_value;
}clbinfo;

typedef struct 
{
	signed int buf[SMOOTH_BUFFER_SIZE];
	unsigned char index_num;
	unsigned char count_len;
}gas_data;

typedef struct
{
	///气体特性描述:
	unsigned char gas_attr;
	///传感器检测气体原始浓度.
	signed short   raw_conc;
	/////当前浓度.
	unsigned short   current_conc;
	////上次的浓度.
	unsigned short   last_conc;
	////显示的浓度.
	signed short		display_conc;
	signed short 	displcd_conc;
	////与气体特性相关的数据
	void * gas_private;
	////气体类型.
	unsigned char gas_type;
	gas_data * data;
	/////传感器满量程标志.
	unsigned char overflow_flag;
	/////传感器满量程次数.
	unsigned char overflow_counter;
	////传感器状态.
	unsigned char status;
	///传感器故障信息.
	unsigned char alarm_info;
	/////传感器校准信息.
	cfginfo cfg;
	/////传感器配置信息.
	clbinfo  clb;
}DetecStr;

typedef struct
{
	float buf[STEL_BUF_SIZE];
	unsigned char stel_index;
	unsigned char count_len;
}stel_data;

typedef struct 
{
	float buf[TWA_BUF_SIZE];
	unsigned char twa_index;
	unsigned char count_len;
}twa_data;

typedef struct
{
	unsigned char idlh_state;
	unsigned char stel_twa_state;
	//////stel_alarm报警值.///
	unsigned short stel_alarm;
	/////twa_alarm报警值.///
	unsigned short twa_alarm;
	/////idlh_alarm报警值.///
	unsigned short idlh_alarm;
	/////时间加权平均值.///
	signed short twa_conc;
	/////十五分钟短期极限值.///
	signed short stel_conc;
	signed short idlh_conc;
	//////秒钟计算器.//////
	unsigned char seconds_counter;
	///////秒钟内的浓度求和.////
	signed int  seconds_conc_sum;
	///////分钟内的浓度求和.///
	signed int  minites_conc_sum;
	//////分钟计算器./////
	unsigned char minites_counter;
	//////1分钟内的浓度平均值.////
	twa_data twa;
	//////15分钟内的浓度极限值.///
	stel_data stel;
}toxic;


extern DetecStr gst_Detec[4];

#define ELECDetec_poweroff() 			lmp910x_H2S_powerOff();\
																	lmp910x_O2_powerOff();\
																	lmp910x_CO_powerOff();

#define FLADetec_hardware_init() 	ads1110_init(gst_Detec[0].clb.pga)
#define ELECDetec_hardware_init()	lmp91000_init(gst_Detec[1].clb.pga, gst_Detec[1].clb.bias_vol,\
						gst_Detec[2].clb.pga, gst_Detec[2].clb.bias_vol, gst_Detec[3].clb.pga,\
																								gst_Detec[3].clb.bias_vol);

//extern unsigned char FLADetec_hardware_init(void);
//extern unsigned char ELECDetec_hardware_init(void);
extern void gas_data_signal_clear(void);
extern void gst_Detec_init(void);
extern unsigned char detec_load_data(void);
extern unsigned char detec_save_toxic_data(void);
extern unsigned char detec_save_config_data(unsigned char detec);
extern unsigned char detec_save_clib_data(unsigned char detec);
extern unsigned char detec_save_sleep_voltage(void);
extern unsigned char detec_save_pump_state(void);
extern unsigned char detec_save_state(void);
extern unsigned char detec_params_recovery_default(void);
extern void sensor_save_signal_value(gas_data *pgas_des,signed int signal_value);
extern unsigned char sensor_get_signal_value(void);
extern signed short gas_get_AD_value(unsigned char which_sensor);
extern unsigned short gas_get_AD_abs_value(unsigned char which_sensor);
extern unsigned char gas_conce_calc_process(unsigned char gas_type,signed char temprature);
extern unsigned short detec_get_conce(unsigned char which_detec);
extern unsigned char detec_get_state(unsigned char which_detec);
extern unsigned char detec_set_state(unsigned char which_detec,unsigned char state);
extern unsigned char sensor_state_check(unsigned char gas_type);
extern unsigned char Detec_regular_alarm_check(unsigned char gas_type);
extern unsigned char detec_toxic_alarm_check(unsigned char gas_type);
extern unsigned char detec_set_high_alarm_vaue(unsigned char gas_type,unsigned short alarm);
extern unsigned short detec_get_high_alarm_value(unsigned char gas_type);
extern unsigned char detec_set_low_alarm_vaue(unsigned char gas_type,unsigned short alarm);
extern unsigned short detec_get_low_alarm_value(unsigned char gas_type);
extern unsigned short detec_get_TWA_alarm_value(unsigned char gas_type);
extern unsigned char detec_set_TWA_alarm_value(unsigned char gas_type,unsigned short alarm);
extern unsigned short detec_get_STEL_alarm_value(unsigned char gas_type);
extern unsigned short detec_set_STEL_alarm_value(unsigned char gas_type,unsigned short alarm);
extern unsigned short detec_set_TWASTEL_state(unsigned char gas_type,unsigned short state);
extern unsigned char detec_get_TWASTEL_state(unsigned char gas_type);
extern unsigned short detec_get_conce_FS(unsigned char gas_type);
extern unsigned char detec_set_conce_FS(unsigned char gas_type,unsigned short temp_conce_FS);
extern unsigned char detec_update_clb_param(unsigned char which_detec,signed short offset);
extern unsigned char detec_update_clb_conce_FS(unsigned char which_detec,signed short span_conce);
extern unsigned char detec_config_conce_FS(unsigned char which_detec,unsigned int conce_FS);
extern unsigned char detec_hardware_config(unsigned char which_detec,unsigned char pga);
extern unsigned char detec_config_high_alarm(unsigned char which_detec,unsigned int high_alarm);
extern unsigned char detec_config_low_alarm(unsigned char which_detec,unsigned int low_alarm);


#endif



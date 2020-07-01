#ifndef _GAS_MEASURE_H
#define _GAS_MEASURE_H

#define SMOOTH_BUFFER_SIZE  16

//��Ũ����û������ָ�ʱŨ�ȳ����ĸ��� (<SMOOTH_BUFFER_SIZE)
#define OVERFLOW_CONTINUE_COUNT       8

//��Ũ����û��������ʱ��(50ms��ʱ) 400*50=20s 
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

///ָ���͵�����־��EEPROMд���ַ.
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
	////��������,��λΪŨ��.
	unsigned short conce_FS;
	///������ޱ�����,��λΪ������̰ٷֱ�.
	unsigned short alarm_high;
	///������ޱ�����,��λΪ������̰ٷֱ�.
	unsigned short alarm_low;
	////������ޱ�����ı�������.
	unsigned char alarm_high_dir;
	///������ޱ�����ı�������.
	unsigned char alarm_low_dir;
	unsigned char flag;
	unsigned int crc_value;
}cfginfo;

typedef struct
{
	unsigned char pga;
	////����Ũ��Ϊ0ʱ��adֵ.
	signed int conc_zero_AD;
	////����Ũ��Ϊ������ʱ��adֵ.
	signed int conce_FS_AD;
	////����궨Ũ��.
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
	///������������:
	unsigned char gas_attr;
	///�������������ԭʼŨ��.
	signed short   raw_conc;
	/////��ǰŨ��.
	unsigned short   current_conc;
	////�ϴε�Ũ��.
	unsigned short   last_conc;
	////��ʾ��Ũ��.
	signed short		display_conc;
	signed short 	displcd_conc;
	////������������ص�����
	void * gas_private;
	////��������.
	unsigned char gas_type;
	gas_data * data;
	/////�����������̱�־.
	unsigned char overflow_flag;
	/////�����������̴���.
	unsigned char overflow_counter;
	////������״̬.
	unsigned char status;
	///������������Ϣ.
	unsigned char alarm_info;
	/////������У׼��Ϣ.
	cfginfo cfg;
	/////������������Ϣ.
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
	//////stel_alarm����ֵ.///
	unsigned short stel_alarm;
	/////twa_alarm����ֵ.///
	unsigned short twa_alarm;
	/////idlh_alarm����ֵ.///
	unsigned short idlh_alarm;
	/////ʱ���Ȩƽ��ֵ.///
	signed short twa_conc;
	/////ʮ����Ӷ��ڼ���ֵ.///
	signed short stel_conc;
	signed short idlh_conc;
	//////���Ӽ�����.//////
	unsigned char seconds_counter;
	///////�����ڵ�Ũ�����.////
	signed int  seconds_conc_sum;
	///////�����ڵ�Ũ�����.///
	signed int  minites_conc_sum;
	//////���Ӽ�����./////
	unsigned char minites_counter;
	//////1�����ڵ�Ũ��ƽ��ֵ.////
	twa_data twa;
	//////15�����ڵ�Ũ�ȼ���ֵ.///
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



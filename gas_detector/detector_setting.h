#ifndef _DETECTOR_SETTING_H
#define _DETECTOR_SETTING_H

#define INQUIRE_FUNC      0

#define SENSOR_FAULT			6
#define PUMP_FAULT				7
#define POWER_ON					8
#define POWER_OFF					9

#define SET_YEAR          0
#define SET_MONTH         1
#define SET_DAY						2
#define SET_HOUR					3
#define SET_MIN						4
#define SET_FINISH				5

///̽��������.
#define DETEC_INIT_ZERO  	1
///̽�����궨.
#define DETEC_SPAN				2
///̽�������ޱ���ֵ.
#define SET_LALARM_VALUE	3
///̽�������ޱ���ֵ.
#define SET_HALARM_VALUE	4
////̽����TWA����ֵ.
#define SET_TWA_VALUE			5
////̽����STEL����ֵ.
#define SET_STEL_VALUE		6
////̽�����ÿ���״̬.
#define SET_PUMP_STATE		7
////̽����STEL&TWA����״̬.
#define SET_STELTWA_STATE	8
///̽����Ӳ���Լ�.
#define HARDWARE_TEST			9
////̽��������״̬.
#define SET_DETEC_STATE		10
////̽����Ƿѹֵ����.
#define DETEC_LOW_VOLTAGE	 11

extern unsigned char inquire_record(key_manage *pkey,unsigned char *selected_item);
extern unsigned char setting_time(key_manage *pkey,unsigned char *time_buf,unsigned char *set_state);

extern unsigned char select_maintain_iterm(key_manage *pkey,
																						unsigned char *selected_item);
extern unsigned char detec_measure_zero(key_manage *pkey,unsigned char gas_type,
																				unsigned char decimal,signed short *conce); //
extern unsigned char detec_measure_span(key_manage *pkey,unsigned char gas_type,
																				unsigned char decimal,unsigned int conce);
extern unsigned char detec_hardware_selftest(unsigned short conce_FS,
																								signed short *selftest_conce);
extern unsigned char maintain_affirm(key_manage *pkey,unsigned char select_iterm,
																										unsigned char *maintain_iterm);
extern unsigned int do_maintain_setting(key_manage *pkey,unsigned int *data,
																			unsigned char step,unsigned int high,unsigned int low);
extern unsigned char detec_set_pump(key_manage *pkey,unsigned char *pump_state);


#endif


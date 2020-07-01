#ifndef _GAS_ALARM_H
#define _GAS_ALARM_H

#define SLEEP_STATE					0
#define SELFTEST_STATE     	1
#define PREHEAT_STATE      	2
#define MEASURE_STATE				3 
#define CUSTERM_STATE      	4
#define MAINTAIN_STATE			5
#define RECOVRY_FACTORY_SET 6

#define cancel_preheat()   detec_preheat_time=0

extern unsigned char detec_preheat_time;


extern unsigned char detector_init(void);
extern void detector_reset_reason(void);
extern void detec_alarm_display(unsigned char status);
extern void detec_display_voltage(void);
extern unsigned char selftest(unsigned char *sys_work_state);
extern unsigned char sleep_process(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas);
extern unsigned char * measure_process(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas);
extern unsigned char customer_setting(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas);
extern unsigned char maintain_setting(unsigned char *sys_work_state,
																				key_manage *pkey,DetecStr *pgas);
extern unsigned char recovery_factory_setting(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas);
extern unsigned char preheat(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas);
extern unsigned char detec_get_signal(void);

extern unsigned char detec_state_judgemnet(DetecStr *pgas,unsigned char *pstate);

extern unsigned char detec_record_process(void);
extern void detec_alarm_slience(unsigned char key_value,unsigned char alarm_value);
extern void detec_state_display(unsigned char detec_state_value,unsigned char work_state);
void detec_backlight_process(unsigned char work_state,unsigned char key_value
																					,unsigned char detec_state_value);
extern unsigned char detec_power_down(unsigned char *sys_work_state,unsigned char power_down_flag);
extern unsigned char detec_pump_manage(unsigned char *sys_work_state);

#endif


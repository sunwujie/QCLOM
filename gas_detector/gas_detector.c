#include "component_config.h"
#include "gas_measure.h"
#include "detector_setting.h"
#include "gas_detector.h"
#include <string.h>
#include "gas_detector_cfg.h"

extern volatile unsigned int sys_tick;
extern unsigned char sys_time[];

extern void detec_proc_init(void);

const char customer_passwd[4] = {LEFT_KEY, RIGHT_KEY, LEFT_KEY, RIGHT_KEY};
const char maintain_passwd[4] = {UP_KEY, DOWN_KEY, UP_KEY, DOWN_KEY};
const char factory_passwd[6] = {UP_KEY, DOWN_KEY,UP_KEY,UP_KEY,DOWN_KEY
																																	,DOWN_KEY};

const char show_gas_AD_passwd[4] = {UP_KEY,RIGHT_KEY,RIGHT_KEY,UP_KEY};
const char show_BAT_AD_passwd[4] = {LEFT_KEY,DOWN_KEY,LEFT_KEY,DOWN_KEY};

/*
const float STEL_limit[] = 
{
     0,  //FLA
     0,  //保留
  50.0,  //CO
     0,  //CO2
  35.0,  //NO 
     0,  //保留
   5.0,  //NO2
  15.0,  //H2S 
     0,  //H2
     0,  //O2
   5.0,  //SO2 
  35.0,  //NH3
   1.0  //Cl2                                                                                            
};

const float TWA_limit[] = 
{
       0,  //FLA
       0,  //保留
    35.0,  //CO 
       0,  //CO2
    25.0,  //NO
       0,  //保留
     2.0,  //NO2
    10.0,  //H2S
       0,  //H2
       0,  //O2
     2.0,  //SO2
    25.0,  //NH3
     0.5  //Cl2    
}; 
*/

#define MEASURE_INTFACE 0
#define PASSWD_INPUT  	1 
#define PASSWD_ERROR  	2
#define SHOW_GAS_AD    	3
#define SHOW_BAT_AD     4


#define PREHEAT_TIME 				180

unsigned char detec_preheat_time=PREHEAT_TIME;

unsigned char FLA_detec_test=0;

unsigned char alarm_slience=0;

unsigned char backlight_duty_state=0;

unsigned char system_reset_flag=0;

unsigned char detector_init(void)
{
	//探测器协议初始化.
	detec_proc_init();
	key_init();
	battery_init();
	//beep_init();
	//alarm_motor_init();
//	led_init();
	pump_init();
	gst_Detec_init();
	AT24CXX_hardware_init();
	detec_load_data();
  
  return 0;
}

void detector_reset_reason(void)
{
	unsigned char reset_flag;

	reset_flag=CPU_RESET_FLAG();
	if(((reset_flag&0x01)>0)
	&&(AT24CXX_read_byte(CFG_LOW_POWER_ADDR)!=LOW_POWER_FLAG))
	{
  	system_reset_flag=1;
		CPU_RESET_SET(0);
		AT24CXX_write_byte(CFG_LOW_POWER_ADDR,0);
	}
}

void gas_unit_display(unsigned char gas_state,unsigned char gas_type)
{
	unsigned char temp_state=OFF;
	static unsigned int flash_timer[4];

	if(gas_state == DETEC_OFF)
	{
		temp_state=OFF;
	}
	else if(gas_state == DETEC_OK)
	{
		temp_state=ON;
		flash_timer[gas_type]=sys_tick;
	}
	else
	{
		if((sys_tick-flash_timer[gas_type])<110)
		{
			temp_state=ON;		
		}
		else if((sys_tick-flash_timer[gas_type])>220)
		{
			flash_timer[gas_type]=sys_tick;	
		}
	}
	switch(gas_type)
	{
		case 0: display_icon_LEL(temp_state);
		break;
		case 1: display_icon_VOL(temp_state);
		break;
		case 2: display_icon_COPPM(temp_state);
		break;
		case 3: display_icon_H2SPPM(temp_state);
		break;
		default: break;
	}
}


static void gas_name_display(unsigned char gas_state,unsigned char gas_type)
{
	unsigned char temp_state=OFF;
	static unsigned int flash_timer[4];

	if(gas_state == DETEC_OFF)
	{
		temp_state=OFF;
	}
	else if(gas_state == DETEC_OK)
	{
		temp_state=ON;
		flash_timer[gas_type]=sys_tick;
	}
	else
	{
		if((sys_tick-flash_timer[gas_type])<110)
		{
			temp_state=ON;		
		}
		else if((sys_tick-flash_timer[gas_type])>220)
		{
			flash_timer[gas_type]=sys_tick;	
		}
	}
	switch(gas_type)
	{
		case 0: display_icon_FLA(temp_state);
		break;
		case 1: display_icon_O2(temp_state);
		break;
		case 2: display_icon_CO(temp_state);
		break;
		case 3: display_icon_H2S(temp_state);
		break;
		default: break;
	}
}

static void gas_value_display(DetecStr *pgas,unsigned int gas_conc,
																								unsigned char gas_type)
{
	unsigned char postion;
//	unsigned int  temp_data;
	static unsigned int flash_timer[4]={0,0,0,0};
	unsigned char decimal_point;
	unsigned char decimal_position;
	
	if(pgas->status == DETEC_OFF){
		postion=8+gas_type*4+1;
		display_OFF(postion);		
	}
	else if(pgas->status == DETEC_FAULT){
		postion=8+gas_type*4+1;
		display_Err(postion);	
	}
	else {
		decimal_point=pgas->cfg.decimal;
		postion=8+gas_type*4;
	
		if(pgas->alarm_info & (1<<OVRANG_ALARM)){
			if((sys_tick-flash_timer[gas_type])<110){	
				
				display_num(gas_conc, postion,decimal_point, 3);
				
				if(decimal_point>0){
					decimal_position=gas_type*2+4-decimal_point;
					display_decimal_point(decimal_position);
				}
			}
			else if((sys_tick-flash_timer[gas_type])> 220){
				flash_timer[gas_type]=sys_tick;
			}
		}
		else{
			display_num(gas_conc, postion,decimal_point,3);
			if(decimal_point>0){
				decimal_position=gas_type*2+4-decimal_point;
				display_decimal_point(decimal_position);
			}
		}
	}
}


void detec_alarm_display(unsigned char status)
{
	static unsigned int alarm_timer=0;

	if(status==0)
	{
		alarm_timer=sys_tick;
		return;
	}
	
	if((sys_tick-alarm_timer)<109)
	{
		if(status & (1<<SENSOR_FAULT))
		{
			display_icon_F(ON);	
		}
		else 
		{
			if(status & (1<<L_ALARM))
			{
				display_icon_L(ON);	
			}
			if(status & (1<<H_ALARM))
			{
				display_icon_H(ON);	
			}
			if(status & (1<<TWA_ALARM))
			{
				display_icon_TWA(ON);	
			}
			if(status & (1<<STEL_ALARM))
			{
				display_icon_STEL(ON);	
			}
		}
		if(status & (1<<PUMP_FAULT))
		{
			display_icon_F(ON);	
		}	
	}
	else if((sys_tick-alarm_timer)>218)
	{
		alarm_timer=sys_tick;
	}	
}

void detec_display_voltage(void)
{
	static unsigned char energy=0;
	static unsigned int update_display_energy_timer;

	if(battery_check_connect_state())
	{
		if((sys_tick-update_display_energy_timer)<150)
		{
			display_icon_BAT(0);	
		}
		else if((sys_tick-update_display_energy_timer)>=300)
		{
			update_display_energy_timer=sys_tick;	
		}
		return;
	}
	
	if((battery_get_state(0)==CHARGING)
	||(battery_get_state(1)==CHARGING))
	{
		if((sys_tick-update_display_energy_timer)>150)
		{
			update_display_energy_timer=sys_tick;
			energy++;
			if(energy>4)
			{
				energy=0;
			}
		}
		display_icon_BAT(energy);
	}
	else
	{
		//update_display_energy_timer=sys_tick;
		energy=battery_voltage_level_get();
		if(energy==0)
		{
			if((sys_tick-update_display_energy_timer)<150)
			{
				display_icon_BAT(energy);	
			}
			else if((sys_tick-update_display_energy_timer)>=300)
			{
				update_display_energy_timer=sys_tick;	
			}
		}
		else
		{
			display_icon_BAT(energy);	
		}
	}
}

unsigned char selftest(unsigned char *sys_work_state)
{
	static unsigned char selftest_mode=0;
	static unsigned int selftest_timer=0;
	static unsigned int selftest_lcd_timer;
	
	if(selftest_mode == 0)
	{
		selftest_timer=sys_tick;
		selftest_lcd_timer=sys_tick;
		selftest_mode=1;
	}
	
	if(selftest_mode == 1)
	{
		if((sys_tick-selftest_timer)<200)
		{
			YELL_LED_ON;
			BEEP_ON;
			MOTOR_OFF;
		}
		else if((sys_tick-selftest_timer)<400)
		{
			YELL_LED_OFF;
			BEEP_OFF;
			MOTOR_ON;
		}
		else if((sys_tick-selftest_timer)<600)
		{
			if((sys_tick-selftest_timer)<500)
			{
				RED_LED_ON;
				BEEP_ON;
				MOTOR_OFF;
			}
			else
			{
				RED_LED_OFF;
				BEEP_OFF;
				MOTOR_ON;
			}
		}
		else if((sys_tick-selftest_timer)<800)
		{
			if(((sys_tick-selftest_timer)%100)<50)
			{
				RED_LED_ON;
				BEEP_ON;
				MOTOR_OFF;
			}
			else
			{
				RED_LED_OFF;
				BEEP_OFF;
				MOTOR_ON;
			}
		}
		else if((sys_tick-selftest_timer)<1000)
		{
			MOTOR_OFF;
			BEEP_OFF;
			RED_LED_OFF;
		}
		else
		{
			selftest_mode=0;
			detec_preheat_time=PREHEAT_TIME;
			*sys_work_state=PREHEAT_STATE;
		}
	}
	if((sys_tick-selftest_lcd_timer)<200)
	{
		display_state(DISPLAY_FULL);
	}
	else if((sys_tick-selftest_lcd_timer)>400)
	{
		selftest_lcd_timer=sys_tick;
	}
	
	return 0;	
}

#define BATTERY_LEVEL_ONE  		1
#define BATTERY_LEVEL_THREE		3

unsigned char sleep_process(unsigned char *sys_work_state,
																key_manage *pkey,DetecStr *pgas)
{
//	unsigned int sleep_timer;
	unsigned int power_up_counter=0;
//	static unsigned char wakeup_state=0;
	volatile unsigned int delay_counter=0;
	unsigned char detec_state_standby;
	unsigned int affirm_bat_voltage_timer;
	
#define LOW_POSITION  			23

	if(system_reset_flag==0)
	{
		mcu_poweroff();
		if(EXTTERN_POWER == OFF)
		{
			DISABLE_WDT();
			extern_int_init();
			CPU_sleep();
		}
	}
	DISABLE_INT;
	mcu_init();	
	ENABLE_WDT();
	ENABLE_INT;
	
	/////延时确定电池电量.//////		
	affirm_bat_voltage_timer=sys_tick;
	battery_init();
	while((sys_tick-affirm_bat_voltage_timer)<200)
	{
		feed_WDT();
		detec_get_signal();
		battery_manage();
	}
	if(system_reset_flag==1)
	{
		MPOW_POWER_ON;
		BEEP_POWER_ON;
		PUMP_POWER_ON;
		ADS_POWER_ON;
		system_reset_flag=0;
		*sys_work_state = MEASURE_STATE;
		return 0;
	}
	display_init();
	power_up_counter=sys_tick;
	detec_state_standby=1;
	while(*sys_work_state == SLEEP_STATE)
	{
		feed_WDT();
	//	key_value_affirm();
		detec_get_signal();
		battery_manage();
		if(pkey->raw_value == POWER_KEY)
		{
			if((sys_tick-power_up_counter)<=600)
			{
				display_state(DISPLAY_CLEAR);
				display_seg7(LOW_POSITION, 3-(sys_tick-power_up_counter)/200);
				#ifdef DEBUG_BAT 
				//	display_num(battery_get_state(0), 7,0,3);
					display_num(battery_get_state(1), 7,0,3);	
				//display_num(battery_get_sample_value(0), 7,0,4);
				display_num(battery_get_sample_value(1), 15,0,4);
				#endif
				display_update();
			}
			else
			{
				if((EXTTERN_POWER == OFF)
				&&(battery_voltage_level_get()<BATTERY_LEVEL_ONE))
				{
					display_state(DISPLAY_CLEAR);
					#ifdef DEBUG_POWERON
						//BEEP_POWER_ON;
						//BEEP_ON;
						display_num(battery_get_sample_value(0), 7,0,4);
						display_num(battery_get_sample_value(1), 15,0,4);
						display_num(battery_get_state(0), 12, 0, 3);
						display_num(battery_get_state(1), 20, 0, 3);
						display_update();
						while((sys_tick-power_up_counter)<=1600);
					#endif
					display_update();
					return 1;
				}
				BEEP_POWER_ON;
				PUMP_POWER_ON;
				if((detec_get_state(1)== DETEC_OFF)
				&&(detec_get_state(2)== DETEC_OFF)
				&&(detec_get_state(3)== DETEC_OFF))
				{
					MPOW_POWER_OFF;
				}
				else
				{
					MPOW_POWER_ON;
					delay_counter=500;
					while(delay_counter--);
					ELECDetec_hardware_init();
				}
				feed_WDT();
				if(detec_get_state(0)==DETEC_OFF)
				{
					ADS_POWER_OFF;
				}
				else
				{
					ADS_POWER_ON;
					delay_counter=400;
					while(delay_counter--);
					FLADetec_hardware_init();
				}
				#if(!PUMP_SAVE)
					pump_init();
				#endif
				gas_data_signal_clear();
				if(EXTTERN_POWER !=0)
				{
					uart1_config(1200);
				}
				backlight_duty_state=1;
				set_backlight_duty(2000);
				ds1302_init(sys_time);
				event_request(POWER_ON_EVENT);
				*sys_work_state=SELFTEST_STATE;				
			}
		}
		else
		{
			power_up_counter=sys_tick;
			if(EXTTERN_POWER == OFF)
			{
	//			wakeup_state=0;
				return 1;
			}
			else
			{
				if(detec_state_standby == 1)
				{
					MPOW_POWER_ON;
					delay_counter=400;
					while(delay_counter--);
					ELECDetec_hardware_init();
					ELECDetec_poweroff();
					detec_state_standby=0;
				}
				display_state(DISPLAY_CLEAR);
				detec_display_voltage();	
				display_update();
			}
		}
	}
	return 0;
}

//#define LBUF_LENGTH  16
#define  DIFFER_VOL  1
#define  DIFFER_GAS  3
static unsigned char power_on_flag=1;

unsigned char low_voltage_protect_O2_value(void)
{
//	static unsigned int voltage_buf[LBUF_LENGTH];
//	static unsigned int gas_buf[LBUF_LENGTH];
//	static unsigned char data_counter=0;
	static unsigned int old_voltage_value=0;
	static unsigned int old_gas_value=0;
	unsigned char which_bat;
//	unsigned char i;
	static unsigned char tender_counter=0;
	unsigned char temp=0;
	static unsigned int  power_on_delay=0;

	if(power_on_flag>0)
	{
		if(power_on_flag==1)
		{
			power_on_delay=sys_tick;
			power_on_flag=2;
		}
		else
		{
			if((sys_tick-power_on_delay)>=3400)
			{
				power_on_flag=0;
			}
		}
		temp=1;
	}
	else
	{
		if((old_gas_value+DIFFER_GAS)<gas_get_AD_value(1))
		{
			if(EXTTERN_POWER != 0)
			{
				return 0;
			}	
			if(battery_get_state(0)==DISCHARGING)
			{
				which_bat=0;			
			}
			else
			{
				which_bat=1;
			}
			if(old_voltage_value>(battery_get_sample_value(which_bat)+DIFFER_VOL))
			{
				if(tender_counter<3)
				{
					tender_counter++;
				}
				else
				{
					temp=1;
				}
			}
			else
			{
				if(old_voltage_value==battery_get_sample_value(which_bat))
				{
					tender_counter=0;	
				}
				temp=0;
			}
		}
	}
	old_gas_value=gas_get_AD_value(1);

	
/*	if(data_counter < LBUF_LENGTH)
	{
		voltage_buf[data_counter]=battery_get_sample_value(which_bat);
		data_counter++;
	}
	else
	{
		for(i=0;i<(LBUF_LENGTH-1);++i)
		{
			voltage_buf[i]=voltage_buf[i+1];		
		}
		voltage_buf[LBUF_LENGTH-1]=battery_get_sample_value(which_bat);
		
	}*/

/*	if((old_voltage_value>(battery_get_sample_value(which_bat)+DIFFER_VOL))
	&&((old_gas_value+DIFFER_GAS)<gas_get_AD_value(1)))
	{
		if(tender_counter<3)
		{
			tender_counter++;
		}
		else
		{
			temp=1;
		}
	}
	else
	{
		tender_counter=0;
	}
	old_voltage_value=battery_get_sample_value(which_bat);
	*/
	//old_gas_value=gas_get_AD_value(1);

//	if(gas_get_AD_value(1)<=40)
//	{
//		temp=1;
//	}
	return temp;
}

////////////////////////////////////////////////////
unsigned char * measure_process(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas)
{
	static unsigned int  common_timer=0;
		//		 unsigned char display_value=0;
	static unsigned char intface=MEASURE_INTFACE;
	static unsigned char passwd_counter=0;
	static char passwd[6];
	static unsigned int exit_timer=0;
	static unsigned char exit_counter=0;
	static unsigned char press_key_flag=0;
	unsigned short display_conc=0;
	unsigned char i;
	unsigned char temp;

/*	display_icon_FLA(ON);
	display_icon_O2(ON);
	display_icon_CO(ON);
	display_icon_H2S(ON);

	display_icon_LEL(ON);
	display_icon_VOL(ON);
	display_icon_COPPM(ON);
	display_icon_H2SPPM(ON);
*/

	if(FLA_detec_test ==0){
		gas_conce_calc_process(0,sys_temperatue);
	}
	if(low_voltage_protect_O2_value()==0)
		gas_conce_calc_process(1,sys_temperatue);	
	gas_conce_calc_process(2,sys_temperatue);
	gas_conce_calc_process(3,sys_temperatue);
	
	//测量界面.
	if((intface == MEASURE_INTFACE)||(intface == SHOW_GAS_AD))
	{
		if(FLA_detec_test >0){
			temp=detec_hardware_selftest(pgas->cfg.conce_FS,&(pgas->display_conc));
			if(temp==0xff)
			{
				FLA_detec_test=0;
			}
			else if((temp==1)||(temp==3))
				return (&intface);
		}
		if(pgas->display_conc > pgas->cfg.conce_FS){
			display_conc=pgas->cfg.conce_FS;	
		}
		else{
			display_conc=pgas->display_conc;
		}
		if(intface == MEASURE_INTFACE)
		{
			gas_value_display(pgas,display_conc ,0);
		}
		else
		{
			display_conc=gas_get_AD_abs_value(0);
			display_num(display_conc,8,pgas->cfg.decimal,3);
		}		
		gas_unit_display(pgas->status,0);
		gas_name_display(pgas->status,0);
		pgas++;
		//if(low_voltage_protect_O2_value()==0)
		//		gas_conce_calc_process(1,sys_temperatue);			
		if(pgas->displcd_conc > pgas->cfg.conce_FS){
			display_conc=pgas->cfg.conce_FS;	
		}
		else{
			display_conc=pgas->displcd_conc;
		}
		if(intface == MEASURE_INTFACE)
		{
			gas_value_display(pgas, display_conc,1);
		}
		else
		{
			display_conc=gas_get_AD_abs_value(1);
			display_num(display_conc,12,pgas->cfg.decimal,3);
		}
		gas_unit_display(pgas->status,1);
		gas_name_display(pgas->status,1);
		pgas++;
		//gas_conce_calc_process(2,sys_temperatue);
		if(pgas->displcd_conc > pgas->cfg.conce_FS){
			display_conc=pgas->cfg.conce_FS;	
		}
		else{
			display_conc=pgas->displcd_conc;
		}
		if(intface == MEASURE_INTFACE)
		{
			gas_value_display(pgas, display_conc,2);
		}
		else
		{
			display_conc=gas_get_AD_abs_value(2);
			display_num(display_conc,16,pgas->cfg.decimal,3);
		}		
		gas_unit_display(pgas->status,2);
		gas_name_display(pgas->status,2);
		pgas++;
		//gas_conce_calc_process(3,sys_temperatue);
		if(pgas->displcd_conc > pgas->cfg.conce_FS){
			display_conc=pgas->cfg.conce_FS;	
		}
		else{
			display_conc=pgas->displcd_conc;
		}
		if(intface == MEASURE_INTFACE)
		{
			gas_value_display(pgas, display_conc,3);
		}
		else
		{
			display_conc=gas_get_AD_abs_value(3);
			display_num(display_conc,20,pgas->cfg.decimal,3);
		}
		gas_unit_display(pgas->status,3);
		gas_name_display(pgas->status,3);
		if(intface == SHOW_GAS_AD)
		{
			if(pkey->code_value == BACK_KEY)
			{
				pkey->code_value=0;
				intface=MEASURE_INTFACE;
			}
			return (&intface);
		}
		if(pkey->raw_value ==POWER_KEY)
		{
			if(press_key_flag==0)
			{
				press_key_flag=1;
				common_timer= sys_tick;	
			}
			if((sys_tick-common_timer)>=600)
			{
				///进入关机状态.
				display_state(0);
				*sys_work_state = SLEEP_STATE;	
				press_key_flag=0;
				event_request(POWER_OFF_EVENT);
			}
		}
		else
		{
			if((press_key_flag==1)&&(sys_tick-common_timer)>=25)
			{
				intface = PASSWD_INPUT;
				exit_timer=sys_tick;
				exit_counter=0;
				press_key_flag=0;
				passwd_counter=0;
				common_timer= sys_tick;
			}	
			else
			{
				press_key_flag=0;	
			}
		}
		if(pkey->code_value == BACK_LONG_KEY)
		{
			pkey->code_value=0;
			///泵开关切换.
			pump_change_state();
		}
	}
	else if(intface == PASSWD_INPUT){
		//密码输入界面.
		for(i=0; i<passwd_counter;i++)
		{
			display_seg7(16+i, 8);
		}
		if((sys_tick-common_timer)<100){
		//	display_value=char7_underline;
			display_seg7(16+passwd_counter, char7_underline);
		}
		else if((sys_tick-common_timer)>200){
			common_timer= sys_tick;
		}
		
		if(pkey->code_value == BACK_KEY){
			pkey->code_value=0;
			intface=MEASURE_INTFACE;
			passwd_counter=0;
		}
		else if(pkey->code_value == ENTER_KEY){
			pkey->code_value=0;
			if((passwd_counter==4)
			&&(strncmp(customer_passwd,passwd,4)== 0))
			{
			//用户界面.
				intface=MEASURE_INTFACE;
				*sys_work_state=CUSTERM_STATE; 	
			}
			else if((passwd_counter==4)
			&&(strncmp(maintain_passwd,passwd,4)== 0))
			{
			////维护界面.	
				intface=MEASURE_INTFACE;
				*sys_work_state=MAINTAIN_STATE;
			}
			else if((passwd_counter==6)
			&&(strncmp(factory_passwd,passwd,6)== 0))
			{
			////恢复出厂设置界面.	
				intface=MEASURE_INTFACE;
				*sys_work_state=RECOVRY_FACTORY_SET;
			}
			else if((passwd_counter==4)
			&&(strncmp(show_gas_AD_passwd,passwd,4)== 0))
			{
				intface=SHOW_GAS_AD;
			}
			else if((passwd_counter==4)
			&&(strncmp(show_BAT_AD_passwd,passwd,4)== 0))
			{
				intface=SHOW_BAT_AD;
			}	
			else
			{
				intface = PASSWD_ERROR;
				common_timer= sys_tick;
			}
			passwd_counter=0;
			for(i=0;i<sizeof(passwd);++i)
			{
				passwd[i]=0;
			}
		}
		else if((pkey->code_value >=UP_KEY)
		&&(pkey->code_value <=RIGHT_KEY)){
			if(passwd_counter <sizeof(passwd)){
				passwd[passwd_counter]=pkey->code_value;
				++passwd_counter;
			}
			pkey->code_value=0;
		}
		if(pkey->raw_value == 0){
			if((sys_tick-exit_timer)>=1200){
				exit_timer=sys_tick;
				exit_counter++;
				if(exit_counter>=100){
					intface=MEASURE_INTFACE;
					exit_counter=0;
				}
			}
		}
		else{
			exit_timer=sys_tick;
			exit_counter=0;
		}
	}
	else if(intface == PASSWD_ERROR){
		//实现Err字符闪烁功能
		if((sys_tick-common_timer)<100)
		{
			display_Err(19);
		}
		else if((sys_tick-common_timer)>200)
		{
			common_timer= sys_tick;
			exit_counter++;
			if(exit_counter>=3)
			{
				intface=PASSWD_INPUT;	
				exit_counter=0;
			}
		}
		if(pkey->code_value >NO_KEY)
		{
			pkey->code_value=0;
			intface=PASSWD_INPUT;
		}
	}
	else if(intface == SHOW_BAT_AD){
		
		display_num(battery_get_sample_value(0), 7,0,4);
		display_num(battery_get_sample_value(1), 15,0,4);
	#ifdef DEBUG_BAT
		display_num(battery_get_state(0), 12, 0, 3);
		display_num(battery_get_state(1), 20, 0, 3);
	#else
		display_num(pump_get_sample_value(), 12,0,3);
	#endif
		display_seg7(1, char7_A);
		display_seg7(2, char7_D);
		
		if(pkey->code_value == BACK_KEY)
		{
			pkey->code_value=0;
			intface=MEASURE_INTFACE;
		}
	}

	return (&intface);
}

#define SELECT_FUNC     0
#define RECORD_INQUIRE  1
#define SET_TIME        2
///包括记录查询和时间设置.
unsigned char customer_setting(unsigned char *sys_work_state,
																					key_manage *pkey,DetecStr *pgas)
{
	static unsigned int  display_timer=0;
	static unsigned char intface=SELECT_FUNC;
	static unsigned char select_item=RECORD_INQUIRE;
	static unsigned char inquire_item=0;
	static unsigned char time_buf[6];
	static unsigned int cexit_timer;
	static unsigned char cexit_counter=0;
	unsigned char ret_value=0;
	
	if(intface == SELECT_FUNC)
	{
		if((sys_tick-display_timer)<100)
		{
			if(select_item == RECORD_INQUIRE)
			{
				display_icon_record(ON);	
			}
			else
			{
				display_icon_CLOCK(ON);
			}
		}
		else if((sys_tick-display_timer)>200)
		{
			display_timer=sys_tick;
		}
		
		if(pkey->code_value == ENTER_KEY)
		{
			pkey->code_value=0;
			intface = select_item;
			if(intface == SET_TIME)
			{
				//读取当前时间.
				ret_value=ds1302_read_time(time_buf);
				if(ret_value>0)
				{
					time_buf[0]=0x19;
					time_buf[1]=0x08;
					time_buf[2]=0x09;
					time_buf[3]=0x15;
					time_buf[4]=0x15;
					ds1302_init(time_buf);
				}
			}
			inquire_item=0;
		}
		else if(pkey->code_value == BACK_KEY)
		{
			pkey->code_value=0;
			select_item=RECORD_INQUIRE;
			*sys_work_state = MEASURE_STATE;
		}
		else if((pkey->code_value == LEFT_KEY)
		||(pkey->code_value == RIGHT_KEY))
		{
			pkey->code_value=0;
			if(select_item == RECORD_INQUIRE)
			{
				select_item = SET_TIME;
			}
			else
			{
				select_item = RECORD_INQUIRE;
			}
			display_timer=sys_tick;
		}
		
	}
	else if(intface == RECORD_INQUIRE)
	{
		display_icon_record(ON);
		ret_value=inquire_record(pkey,&inquire_item);
		//返回操作.
		if(ret_value == 1)
		{
			intface=SELECT_FUNC;	
			display_timer=sys_tick;
		}
	}
	else if(intface == SET_TIME)
	{
		display_icon_CLOCK(ON);
		ret_value=setting_time(pkey,time_buf,&inquire_item);
		//返回按键或者操作完成返回.
		if(ret_value > 0)
		{
			intface=SELECT_FUNC;	
			//操作完成,写入新的时间.
			if(ret_value == 2)
			{
				ds1302_write_time(time_buf);
			}
			display_timer=sys_tick;
		}
	}

	if(pkey->raw_value == 0){
		if((sys_tick-cexit_timer)>=1200){
			cexit_timer=sys_tick;
			cexit_counter++;
			if(cexit_counter>=100){
				*sys_work_state = MEASURE_STATE;
				cexit_counter=0;
				intface=SELECT_FUNC;
				select_item=RECORD_INQUIRE;
			}
		}
	}
	else{
		cexit_timer=sys_tick;
		cexit_counter=0;
	}
	return ret_value;
}

////
unsigned char maintain_setting(unsigned char *sys_work_state,
																				key_manage *pkey,DetecStr *pgas)
{
	static unsigned char select_item=0;
	static unsigned char maintain_item=DETEC_INIT_ZERO;
	static unsigned int setting_timer=0;
	static unsigned int  set_value;
	static unsigned char maintain_type=0;
	static unsigned int high_limit;
	static unsigned int low_limit;
	static unsigned char flash_counter=0;
	static unsigned char set_finish_flag=0;
	static unsigned char set_pump_state=0;
	static unsigned int mexit_timer;
	static unsigned char mexit_counter;
	static unsigned char flash_iterm=1;
	unsigned char ret_value=0;
	unsigned char temp;
	static unsigned char step;
	
	if(pkey->raw_value == 0){
		if((sys_tick-mexit_timer)>=1200){
			mexit_timer=sys_tick;
			mexit_counter++;
			if(mexit_counter>=100){
				*sys_work_state = MEASURE_STATE;
				maintain_item=DETEC_INIT_ZERO;
				mexit_counter=0;
				select_item=0;
				maintain_type=0;
			}
		}
	}
	else{
		mexit_timer=sys_tick;
		mexit_counter=0;
	}	
	
	if(select_item == 0){
		ret_value=select_maintain_iterm(pkey,&maintain_item);
		
		///返回按键或者者还未选择.
		if(ret_value <= 1){
			if(ret_value == 1){
				maintain_item=DETEC_INIT_ZERO;
				*sys_work_state=MEASURE_STATE;
			}
			return ret_value;
		}
		
		select_item=maintain_item;
		if(select_item == SET_PUMP_STATE){
			set_pump_state=1;
		}
		else if(select_item == DETEC_LOW_VOLTAGE){
			set_value=battery_low_voltage_get();
			high_limit=VOLTAGE_HALF_AD;
			low_limit=VOLTAGE_LOW_AD;
			maintain_type=4;
		}
		else{
			maintain_type=0;
			if((select_item==SET_TWA_VALUE)
			||(select_item==SET_STEL_VALUE)
			||(select_item==SET_STELTWA_STATE)){
				flash_iterm=3;
			}
			else{
				flash_iterm=1;
			}
		}
		setting_timer=sys_tick;
		
		return ret_value;
	}
	
	if(select_item == HARDWARE_TEST){
		FLA_detec_test=1;
		*sys_work_state=MEASURE_STATE;
		select_item=0;
		maintain_item=DETEC_INIT_ZERO;
		return 0;
	}
	
	display_seg7(1,char7_F);
	if(select_item <= SET_STELTWA_STATE){
		display_seg7(2,select_item);
		if(select_item != SET_PUMP_STATE){
			display_seg7(3,char7_minus_sign);
		}
	}
	else {
		display_seg7(2,select_item/10);
		display_seg7(3,select_item%10);
		if(select_item !=DETEC_LOW_VOLTAGE){
			display_seg7(4,char7_minus_sign);
		}
	}	
	
	if(select_item == SET_PUMP_STATE){	
		ret_value=detec_set_pump(pkey,&set_pump_state);
		if(ret_value == 1)
		{
			select_item=0;
		}
		return 0;
	}
	
	switch(select_item)
	{
		case SET_LALARM_VALUE: display_icon_L(ON);break;
		case SET_HALARM_VALUE: display_icon_H(ON);break;
		case SET_TWA_VALUE: display_icon_TWA(ON);break;
		case SET_STEL_VALUE: display_icon_STEL(ON); break; 
		case SET_STELTWA_STATE: display_icon_TWA(ON);
		display_icon_STEL(ON); break;
		default: break;
	}
	
	if(maintain_type==0){
		ret_value=maintain_affirm(pkey,select_item,&flash_iterm);
		if(ret_value <= 1){
			if(ret_value == 1)
				select_item=0;
			return ret_value;
		}
	//	else if(ret_value == 2){		
		maintain_type=flash_iterm;
		if(select_item ==SET_LALARM_VALUE)
		{
			set_value=((long)pgas[maintain_type-1].cfg.alarm_low
											*(pgas[maintain_type-1].cfg.conce_FS)+50)/1000;
			
			high_limit=((long)pgas[maintain_type-1].cfg.alarm_high
											*(pgas[maintain_type-1].cfg.conce_FS)+50)/1000;
			
			low_limit=(pgas[maintain_type-1].cfg.conce_FS+50)/100;		
		}
		else if(select_item ==SET_HALARM_VALUE)
		{
			set_value=((long)pgas[maintain_type-1].cfg.alarm_high
											*(pgas[maintain_type-1].cfg.conce_FS)+50)/1000;
			
			high_limit=pgas[maintain_type-1].cfg.conce_FS;
			
			low_limit=((long)pgas[maintain_type-1].cfg.alarm_low
											*(pgas[maintain_type-1].cfg.conce_FS)+50)/1000;
		}
		else if(select_item == SET_TWA_VALUE)
		{
			set_value=detec_get_TWA_alarm_value(maintain_type-1);
			if(pgas[maintain_type-1].cfg.decimal>0)
			{
				temp=10;
			}
			else
			{
				temp=1;
			}
			//high_limit=(unsigned int)(TWA_limit[maintain_type-1]*temp);
			if(maintain_type==3)
			{
				high_limit=35*temp;
			}
			else
			{
				high_limit=10*temp;
			}
			low_limit=0;
		}
		else if(select_item == SET_STEL_VALUE)
		{
			set_value=detec_get_STEL_alarm_value(maintain_type-1);
			if(pgas[maintain_type-1].cfg.decimal>0)
			{
				temp=10;
			}
			else
			{
				temp=1;
			}
			//high_limit=(unsigned int)(STEL_limit[maintain_type-1]*temp);
			if(maintain_type==3)
			{
				high_limit=50*temp;
			}
			else
			{
				high_limit=15*temp;
			}
			low_limit=0;
		}
		else if(select_item == SET_STELTWA_STATE)
		{
			set_value=detec_get_TWASTEL_state(maintain_type-1);
			high_limit=1;
			low_limit=0;
		}
		else if(select_item == SET_DETEC_STATE)
		{
			set_value=detec_get_state(maintain_type-1);
			high_limit=1;
			low_limit=0;
		}
		if((select_item>=SET_LALARM_VALUE)
		&&(select_item<=SET_STEL_VALUE))
		{
			step=(pgas[maintain_type-1].cfg.conce_FS+10)/100;
		}
		else
		{
			step=1;
		}
//		}
		setting_timer=sys_tick;
		return ret_value;
	}
//	if(select_item<=9)
//		display_seg7(4,maintain_type);
//	else 
	if(select_item !=DETEC_LOW_VOLTAGE)
	{
		display_seg7(5,maintain_type);
		switch(maintain_type)
		{
			case 1: display_icon_FLA(ON);
			display_icon_LEL(ON); break;
			case 2: display_icon_O2(ON);
			display_icon_VOL(ON); break;
			case 3: display_icon_CO(ON);
			display_icon_COPPM(ON); break;
			case 4: display_icon_H2S(ON);
			display_icon_H2SPPM(ON); break;
			default: break;
		}
	}
	
	if((select_item==DETEC_INIT_ZERO)
		||(select_item==DETEC_SPAN))
	{
		gas_conce_calc_process(maintain_type-1,sys_temperatue);
	}
		
	if(select_item==DETEC_INIT_ZERO)
	{
		ret_value=detec_measure_zero(pkey,maintain_type-1,pgas[maintain_type-1].cfg.decimal,
																  &(pgas[maintain_type-1].displcd_conc));
		if(ret_value==1)
		{
			maintain_type=0;
		}
		return ret_value;
	}
	else if(select_item==DETEC_SPAN)
	{
		ret_value=detec_measure_span(pkey,maintain_type-1,pgas[maintain_type-1].cfg.decimal,
																  pgas[maintain_type-1].displcd_conc);	
		if(ret_value==1)
		{
			maintain_type=0;
		}
		return ret_value;
	}
	else
	{
		if(set_finish_flag >0)
		{
			if((select_item == SET_DETEC_STATE)
			||(select_item == SET_STELTWA_STATE))
			{
				if((sys_tick-setting_timer)<200){
					if(set_value>0){
						display_ON(8+4*(maintain_type-1));
					}
					else{
						display_OFF(8+4*(maintain_type-1));
					}
				}
				else if((sys_tick-setting_timer)>400){
					setting_timer=sys_tick;
					flash_counter++;
					if(flash_counter>=3)
					{
						if(select_item == SET_DETEC_STATE)
						{
							///暂时不添加该功能,有副作用.或者需要系统复位.
							//if((maintain_type==1)
							//&&(detec_get_state(maintain_type-1)!=set_value))
							//{
							//	ADS_POWER_OFF;	
							//  ADS_POWER_ON;
							//}
							///保存探测器状态信息.///
							detec_set_state(maintain_type-1,set_value);
							detec_save_state();
						}
						else
						{
							detec_set_TWASTEL_state(maintain_type-1,set_value);
							detec_save_toxic_data();
						}
						set_finish_flag=0;
						flash_counter=0;
						maintain_type=0;
						//select_item=0;
					}
				}
			}
			else
			{
				if((sys_tick-setting_timer)<200){					
					display_num(set_value, 8+4*(maintain_type-1),
										pgas[maintain_type-1].cfg.decimal, 3);
					
					if((select_item !=DETEC_LOW_VOLTAGE)
					&&(pgas[maintain_type-1].cfg.decimal>0)){
						temp=maintain_type*2+2-pgas[maintain_type-1].cfg.decimal;
						display_decimal_point(temp);
					}
				}
				else if((sys_tick-setting_timer)>400){
					setting_timer=sys_tick;
					flash_counter++;
					if(flash_counter>=3)
					{
						//更新探测器的设置状态.
						if((select_item>=SET_LALARM_VALUE)
						&&(select_item<=SET_STEL_VALUE))
						{
							if(select_item == SET_LALARM_VALUE)
							{
								detec_set_low_alarm_vaue(maintain_type-1,set_value);	
							}
							else if(select_item == SET_HALARM_VALUE)
							{
								detec_set_high_alarm_vaue(maintain_type-1,set_value);
							}
							else if(select_item == SET_TWA_VALUE)
							{
								detec_set_TWA_alarm_value(maintain_type-1,set_value);
							}
							else if(select_item == SET_STEL_VALUE)
							{
								detec_set_STEL_alarm_value(maintain_type-1,set_value);
							}
							if((select_item == SET_LALARM_VALUE)
							||(select_item == SET_HALARM_VALUE))
							{
								detec_save_config_data(maintain_type-1);
							}
							else
							{
								detec_save_toxic_data();
							}
						}
						else if(select_item==DETEC_LOW_VOLTAGE)
						{
							detec_save_sleep_voltage();
						}
						set_finish_flag=0;
						flash_counter=0;
						maintain_type=0;
					}
				}	
			}	
			return 0;
		}
		
		if((select_item == SET_DETEC_STATE)
		||(select_item == SET_STELTWA_STATE)){
		
			if((sys_tick-setting_timer)<100){
				if(set_value>0){
					display_ON(8+4*(maintain_type-1));
				}
				else{
					display_OFF(8+4*(maintain_type-1));
				}
			}
			else if((sys_tick-setting_timer)>200){
				setting_timer=sys_tick;
			}
			
			if(pkey->code_value == BACK_KEY)
			{
				pkey->code_value=0;
				ret_value=1;							
			}
			else if(pkey->code_value == ENTER_KEY)
			{
				pkey->code_value=0;
				ret_value=2;	
			}
			else if((pkey->code_value == DOWN_KEY)
			||(pkey->code_value == UP_KEY))
			{
				pkey->code_value=0;
				if(set_value==0)
				{
					set_value=1;
				}
				else
				{
					set_value=0;
				}
				setting_timer=sys_tick;
			}
		}
		else
		{
			if((sys_tick-setting_timer)<100){					
				display_num(set_value, 8+4*(maintain_type-1),
										pgas[maintain_type-1].cfg.decimal, 3);
				
				if((select_item !=DETEC_LOW_VOLTAGE)
				&&(pgas[maintain_type-1].cfg.decimal>0)){
					temp=maintain_type*2+2-pgas[maintain_type-1].cfg.decimal;
					display_decimal_point(temp);
				}
			}
			else if((sys_tick-setting_timer)>200){
				setting_timer=sys_tick;
			}	
			ret_value=do_maintain_setting(pkey,&set_value,step,high_limit,low_limit);
		}
		//返回按键.
		if(ret_value == 1){
			maintain_type=0;
			if(select_item == DETEC_LOW_VOLTAGE){
				select_item=0;
			}
		}  ///确认按键.
		else if(ret_value == 2){
			set_finish_flag=1;
			setting_timer=sys_tick;
		}
	}		
	return ret_value;
}


unsigned char recovery_factory_setting(unsigned char *sys_work_state,
																						key_manage *pkey,DetecStr *pgas)
{
	static unsigned char select_item=0;
	static unsigned int setting_timer=0;

	if(select_item == 0)
	{
		if((sys_tick-setting_timer)<100){
			display_seg7(1, char7_F);
			display_seg7(2, 1);
			display_seg7(3, 0);
		}
		else if((sys_tick-setting_timer)>200){
			setting_timer=sys_tick;
		}	
	}
	else if(select_item == 1)
	{
		if((sys_tick-setting_timer)<100){
			display_state(DISPLAY_FULL);
		}
		else if((sys_tick-setting_timer)>200){
			setting_timer=sys_tick;
			//*sys_work_state=MEASURE_STATE;	
			select_item=0;
		} 
	}

	if(pkey->code_value == ENTER_KEY)
	{
		pkey->code_value=0;
		detec_params_recovery_default();
		clear_all_record();
		setting_timer=sys_tick;
		select_item=1;
	}
	else if(pkey->code_value == BACK_KEY)
	{
		pkey->code_value=0;
		*sys_work_state=MEASURE_STATE;	
		select_item=0;
	}
  return 0;
}

///////////预热处理///////////////
unsigned char preheat(unsigned char *sys_work_state,key_manage *pkey,
															DetecStr *pgas)
{
	static unsigned char preheat_state=0;
	static unsigned int preheat_time;
	
	if(detec_preheat_time>0)
	{
		if(preheat_state == 0)
		{
			preheat_time=sys_tick;
			preheat_state=1;
			power_on_flag=0;
		}
		else
		{
			if((sys_tick-preheat_time)>=200)
			{
				detec_preheat_time--;
				preheat_time=sys_tick;
			}
		}
	}
	else
	{
	//	beep_on(200,0);
		BEEP_ON;
		preheat_state=0;
		display_seg7(6, char7_b+MAIN_VER-0x62);
		display_seg7(7, SUB_VER-0x30);
		if((sys_tick-preheat_time)>220)
		{
			detec_preheat_time=PREHEAT_TIME;
			*sys_work_state = MEASURE_STATE;
			BEEP_OFF;
		}
	}
	display_num(detec_preheat_time, 20,0, 3);
	//display_update();
	if(pkey->code_value == BACK_LONG_KEY)
	{
		pkey->code_value=0;
		//取消预热.
		power_on_flag=1;
		detec_preheat_time=0;
		preheat_time=sys_tick;
	}
	else if(pkey->code_value == ENTER_LONG_KEY)
	{
		pkey->code_value=0;
		preheat_state=0;
		*sys_work_state=SLEEP_STATE;
		event_request(POWER_OFF_EVENT);
	}
	return 0;
}

#ifdef DEBUG_LOW_VOLTAGE
extern unsigned char send_BATVOL_Info(unsigned char *buf,unsigned char length);
#endif

unsigned char detec_get_signal(void)
{
	static unsigned char get_data_state=0;
	static unsigned int get_data_timer=0;
	unsigned char ret;
	
	if(get_data_state==0)
	{
		if((sys_tick-get_data_timer)>17)
		{
			start_get_signal_value();
			get_data_timer=sys_tick;
			get_data_state=1;
		}
	}
	else 
	{
		ret = sensor_get_signal_value();
		ret += battery_get_voltage();
		ret += temperatue_get_value();
		ret += pump_get_current_value();
		if(((sys_tick-get_data_timer)>=30)||(ret == 0)) //
		{
			get_data_state=0;
			get_data_timer=sys_tick;
		}
	}
#ifdef DEBUG_LOW_VOLTAGE
	unsigned char bvbuf[6];
	unsigned int temp;

	temp=battery_get_sample_value(0);
	bvbuf[0]=(unsigned char)(temp&0xff); 
	bvbuf[1]=(unsigned char)(temp>>8);
	temp=battery_get_sample_value(1);
	bvbuf[2]=(unsigned char)(temp&0xff); 
	bvbuf[3]=(unsigned char)(temp>>8);
	temp=gas_get_AD_value(1);
	bvbuf[4]=(unsigned char)(temp&0xff); 
	bvbuf[5]=(unsigned char)(temp>>8);
	send_BATVOL_Info(bvbuf, 6);
#endif

	return get_data_state;
}

unsigned char detec_state_judgemnet(DetecStr *pgas,unsigned char *pstate)
{
	unsigned char alarm_type=0;
	volatile unsigned char statue_value=0;
	static unsigned char old_statue_value=0;
	unsigned char i;
	
	statue_value = pump_get_flag();
	if(statue_value ==PUMP_FAULT_STATE)
	{
		if((old_statue_value & 0x01)==0)
		{
			//泵故障事件.
			event_request(PUMP_FAULT_EVENT);
			old_statue_value |= 0x01;
			alarm_slience=0;
			*pstate=MEASURE_INTFACE;
		}
		alarm_type=(1<<PUMP_FAULT);
	}
	else
	{
		if((old_statue_value & 0x01)>0)
		{
			old_statue_value &= (~0x01);
		}
	}
	
	statue_value=sensor_state_check(0);
	if(statue_value > 0)
	{
		if((old_statue_value & 0x02)==0)
		{
			///传感器故障事件.
			event_request(DETEC_ONE_FAULT_EVENT);
			old_statue_value |= 0x02;
			alarm_slience=0;
			*pstate=MEASURE_INTFACE;
		}
		alarm_type |= (1<<SENSOR_FAULT);
	}
	else
	{
		if((old_statue_value & 0x02)>0)
		{
			old_statue_value &= (~0x02);
		}	
	}
	
//#if 0	
	for(i=0; i<4; i++)
	{
		statue_value=Detec_regular_alarm_check(i);
		alarm_type |= statue_value;
		if(statue_value & (1<<L_ALARM))
		{
			if((pgas[i].alarm_info & (1<<L_ALARM))==0)
			{
				if(((0==i)&&(FLA_detec_test==0))
				||(i>0))
				{
					event_request(DETEC_ONE_A1_ALARM_EVENT+i);
				}
				pgas[i].alarm_info |= (1<<L_ALARM);	
				alarm_slience=0;
				*pstate=MEASURE_INTFACE;
			}
		}
		else
		{
			if(pgas[i].alarm_info & (1<<L_ALARM))
			{
				pgas[i].alarm_info &= (~(1<<L_ALARM));	
			}
		}
		
		if(statue_value & (1<<H_ALARM))
		{
			if((pgas[i].alarm_info & (1<<H_ALARM))==0)
			{
				if(((0==i)&&(FLA_detec_test==0))
				||(i>0))
				{
					event_request(DETEC_ONE_A2_ALARM_EVENT+i);
				}
				pgas[i].alarm_info |= (1<<H_ALARM);	
				alarm_slience=0;
				*pstate=MEASURE_INTFACE;
			}
		}
		else
		{
			if(pgas[i].alarm_info & (1<<H_ALARM))
			{
				pgas[i].alarm_info &= (~(1<<H_ALARM));	
			}	
		}
		
		if(statue_value & (1<<OVRANG_ALARM))
		{
			if((pgas[i].alarm_info & (1<<OVRANG_ALARM))==0)
			{
				if(((0==i)&&(FLA_detec_test==0))
				||(i>0))
				{
					event_request(DETEC_ONE_OVER_RANGE_EVENT+i);
				}
				pgas[i].alarm_info |= (1<<OVRANG_ALARM);
				alarm_slience=0;
				*pstate=MEASURE_INTFACE;
			}
		}
		else
		{
			if(pgas[i].alarm_info & (1<<OVRANG_ALARM))
			{
				pgas[i].alarm_info &= (~(1<<OVRANG_ALARM));	
			}		
		}
	}
//#endif

	statue_value=detec_toxic_alarm_check(2);
	alarm_type |= statue_value;
	if(statue_value & (1<<STEL_ALARM))
	{
		if((pgas[2].alarm_info & (1<<STEL_ALARM))==0)
		{
			event_request(CO_DETEC_STEL_EVENT);
			pgas[2].alarm_info |= (1<<STEL_ALARM);
			alarm_slience=0;
			*pstate=MEASURE_INTFACE;
		}
	}
	else
	{
		if(pgas[2].alarm_info & (1<<STEL_ALARM))
		{
			pgas[2].alarm_info &= (~(1<<STEL_ALARM));	
		}		
	}
	
	if(statue_value & (1<<TWA_ALARM))
	{
		if((pgas[2].alarm_info & (1<<TWA_ALARM))==0)
		{
			event_request(CO_DETEC_TWA_EVENT);
			pgas[2].alarm_info |= (1<<TWA_ALARM);
			alarm_slience=0;
			*pstate=MEASURE_INTFACE;
		}
	}
	else
	{
		if(pgas[2].alarm_info & (1<<TWA_ALARM))
		{
			pgas[2].alarm_info &= (~(1<<TWA_ALARM));	
		}			
	}
	
	statue_value=detec_toxic_alarm_check(3);
	alarm_type |= statue_value;
	if(statue_value & (1<<STEL_ALARM))
	{
		if((pgas[3].alarm_info & (1<<STEL_ALARM))==0)
		{
			event_request(H2S_DETEC_STEL_EVENT);
			pgas[3].alarm_info |= (1<<STEL_ALARM);
			alarm_slience=0;
			*pstate=MEASURE_INTFACE;
		}
	}
	else
	{
		if(pgas[3].alarm_info & (1<<STEL_ALARM))
		{
			pgas[3].alarm_info &= (~(1<<STEL_ALARM));	
		}		
	}
	
	if(statue_value & (1<<TWA_ALARM))
	{
		if((pgas[3].alarm_info & (1<<TWA_ALARM))==0)
		{
			event_request(H2S_DETEC_TWA_EVENT);
			pgas[3].alarm_info |= (1<<TWA_ALARM);
			alarm_slience=0;
			*pstate=MEASURE_INTFACE;
		}
	}
	else
	{
		if(pgas[3].alarm_info & (1<<TWA_ALARM))
		{
			pgas[3].alarm_info &= (~(1<<TWA_ALARM));	
		}			
	}	
	
	return alarm_type;
}

unsigned char detec_record_process(void)
{
	signed char event_type;
	unsigned int event_data=0;
	unsigned char event_time[6];
	
	do{
		event_type=check_event_request();
		if(event_type>=0)
		{
			if((event_type >= DETEC_ONE_A1_ALARM_EVENT)
			&&(event_type <= DETEC_FOUR_A2_ALARM_EVENT))
			{
				event_data=detec_get_conce((event_type-DETEC_ONE_A1_ALARM_EVENT)%4);	
			}
			else if((event_type == CO_DETEC_TWA_EVENT)
			||(event_type == CO_DETEC_STEL_EVENT))
			{
				event_data=detec_get_conce(2);
			}
			else if((event_type == H2S_DETEC_TWA_EVENT)
			||(event_type == H2S_DETEC_STEL_EVENT))
			{
				event_data=detec_get_conce(3);
			}
			ds1302_read_time(event_time);
			write_record((unsigned char)event_type, event_data, event_time);
			event_clear(event_type);
		}
	}while(event_type>=0);

	return 0;
}

void detec_alarm_slience(unsigned char key_value,unsigned char alarm_value)
{
	if((key_value==BACK_KEY)&&(alarm_value>0))
	{
		BEEP_OFF;
		RED_LED_OFF;
		MOTOR_OFF;
		YELL_LED_OFF;	
		alarm_slience=1;
	}
}


void detec_state_display(unsigned char detec_state_value,
											unsigned char work_state)
{
	static unsigned int alarm_timer;
	static unsigned int alarm2_timer;
	static unsigned int state_timer;
	static unsigned char old_voltage_value=0;
	
	if(work_state<MEASURE_STATE)
		return;
	
	if(EXTTERN_POWER !=OFF){
		GREEN_LED_ON;
	}
	else {
		if((detec_state_value==0)&&(work_state==MEASURE_STATE)
		&&(battery_voltage_level_get()>=BATTERY_LEVEL_ONE)
		&&(backlight_duty_state==0)){
			if((sys_tick-state_timer)<10){
				GREEN_LED_ON;
			}
			else{
				GREEN_LED_OFF;
				if((sys_tick-state_timer)>=400){
					state_timer=sys_tick;
				}
			}
		}
		else {
			GREEN_LED_OFF;
			state_timer=sys_tick;
		}
	}	

///alarm_slience 只是关闭蜂鸣器和振动.//////
///所以下面的代码不对,需要屏蔽.
/*	if(alarm_slience>0)
	{
		alarm_timer=sys_tick;
		alarm2_timer=sys_tick;
		
	//	if(detec_state_value==0)
	//	{
	//		alarm_slience=0;	
	//	}
		return;
	}
*/
	
	///探测器高报与毒报.
	if((detec_state_value&(1<<H_ALARM))
	||(detec_state_value&(1<<STEL_ALARM))
	||(detec_state_value&(1<<TWA_ALARM))){
		if((sys_tick-alarm_timer)<40){
			if(alarm_slience==0)	
				BEEP_ON;
		}
		else if((sys_tick-alarm_timer)<60){
			if(alarm_slience==0)
			{
				BEEP_OFF;
				MOTOR_ON;
			}
		}
		else if((sys_tick-alarm_timer)<80){
			if(alarm_slience==0)
				MOTOR_OFF;
			RED_LED_ON;
		}
		else {
			RED_LED_OFF;
			if((sys_tick-alarm_timer)>100){
				alarm_timer=sys_tick;
			}
		}

		if((detec_state_value&(1<<PUMP_FAULT))
		||(detec_state_value&(1<<SENSOR_FAULT))){
			if((sys_tick-alarm2_timer)<300){
				YELL_LED_ON;
			}
			else{
				YELL_LED_OFF;
				if((sys_tick-alarm2_timer)>400){
					alarm2_timer=sys_tick;				
				}
			}
		}
		else if(battery_voltage_level_get()< BATTERY_LEVEL_ONE)
		{
			if(old_voltage_value>=BATTERY_LEVEL_ONE){
				alarm_slience=0;
			}
			if((sys_tick-alarm2_timer)<60){
				YELL_LED_ON;
			}
			else{
					YELL_LED_OFF;
				if((sys_tick-alarm2_timer)>6000){
					alarm2_timer=sys_tick;
				}
			}	
		}
		else{
			YELL_LED_OFF;
		}
	}
	///探测器低报.
	else if(detec_state_value & (1<<L_ALARM)){
		if((sys_tick-alarm_timer)<100){
			if(alarm_slience==0)
				BEEP_ON;
		}
		else if((sys_tick-alarm_timer)<150){
			if(alarm_slience==0)
			{
				BEEP_OFF;
				MOTOR_ON;
			}
		}
		else if((sys_tick-alarm_timer)<200){
			if(alarm_slience==0)
				MOTOR_OFF;
			RED_LED_ON;
		}
		else {
			RED_LED_OFF;
			alarm_timer=sys_tick;
		}
		if((detec_state_value&(1<<PUMP_FAULT))
		||(detec_state_value&(1<<SENSOR_FAULT))){
			if((sys_tick-alarm2_timer)<300){
				YELL_LED_ON;
			}
			else{
				YELL_LED_OFF;
				if((sys_tick-alarm2_timer)>400){
					alarm2_timer=sys_tick;				
				}
			}
		}
		else if(battery_voltage_level_get()< BATTERY_LEVEL_ONE)
		{
			if(old_voltage_value>=BATTERY_LEVEL_ONE){
				alarm_slience=0;
			}
			if((sys_tick-alarm2_timer)<60){
				YELL_LED_ON;
			}
			else{
					YELL_LED_OFF;
					if((sys_tick-alarm2_timer)>6000){
					alarm2_timer=sys_tick;
				}
			}	
		}
		else{
			YELL_LED_OFF;
		}
	}
	//探测器故障.
	else if((detec_state_value&(1<<PUMP_FAULT))
	||(detec_state_value&(1<<SENSOR_FAULT))){
		RED_LED_OFF;
		if((sys_tick-alarm_timer)<200){
			if(alarm_slience==0)
				BEEP_ON;
		}
		else if((sys_tick-alarm_timer)<250){
			if(alarm_slience==0)
			{
				BEEP_OFF;
				MOTOR_ON;
			}
		}
		else if((sys_tick-alarm_timer)<300){
			if(alarm_slience==0)
				MOTOR_OFF;
			YELL_LED_ON;
		}
		else {
			YELL_LED_OFF;
			if((sys_tick-alarm_timer)>400){
				alarm_timer=sys_tick;
			}
		}
	}
	else {
		////低电量报警.
		if((battery_voltage_level_get()< BATTERY_LEVEL_ONE)
		&&(EXTTERN_POWER ==OFF)){
			if(old_voltage_value>=BATTERY_LEVEL_ONE){
				alarm_slience=0;
			}
			if((sys_tick-alarm_timer)<20){
				if(alarm_slience==0)
					BEEP_ON;
			}
			else if((sys_tick-alarm_timer)<40){
				if(alarm_slience==0)
				{
					BEEP_OFF;
					MOTOR_ON;
				}
			}
			else if((sys_tick-alarm_timer)<60){
				if(alarm_slience==0)
					MOTOR_OFF;
				YELL_LED_ON;
			}
			else{
				YELL_LED_OFF;
				if((sys_tick-alarm_timer)>=3000){
					alarm_timer=sys_tick;
				}
			}
		}
		else{
			BEEP_OFF;
			RED_LED_OFF;
			MOTOR_OFF;
			YELL_LED_OFF;	
			alarm_timer=sys_tick;
			alarm2_timer=sys_tick;
		}
	}
	old_voltage_value=battery_voltage_level_get();
}

#define BACKLIGHT_COUNTER 12000
/////一段时间未按键息屏功能.
void detec_backlight_process(unsigned char work_state,unsigned char key_value
												,unsigned char detec_state_value)
{
	static unsigned int backlight_timer=0;
	
	if(work_state == SLEEP_STATE)
	{
		backlight_duty_state=0;
		set_backlight_duty(0);
		return;
	}
	
	if((key_value>0)||((detec_state_value>0)&&(alarm_slience==0)))
	{
		backlight_duty_state=1;
		set_backlight_duty(1800);
		backlight_timer=sys_tick;
	}
	else
	{
		if((backlight_duty_state == 1)
		&&(work_state >PREHEAT_STATE))
		{
			if((sys_tick-backlight_timer)>=BACKLIGHT_COUNTER)
			{
				backlight_duty_state=0;
				set_backlight_duty(0);	
			}
		}
	}
}

unsigned char detec_power_down(unsigned char *sys_work_state,unsigned char power_down_flag)
{
	static unsigned int power_down_counter=0;
	static unsigned int power_down_timer;
	
	if((EXTTERN_POWER == 0)&&(power_down_flag >= 2))
	{
		if((power_down_flag ==3)
		&&(power_down_counter<=894))
		{
			BAT_STOP_DISCHARGE;
			if(*sys_work_state>= MEASURE_STATE)
			{
				AT24CXX_write_byte(CFG_LOW_POWER_ADDR, LOW_POWER_FLAG);	
				event_request(POWER_OFF_EVENT);
				detec_record_process();
			}
			while(1);
		}
		if((sys_tick-power_down_timer)>=200)
		{
			power_down_timer=sys_tick;
			power_down_counter++;
			
			if(power_down_counter==895)
			{
				BAT_STOP_DISCHARGE;
				AT24CXX_write_byte(CFG_LOW_POWER_ADDR, LOW_POWER_FLAG);	
			}
			if(power_down_counter>=896)
			{
			//	BAT_STOP_DISCHARGE;
			//	power_down_counter=0;
			//	*sys_work_state=SLEEP_STATE;
				event_request(POWER_OFF_EVENT);
				detec_record_process();
				while(1);
			}
		}
	}
	else
	{
		power_down_timer=sys_tick;	
		if(battery_voltage_level_get()>=BATTERY_LEVEL_THREE)
		{
			power_down_counter=0;
		}
	}
	return 0;
}


unsigned char detec_pump_manage(unsigned char *sys_work_state)
{
	///pump_state=0:泵不存在, pump_state=1:泵存在.
//	static unsigned char pump_state=1; 
	static unsigned int fan_flash_timer=0;
		
	if(PUMP_CHECK==0)
	{
		PUMP_OFF;
		pump_clear_alarm_info();
		return 0;
	}
	
	if((*sys_work_state >PREHEAT_STATE)
	&&(*sys_work_state<RECOVRY_FACTORY_SET))
	{	
		if(pump_get_state()==PUMP_ON_STATE)
		{
			PUMP_ON;
			pump_state_check();
			if(pump_get_flag()==PUMP_FAULT_STATE)
			{
				if((sys_tick-fan_flash_timer)<100)
				{
					display_icon_FAN(ON);
				}
				else if((sys_tick-fan_flash_timer)>200)
				{
					fan_flash_timer=sys_tick;	
				}
			}
			else
			{
				display_icon_FAN(ON);	
				fan_flash_timer=sys_tick;
			}
		}
		else
		{
			PUMP_OFF;
		}
	}
	else if(*sys_work_state != SELFTEST_STATE)
	{
		PUMP_OFF;
	}
	
	return 0;
}

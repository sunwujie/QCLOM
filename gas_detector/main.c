#include "component_config.h"
#include "gas_measure.h"
#include "detector_setting.h"
#include "gas_detector.h"
#include "gas_detector_cfg.h"

extern unsigned char detec_comm_setting(void);

unsigned char sys_time[]={0x19,0x8,0x1,0x14,0x31,0};
volatile unsigned int sys_tick =0;
unsigned char sys_work_state=SLEEP_STATE;
unsigned char detec_alam_type=0;

//////5ms定时器中断.
void timerA_handle(void)
{
	++sys_tick;
	battery_charge_timer();
	detec_state_display(detec_alam_type,sys_work_state);
	key_value_affirm();
	key_speed_up_attr();
//	beep_driver();
}

/////按键唤醒中断.
//void port1_handle(void)
//{

//}

////USB充电和串口通信功能.
//void port2_handle(void)
//{

//}
#ifdef DEBUG
unsigned int main_cycle_timer;
unsigned int max_cycle_time=0;
unsigned int min_cycle_time=0xffff;
#endif

#ifdef DEBUG_EEP
unsigned char checkvalue;
unsigned char buf[5]={1,2,3,4,5};
unsigned char rbuf[5];
#endif

void main(void)
{
//	unsigned int press_key_timer;
//	unsigned char detec_alam_type=0;
	unsigned char flag;
	unsigned char *pmeasure_state;
	
	DISABLE_INT;
	DISABLE_WDT();
  clock_init();
	detector_init();
	detector_reset_reason();
	ENABLE_INT;
	ENABLE_WDT();	
//	press_key_timer=sys_tick;
  while(1)
  {
  #ifdef DEBUG
		if(sys_work_state>SLEEP_STATE)
  		main_cycle_timer=sys_tick;
  #endif
		display_state(DISPLAY_CLEAR);
		switch(sys_work_state)
		{
			case SLEEP_STATE: 
			sleep_process(&sys_work_state,&key,gst_Detec);
			break;
			case SELFTEST_STATE:
			selftest(&sys_work_state);
			break;
			case PREHEAT_STATE:
			preheat(&sys_work_state,&key,gst_Detec);
			break;
			case MEASURE_STATE:
			pmeasure_state=measure_process(&sys_work_state,&key,gst_Detec);
			break;
			case CUSTERM_STATE:
			customer_setting(&sys_work_state,&key,gst_Detec);
			break;
			case MAINTAIN_STATE:
			maintain_setting(&sys_work_state,&key, gst_Detec);
			break;
			case RECOVRY_FACTORY_SET:
			recovery_factory_setting(&sys_work_state,&key, gst_Detec);
			break;
			default:
			
			break;
		}
		feed_WDT();
		if(sys_work_state>PREHEAT_STATE)
		{
			if(sys_work_state != MAINTAIN_STATE)
			{
				detec_alam_type=detec_state_judgemnet(gst_Detec,pmeasure_state);
			}
			else
			{
				detec_alam_type=0;	
			}
			detec_alarm_display(detec_alam_type);
			detec_alarm_slience(key.code_value,detec_alam_type);
		}
		////放中断中执行.
		//detec_state_display(detec_alam_type,sys_work_state);
		detec_backlight_process(sys_work_state,key.raw_value,detec_alam_type);
		detec_pump_manage(&sys_work_state);
		detec_get_signal();
		detec_comm_setting();
		feed_WDT();
		#ifdef DEBUG_EEP
			if(sys_work_state==MEASURE_STATE)
			{
				if(key.code_value==LEFT_KEY)
				{
					key.code_value=0;
					//AT24CXX_write_byte(0x0010,0x55);
					AT24CXX_write_page(0X0010, buf, 5);
				}
				if(key.code_value==RIGHT_KEY)
				{
					key.code_value=0;
					//checkvalue=AT24CXX_read_byte(0x0010);
					//if(checkvalue==0x55)
					//{
					//	key.code_value=0;
					//}
					AT24CXX_read_page(0X0010,rbuf ,5);
				}
			}
		#else
			detec_record_process();
		#endif
  		flag=battery_manage();
		detec_power_down(&sys_work_state,flag);
		if((sys_work_state>=PREHEAT_STATE)
		&&(sys_work_state<RECOVRY_FACTORY_SET))
		{
			detec_display_voltage();
		}
		display_update();
	//	key_clear();
		#ifdef DEBUG
		main_cycle_timer=sys_tick-main_cycle_timer;
		if(main_cycle_timer>max_cycle_time)
		{
			max_cycle_time=main_cycle_timer;
		}
		if(main_cycle_timer<min_cycle_time)
		{
			min_cycle_time=main_cycle_timer;
		}
		#endif
  }
}



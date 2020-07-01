#include "component_config.h"
#include "gas_measure.h"
#include "detector_setting.h"

extern volatile unsigned int sys_tick;

const signed short adjust_zeor_AD_range[][2]=
{
    -13100,    	5000,      //
    1637,       4095,
    500,        1300,
    500,        1300
};

unsigned char select_maintain_iterm(key_manage *pkey,
																						unsigned char *selected_item)
{
  static unsigned int inquire_timer;
	//static unsigned char flash_item=DETEC_INIT_ZERO;
//	unsigned char ret_value=0;
	
	if((sys_tick-inquire_timer)<100){
		display_seg7(1,char7_F);
		if(*selected_item <= 9){
			display_seg7(2,*selected_item);
		}
		else {
			display_seg7(2,*selected_item/10);
			display_seg7(3,*selected_item%10);
		}
		switch(*selected_item)
		{
			case SET_LALARM_VALUE: display_icon_L(ON); break;
			case SET_HALARM_VALUE:	display_icon_H(ON); break;
			case SET_TWA_VALUE: display_icon_TWA(ON); break;
			case SET_STEL_VALUE: display_icon_STEL(ON);break;
			case SET_STELTWA_STATE:display_icon_TWA(ON);
			display_icon_STEL(ON);break;
			default: break;	
		}
	}
	else if((sys_tick-inquire_timer)>200){
		inquire_timer=sys_tick;
	}
		
	if(pkey->code_value == ENTER_KEY){
		pkey->code_value=0;	
		return 2;
	}
	else if(pkey->code_value == BACK_KEY){
		pkey->code_value=0;
		return 1;				
	}
	else if(pkey->code_value == LEFT_KEY){
		pkey->code_value=0;
		if(*selected_item>DETEC_INIT_ZERO){
			--(*selected_item);
		}
		else{
			*selected_item = DETEC_LOW_VOLTAGE;
		}
	}
	else if(pkey->code_value == RIGHT_KEY){
		pkey->code_value=0;
		if(*selected_item<DETEC_LOW_VOLTAGE){
			++(*selected_item);
		}
		else{
			*selected_item = DETEC_INIT_ZERO;
		}
	}	
	return 0;
}

////////维护操作1:调零////
unsigned char detec_measure_zero(key_manage *pkey,unsigned char gas_type,
																unsigned char decimal,signed short *conce) 
{
  static unsigned int zero_timer;
	signed short conce_zero_ad;
	static unsigned char adjust_zero_flag=0;
	static unsigned char flash_counter=0;
	unsigned char ret_value=0;
	static unsigned char adjust_disp_flag=0;
	static signed short display_conce=0;

	if(adjust_zero_flag >0)
	{
		if((sys_tick-zero_timer)<200)
		{
			if(adjust_zero_flag==1)
			{
				display_num(display_conce, 8+4*gas_type,decimal,3);
				if(decimal>0){
					display_decimal_point(gas_type*2+4-decimal);
				}
			}
			else if(adjust_zero_flag ==2)
			{
				display_Err(8+4*gas_type);
			}
		}
		else if((sys_tick-zero_timer)>400)
		{
			zero_timer=sys_tick;
			++flash_counter;
			if(flash_counter>=3)
			{
				///调零成功.
				if(adjust_zero_flag==1)
				{
					detec_save_clib_data(gas_type);		
				}
				adjust_disp_flag=0;
				flash_counter=0;
				adjust_zero_flag=0;
				ret_value=1;
			}
		}	
		return ret_value;
	}
	
	if((sys_tick-zero_timer)<100)
	{
		if(adjust_disp_flag==0)
		{
			display_conce=*conce;
		}
		display_num(display_conce, 8+4*gas_type,decimal, 3);
		if(decimal>0){
			display_decimal_point(gas_type*2+4-decimal);
		}
	}
	else if((sys_tick-zero_timer)>200)
	{
		zero_timer=sys_tick;
	}
	
	if(pkey->code_value == BACK_KEY)
	{
		pkey->code_value=0;
		ret_value=1;
	}
	else if(pkey->code_value == ENTER_KEY){
		pkey->code_value=0;
		if(adjust_disp_flag==0)
		{
			display_conce=*conce;
		}
		if(display_conce == 0){
			conce_zero_ad=gas_get_AD_value(gas_type); 
			if((conce_zero_ad >adjust_zeor_AD_range[gas_type][0])
			&&(conce_zero_ad <adjust_zeor_AD_range[gas_type][1]))
			{
				ret_value=detec_update_clb_param(gas_type,conce_zero_ad);
				if(ret_value==0){
					adjust_zero_flag=1;
				}
				else{
					adjust_zero_flag=2;
				}
			}
			else{
				adjust_zero_flag=2;
			}
		}
		else {
			adjust_zero_flag=2;
		}
	}
	else if(pkey->code_value == UP_KEY)
	{
		if(adjust_disp_flag==0)
		{
			display_conce=*conce;
			adjust_disp_flag=1;
		}
		pkey->code_value=0;
		if(display_conce < 0){
			display_conce=0;
		}
	}
	else if(pkey->code_value == DOWN_KEY)
	{
		if(adjust_disp_flag==0)
		{
			display_conce=*conce;
			adjust_disp_flag=1;
		}
		pkey->code_value=0;
		if(display_conce > 0){
			display_conce=0;
		}
	}
	
	return ret_value;
}

////////维护操作2:标定//////
unsigned char detec_measure_span(key_manage *pkey,unsigned char gas_type,
																unsigned char decimal,unsigned int conce)  //
{
  static unsigned int span_timer;
	static unsigned int span_unit_value=0;
	static unsigned char span_finish_flag=0;
	static unsigned char flash_counter=0;
	unsigned char ret_value=0;
	static unsigned char span_adjust_flag=0;
	static unsigned short span_conce=0;

	if(span_finish_flag >0)
	{
		if((sys_tick-span_timer)<200){
			if(span_finish_flag==1)
			{
				display_Err(8+4*gas_type);
			}
			else
			{
				if(span_adjust_flag==0)
				{
					span_conce=conce;
				}
				display_num(span_conce+span_unit_value, 8+4*gas_type,decimal,3);
				if(decimal>0){
					display_decimal_point(gas_type*2+4-decimal);
				}
			}
		}
		else if((sys_tick-span_timer)>400){
			span_timer=sys_tick;
			++flash_counter;
			if(flash_counter>=3)
			{
				if(span_finish_flag==2)
				{
					detec_save_clib_data(gas_type);			
				}
				flash_counter=0;
				span_finish_flag=0;
				span_adjust_flag=0;
				span_unit_value=0;
				return 1;
			}
		}
		return 0;
	}
	
	if((sys_tick-span_timer)<100){
		/*为0时，一直显示实际的浓度,为1时显示浓度为
		最后一次显示浓度加上手动调整的浓度.**/
		if(span_adjust_flag==0)
		{
			span_conce=conce;
		}
		display_num(span_conce+span_unit_value, 8+4*gas_type,decimal,3);
		if(decimal>0){
			display_decimal_point(gas_type*2+4-decimal);
		}
	}
	else if((sys_tick-span_timer)>200){
		span_timer=sys_tick;
	}
	
	if(pkey->code_value == BACK_KEY){
		pkey->code_value=0;
		ret_value=1;
		span_unit_value=0;
	}
	else if(pkey->code_value == ENTER_KEY){
		pkey->code_value=0;
		//调用标定处理函数.
		span_finish_flag= detec_update_clb_conce_FS(gas_type,(conce+span_unit_value));	
		span_timer=sys_tick;
	}
	else if((pkey->code_value == UP_KEY)
	||(pkey->code_value == UP_LONG_KEY)){
		pkey->code_value=0;
		if(span_adjust_flag==0)
		{
			span_conce=conce;
			span_adjust_flag=1;
		}
		if((span_conce+span_unit_value)<detec_get_conce_FS(gas_type))
		{
			span_unit_value++;	
		}
	}
	else if((pkey->code_value == DOWN_KEY)
	||(pkey->code_value == DOWN_LONG_KEY)){
		pkey->code_value=0;
		if(span_unit_value>0)
		{
			span_unit_value--;
		}
		if(span_adjust_flag==0)
		{
			span_conce=conce;
			span_adjust_flag=1;
		}
	}
	return ret_value;
}

/*
////////维护操作3~4:设置报警值//////
unsigned char detec_set_alarm_value(key_manage *pkey,unsigned char gas_type,
																										 unsigned short *alarm_value)
{
	unsigned char ret_value;
	volatile static unsigned int alarm_timer;

	if((sys_tick-alarm_timer)<10){
	
		display_num(*alarm_value, 8+4*gas_type, 3);
	}
	else if((sys_tick-alarm_timer)>20){
		alarm_timer=sys_tick;
	}
	if(pkey->code_value == BACK_KEY){
		ret_value=1;
	}
	else if(pkey->code_value == ENTER_KEY){
		//调用设置报警值函数.
		ret_value = ;	
	}
	else if(pkey->code_value & UP_KEY){
		if(){
					
		}
		else {
			*alarm_value++;	
		}
		//限制最大值.
		if(*alarm_value >=)
		{
			
		}
	}
	else if(pkey->code_value & DOWN_KEY){
		if(){

		}
		else {
			*alarm_value--;
		}
		//限制最小值.
		if(*alarm_value <= )
		{
			
		}
	}
	return ret_value;
}

///////维护操作5~6:STEL & TWA 报警值////
unsigned char detec_set_STELTWA_value(key_manage *pkey,unsigned char gas_type,
																										 unsigned short *alarm_value)
{
	unsigned char ret_value;
	volatile static unsigned int alarm_timer;

	if((sys_tick-alarm_timer)<10){
 
		display_num(*alarm_value, 8+4*gas_type, 3);
 	}
 	else if((sys_tick-alarm_timer)>20){
		alarm_timer=sys_tick;
 	}
 	if(pkey->code_value == BACK_KEY){
		ret_value=1;
 	}
 	else if(pkey->code_value == ENTER_KEY){
	 //调用设置报警值函数.
		ret_value = ; 
 	}
 	else if(pkey->code_value & UP_KEY){
		if(){
				 
	 	}
	 	else {
			*alarm_value++; 
	 	}
		//限制最大值.
		if(*alarm_value >=)
		{
			 
		}
 	}
 	else if(pkey->code_value & DOWN_KEY){
		if(){

	 	}
	 	else {
			*alarm_value--;
	 	}
	 	//限制最小值.
	 	if(*alarm_value <= )
	 	{
		 
	 	}
 	}
 	return ret_value;
}


//////维护操作8:STEL & TWA 开关//////
unsigned char detec_set_STELTWA_state(key_manage *pkey,unsigned char gas_type)
{
	volatile static unsigned int set_timer;
	unsigned char ret_value;
	static unsigned char set_state=0;

	if((sys_tick-set_timer)<10){
		display_OFF(21);	
	}
 	else {
		display_ON(21);
			if((sys_tick-set_timer)>20){
		 set_timer=sys_tick;
	 	}
 	}
	if(pkey->code_value == BACK_KEY){
	 ret_value=1;
	}
 	else if(pkey->code_value == ENTER_KEY){
	 //调用设置STELTWA函数.
		ret_value = ; 
 	}
 	else if((pkey->code_value & UP_KEY)||(pkey->code_value & DOWN_KEY)){
		if(set_state == 0){
			set_state=1;
	 	}
	 	else {
			set_state=0; 
	 	}
 	}
	
	return ret_value;
}
*/

//////维护操作9:探测器硬件自检////////
unsigned char detec_hardware_selftest(unsigned short conce_FS,signed short *selftest_conce)
{
  static unsigned int selftest_timer=0;
	static unsigned char selftest_state=0;
	static unsigned short selftest_temp=0;
	static unsigned char flash_counter=0;
	static unsigned char flag=0;
	
	if(selftest_state == 0){
		selftest_timer=sys_tick;
		selftest_state=1;
	}
	else if(selftest_state == 1){
		if((sys_tick-selftest_timer)<100){
			display_state(DISPLAY_FULL);
		}
		else{
			if((sys_tick-selftest_timer)>200){
				selftest_state=2;	
				selftest_temp=0;
				selftest_timer=sys_tick;
			}
		}
	}
	else if(selftest_state == 2){
		if(flag == 0){
			if((sys_tick-selftest_timer)>=25){
				selftest_temp += conce_FS/100;
				if(selftest_temp >= (conce_FS+conce_FS/5)){
					flag=1;
				}
				selftest_timer=sys_tick;
			}
		}
		else if(flag == 1)
		{
			if((sys_tick-selftest_timer)>=200){
				selftest_timer=sys_tick;
				flash_counter++;
				if(flash_counter>=3){
					flash_counter=0;
					flag=2;
				}
			}
		}
		else{
			if((sys_tick-selftest_timer)>=25){
				if(selftest_temp>=(conce_FS/100)){
					selftest_temp -= conce_FS/100;
				}
				else{
					flag=0;
					selftest_temp=0;
					selftest_state=3;
				}	
				selftest_timer=sys_tick;
			}
		}
		*selftest_conce=selftest_temp;
	}
	else if(selftest_state == 3){
		if((sys_tick-selftest_timer)<50){
			display_state(DISPLAY_FULL);
		}
		else{
			if((sys_tick-selftest_timer)>100){
				selftest_timer=sys_tick;
				flash_counter++;
				if(flash_counter>=3){
					flash_counter=0;
					selftest_state=0;
					return 0xff;
				}
			}
		}		
	}
	return selftest_state;
}

/*
/////维护操作10: 探测器的传感器屏蔽//////
unsigned char detec_measure_switch(key_manage *pkey,unsigned char gas_type)
{
	volatile static unsigned int set_timer;
	unsigned char ret_value;
	static unsigned char set_state=0;
	
	if((sys_tick-set_timer)<10){
		display_OFF(21);	
	}
	else {
		display_ON(21);
			if((sys_tick-set_timer)>20){
		 set_timer=sys_tick;
		}
	}

	if(pkey->code_value == BACK_KEY){
	 ret_value=1;
	}
	else if(pkey->code_value == ENTER_KEY){
	 //调用设置探测器开关函数.
		ret_value = ; 
	}
	else if((pkey->code_value & UP_KEY)||(pkey->code_value & DOWN_KEY)){
		if(set_state == 0){
			set_state=1;
		}
		else {
			set_state=0; 
		}
	}
		
	return ret_value;
}

/////维护操作11: 设置探测器的欠压值//////
unsigned char detec_set_lowVoltage_value(key_manage *pkey,
																								 unsigned int voltage)
{
	unsigned char ret_value;
	
	
	
	return ret_value;
}
*/

unsigned char maintain_affirm(key_manage *pkey,unsigned char select_iterm,
																										unsigned char *maintain_iterm)
{
	static unsigned int  display_timer=0;
//	static unsigned char flash_iterm=1;
	
	switch(*maintain_iterm)
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
	if((sys_tick-display_timer)<100)
	{
		display_seg7(5,*maintain_iterm);
	}
	else if((sys_tick-display_timer)>200)
	{
		display_timer=sys_tick;
	}
	
	if(pkey->code_value == ENTER_KEY){
		pkey->code_value=0;
		return 2;
	}
	else if(pkey->code_value == BACK_KEY){
		pkey->code_value=0;
		return 1;
	}
	else if(pkey->code_value == LEFT_KEY){
		pkey->code_value=0;
		
		if((select_iterm == SET_TWA_VALUE)
		||(select_iterm == SET_STEL_VALUE)
		||(select_iterm==SET_STELTWA_STATE)){
		
			if(*maintain_iterm == 3){
				*maintain_iterm=4;
			}
			else{
				*maintain_iterm=3;
			}
		}
		else{
			if(*maintain_iterm>1){
				--(*maintain_iterm);
			}
			else{
				*maintain_iterm=4;
			}
		}
	}
	else if(pkey->code_value == RIGHT_KEY){
		pkey->code_value=0;
		
		if((select_iterm == SET_TWA_VALUE)
		||(select_iterm == SET_STEL_VALUE)
		||(select_iterm==SET_STELTWA_STATE)){
		
			if(*maintain_iterm == 3){
				*maintain_iterm=4;
			}
			else{
				*maintain_iterm=3;
			}
		}
		else{
			if(*maintain_iterm<4){
				++(*maintain_iterm);
			}
			else {
				*maintain_iterm=1;
			}
		}
	}
	
	return 0;
}

unsigned int do_maintain_setting(key_manage *pkey,unsigned int *data,
																unsigned char step,unsigned int high,unsigned int low)
{
//	unsigned int ret_value=0;
	
	if(pkey->code_value == BACK_KEY)
	{
		pkey->code_value=0;
		return 1;							
	}
	else if(pkey->code_value == ENTER_KEY)
	{
		pkey->code_value=0;
		return 2;	
	}
	else if((pkey->code_value == UP_LONG_KEY)
	||(pkey->code_value == UP_KEY))
	{
		if(*data < high)
		{
			if(pkey->code_value == UP_LONG_KEY){
				(*data) += step;			
			}
			else {
				(*data)++;
			}
			if(*data > high)
			{
				*data = high;
			}
		}
	//	else
	//	{
	//		*data=low;
	//	}
		pkey->code_value=0;
	}
	else if((pkey->code_value == DOWN_KEY)
	||(pkey->code_value == DOWN_LONG_KEY))
	{
		if(*data > low)
		{
			if(pkey->code_value == DOWN_LONG_KEY){
				if(*data>=step)
					(*data) -= step;
				else
					*data=low;
			}
			else{
				(*data)--;
			}
			if(*data <low){
				*data=low;
			}
		}
		//else
	//	{
	//		*data=high;
	//	}
		pkey->code_value=0;
	}
	return 0;
}

unsigned char detec_set_pump(key_manage *pkey,unsigned char *set_state)
{
	static unsigned int set_pump_timer;
	static unsigned char flash_counter=0;
	static unsigned char set_finish_flag=0;
	static unsigned int pump_state;
	//unsigned char ret_value;

	if(*set_state==1)
	{
		pump_state=(unsigned int)pump_get_state();
		set_pump_timer=sys_tick;
		*set_state=0;
	}
	
	if(set_finish_flag >0)
	{
		if((sys_tick-set_pump_timer)<200){
			if(pump_state>0){
				display_ON(21);
			}
			else{
				display_OFF(21);
			}
		}
		else if((sys_tick-set_pump_timer)>400){
			set_pump_timer=sys_tick;
			flash_counter++;
			if(flash_counter>=3){
				pump_set_state((unsigned char)pump_state);
				detec_save_pump_state();
				flash_counter=0;
				set_finish_flag=0;
				return 1;
			}
		}		
		return 0;
	}
	
	if((sys_tick-set_pump_timer)<100){
		if(pump_state>0){
			display_ON(21);
		}
		else{
			display_OFF(21);
		}
	}
	else if((sys_tick-set_pump_timer)>200){
		set_pump_timer=sys_tick;
	}

	if(pkey->code_value == BACK_KEY)
	{
		pkey->code_value=0;
		return 1;							
	}
	else if(pkey->code_value == ENTER_KEY)
	{
		set_pump_timer=sys_tick;
		set_finish_flag=1;
		pkey->code_value=0;
		return 2;	
	}
	else if((pkey->code_value == UP_KEY)
	||(pkey->code_value == DOWN_KEY))
	{
		pkey->code_value=0;
		if(pump_state==0)
		{
			pump_state=1;
		}
		else
		{
			pump_state=0;
		}
		set_pump_timer=sys_tick;
	}
	
	/////////why ? waite for reason.
	/*ret_value=do_maintain_setting(pkey,&pump_state,1,0); 
	if(ret_value==1)
	{
		return 1;
	}
	else if(ret_value==2)
	{
		set_pump_timer=sys_tick;
		set_finish_flag=1;
	}*/
	
	return 0;
}


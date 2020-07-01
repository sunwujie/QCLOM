#include "component_config.h"
#include "gas_measure.h"
#include "detector_setting.h"

extern volatile unsigned int sys_tick;

const unsigned char event_table[]={0,13,17,9,21,23,4};

unsigned char do_inquire_item(key_manage *pkey,unsigned char flash_iterm,unsigned char *flash_alarm)
{
	static unsigned char select_alarm=0;
	static unsigned int flash_alarm_timer;
	static unsigned char record_buf[9];
	static unsigned char record_info[2];
	unsigned char record_ret;
	static unsigned char read_index=0;
	static unsigned char display_index=0;
	static unsigned char record_type;
	static unsigned char record_total=0;
	
	if(select_alarm == 0){		
		if((sys_tick-flash_alarm_timer)< 100){
			display_seg7(4, *flash_alarm);		
		}
		else if((sys_tick-flash_alarm_timer)> 200){
			flash_alarm_timer=sys_tick;
		}
		
		switch(*flash_alarm)
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
		
		if(pkey->code_value == ENTER_KEY){
			pkey->code_value=0;
			select_alarm=*flash_alarm;	
			//获取记录类型.
			record_type=event_table[flash_iterm]+select_alarm-1;
			////读取记录信息.
			record_ret=read_record_info(record_info,record_type);
			////0: 记录读取成功.
			if(record_ret == 0){
				/////record_info[1]:当前写记录的索引号.
				////record_info[0]: 记录是否满标志.
				///record_total: 记录条数.
				if(record_info[0]==1)
				{
					record_total = EVENT_RECORD_NUM;
				}
				else
				{
					record_total = record_info[1];
				}
				///record_index: 最近一条记录的索引位置.
				read_index = record_info[1];
				//读取最近一条记录的内容.
				read_record_data(record_buf, record_type,read_index);
				display_index=record_total;
			}
			else{
				read_index=0;
				record_total=0;
				display_index=0;
			}
		}
		else if(pkey->code_value == BACK_KEY){
			pkey->code_value=0;
			return 1;
		}
		else if(pkey->code_value == LEFT_KEY){
			pkey->code_value=0;
			if((flash_iterm==4)||(flash_iterm==5)){
				if(*flash_alarm == 3){
					*flash_alarm=4;
				}
				else{
					*flash_alarm=3;
				}	
			}
			else{
				if(*flash_alarm > 1){
					(*flash_alarm)--;
				}
				else{
					*flash_alarm=4;
				}
			}
		}
		else if(pkey->code_value == RIGHT_KEY){
			pkey->code_value=0;
			if((flash_iterm==4)||(flash_iterm==5)){
				if(*flash_alarm == 3){
					*flash_alarm=4;
				}
				else{
					*flash_alarm=3;
				}	
			}
			else{
				if(*flash_alarm < 4){
					(*flash_alarm)++;
				}
				else{
					*flash_alarm=1;
				}
			}
		}		
	}
	else{
		display_seg7(4, select_alarm);
		display_seg7(5, char7_minus_sign);
		switch(select_alarm)
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
		if(pkey->code_value == BACK_KEY){
			pkey->code_value=0;
			select_alarm=0;		
		}
		else if(pkey->code_value == UP_KEY){
			pkey->code_value=0;
			if(record_info[0]==0){
				if(read_index < record_info[1]){
					read_index++;
				}
				else{
					if(record_total > 0){
						read_index=1;
					}
				}
				display_index=read_index;
			}
			else{
				if(read_index < EVENT_RECORD_NUM){
					read_index++;
				}
				else{
					read_index = 1;
				}
				if(display_index<EVENT_RECORD_NUM){
					display_index++;	
				}
				else{
					display_index=1;
				}
			}
			if(record_total > 0){
				read_record_data(record_buf, record_type,read_index);	
			}
		}
		else if(pkey->code_value == DOWN_KEY){
			pkey->code_value=0;
			if(record_info[0]==0){
				if(read_index > 1){
					read_index--;
				}
				else{
					read_index=record_total;
				}
				display_index=read_index;
			}
			else{
				if(read_index > 1){
					read_index--;
				}
				else{
					read_index = EVENT_RECORD_NUM;
				}
				if(display_index > 1){
					display_index--;	
				}
				else{
					display_index= EVENT_RECORD_NUM;
				}			
			}
			if(record_total > 0){
				read_record_data(record_buf, record_type,read_index);
			}
		}
		///显示当前索引记录号.
		if((sys_tick-flash_alarm_timer)< 100){			
			display_int_value(display_index,7,2);		
		}
		else if((sys_tick-flash_alarm_timer)> 200){
			flash_alarm_timer=sys_tick;
		}
		if(record_total > 0){
			display_time(&record_buf[2]);
		}
	}
	return 0;
}


unsigned char select_record(key_manage *pkey,unsigned char *flash_item)
{
	static unsigned int inquire_timer;
//	static unsigned char flash_item=L_ALARM;
//	unsigned char ret_value=0;
	
	if((sys_tick-inquire_timer)<100){
		if((*flash_item>=L_ALARM)&&(*flash_item<=SENSOR_FAULT)){
			display_seg7(1,char7_C);
			display_seg7(2,*flash_item);
		}
		else{
			display_seg7(3,char7_C);
			display_seg7(4,*flash_item);
		}
		switch(*flash_item)
		{
			case L_ALARM: display_icon_L(ON); break;
			case H_ALARM:	display_icon_H(ON); break;
			case TWA_ALARM: display_icon_TWA(ON); break;
			case STEL_ALARM: display_icon_STEL(ON);break;
			case SENSOR_FAULT:
			case PUMP_FAULT: display_icon_F(ON); break;
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
//		*flash_item=L_ALARM;
		return 1;				
	}
	else if(pkey->code_value == LEFT_KEY){
		pkey->code_value=0;
		if(*flash_item>L_ALARM){
			(*flash_item)--;
		}
		else{
			*flash_item = POWER_OFF;
		}
	}
	else if(pkey->code_value == RIGHT_KEY){
		pkey->code_value=0;
		if(*flash_item<POWER_OFF){
			(*flash_item)++;
		}
		else{
			*flash_item = L_ALARM;
		}
	}	
	return 0;
}

/////用户设置1:记录查询.
unsigned char inquire_record(key_manage *pkey,unsigned char *selected_item)
{
	unsigned char ret_value=0;
	static unsigned char record_inf[2];
	static unsigned int record_timer;
	static unsigned char read_iindex=0;
	static unsigned char record_ibuf[9];
	static unsigned char flash_item=L_ALARM;
	static unsigned char record_type;
	static unsigned char flash_alrm=1;
	static unsigned char record_itotal=0;
	static unsigned char display_iindex=0;
	unsigned char record_ret=0;
	
	if(*selected_item == 0){
		ret_value=select_record(pkey,&flash_item);
		if(ret_value == 1){
			flash_item=L_ALARM;
		}
		else if(ret_value > 1){
			*selected_item=flash_item;
	//		flash_item=L_ALARM;
			if(*selected_item >= PUMP_FAULT){
				if(*selected_item == PUMP_FAULT)
				{
					record_type=8;	
				}
				else if(*selected_item == POWER_ON)
				{
					record_type=0;
				}
				else
				{
					record_type=1;
				}
				record_ret=read_record_info(record_inf,record_type);
				if(record_ret == 0){
					if(record_inf[0]==0){
						record_itotal = record_inf[1];	
					}
					else{
						record_itotal = EVENT_RECORD_NUM;	
					}
					read_iindex= record_inf[1];
					read_record_data(record_ibuf, record_type,read_iindex);
					display_iindex=record_itotal;
				}
				else{
					read_iindex=0;
					record_itotal=0;
					display_iindex=0;
				}	
			}
			if((*selected_item == TWA_ALARM)
			||(*selected_item == STEL_ALARM))
			{
				flash_alrm=3;
			}
			else
			{
				flash_alrm=1;
			}
		}
		return ret_value;
	}
			
	if((*selected_item>=L_ALARM)&&(*selected_item<=SENSOR_FAULT)){
		display_seg7(1,char7_C);
		display_seg7(2,*selected_item);
		display_seg7(3, char7_minus_sign);
		switch(*selected_item)
		{
			case L_ALARM: display_icon_L(ON);
			break;
			case H_ALARM: display_icon_H(ON);
			break;
			case TWA_ALARM: display_icon_TWA(ON);
			break;
			case STEL_ALARM: display_icon_STEL(ON);
			break;
			case SENSOR_FAULT: display_icon_F(ON); 
			break;
			default: break;
		}
		ret_value=do_inquire_item(pkey,*selected_item,&flash_alrm);
		if(ret_value==1)
		{
			*selected_item=0;
		}
	}
	else{
		display_seg7(3,char7_C);
		display_seg7(4,*selected_item);
		display_seg7(5, char7_minus_sign);
		
		if(*selected_item == PUMP_FAULT){
			display_icon_F(ON);
		}

		if(pkey->code_value == BACK_KEY){
			pkey->code_value=0;
			*selected_item=0;			
		}
		else if(pkey->code_value == UP_KEY){
			pkey->code_value=0;
			if(record_inf[0] == 0){
				if(read_iindex<record_inf[1]){
					read_iindex++;
				}
				else{
					if(record_itotal > 0){
						read_iindex=1;
					}
				}
				display_iindex= read_iindex;
			}
			else{
				if(read_iindex < EVENT_RECORD_NUM){
					read_iindex++;
				}
				else{
					read_iindex = 1;
				}
				if(display_iindex<EVENT_RECORD_NUM){
					display_iindex++;	
				}
				else{
					display_iindex=1;
				}
			}
			if(record_itotal > 0){
				read_record_data(record_ibuf, record_type,read_iindex);	
			}		
		}
		else if(pkey->code_value == DOWN_KEY){
			pkey->code_value=0;
			if(record_inf[0]==0){
				if(read_iindex > 1){
					read_iindex--;
				}
				else{
					read_iindex=record_itotal;
				}
				display_iindex=read_iindex;
			}
			else{
				if(read_iindex > 1){
					read_iindex--;
				}
				else{
					read_iindex = EVENT_RECORD_NUM;
				}
				if(display_iindex > 1){
					display_iindex--;	
				}
				else{
					display_iindex= EVENT_RECORD_NUM;
				}			
			}
			if(record_itotal > 0){
				read_record_data(record_ibuf, record_type,read_iindex);
			}	
		}
		if((sys_tick-record_timer)<100){
			///显示记录索引号.
			display_int_value(display_iindex,7,2);
		}
		else if((sys_tick-record_timer)> 200){
			record_timer=sys_tick;
		}
		if(record_itotal > 0)
		{
			display_time(&record_ibuf[2]);
		}
	}
	return 0;
}

//////用户设置2:时间设置.
unsigned char setting_time(key_manage *pkey,unsigned char *time_buf,unsigned char *set_state)
{
	static unsigned int set_state_timer;
	unsigned char temp;
	//static unsigned char flash_value=0xff;
	static unsigned char flash_counter=0;

	if(*set_state == SET_FINISH)
	{
		if((sys_tick-set_state_timer)<200)
		{
			display_time(time_buf);
		}	
		else if((sys_tick-set_state_timer)>400)
		{
			set_state_timer=sys_tick;
			flash_counter++;
			if(flash_counter>=3)
			{
				flash_counter=0;
			//	flash_value=0xff;
				*set_state=SET_YEAR;	
				return 2;
			}
		}
		return 0;
	}

	display_time(time_buf);
	if((sys_tick-set_state_timer)<100)
	{
		flash_display_time(*set_state, char7_no_seg);
		//要先执行.
	//	flash_value=time_buf[*set_state];
	//	time_buf[*set_state]=char7_no_seg;
	}
	else if((sys_tick-set_state_timer)>200)
	{
		//if(flash_value !=0xff)
		//{
		//	time_buf[*set_state]=flash_value;
		//}
		set_state_timer=sys_tick;
	}
	
	if(pkey->code_value == ENTER_KEY){
		pkey->code_value=0;
		//设置完成. 	
		set_state_timer=sys_tick;
		*set_state=SET_FINISH;
	}
	else if(pkey->code_value == BACK_KEY){
		pkey->code_value=0;
	//	flash_value=0xff;
		*set_state=SET_YEAR;
		return 1;				
	}
	else if(pkey->code_value == LEFT_KEY){
		pkey->code_value=0;
		set_state_timer=sys_tick;
		if(*set_state > SET_YEAR){
			(*set_state)--;
		}
		else{
			*set_state=SET_MIN;
		}
	}
	else if(pkey->code_value == RIGHT_KEY){
		pkey->code_value=0;
		set_state_timer=sys_tick;
		if(*set_state < SET_MIN){
			(*set_state)++;
		}
		else{
			*set_state=SET_YEAR;
		}
	}
	else if((pkey->code_value == UP_KEY)
	||(pkey->code_value == UP_LONG_KEY)){
		pkey->code_value=0;
		if(*set_state == SET_YEAR){
			if(time_buf[0]<99){
				time_buf[0]++;
			}
			else{
				time_buf[0]=0;
			}
		}
		else if(*set_state == SET_MONTH){
			if(time_buf[1]<12){
				time_buf[1]++;
			}
			else{
				time_buf[1]=1;
			}
		}
		else if(*set_state == SET_DAY){
			if(time_buf[1]==2){
				if(((time_buf[0]%4)==0)&&(time_buf[0]%100)){
					temp=29;
				}
				else{
					temp=28;
				}
			}
			else if((time_buf[1]==4)||(time_buf[1]==6)
			||(time_buf[1]==9)||(time_buf[1]==11)){
				temp=30;
			}
			else{
				temp=31;
			}
			if(time_buf[2]<temp){
				time_buf[2]++;
			}
			else{
				time_buf[2]=1;
			}
		}	
		else if(*set_state == SET_HOUR){
			if(time_buf[3]<23){
				time_buf[3]++;
			}
			else{
				time_buf[3]=0;
			}
		}
		else if(*set_state == SET_MIN){
			if(time_buf[4]<59){
				time_buf[4]++;
			}
			else{
				time_buf[4]=0;
			}
		}
	}
	else if((pkey->code_value == DOWN_KEY)
	||(pkey->code_value == DOWN_LONG_KEY)){
		pkey->code_value=0;
		if(*set_state == SET_YEAR){
			if(time_buf[0]>0){
				time_buf[0]--;
			}
			else{
				time_buf[0]=99;
			}
		}
		else if(*set_state == SET_MONTH){
			if(time_buf[1]>1){
				time_buf[1]--;
			}
			else{
				time_buf[1]=12;
			}
		}
		else if(*set_state == SET_DAY){
			if(time_buf[1]==2){
				if(((time_buf[0]%4)==0)&&(time_buf[0]%100)){
					temp=29;
				}
				else{
					temp=28;
				}
			}
			else if((time_buf[1]==4)||(time_buf[1]==6)
			||(time_buf[1]==9)||(time_buf[1]==11)){
				temp=30;
			}
			else{
				temp=31;
			}
			if(time_buf[2]>1){
				time_buf[2]--;
			}
			else{
				time_buf[2]=temp;
			}
		}	
		else if(*set_state == SET_HOUR){
			if(time_buf[3]>0){
				time_buf[3]--;
			}
			else{
				time_buf[3]=23;
			}
		}
		else if(*set_state == SET_MIN){
			if(time_buf[4]>0){
				time_buf[4]--;
			}
			else{
				time_buf[4]=59;
			}
		}
	}
	return 0;
}





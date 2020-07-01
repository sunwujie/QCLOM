#include "component_config.h"
#include "gas_measure.h"
#include "detector_setting.h"
#include "gas_detector.h"
#include "gas_detector_cfg.h"

#if DEBUG_CACL_CONCE
unsigned int debug_delay_send_counter=0;
#endif

typedef struct
{
	unsigned char dev_addr;
	volatile unsigned char rx_counter;
	volatile unsigned char tx_counter;
	unsigned char proc_length;
	volatile unsigned char state;
	void *        proc_private;
}modbus_proc;

modbus_proc detec_proc;
unsigned char proc_buf[24];


void detec_proc_init(void)
{
	unsigned char i;

	for(i=0;i<sizeof(proc_buf);++i)
	{
		proc_buf[i]=0;
	}
	detec_proc.dev_addr = 1;
	detec_proc.proc_length = 0;
	detec_proc.rx_counter =0;
	detec_proc.tx_counter =0;
	detec_proc.proc_private = (void *)0;
	detec_proc.state=0;
}

void uart1_tx_handle(void)
{
	++detec_proc.tx_counter;
	if(detec_proc.tx_counter < detec_proc.proc_length)
	{	
		uart1_send_data(proc_buf[detec_proc.tx_counter]);			
	}
	else
	{
		detec_proc.proc_length = 0;	
		detec_proc.tx_counter =0;
	#if DEBUG_CACL_CONCE
		debug_delay_send_counter=1;
	#endif
	}
}

void uart1_rx_handle(unsigned char revice_data)
{
	unsigned char length;
	
	if(detec_proc.state == 0)
	{
		if(revice_data == detec_proc.dev_addr)
		{
			proc_buf[0]= revice_data;
			detec_proc.rx_counter =1;
			detec_proc.state =1;
		}
	}
	else if(detec_proc.state == 1)
	{
		if((revice_data == 0x05)
		||(revice_data == 0x06)
		||(revice_data == 0x10))
		{
			proc_buf[1] = revice_data;
			++detec_proc.rx_counter;
			detec_proc.state =2;
		}
		else
		{
			//出现错误.重新开始接收.
			detec_proc.state=0;
		}
	}
	else if(detec_proc.state == 2)
	{
		proc_buf[detec_proc.rx_counter]=revice_data;
		++detec_proc.rx_counter;
		if((proc_buf[1] == 0x05)||(proc_buf[1] == 0x06))
		{
			length = 8;
		}
		else
		{
			length = 15;
		}
		if(detec_proc.rx_counter >=length)
		{
			//数据接收完成.
			detec_proc.state = 3;
		}
	}
	else
	{
		////数据未处理，又接收到数据.
	}
}

unsigned char detec_misc_cmd_process(unsigned char cmd,unsigned char *pdata)
{
	switch(cmd)
	{
		///调零.
		case 0x11: break;
							///退出预热.
		case 0x03: cancel_preheat();
			break;
		////恢复出厂设置.	
		case 0x05: detec_params_recovery_default();
							clear_all_record();
			break;
		////记录清除.
		case 0x06: clear_all_record();
			break;
		default: return 1;
	}
	return 0;
}

unsigned char detec_param_config(unsigned char cmd,unsigned char *pdata)
{
	unsigned int temp;
	unsigned char ret;

	temp=(*pdata<<8)+*(pdata+1);
	
	if((cmd>=12)&&(cmd<=14))
	{
		ret=detec_config_conce_FS(cmd-11, temp);
		if(ret == 0)
		{
			detec_save_config_data(cmd-11);
			event_request(DETEC_CONFIG_EVENT);
		}
	}
	else if((cmd>=15)&&(cmd<=17))
	{
		ret=detec_hardware_config(cmd-14, temp);
		if(ret == 0)
		{
			detec_save_clib_data(cmd-14);
			event_request(DETEC_CLIB_EVENT);
		}
	}
	else if((cmd>=18)&&(cmd<=21))
	{
		ret=detec_config_low_alarm(cmd-18, temp);
		if(ret == 0)
		{
			detec_save_config_data(cmd-18);
			event_request(DETEC_CONFIG_EVENT);
		}
	}
	else if((cmd>=22)&&(cmd<=25))
	{
		ret=detec_config_high_alarm(cmd-22, temp);
		if(ret == 0)
		{
			detec_save_config_data(cmd-22);
			event_request(DETEC_CONFIG_EVENT);
		}
	}
	
	return 0;
}

unsigned char detec_RTC_update(unsigned char *pdata)
{
	//unsigned char time[6];
	unsigned int  temp_CRC_value;
	
	ds1302_write_time(pdata);
//	ds1302_read_time(time);
	temp_CRC_value=CRC16((pdata-7), 6);
	*(pdata-1)= (unsigned char)(temp_CRC_value>>8);
	*pdata = (unsigned char)(temp_CRC_value);
	return 0;	
}


unsigned char detec_comm_setting(void)
{
	unsigned char CRC_len;
	unsigned int CRC_value;
	unsigned int CRC_revice;
	unsigned char i;
	unsigned char cmd;
	unsigned int reg;
	unsigned char ret=0;

	if(EXTTERN_POWER != OFF)
	{
		display_icon_COMM(ON);
	}
	
	if(detec_proc.state != 3)
	{
		return 0;
	}
	cmd = proc_buf[1];
	if(cmd == 0x10)
	{
		CRC_len = 13;
	}
	else
	{
		CRC_len = 6;
	}
	CRC_value = CRC16(proc_buf, CRC_len);
	CRC_revice = (proc_buf[CRC_len]<<8)+proc_buf[CRC_len+1];
	///CRC校验不相等.
	if(CRC_revice != CRC_value)
	{
		////清空接收缓冲区/////
		for(i=0; i<sizeof(proc_buf);i++)
		{
			proc_buf[i]=0;
		}
		////出现错误则发送全0应答.
		detec_proc.tx_counter = 0;
		detec_proc.proc_length = detec_proc.rx_counter;
		uart1_start_send(proc_buf[0]);
		/////允许开始接收数据.
		detec_proc.state=0;
		detec_proc.rx_counter = 0;
		return 1;
	}
	reg = (proc_buf[2]<<8)+ proc_buf[3];
	if(cmd == 0x05)    
	{
		detec_misc_cmd_process(reg, &proc_buf[4]);	
	}
	else if(cmd == 0x06)
	{
		detec_param_config(reg, &proc_buf[4]);	
	}
	else if(cmd == 0x10)
	{
		detec_RTC_update(&proc_buf[7]);
	}
	else
	{
		ret = 2;
	}
	detec_proc.proc_length = detec_proc.rx_counter;
	detec_proc.tx_counter =0;
	uart1_start_send(proc_buf[0]);
	///////允许开始接收数据.
	detec_proc.state = 0;
	detec_proc.rx_counter = 0;
	return ret;
}

#if DEBUG_CACL_CONCE
unsigned char send_debug_Info(unsigned char head,unsigned char *buf,
													unsigned char length)
{
	unsigned char i;
	static unsigned char cmd_num=1;
	
	if(detec_proc.proc_length > 0)
		return 1;
	if(debug_delay_send_counter>0)
	{
		debug_delay_send_counter++;
		if(debug_delay_send_counter>200)
			debug_delay_send_counter=0;
		return 1;
	}
	if(head!=cmd_num)
		return 1;
	cmd_num++;
	if(cmd_num>=4)
	{
		cmd_num=1;
	}
	proc_buf[0]=head;
	for(i=0;i<length;i++)
	{
		proc_buf[i+1]=buf[i];
	}
	detec_proc.proc_length=length+1;
	detec_proc.tx_counter =0;
	uart1_send_data(proc_buf[0]);
	
	return 0;
}

#endif


#ifdef DEBUG_LOW_VOLTAGE
unsigned char send_BATVOL_Info(unsigned char *buf,unsigned char length)
{
	unsigned char i;
	
	if(detec_proc.proc_length > 0)
		return 1;
	
	for(i=0;i<length;i++)
	{
		proc_buf[i]=buf[i];
	}
	detec_proc.proc_length=length;
	detec_proc.tx_counter =0;
	uart1_send_data(proc_buf[0]);

	return 0;
}

#endif


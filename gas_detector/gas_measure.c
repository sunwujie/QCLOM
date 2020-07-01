#include "component_config.h"
#include "gas_measure.h"
#include "gas_detector_cfg.h"


#if DEBUG_CACL_CONCE
extern unsigned char send_debug_Info(unsigned char head,unsigned char *buf,
																		unsigned char length);
#endif

#define FLA  		0
#define TXIC  	1
#define NOMAL		2

//定义温补的高低温度
#define TEMP_H  55
#define TEMP_N  28
#define TEMP_L  -10

//温补数据
const signed short CON_COMP[4][6][2]=
{
//CH4
  {
     {290, 280},    //低温
     {290, 300},    //常温
     {290, 215},    //高温
     {600, 620},    //低温
     {600, 600},    //常温
     {600, 445}
  }, 

//O2#
 { 
     {209, 209},    //低温
     {209, 209},    //常温
     {209, 209},    //高温
     {230, 230},
     {230, 230},
     {230, 230}
 }, 

//CO
//203, 135,    
 { 
     {203, 150},    //低温           //20150826调整
     {203, 200},    //常温
     {203, 310},    //高温
     {600, 460},    //低温
     {600, 600},    //常温
     //600, 880,    //高温
     {600, 870}
 },                

//H2S
 { 
     {305, 270},    //低温			//20150826调整
     {305, 305},    //常温410
     {305, 410},    //高温
     {530, 335},    //低温
     {530, 530},    //常温
     {530, 530}     //高温
 }
};

const signed short span_AD_range[]=
{
    800,
    100,
    250,   
    250,   
};

gas_data gas_des[4];

DetecStr gst_Detec[4];

toxic co_toxic;
toxic h2s_toxic;

/*
///催化探测器.
unsigned char FLADetec_hardware_init(void)
{
	return ads1110_init(gst_Detec[0].clb.pga);
}

////电化学探测器.
unsigned char ELECDetec_hardware_init(void)
{
	return lmp91000_init(gst_Detec[1].clb.pga, gst_Detec[1].clb.bias_vol,
		gst_Detec[2].clb.pga, gst_Detec[2].clb.bias_vol, gst_Detec[3].clb.pga,
																								gst_Detec[3].clb.bias_vol);	
}
*/

//可燃的零点AD和满量程AD值 零点AD: 0mv  满量程AD值: 32768/2.048*(0.12*8)
#define LEL_ZERO_AD                          0
#define LEL_FULLRANGE_AD                     15360

#define O2_ZERO_AD                           3290
#define O2_FULLRANGE_AD                      1980

#define EC_ZERO_AD                           800
#define EC_FULLRANGE_AD                      2691

void gas_data_signal_clear(void)
{
	unsigned char i;
	unsigned char *pdata;

	pdata=(unsigned char *)gas_des;
	for(i=0;i<sizeof(gas_des);i++)
	{
		*pdata=0;
		pdata++;
	}
	for(i=0;i<4;i++)
	{
		gst_Detec[i].alarm_info=0;
		gst_Detec[i].current_conc=0;
		gst_Detec[i].display_conc=0;
		gst_Detec[i].displcd_conc=0;
		gst_Detec[i].last_conc=0;
		gst_Detec[i].overflow_counter=0;
		gst_Detec[i].overflow_flag=0;
		gst_Detec[i].raw_conc=0;
	}
	gst_Detec[1].current_conc=209;
	gst_Detec[1].raw_conc=209;
	gst_Detec[1].last_conc=209;
	gst_Detec[1].display_conc=209;
	gst_Detec[1].displcd_conc=209;

	co_toxic.idlh_conc=0;
	co_toxic.minites_conc_sum=0;
	co_toxic.minites_counter=0;
	co_toxic.seconds_conc_sum=0;
	co_toxic.seconds_counter=0;
	co_toxic.stel_conc=0;
	co_toxic.twa_conc=0;
	co_toxic.stel.count_len=0;
	co_toxic.stel.stel_index=0;
	co_toxic.twa.count_len=0;
	co_toxic.twa.twa_index=0;

	h2s_toxic.idlh_conc=0;
	h2s_toxic.minites_conc_sum=0;
	h2s_toxic.minites_counter=0;
	h2s_toxic.seconds_conc_sum=0;
	h2s_toxic.seconds_counter=0;
	h2s_toxic.stel_conc=0;
	h2s_toxic.twa_conc=0;
	h2s_toxic.stel.count_len=0;
	h2s_toxic.stel.stel_index=0;
	h2s_toxic.twa.count_len=0;
	h2s_toxic.twa.twa_index=0;
}

void gst_Detec_init(void)
{
	unsigned int i;
	unsigned char *pdata;

	pdata=(unsigned char *)&co_toxic;
	for(i=0;i<sizeof(co_toxic);i++)
	{
		*pdata=0;
		pdata++;
	}

	pdata=(unsigned char *)&h2s_toxic;
	for(i=0;i<sizeof(h2s_toxic);i++)
	{
		*pdata=0;
		pdata++;
	}

	pdata=(unsigned char *)gst_Detec;
	for(i=0;i<sizeof(gst_Detec);i++)
	{
		*pdata=0;
		pdata++;
	}
/*	
	pdata=(unsigned char *)&gst_Detec[1];
	for(i=0;i<sizeof(DetecStr);i++)
	{
		*pdata=0;
		pdata++;
	}
	pdata=(unsigned char *)&gst_Detec[2];
	for(i=0;i<sizeof(DetecStr);i++)
	{
		*pdata=0;
		pdata++;
	}
	pdata=(unsigned char *)&gst_Detec[3];
	for(i=0;i<sizeof(DetecStr);i++)
	{
		*pdata=0;
		pdata++;
	}
*/
	co_toxic.stel_alarm=50;
	co_toxic.twa_alarm=35;
	co_toxic.stel_twa_state=OFF;

	h2s_toxic.stel_alarm=150;
	h2s_toxic.twa_alarm=100;
	h2s_toxic.stel_twa_state=OFF;
	
	gst_Detec[0].cfg.alarm_high=500;
	gst_Detec[0].cfg.alarm_high_dir=0;
	gst_Detec[0].cfg.alarm_low=250;
	gst_Detec[0].cfg.alarm_low_dir=0;
	gst_Detec[0].cfg.decimal=1;
	gst_Detec[0].cfg.flag=1;
	gst_Detec[0].cfg.conce_FS=1000;
	gst_Detec[0].cfg.unit=0;

	gst_Detec[0].clb.bias_vol=0;
	gst_Detec[0].clb.pga=8;
	gst_Detec[0].clb.conce_FS_AD=LEL_FULLRANGE_AD;
	gst_Detec[0].clb.span_conc=600;
	gst_Detec[0].clb.conc_zero_AD=LEL_ZERO_AD;
	gst_Detec[0].clb.zero_offset=0;
	
	gst_Detec[0].gas_attr = FLA;
	gst_Detec[0].gas_private = (void *)0;
	gst_Detec[0].gas_type = 0;
	gst_Detec[0].data = &gas_des[0];
	gst_Detec[0].status=DETEC_OK;

	gst_Detec[1].cfg.alarm_high=920;
	gst_Detec[1].cfg.alarm_high_dir=0;
	gst_Detec[1].cfg.alarm_low=720;
	gst_Detec[1].cfg.alarm_low_dir=1;
	gst_Detec[1].cfg.decimal=1;
	gst_Detec[1].cfg.flag=1;
	gst_Detec[1].cfg.conce_FS=250;
	gst_Detec[1].cfg.unit=0;

	gst_Detec[1].clb.bias_vol=0;
	gst_Detec[1].clb.pga=0x0c;   //LMP91000_TIA_7K
	gst_Detec[1].clb.conce_FS_AD=313;   //O2_FULLRANGE_AD;
	gst_Detec[1].clb.span_conc=209;
	gst_Detec[1].clb.conc_zero_AD=O2_ZERO_AD;
	gst_Detec[1].clb.zero_offset=0;
	
	gst_Detec[1].gas_attr = NOMAL;
	gst_Detec[1].gas_private = (void *)0;
	gst_Detec[1].gas_type = 1;
	gst_Detec[1].data= &gas_des[1];
	gst_Detec[1].status=DETEC_OK;
	gst_Detec[1].current_conc=209;
	gst_Detec[1].raw_conc=209;
	gst_Detec[1].last_conc=209;
	gst_Detec[1].display_conc=209;
	gst_Detec[1].displcd_conc=209;
	
	gst_Detec[2].cfg.alarm_high=500;
	gst_Detec[2].cfg.alarm_high_dir=0;
	gst_Detec[2].cfg.alarm_low=250;
	gst_Detec[2].cfg.alarm_low_dir=0;
	gst_Detec[2].cfg.decimal=0;
	gst_Detec[2].cfg.flag=1;
	gst_Detec[2].cfg.conce_FS=1000;
	gst_Detec[2].cfg.unit=0;

	gst_Detec[2].clb.bias_vol=0;
	gst_Detec[2].clb.pga=0x10;   //LMP91000_TIA_14K
	gst_Detec[2].clb.conce_FS_AD=EC_FULLRANGE_AD;
	gst_Detec[2].clb.span_conc=600;
	gst_Detec[2].clb.conc_zero_AD=EC_ZERO_AD;
	gst_Detec[2].clb.zero_offset=0;

	gst_Detec[2].gas_attr = TXIC;
	gst_Detec[2].gas_private = &co_toxic;
	gst_Detec[2].gas_type = 2;
	gst_Detec[2].data = &gas_des[2];
	gst_Detec[2].status=DETEC_OK;

	gst_Detec[3].cfg.alarm_high=500;
	gst_Detec[3].cfg.alarm_high_dir=0;
	gst_Detec[3].cfg.alarm_low=250;
	gst_Detec[3].cfg.alarm_low_dir=0;
	gst_Detec[3].cfg.decimal=1;
	gst_Detec[3].cfg.flag=1;
	gst_Detec[3].cfg.conce_FS=1000;
	gst_Detec[3].cfg.unit=0;

	gst_Detec[3].clb.bias_vol=0;
	gst_Detec[3].clb.pga=0x14;   //LMP91000_TIA_35K
	gst_Detec[3].clb.conce_FS_AD=EC_FULLRANGE_AD;
	gst_Detec[3].clb.span_conc=500;
	gst_Detec[3].clb.conc_zero_AD=EC_ZERO_AD;
	gst_Detec[3].clb.zero_offset=0;
	
	gst_Detec[3].gas_attr = TXIC;
	gst_Detec[3].gas_private = &h2s_toxic;
	gst_Detec[3].gas_type = 3;
	gst_Detec[3].data = &gas_des[3];
	gst_Detec[3].status=DETEC_OK;
}


unsigned char detec_load_data(void)
{
	unsigned int crc_value;
	unsigned int check_crc;	
	unsigned char buf[32];
	unsigned char i,j;
	unsigned char lenth;
	unsigned char *pdata;
	
	for(i=0;i<4;i++)
	{
		AT24CXX_read_page(DETEC_CONFIG_ADDR+i*32, buf,sizeof(gst_Detec[i].cfg));
		lenth=sizeof(gst_Detec[i].cfg)-2;
		crc_value=CRC16(buf,lenth);
		check_crc=(buf[lenth+1]<<8);
		check_crc+=buf[lenth];
		if(check_crc == crc_value)
		{
			pdata=(unsigned char *)&gst_Detec[i].cfg;
			for(j=0;j<sizeof(gst_Detec[i].cfg);j++)
			{
				*pdata=buf[j];
				pdata++;
			}
		}
	}
	for(i=0;i<4;i++)
	{
		AT24CXX_read_page(DETEC_CLIB_ADDR+i*32, buf,sizeof(gst_Detec[i].clb));
		lenth=sizeof(gst_Detec[i].clb)-2;
		crc_value=CRC16(buf,lenth);
		check_crc=(buf[lenth+1]<<8);
		check_crc+=buf[lenth];
		if(check_crc == crc_value)
		{
			pdata=(unsigned char *)&gst_Detec[i].clb;
			for(j=0;j<sizeof(gst_Detec[i].clb);j++)
			{
				*pdata=buf[j];
				pdata++;
			}
		}
	}
	AT24CXX_read_page(DETEC_BAT_INFO_ADDR,buf, 4);
	lenth=2;
	crc_value=CRC16(buf, lenth);
	check_crc=(buf[lenth+1]<<8);
	check_crc+=buf[lenth];
	if(check_crc == crc_value)
	{
		battery_low_voltage_set(((unsigned int)(buf[1]<<8)+buf[0]));
	}
	
#if PUMP_SAVE	
	AT24CXX_read_page(DETEC_PUMP_INFO_ADDR,buf, 4);
	lenth=2;
	crc_value=CRC16(buf, lenth);
	check_crc=(buf[lenth+1]<<8);
	check_crc+=buf[lenth];
	if(check_crc == crc_value)
	{
		pump_set_state(buf[0]);
	}
#endif

	AT24CXX_read_page(DETEC_OTHE_INFO_ADDR,buf,6);
	lenth=4;
	crc_value=CRC16(buf, 4);
	check_crc=(buf[lenth+1]<<8);
	check_crc+=buf[lenth];
	if(check_crc == crc_value)
	{
		detec_set_state(0, buf[0]);
		detec_set_state(1, buf[1]);
		detec_set_state(2, buf[2]);
		detec_set_state(3, buf[3]);
	}

	AT24CXX_read_page(DETEC_TOXIC_ADDR,buf,12);
	lenth=10;
	crc_value=CRC16(buf, 10);
	check_crc=(buf[lenth+1]<<8);
	check_crc+=buf[lenth];
	if(check_crc == crc_value)
	{
		co_toxic.stel_alarm=(buf[1]<<8);
		co_toxic.stel_alarm += buf[0];
		co_toxic.stel_twa_state=buf[2];
		co_toxic.twa_alarm=(buf[4]<<8);
		co_toxic.twa_alarm += buf[3];
		
		h2s_toxic.stel_alarm=(buf[6]<<8);
		h2s_toxic.stel_alarm += buf[5];
		h2s_toxic.stel_twa_state=buf[7];
		h2s_toxic.twa_alarm=(buf[9]<<8);
		h2s_toxic.twa_alarm += buf[8];
	}
	return 0;
}

unsigned char detec_save_toxic_data(void)
{
	unsigned char buf[12];
	unsigned int crc_value;

	buf[0]=(unsigned char)(co_toxic.stel_alarm&0xff);
	buf[1]=(unsigned char)(co_toxic.stel_alarm>>8);
	buf[2]=co_toxic.stel_twa_state;
	buf[3]=(unsigned char)(co_toxic.twa_alarm&0xff);
	buf[4]=(unsigned char)(co_toxic.twa_alarm>>8);
	
	buf[5]=(unsigned char)(h2s_toxic.stel_alarm&0xff);
	buf[6]=(unsigned char)(h2s_toxic.stel_alarm>>8);
	buf[7]=h2s_toxic.stel_twa_state;
	buf[8]=(unsigned char)(h2s_toxic.twa_alarm&0xff);
	buf[9]=(unsigned char)(h2s_toxic.twa_alarm>>8);
	crc_value=CRC16(buf, 10);
	buf[10]=(unsigned char)(crc_value&0xff);
	buf[11]=(unsigned char)(crc_value>>8);
	
	AT24CXX_write_page(DETEC_TOXIC_ADDR, buf, 12);
	
	return 0;
}

unsigned char detec_save_config_data(unsigned char detec)
{
	unsigned char *pdata;
	unsigned int cfg_crc;
	unsigned int addr;

	if(detec>3)
		return 1;
		
	pdata=(unsigned char *)&gst_Detec[detec].cfg;
	cfg_crc=CRC16(pdata, sizeof(gst_Detec[detec].cfg)-2);
	gst_Detec[detec].cfg.crc_value=cfg_crc;
	addr=DETEC_CONFIG_ADDR+detec*32;
	
	AT24CXX_write_page(addr,pdata,sizeof(gst_Detec[detec].cfg));
	return 0;
}

unsigned char detec_save_clib_data(unsigned char detec)
{
	unsigned char *pdata;
	unsigned int cfg_crc;
	unsigned int addr;

	if(detec>3)
		return 1;
	
	pdata=(unsigned char *)&gst_Detec[detec].clb;
	cfg_crc=CRC16(pdata, sizeof(gst_Detec[detec].clb)-2);
	gst_Detec[detec].clb.crc_value=cfg_crc;
	addr=DETEC_CLIB_ADDR+detec*32;
	
	AT24CXX_write_page(addr,pdata,sizeof(gst_Detec[detec].clb));
	return 0;
}

unsigned char detec_save_sleep_voltage(void)
{
	unsigned char buf[4];
	unsigned int voltage;
	unsigned int vol_crc;
	
	voltage=battery_low_voltage_get();
	buf[0]=(unsigned char)(voltage&0xff);
	buf[1]=(unsigned char)(voltage>>8);
	vol_crc=CRC16(buf, 2);
	buf[2]=(unsigned char)(vol_crc&0xff);
	buf[3]=(unsigned char)(vol_crc>>8);
	
	AT24CXX_write_page(DETEC_BAT_INFO_ADDR,buf,4);
	return 0;
}

unsigned char detec_save_pump_state(void)
{
#if PUMP_SAVE
	unsigned char buf[4];
	unsigned int crc;
	
	buf[0]=pump_get_state();
	buf[1]=0;
	crc=CRC16(buf, 2);
	buf[2]=(unsigned char)(crc&0xff);
	buf[3]=(unsigned char)(crc>>8);
	
	AT24CXX_write_page(DETEC_PUMP_INFO_ADDR,buf,4);
#endif
	return 0;	
}

unsigned char detec_save_state(void)
{
	unsigned char buf[6];
	unsigned int crc;
	unsigned char i;

	for(i=0;i<4;i++)
	{
		if(gst_Detec[i].status==DETEC_OFF)
		{
			buf[i]=DETEC_OFF;
		}
		else
		{
			buf[i]=DETEC_OK;
		}
	}
	crc=CRC16(buf, 4);
	buf[i]=(unsigned char)(crc&0xff);
	buf[i+1]=(unsigned char)(crc>>8);
	
	AT24CXX_write_page(DETEC_OTHE_INFO_ADDR,buf,6);
	return 0;	
}

unsigned char detec_params_recovery_default(void)
{
	unsigned char buf[32];
	unsigned char i;
	unsigned int addr;
	volatile unsigned int delay_counter;
	
	for(i=0;i<sizeof(buf);i++)
	{
		buf[i]=0;
	}
	for(i=0; i<4;i++)
	{
		addr=DETEC_CONFIG_ADDR+i*32;
		AT24CXX_write_page(addr,buf,sizeof(buf));
		delay_counter=300;
		while(delay_counter--);
	}
	for(i=0; i<4;i++)
	{
		addr=DETEC_CLIB_ADDR+i*32;
		AT24CXX_write_page(addr,buf,sizeof(buf));
		delay_counter=300;
		while(delay_counter--);
	}
	AT24CXX_write_page(DETEC_BAT_INFO_ADDR,buf,4);
	delay_counter=300;
	while(delay_counter--);
	
	AT24CXX_write_page(DETEC_TOXIC_ADDR,buf,4);
	//delay_counter=300;
	//while(delay_counter--);
	
	gst_Detec_init();
	
	return 0;
}

void sensor_save_signal_value(gas_data *pgas_des,signed int signal_value)
{
	pgas_des->buf[pgas_des->index_num]= signal_value;
	pgas_des->index_num++;
	if(pgas_des->index_num >= SMOOTH_BUFFER_SIZE)
	{
		pgas_des->index_num = 0;
	}
	if(pgas_des->count_len < SMOOTH_BUFFER_SIZE)
	{
		pgas_des->count_len++;
	}	
}

//////读信号值.//////
unsigned char sensor_get_signal_value(void)
{
	unsigned char ret_value;
	signed int fla_value=0;
	
	if((O2_value_check()> 0)
	&&(CO_value_check()> 0)
	&&(H2S_value_check()> 0))
	{
		sensor_save_signal_value(&gas_des[1],get_O2_value());
		sensor_save_signal_value(&gas_des[2],get_CO_value());
		sensor_save_signal_value(&gas_des[3],get_H2S_value());
		ret_value=ads1110_read_data(&fla_value);
		if(ret_value == 0)
		{
			sensor_save_signal_value(&gas_des[0],fla_value);
			return 0;
		}
		else 
			return 2;
	}
	else
		return 1;
}

signed short get_average(signed int buf[],unsigned char len)
{
	unsigned char real_len;
	signed long sum_value=0;
	unsigned char i,j;
	signed int max,min;
	signed int old_max=-32767,old_min=32767;
	signed short ret_value;

	if(len==0)
		return 0;
	
	for(i=0;i<len;i++)
	{
		sum_value += buf[i];
	}
	if(len > 8)
	{
		//代码缺陷:不能求出多个相同的最大值和最小值
		for(j=0; j < 4;j++)
		{
			max =  buf[0];
			min =  buf[0];
			for(i=0;i<len;i++)
			{
				if((max < buf[i])&&(buf[i]<old_max))
				{
					max = buf[i];
				}
				if((min > buf[i])&&(buf[i]>old_min))
				{
					min = buf[i];
				}
			}
			old_max = max;
			old_min = min;
			sum_value = sum_value - max - min;
		}
		real_len = len - 8;
	}
	else
	{
		real_len = len;	
	}
	ret_value = sum_value/real_len;

	return ret_value;
}

signed short linear_process(short in1, short out1, short in2, short out2, 
																																short vout)
{
  signed short vin,jz_b;
  float jz_k;

  jz_k = (float)(in2 - in1) / (float)(out2 - out1);
  jz_b = in1 - (int)(jz_k * out1);

  vin = (short)(jz_k * vout) + jz_b;   

  return vin;
}

//温度线性补偿.
signed short comp_process(unsigned char gas_type,signed short concn,
																									signed char temprature)
{
	signed short out1, out2, vcomp;          
   
  if (temprature > 30)           //高温补偿
  {
    //计算出当前温度的低浓度输出值
    out1 = linear_process(CON_COMP[gas_type][1][1], TEMP_N, 
    											CON_COMP[gas_type][2][1], TEMP_H, temprature);
    //计算出当前温度的高浓度输出值
    out2 = linear_process(CON_COMP[gas_type][4][1], TEMP_N, 
    											CON_COMP[gas_type][5][1], TEMP_H, temprature);
  }
  else if (temprature < 15)      //低温补偿
  {
    //计算出当前温度的低浓度输出值
    out1 = linear_process(CON_COMP[gas_type][0][1], TEMP_L, 
    											CON_COMP[gas_type][1][1], TEMP_N, temprature);
    //计算出当前温度的高浓度输出值
    out2 = linear_process(CON_COMP[gas_type][3][1], TEMP_L, 
    											CON_COMP[gas_type][4][1], TEMP_N, temprature); 		
	}
  else                              //常温补偿
  {     
    out1 = CON_COMP[gas_type][1][1];
    out2 = CON_COMP[gas_type][4][1]; 
  }
  //计算出当前温度的理想浓度值
  vcomp = linear_process(CON_COMP[gas_type][0][0], out1, 
  												CON_COMP[gas_type][3][0],out2, concn);     
  
  return vcomp;
}

signed short gas_get_AD_value(unsigned char which_sensor)
{ 
	if(which_sensor> 3)
		return 0;

	return get_average(gas_des[which_sensor].buf,gas_des[which_sensor].count_len);	
}

unsigned short gas_get_AD_abs_value(unsigned char which_sensor)
{
	signed short temp;
	
	if(which_sensor> 3)
		return 0;

	temp=get_average(gas_des[which_sensor].buf,gas_des[which_sensor].count_len);
	if(temp<0)
	{
		temp=~temp+1;
	}
	return (unsigned short)temp;	
}

//////高浓度淹没判断////////////
unsigned char pure_flag_gas_inundate(unsigned char gas_type)
{
	signed long pure_gas_judge_stadard;
	unsigned char i,index,count=0;
	signed long temp;
	signed long zero_offset_ad;

	temp=(signed long)(gst_Detec[gas_type].clb.conce_FS_AD-gst_Detec[gas_type].clb.conc_zero_AD);
	pure_gas_judge_stadard = temp *11;
	pure_gas_judge_stadard /= 10;
	pure_gas_judge_stadard += gst_Detec[gas_type].clb.conc_zero_AD;

	if(gst_Detec[gas_type].clb.zero_offset != 0)
	{
		zero_offset_ad = temp*(signed)gst_Detec[gas_type].clb.zero_offset;
		zero_offset_ad += 50;
		zero_offset_ad /= 100;
		pure_gas_judge_stadard -= zero_offset_ad;
	}

	count = 0;
	index = gst_Detec[gas_type].data->index_num;
	for(i=0; i < gst_Detec[gas_type].data->count_len;i++)
	{
		if(gst_Detec[gas_type].data->buf[index] >= pure_gas_judge_stadard)
		{
			count++;
			if(count >= OVERFLOW_CONTINUE_COUNT)
			{
				break;
			}
		}
		if(index > 0)
		{
			index--;
		}
		else
		{
			index = SMOOTH_BUFFER_SIZE - 1;
		}
	}
#if DEBUG_CACL_CONCE
	unsigned char info_buf[6];
	unsigned char *pbuf;
	
	if((EXTTERN_POWER != 0)&&(gas_type == (DEBUG_CACL_CONCE-1)))
	{
		info_buf[0]=count;
		pbuf=(unsigned char *)&pure_gas_judge_stadard;
		for(int i=0; i<sizeof(pure_gas_judge_stadard);++i)
		{
			info_buf[i+1]=*pbuf;
			++pbuf;
		}
		send_debug_Info(2,info_buf,1+sizeof(pure_gas_judge_stadard));	
	}
#endif
	if(count >= OVERFLOW_CONTINUE_COUNT)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//////////////信号转换为浓度//////////////////////
short sensor_calc_raw_conce(unsigned char gas_type)
{
	long ad_avg;
//	short span;
	short conce;
//	short conce_FS_10percent;
	short tmp;

	ad_avg = get_average(gas_des[gas_type].buf,gas_des[gas_type].count_len);
#if DEBUG_CACL_CONCE
	unsigned char info_buf[8];
	unsigned char *pbuf;
	int i;
	
	if((EXTTERN_POWER != 0)&&(gas_type == (DEBUG_CACL_CONCE-1)))
	{
		pbuf=(unsigned char *)&ad_avg;
		for(i=0;i<sizeof(ad_avg);++i)
		{
			info_buf[i]=*pbuf;
			++pbuf;
		}
		pbuf=(unsigned char *)&gst_Detec[gas_type].clb.conc_zero_AD;
		for(i=0;i<sizeof(conce);++i)
		{
			info_buf[4+i]=*pbuf;
			++pbuf;
		}
		send_debug_Info(1,info_buf,6);	
	}
#endif	
	if((gas_type == 0)&&(gst_Detec[gas_type].overflow_flag == 0)
	&&(gst_Detec[gas_type].status != DETEC_FAULT))
	{
		gst_Detec[gas_type].overflow_flag = pure_flag_gas_inundate(gas_type);
		if(gst_Detec[gas_type].overflow_flag == 1)
		{
			conce = gst_Detec[gas_type].cfg.conce_FS+(gst_Detec[gas_type].cfg.conce_FS+5)/10;
			gst_Detec[gas_type].overflow_counter++;
			if(gst_Detec[gas_type].overflow_counter >= PUREGAS_OVERFLOW_DELAY_TIME)
			{
				gst_Detec[gas_type].overflow_flag=0;
				gst_Detec[gas_type].overflow_counter=0;
			}
			return conce;
		}
	}
#if DEBUG_CACL_CONCE
	unsigned char buf[6]={0,0};
	
	if((EXTTERN_POWER != 0)&&(gas_type>0)
	&&(gas_type == (DEBUG_CACL_CONCE-1)))
	{
		send_debug_Info(2,buf,1);	
	}
#endif
	if(gas_type == 1)
	{
		if(ad_avg < gst_Detec[gas_type].clb.conc_zero_AD)
		{
			//浓度值为正.
			ad_avg = (gst_Detec[gas_type].clb.conc_zero_AD-ad_avg)
								*((long)gst_Detec[gas_type].cfg.conce_FS);
			ad_avg += (gst_Detec[gas_type].clb.conc_zero_AD-gst_Detec[gas_type].clb.conce_FS_AD)/2;
		}
		else
		{
			ad_avg = (gst_Detec[gas_type].clb.conc_zero_AD-ad_avg)
								*((long)gst_Detec[gas_type].cfg.conce_FS);
			ad_avg -= (gst_Detec[gas_type].clb.conc_zero_AD-gst_Detec[gas_type].clb.conce_FS_AD)/2;
		}
		conce = (short)(ad_avg
					/(gst_Detec[gas_type].clb.conc_zero_AD-gst_Detec[gas_type].clb.conce_FS_AD));
	}
	else
	{
		if(ad_avg >= gst_Detec[gas_type].clb.conc_zero_AD)
		{
			ad_avg = (ad_avg-gst_Detec[gas_type].clb.conc_zero_AD)
							*((long)gst_Detec[gas_type].cfg.conce_FS);
			ad_avg += (gst_Detec[gas_type].clb.conce_FS_AD-gst_Detec[gas_type].clb.conc_zero_AD)/2;
		}
		else
		{
			ad_avg = (ad_avg-gst_Detec[gas_type].clb.conc_zero_AD)
								*((long)gst_Detec[gas_type].cfg.conce_FS);
			ad_avg -=(gst_Detec[gas_type].clb.conce_FS_AD-gst_Detec[gas_type].clb.conc_zero_AD)/2;
		}
		conce = (short)(ad_avg
					/(gst_Detec[gas_type].clb.conce_FS_AD-gst_Detec[gas_type].clb.conc_zero_AD));	
	}
	
#ifdef LINE_COMPENSATION
	//修正线性偏差
  //线性修正起始点,标定点,标定点偏差
  #define LINE_REVISE_STARTPOS_CONC      60
  #define LINE_REVISE_CHECKPOS_CONC      90        
  #define LINE_REVISE_CHECKPOS_OFFSET    5
	if(conce > LINE_REVISE_STARTPOS_CONC)
	{
		
	}
#endif
	if(gst_Detec[gas_type].clb.zero_offset != 0)
	{
		tmp = (short)((long)gst_Detec[gas_type].cfg.conce_FS
					*gst_Detec[gas_type].clb.zero_offset+50)/100;	
		conce -= tmp;
	}
	
	return conce;
}

////////浓度值范围限制处理//////
unsigned short conce_limit(short raw_conce,unsigned short conce_FS)
{
	unsigned short conce_FS_10percent;	

	conce_FS_10percent = (conce_FS+5)/10;
	
	#ifdef NEW_GB_STANDARD
		if(raw_conce < - conce_FS_10percent)
		{
			raw_conce = -conce_FS_10percent;
		}
	#else
		if(raw_conce < 0)
		{
			raw_conce =0;
		}
	#endif
	if(raw_conce > (conce_FS+conce_FS_10percent))
	{
		raw_conce = conce_FS+conce_FS_10percent;
	}
	return raw_conce;
}

///////浓度值埋零点处理///////
unsigned short conce_set_zero_value(unsigned short limit_conce,
																unsigned short conce_FS,unsigned char flag)
{
	unsigned short temp;
	
	if(flag > 0)
	{
		temp = ((long)(conce_FS*5)+100)/200;		
		if(limit_conce < temp)
		{
			limit_conce = 0;
		}
	}
	return limit_conce;
}

///////浓度值阻尼处理///////////
signed short conce_damping_process(signed short new_conce,unsigned char gas_type)
{
	static unsigned short total_counter[4]={0,0,0,0};
	signed short damp_conce;
	long temp;
	signed short diff_conce;
	unsigned short temp_value;
	signed short real_change_value;
	
	if(gst_Detec[gas_type].display_conc == new_conce)
	{
		total_counter[gas_type]=0;
		return new_conce;
	}
	if(total_counter[gas_type] < 100)
	{
		total_counter[gas_type]++;
	}
	if(gst_Detec[gas_type].display_conc>new_conce)
	{
		diff_conce = gst_Detec[gas_type].display_conc - new_conce;
	}
	else
	{
		diff_conce = new_conce - gst_Detec[gas_type].display_conc;
	}
	
	temp =(((long)diff_conce*(long)total_counter[gas_type]*10)/100); 
	
	temp_value =(temp*1000)/(long)gst_Detec[gas_type].cfg.conce_FS;

	//变化量小于0.25%时,变化值为0.
	if(temp_value < CONCN_MIN_INCREASE_L)
	{
		real_change_value = 0;	
	}
	///变化量小于0.75%大于0.25%时,变化值为0.5%.
	else if(temp_value < CONCN_MIN_INCREASE_H)
	{
		real_change_value = (gst_Detec[gas_type].cfg.conce_FS/200);
	}
	
	if(total_counter [gas_type]< 100)
	{
		//变化量大于0.75%时,变化值为1%FS.
		if(temp_value >= CONCN_MIN_INCREASE_H)
		{
			real_change_value = 2*(gst_Detec[gas_type].cfg.conce_FS/200);	
		}
	}
	else
	{
		if(temp_value >= CONCN_MIN_INCREASE_OVER)
		{
			real_change_value = 3*(gst_Detec[gas_type].cfg.conce_FS/200);	
		}
		else if(temp_value >= CONCN_MIN_INCREASE_H)
		{
			real_change_value = 2*(gst_Detec[gas_type].cfg.conce_FS/200);
		}
	}
	
	if(new_conce > gst_Detec[gas_type].display_conc)
	{
		damp_conce = gst_Detec[gas_type].display_conc + real_change_value;
	}
	else
	{
		damp_conce = gst_Detec[gas_type].display_conc - real_change_value;
	}
	return damp_conce;
}

///////浓度值显示处理/////////
signed short conce_display_process(signed short *damp_conce,unsigned char gas_type)																														
{
	static unsigned char show_count[4]={0,0,0,0};
	static unsigned char show_flag[4]={1,1,1,1};
	static signed short temp_conce[4]={0,0,0,0};
	
	if((*damp_conce > 50)||(*damp_conce <= 0))
	{
		if((*damp_conce%10)<5)
		{
			*damp_conce -= (*damp_conce%10);
		}
		else
		{
			*damp_conce -= ((*damp_conce%10)-5);
		}
		show_count[gas_type]=0;
		show_flag[gas_type]=1;
		temp_conce[gas_type]= *damp_conce;
	}
	else 
	{
		show_count[gas_type]++;
		if((show_count[gas_type]%2)== 0)
		{
			show_count[gas_type]=0;
			if(show_flag[gas_type] == 1)
			{
				temp_conce[gas_type] -= 5;
				if(temp_conce[gas_type] <=5)
					temp_conce[gas_type] =0;
			}
			else
			{
				if(temp_conce[gas_type] < 50)
					temp_conce[gas_type] += 5;
			}
		}
	}
  return temp_conce[gas_type];
}
																												

///////////////stel与twa浓度计算/////////////////////////////
unsigned char stel_twa_calc_conc(toxic *ptoxi,signed short conc)
{
	unsigned char i;
	float conc_sum =0;

	if(ptoxi->stel_twa_state == 0)
		return 0;
	
	ptoxi->seconds_counter++;
	if(ptoxi->seconds_counter >= NUM_SIGNAL_OF_ONE_SECOD)
	{
		ptoxi->seconds_counter=0;
		ptoxi->minites_conc_sum += conc;
		ptoxi->minites_counter++;
		if(ptoxi->minites_counter >= NUM_SIGNAL_OF_ONE_MINITE)
		{
			ptoxi->stel.buf[ptoxi->stel.stel_index]=
											(float)ptoxi->minites_conc_sum/NUM_SIGNAL_OF_ONE_MINITE;
			ptoxi->stel.stel_index++;
			if(ptoxi->stel.stel_index >= STEL_BUF_SIZE)
			{
				ptoxi->stel.stel_index = 0;
			}
			if(ptoxi->stel.count_len < STEL_BUF_SIZE)
			{
				ptoxi->stel.count_len++;
			}
			for(i=0; i< ptoxi->stel.count_len;i++)
			{
				conc_sum += ptoxi->stel.buf[i];
			}
			ptoxi->stel_conc = (int)conc_sum /ptoxi->stel.count_len;
			ptoxi->twa.buf[ptoxi->twa.twa_index]=ptoxi->stel_conc;
			ptoxi->twa.twa_index++;
			if(ptoxi->twa.twa_index >= TWA_BUF_SIZE)
			{
				ptoxi->twa.twa_index = 0;
			}
			if(ptoxi->twa.count_len < TWA_BUF_SIZE)
			{
				ptoxi->twa.count_len++;
			}
			conc_sum=0;
			for(i=0;i<ptoxi->twa.count_len;i++)
			{
				conc_sum += ptoxi->twa.buf[i];
			}
			ptoxi->twa_conc = (short)(conc_sum/ptoxi->twa.count_len);
			ptoxi->minites_conc_sum = 0;
			ptoxi->minites_counter = 0;
		}
	}
  return 0;
}

unsigned char gas_conce_calc_process(unsigned char gas_type,
																					signed char temprature)
{
//	signed short raw_conce;
	signed short limit_conce;
	signed short zero_conce;
	signed short tempr_conce;
	signed short damp_conce;
	unsigned int conce_FS;
	toxic * ptoxi;

	//若探测器被屏蔽,则直接返回.
	if(gst_Detec[gas_type].status == DETEC_OFF)
		return 0;
	
	conce_FS=gst_Detec[gas_type].cfg.conce_FS;
	
	//计算气体原始浓度.
	gst_Detec[gas_type].raw_conc=sensor_calc_raw_conce(gas_type);
	
	///未发生高浓度淹没,对浓度值进行限制大小.
	if(gst_Detec[gas_type].overflow_flag == 0){
		limit_conce=conce_limit(gst_Detec[gas_type].raw_conc, conce_FS);
	}
	else{
		//不需要修正浓度值,固定为110%FS
		limit_conce=gst_Detec[gas_type].raw_conc;
	}
	
	////小信号埋零点处理. zero_conce为埋零点后的浓度值.
	zero_conce=conce_set_zero_value(limit_conce,conce_FS,
																							gst_Detec[gas_type].cfg.flag);
	
	gst_Detec[gas_type].last_conc = gst_Detec[gas_type].current_conc;	
	gst_Detec[gas_type].current_conc = zero_conce;
	
	if((gst_Detec[gas_type].cfg.conce_FS == 1000)||gas_type == 1)
	{
		///对浓度进行温度补偿. tempr_conce为温度补偿后的浓度值.
		tempr_conce=comp_process(gas_type, zero_conce,temprature);
    //tempr_conce=0;
		///对FLA气体进行单独线性处理.
		if((gas_type == 0)&&(tempr_conce > 600)
		&&(temprature>15)&&(temprature<35))
		{
			tempr_conce= linear_process(600,600,900,865,tempr_conce);
		}
	}
	else
	{
		//////不进行温度补偿./////
		tempr_conce=zero_conce;
	}
	
	/////damp process:damp_conce为阻尼处理后的浓度值.
	damp_conce=conce_damping_process(tempr_conce,gas_type);
	//damp_conce=zero_conce;
	//damp_conce=tempr_conce;
	
	if((damp_conce>0)&&(damp_conce>(conce_FS+(conce_FS+5)/10)))
	{
		damp_conce=conce_FS+(conce_FS+5)/10;	
	}
	else if(damp_conce<0)
	{
		damp_conce=0;	
	}
	
	gst_Detec[gas_type].display_conc=damp_conce;
	
	if((gas_type!=1)&&(conce_FS>500)){
		
		gst_Detec[gas_type].displcd_conc=conce_display_process(&gst_Detec[gas_type].display_conc,gas_type);
	}
	else
	{
		gst_Detec[gas_type].displcd_conc=damp_conce;	
	}
	ptoxi=gst_Detec[gas_type].gas_private;
	if(ptoxi != (void *)0)
		stel_twa_calc_conc(ptoxi, gst_Detec[gas_type].raw_conc);
	
#if DEBUG_CACL_CONCE	
	unsigned char temp_buf[14];
	unsigned char *pbuf;
	unsigned char i;

	if((EXTTERN_POWER != 0)&&(gas_type == (DEBUG_CACL_CONCE-1)))
	{
		pbuf=(unsigned char *)&gst_Detec[gas_type].raw_conc;
		for(i=0;i<2;i++)
		{
			temp_buf[i]=*pbuf;
			++pbuf;
		}
		pbuf=(unsigned char *)&limit_conce;
		for(i=0;i<2;i++)
		{
			temp_buf[2+i]=*pbuf;
			++pbuf;
		}
		pbuf=(unsigned char *)&zero_conce;
		for(i=0;i<2;i++)
		{
			temp_buf[4+i]=*pbuf;
			++pbuf;
		}
		pbuf=(unsigned char *)&tempr_conce;
		for(i=0;i<2;i++)
		{
			temp_buf[6+i]=*pbuf;
			++pbuf;
		}
		pbuf=(unsigned char *)&damp_conce;
		for(i=0;i<2;i++)
		{
			temp_buf[8+i]=*pbuf;
			++pbuf;
		}
		pbuf=(unsigned char *)&conce_FS;
		for(i=0;i<sizeof(conce_FS);i++)
		{
			temp_buf[10+i]=*pbuf;
			++pbuf;
		}
		send_debug_Info(3,temp_buf,12);
	}
#endif	

	return 0;
}

unsigned short detec_get_conce(unsigned char which_detec)
{
	if(which_detec >3)
		return (~0);
	
	return gst_Detec[which_detec].display_conc;
}

unsigned char detec_get_state(unsigned char which_detec)
{
	if(which_detec >3)
		return 0xff;
	
	return gst_Detec[which_detec].status;
}

unsigned char detec_set_state(unsigned char which_detec,unsigned char state)
{
	if(which_detec >3)
		return 0xff;
	
	gst_Detec[which_detec].status=state;

	return state;
}

////探测器故障判断,目前只能用来判断可燃性气体.
unsigned char sensor_state_check(unsigned char gas_type)
{
	signed short temp;
	static unsigned char fault_counter=0;

	if(gst_Detec[gas_type].status == DETEC_OFF)
		return 0;
	
	temp=get_average(gas_des[gas_type].buf,gas_des[gas_type].count_len);
	//temp=0;
	if((temp<FLA_HIGH_LIMIT)&&(temp>FLA_LOW_LIMIT))
	{
		fault_counter=0;		
		gst_Detec[gas_type].status = DETEC_OK;
	}
	else
	{
		fault_counter++;
		if(fault_counter>= 20)
		{
			fault_counter=20;
			gst_Detec[gas_type].status = DETEC_FAULT;
			return 1;
		}
	}
	return 0;
}

unsigned char Detec_regular_alarm_check(unsigned char gas_type)
{
  unsigned short conce_110fs, conce_105fs;
  unsigned short conce_alarm1, conce_alarm2;
	unsigned char alarm_value=0;
	unsigned long conce_conce_FS;
	
	if((gst_Detec[gas_type].status ==DETEC_FAULT)
	||(gst_Detec[gas_type].status ==DETEC_OFF))
		return 0;
	
	conce_conce_FS=(unsigned long)gst_Detec[gas_type].cfg.conce_FS;
	conce_110fs = (conce_conce_FS*11+5)/10;
	conce_105fs = (conce_conce_FS*105+50)/100;
	if(gst_Detec[gas_type].display_conc >= conce_110fs)
	{
		alarm_value |= (1<<OVRANG_ALARM);	
	}
	else if(gst_Detec[gas_type].display_conc < conce_105fs)
	{
		alarm_value &= ~(1<<OVRANG_ALARM);	 
	}

	conce_alarm1 = (((long)gst_Detec[gas_type].cfg.alarm_high*conce_conce_FS+50)
										/1000);
	///1:下报. 0: 上报.
	if(gst_Detec[gas_type].cfg.alarm_high_dir == 1)
	{
		if(gst_Detec[gas_type].display_conc <= conce_alarm1)
		{
			alarm_value |= (1<<H_ALARM);	
		}
		else
		{
			alarm_value &= ~(1<<H_ALARM);
		}
	}
	else
	{
		if(gst_Detec[gas_type].display_conc >= conce_alarm1)
		{
			alarm_value |= (1<<H_ALARM); 
		}
		else
		{
			alarm_value &= ~(1<<H_ALARM);
		}
	}
	
	conce_alarm2 = (((long)gst_Detec[gas_type].cfg.alarm_low*conce_conce_FS+50)
										/1000);
	if(gst_Detec[gas_type].cfg.alarm_low_dir == 1)
	{
		if(gst_Detec[gas_type].display_conc <= conce_alarm2)
		{
			alarm_value |= (1<<L_ALARM);	
		}
		else
		{
			alarm_value &= ~(1<<L_ALARM);
		}
	}
	else
	{
		if(gst_Detec[gas_type].display_conc >= conce_alarm2)
		{
			alarm_value |= (1<<L_ALARM); 
		}
		else
		{
			alarm_value &= ~(1<<L_ALARM);
		}
	}
	if(alarm_value >0)
	{
	//	gst_Detec[gas_type].alarm_info=alarm_value;
		gst_Detec[gas_type].status=DETEC_ALARM;
	}
	else
	{
		if(gas_type>0)
		{
			gst_Detec[gas_type].status=DETEC_OK;	
		}
	}
	return alarm_value;
}


unsigned char detec_toxic_alarm_check(unsigned char gas_type)
{
	toxic *ptoxi;
	unsigned char alarm_value=0;

	if((gst_Detec[gas_type].status ==DETEC_FAULT)
		||(gst_Detec[gas_type].status ==DETEC_OFF))
		return 0;
	
	ptoxi= gst_Detec[gas_type].gas_private;
	if((ptoxi == (void *)0)||(ptoxi->stel_twa_state == OFF))
		return 0;

	if(ptoxi->stel_conc > ptoxi->stel_alarm)
	{
		alarm_value |= (1<<STEL_ALARM);
	}
	else
	{
		alarm_value &= ~(1<<STEL_ALARM);
	}
	
	if(ptoxi->twa_conc > ptoxi->twa_alarm)
	{
		alarm_value |= (1<<TWA_ALARM);
	}
	else
	{
		alarm_value &= ~(1<<TWA_ALARM);
	}
	if(alarm_value >0)
	{
	//	gst_Detec[gas_type].alarm_info=alarm_value;
		gst_Detec[gas_type].status=DETEC_ALARM;
	}
	return alarm_value;
}


unsigned char detec_set_high_alarm_vaue(unsigned char gas_type,unsigned short alarm)
{
	if((gas_type >3)||(alarm>gst_Detec[gas_type].cfg.conce_FS))
		return 1;
	
	gst_Detec[gas_type].cfg.alarm_high=alarm;
	return 0;
}

unsigned short detec_get_high_alarm_value(unsigned char gas_type)
{
	if(gas_type>3)
		return 1;

	return gst_Detec[gas_type].cfg.alarm_high;
}

unsigned char detec_set_low_alarm_vaue(unsigned char gas_type,unsigned short alarm)
{
	if((gas_type>3)||(alarm>gst_Detec[gas_type].cfg.conce_FS))
		return 1;
	
	gst_Detec[gas_type].cfg.alarm_low=alarm;
	return 0;
}

unsigned short detec_get_low_alarm_value(unsigned char gas_type)
{
	if(gas_type>3)
		return 1;

	return gst_Detec[gas_type].cfg.alarm_low;	
}

unsigned short detec_get_TWA_alarm_value(unsigned char gas_type)
{
	toxic *ptoxi;
		
	if((gas_type>3)||(gst_Detec[gas_type].gas_private == (void *)0))
		return 1;
	
	ptoxi= gst_Detec[gas_type].gas_private;

	return ptoxi->twa_alarm; 
}

unsigned char detec_set_TWA_alarm_value(unsigned char gas_type,unsigned short alarm)
{
	toxic *ptoxi;
		
	if((gas_type>3)||(gst_Detec[gas_type].gas_private == (void *)0))
		return 1;
	
	ptoxi= gst_Detec[gas_type].gas_private;
	ptoxi->twa_alarm=alarm;

	return 0;
}

unsigned short detec_get_STEL_alarm_value(unsigned char gas_type)
{
	toxic *ptoxi;
		
	if((gas_type>3)||(gst_Detec[gas_type].gas_private == (void *)0))
		return 1;
	
	ptoxi= gst_Detec[gas_type].gas_private;

	return ptoxi->stel_alarm; 
}


unsigned short detec_set_STEL_alarm_value(unsigned char gas_type,unsigned short alarm)
{
	toxic *ptoxi;
		
	if((gas_type>3)||(gst_Detec[gas_type].gas_private == (void *)0))
		return 1;
	
	ptoxi= gst_Detec[gas_type].gas_private;
	ptoxi->stel_alarm=alarm; 

	return 0;
}

unsigned short detec_set_TWASTEL_state(unsigned char gas_type,unsigned short state)
{
	toxic *ptoxi;
		
	if((gas_type>3)||(gst_Detec[gas_type].gas_private == (void *)0))
		return 1;
	
	ptoxi= gst_Detec[gas_type].gas_private;
	ptoxi->stel_twa_state =state; 

	return 0;
}

unsigned char detec_get_TWASTEL_state(unsigned char gas_type)
{
	toxic *ptoxi;
		
	if((gas_type>3)||(gst_Detec[gas_type].gas_private == (void *)0))
		return 1;
	
	ptoxi= gst_Detec[gas_type].gas_private;

	return ptoxi->stel_twa_state ; 
}

unsigned short detec_get_conce_FS(unsigned char gas_type)
{
	if(gas_type>3)
		return 0;

	return gst_Detec[gas_type].cfg.conce_FS;
}


unsigned char detec_set_conce_FS(unsigned char gas_type,unsigned short temp_conce_FS)
{
	if(gas_type>3)
		return 1;

	gst_Detec[gas_type].cfg.conce_FS=temp_conce_FS;
	return 0;	
}

unsigned char detec_update_clb_param(unsigned char which_detec,signed short new_conc_zero_AD)
{
	signed short zero_offset;

	zero_offset = new_conc_zero_AD-gst_Detec[which_detec].clb.conc_zero_AD;
	if((zero_offset+gst_Detec[which_detec].clb.conce_FS_AD) <26000)
	{
		gst_Detec[which_detec].clb.conce_FS_AD += zero_offset;
		gst_Detec[which_detec].clb.conc_zero_AD = new_conc_zero_AD;
		gst_Detec[which_detec].clb.zero_offset=0;	
		return 0;
	}
	else
	{
		return 1;
	}
}

unsigned char detec_update_clb_conce_FS(unsigned char which_detec,signed short span_conce)
{
	unsigned short conce_FS;
	signed short differ_ad;
	float differ_adf;
	signed short span_offset;
	signed short span_conce_FS_AD;
	unsigned char ret_value=0;
	
	if(which_detec == 1)
	{
		differ_ad=gst_Detec[which_detec].clb.conc_zero_AD
								-get_average(gas_des[which_detec].buf,gas_des[which_detec].count_len);		
	}
	else
	{
		differ_ad=get_average(gas_des[which_detec].buf,gas_des[which_detec].count_len)
								-gst_Detec[which_detec].clb.conc_zero_AD;
	}
	
	if(differ_ad>50)
	{
		conce_FS=gst_Detec[which_detec].cfg.conce_FS;
		differ_adf=(float)differ_ad;	
		differ_adf=(differ_adf*conce_FS)/span_conce;
		span_offset=(signed short)differ_adf;
		
		if(which_detec==1)
		{
			span_conce_FS_AD=gst_Detec[1].clb.conc_zero_AD-span_offset;	
		}
		else
		{
			span_conce_FS_AD=gst_Detec[which_detec].clb.conc_zero_AD+span_offset;
		}
		if((span_conce_FS_AD<26000)&&(span_offset>span_AD_range[which_detec]))
		{
			gst_Detec[which_detec].clb.span_conc=span_conce;
			gst_Detec[which_detec].clb.conce_FS_AD=span_conce_FS_AD;
			ret_value=2;
		}
		else
		{
			ret_value=1;	
		}
	}
	else
	{
		ret_value=1;
	}
	return ret_value;
}

unsigned char detec_config_conce_FS(unsigned char which_detec,unsigned int conce_FS)
{
	if((which_detec<1)||(which_detec>3))
		return 1;
	
	if(conce_FS<=10)
	{
		gst_Detec[which_detec].cfg.conce_FS=conce_FS*100;
		gst_Detec[which_detec].cfg.decimal=2;
	}
	else if(conce_FS<=100)
	{
		gst_Detec[which_detec].cfg.conce_FS=conce_FS*10;
		gst_Detec[which_detec].cfg.decimal=1;
	}
	else
	{
		gst_Detec[which_detec].cfg.conce_FS=conce_FS;
		gst_Detec[which_detec].cfg.decimal=0;
	}

	return 0;
}


unsigned char detec_hardware_config(unsigned char which_detec,unsigned char pga)
{
	if((which_detec<1)||(which_detec>3))
		return 1;
	
////#define   LMP91000_TIA_2_75K       0x04
	if(gst_Detec[which_detec].clb.pga != (0x04+pga*4))
	{
		gst_Detec[which_detec].clb.pga=0x04+pga*4;
		///重新配置传感器后,需要重新预热和清除以前的数据.
		if(which_detec == 1)
		{
			lmp910x_O2_init(gst_Detec[1].clb.pga,gst_Detec[1].clb.bias_vol);
		}
		else if(which_detec == 2)
		{
			lmp910x_CO_init(gst_Detec[2].clb.pga,gst_Detec[2].clb.bias_vol);	
		}
		else
		{
			lmp910x_H2S_init(gst_Detec[3].clb.pga,gst_Detec[3].clb.bias_vol);	
		}
	}
	return 0;
}

unsigned char detec_config_high_alarm(unsigned char which_detec,unsigned int high_alarm)
{
	if(which_detec>3)
		return 1;

	gst_Detec[which_detec].cfg.alarm_high=high_alarm;

	return 0;
}


unsigned char detec_config_low_alarm(unsigned char which_detec,unsigned int low_alarm)
{
	if(which_detec>3)
		return 1;
	
	gst_Detec[which_detec].cfg.alarm_low=low_alarm;

	return 0;
}


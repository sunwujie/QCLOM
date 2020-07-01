#include "IO_define.h"

#define   LMP91000_ADDR                  		0x90

#define   LMP91000_STATUS_REG_ADDR     			0x00
#define   LMP91000_LOCK_REG_ADDR       			0x01
#define   LMP91000_TIACN_REG_ADDR      			0x10
#define   LMP91000_REFCN_REG_ADDR      			0x11
#define   LMP91000_MODECN_REG_ADDR     			0x12

#define   LMP91000_UNLOCK                   0x00
#define   LMP91000_LOCK_READ_ONLY           0x01

#define   LMP91000_TIA_2_75K                0x04
#define   LMP91000_TIA_3_5K                 0x08
#define   LMP91000_TIA_7K                   0x0C
#define   LMP91000_TIA_14K                  0x10
#define   LMP91000_TIA_35K                  0x14
#define   LMP91000_TIA_120K                 0x18
#define   LMP91000_TIA_350K                 0x1C

#define   LMP91000_RLOAD_10                 0x00
#define   LMP91000_RLOAD_33                 0x01
#define   LMP91000_RLOAD_50                 0x02
#define   LMP91000_RLOAD_100                0x03

#define   LMP91000_REF_SOURCE_INT           0x00
#define   LMP91000_REF_SOURCE_EXT           0x80

#define   LMP91000_INT_Z_20                 0x00
#define   LMP91000_INT_Z_50                 0x20
#define   LMP91000_INT_Z_67                 0x40
#define   LMP91000_INT_Z_O2                 0x60

#define   LMP91000_BIAS_SIGN_NEG            0x00
#define   LMP91000_BIAS_SIGN_POS            0x10

#define   LMP91000_BIAS_0                   0x00
#define   LMP91000_BIAS_1                   0x01
#define   LMP91000_BIAS_2                   0x02
#define   LMP91000_BIAS_4                   0x03
#define   LMP91000_BIAS_6                   0x04
#define   LMP91000_BIAS_8                   0x05
#define   LMP91000_BIAS_10                  0x06
#define   LMP91000_BIAS_12                  0x07
#define   LMP91000_BIAS_14                  0x08
#define   LMP91000_BIAS_16                  0x09
#define   LMP91000_BIAS_18                  0x0A
#define   LMP91000_BIAS_20                  0x0B
#define   LMP91000_BIAS_22                  0x0C
#define   LMP91000_BIAS_24                  0x0D

#define   LMP91000_OP_MODE_DEEP_SLEEP       0x00
#define   LMP91000_OP_MODE_2_LEAD           0x01
#define   LMP91000_OP_MODE_STANDBY          0x02
#define   LMP91000_OP_MODE_3_LEAD           0x03
#define   LMP91000_OP_MODE_TEMP_TIA_OFF     0x06
#define   LMP91000_OP_MODE_TEMP_TIA_ON      0x07


#define short_delay()  NOP

#define iic4_init()  {DAT4_HIGH;CLK4_HIGH;}

static void iic4_start(void)
{
	DISABLE_INT;
	
	DAT4_OUTPUT;
	DAT4_HIGH;
	short_delay();
	CLK4_HIGH;
	short_delay();
	DAT4_LOW;
	short_delay();
	CLK4_LOW;
	ENABLE_INT;
	short_delay();
}

static void iic4_stop(void)
{
	DISABLE_INT;

	CLK4_HIGH;
	short_delay();
	DAT4_OUTPUT;
	DAT4_HIGH;
	short_delay();
	DAT4_HIGH;
	ENABLE_INT;
	short_delay();
}

static void iic4_write_byte(unsigned char byte)
{
	unsigned char i;

	DISABLE_INT;
	
	DAT4_OUTPUT;
	short_delay();
	for(i=0;i<8;i++)
	{
		if(byte & (1<<(7-i)))
		{
			DAT4_HIGH;	
		}
		else
		{
			DAT4_LOW;
		}
		short_delay();
		CLK4_HIGH;
		short_delay();
		CLK4_LOW;
	}
	DAT4_LOW;
	short_delay();
	CLK4_HIGH;
	short_delay();
	CLK4_LOW;
	ENABLE_INT;
	short_delay();
}

#if 0

static unsigned char iic4_read_byte(void)
{
	unsigned char data;
	unsigned char i;

	DISABLE_INT;

	DAT4_INPUT;
	short_delay();
	for(i=0;i<8;i++)
	{
		CLK4_HIGH;
		short_delay();
		if(DAT4_IS_HIGH)
		{
			data |= (1<<(7-i));
		}
		else
		{
			data &= ~(1<<(7-i));
		}
		CLK4_LOW;
		short_delay();
	}
	DAT4_OUTPUT;
	DAT4_LOW;
	short_delay();
	CLK4_HIGH;
	short_delay();
	CLK4_LOW;
	ENABLE_INT;
	short_delay();
	
  return data;
}

#endif

static unsigned char lmp91000_write_data(unsigned char slave_addr, 
                                 unsigned char reg_addr, unsigned char data)
{
	iic4_start();
	iic4_write_byte(slave_addr);
	short_delay();
	iic4_write_byte(reg_addr);
	short_delay();
	iic4_write_byte(data);
	short_delay();
	iic4_stop();
	
 	return 0; 
}

#if 0
static unsigned char lmp91000_read_data(unsigned char slave_addr, 
                                        			unsigned short reg_addr)
{
	unsigned char data;

	iic4_start();
	iic4_write_byte(slave_addr);
	short_delay();
	iic4_write_byte(reg_addr);
	short_delay();

	iic4_start();
	iic4_write_byte(slave_addr|0x01);
	short_delay();
	data = iic4_read_byte();
	short_delay();
	iic4_stop();

	return data;
}
																							
#endif

void lmp910x_O2_init(unsigned char O2_pag,unsigned char O2_bias)
{
	//////MENB3: O2
	MENB3_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_LOCK_REG_ADDR,
																													LMP91000_UNLOCK);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_TIACN_REG_ADDR,
																						O2_pag+LMP91000_RLOAD_10);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_REFCN_REG_ADDR,
		LMP91000_REF_SOURCE_EXT+ LMP91000_INT_Z_67+LMP91000_BIAS_SIGN_POS
																													+O2_bias);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																									LMP91000_OP_MODE_3_LEAD);
	short_delay();
	short_delay();
	short_delay();
	MENB3_HIGH;
}

void lmp910x_O2_powerOff(void)
{
	MENB3_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																	0x80+LMP91000_OP_MODE_DEEP_SLEEP);
	short_delay();
	short_delay();
	short_delay();
	MENB3_HIGH;
}

void lmp910x_H2S_init(unsigned char H2S_pag,unsigned char H2S_bias)
{
	MENB2_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_LOCK_REG_ADDR, 
																												LMP91000_UNLOCK);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_TIACN_REG_ADDR,
																							H2S_pag+LMP91000_RLOAD_10);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_REFCN_REG_ADDR, 
		LMP91000_REF_SOURCE_EXT+ LMP91000_INT_Z_20+LMP91000_BIAS_SIGN_POS
																														+H2S_bias);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																								LMP91000_OP_MODE_3_LEAD);
	short_delay();
	short_delay();
	short_delay();
	MENB2_HIGH;
}

void lmp910x_H2S_powerOff(void)
{
  MENB2_LOW;
	short_delay();
	short_delay();
	short_delay();
  lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																				0x80+LMP91000_OP_MODE_DEEP_SLEEP);
	short_delay();
	short_delay();
	short_delay();
	MENB2_HIGH;

}
void lmp910x_CO_init(unsigned char CO_pag,unsigned char CO_bias)
{
	//////MENB4: CO
	MENB4_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_LOCK_REG_ADDR, 
																														LMP91000_UNLOCK);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_TIACN_REG_ADDR,
																							CO_pag+LMP91000_RLOAD_10);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_REFCN_REG_ADDR, 
		LMP91000_REF_SOURCE_EXT+ LMP91000_INT_Z_20+LMP91000_BIAS_SIGN_POS
																														+CO_bias);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																									LMP91000_OP_MODE_3_LEAD);
	short_delay();
	short_delay();
	short_delay();
	MENB4_HIGH;	
}

void lmp910x_CO_powerOff(void)
{
  MENB4_LOW;
	short_delay();
	short_delay();
	short_delay();
  lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																					0x80+LMP91000_OP_MODE_DEEP_SLEEP);
	short_delay();
	short_delay();
	short_delay();
	MENB4_HIGH;
}


/////sensor1_pag: O2.
////sensor2_pag:  CO.
/////sensor3_pag: H2S.

unsigned char lmp91000_init(unsigned char O2_pag,unsigned char O2_bias,unsigned char CO_pag,
				unsigned char CO_bias,unsigned char H2S_pag,unsigned char H2S_bias)
{
	MENB2_HIGH;
	MENB3_HIGH;
	MENB4_HIGH;
  iic4_init();

	lmp910x_H2S_init(H2S_pag,H2S_bias);
	lmp910x_O2_init(O2_pag,O2_bias);
	lmp910x_CO_init(CO_pag,CO_bias);
/*
//////MENB2: H2S
	MENB2_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_LOCK_REG_ADDR, 
																												LMP91000_UNLOCK);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_TIACN_REG_ADDR,
																							sensor3_pag+LMP91000_RLOAD_10);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_REFCN_REG_ADDR, 
		LMP91000_REF_SOURCE_EXT+ LMP91000_INT_Z_20+LMP91000_BIAS_SIGN_POS
																														+sensor3_bias);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																								LMP91000_OP_MODE_3_LEAD);
	short_delay();
	short_delay();
	short_delay();
	MENB2_HIGH;

	//////MENB3: O2
	MENB3_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_LOCK_REG_ADDR,
																													LMP91000_UNLOCK);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_TIACN_REG_ADDR,
																						sensor1_pag+LMP91000_RLOAD_10);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_REFCN_REG_ADDR,
		LMP91000_REF_SOURCE_EXT+ LMP91000_INT_Z_67+LMP91000_BIAS_SIGN_POS
																													+sensor1_bias);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																									LMP91000_OP_MODE_3_LEAD);
	short_delay();
	short_delay();
	short_delay();
	MENB3_HIGH;

	//////MENB4: CO
	MENB4_LOW;
	short_delay();
	short_delay();
	short_delay();
	lmp91000_write_data(LMP91000_ADDR, LMP91000_LOCK_REG_ADDR, 
																														LMP91000_UNLOCK);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_TIACN_REG_ADDR,
																							sensor2_pag+LMP91000_RLOAD_10);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_REFCN_REG_ADDR, 
		LMP91000_REF_SOURCE_EXT+ LMP91000_INT_Z_20+LMP91000_BIAS_SIGN_POS
																														+sensor2_bias);
	lmp91000_write_data(LMP91000_ADDR, LMP91000_MODECN_REG_ADDR,
																									LMP91000_OP_MODE_3_LEAD);
	short_delay();
	short_delay();
	short_delay();
	MENB4_HIGH;
*/	
  return 0;
}

/*
unsigned char lmp910x_poweroff(void)
{  

	
  return 0;
}
*/



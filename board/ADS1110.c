#include "IO_define.h"

#define DEV_ADDR 0x90

#define CONFIG_PGA8  0x8F        //PGA=8
#define CONFIG_PGA4  0x8E        //PGA=4
#define CONFIG_PGA2  0x8D        //PGA=2
#define CONFIG_PGA1  0x8C        //PGA=1

#define DELAY_NOP(x) {a=x;while(a--);}

#define SHORT_DELAY()   DELAY_NOP(18)

#define iic1_init()    {DAT1_HIGH;CLK1_HIGH;}


static void iic1_start(void)
{
	unsigned char a;

	DAT1_HIGH;
	DAT1_OUTPUT;	
	SHORT_DELAY();
	CLK1_HIGH;
	SHORT_DELAY();
	DAT1_LOW;
	SHORT_DELAY();
	CLK1_LOW;
	SHORT_DELAY();
}

static void iic1_stop(void)
{
	unsigned char a;
	
	CLK1_HIGH;
	SHORT_DELAY();
	SHORT_DELAY();
	DAT1_HIGH;
	SHORT_DELAY();
}

static void iic1_write(unsigned char data)
{
	unsigned char i;
	unsigned char a;

	DAT1_OUTPUT;
	SHORT_DELAY();
	for(i=0;i<8;i++)
	{
		if(data & (1<<(7-i)))
		{
			DAT1_HIGH;
		}
		else
		{
			DAT1_LOW;
		}
		SHORT_DELAY();
		CLK1_HIGH;
		SHORT_DELAY();
		CLK1_LOW;
		SHORT_DELAY();
	}
	DAT1_LOW;
	SHORT_DELAY();
	CLK1_HIGH;
	SHORT_DELAY();
	CLK1_LOW;
	SHORT_DELAY();
}


static unsigned char iic1_read(void)
{
	unsigned char temp_data=0;
	unsigned char i;
	unsigned char a;
		
	DAT1_INPUT;
	SHORT_DELAY();
	for(i=0;i<8;i++)
	{
		CLK1_HIGH;
		SHORT_DELAY();
		if(DAT1_IS_HIGH)
		{
			temp_data |= (1<<(7-i));
		}
		else
		{
			temp_data &= ~(1<<(7-i));
		}
		CLK1_LOW;
		SHORT_DELAY();
	}
	DAT1_OUTPUT;
	SHORT_DELAY();
	DAT1_LOW;
	SHORT_DELAY();
	CLK1_HIGH;
	SHORT_DELAY();
	CLK1_LOW;
	SHORT_DELAY();
	
	return temp_data;
}

static void iic1_write_cmd(unsigned char slave_addr, unsigned char command)
{
	unsigned char a;
		
	iic1_start();
	iic1_write(slave_addr);
	SHORT_DELAY();
	iic1_write(command);
	SHORT_DELAY();
	iic1_stop();
}


static void iic1_write_data(unsigned char slave_addr, unsigned short reg_addr, 
 										unsigned char data)
{
	unsigned char a;
		
	iic1_start();
	iic1_write(slave_addr);
	SHORT_DELAY();
	iic1_write(reg_addr);
	SHORT_DELAY();
	iic1_write(data);
	SHORT_DELAY();
	iic1_stop();
}


static void iic_read_data(unsigned char slave_addr, unsigned short n_byte, 
									unsigned char *pbuf)
{
	unsigned short i;
	unsigned char a;
		
	iic1_start();
	iic1_write(slave_addr|0x01);
	SHORT_DELAY();
	for(i=0;i<n_byte;i++)
	{
		pbuf[i]= iic1_read();
		SHORT_DELAY();
	}
	iic1_stop();
}


unsigned char ads1110_init(unsigned char pga)
{
	unsigned char temp;
	
	iic1_init();
	switch(pga)
	{
		case 1:
			temp=CONFIG_PGA1;
			break;
		case 2:
			temp=CONFIG_PGA2;
			break;
		case 3:
			temp=CONFIG_PGA4;
			break;
		default: temp=CONFIG_PGA8;
		break;
	}
	iic1_write_cmd(DEV_ADDR, temp);
	return 0;
}

////unuse///
void ads1110_reset(void)
{
	iic1_init();
	iic1_write_data(DEV_ADDR, 0,0x06);
}


unsigned char ads1110_read_data(signed int *advalue)
{
	unsigned char temp_data[3];
	unsigned char error=1;
	
	iic_read_data(DEV_ADDR, 3, temp_data);
	error = temp_data[2];
	if((error & 0x80)== 0)
	{
		*advalue = (temp_data[0] << 8);
		*advalue += temp_data[1];
		error = 0;
	}

	return error;
}


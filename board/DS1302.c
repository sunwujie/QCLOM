#include "IO_define.h"

#define SEC_READ_REG_ADDR                0x80
#define MIN_READ_REG_ADDR                0x82
#define HR_READ_REG_ADDR                 0x84
#define DATE_READ_REG_ADDR               0x86
#define MONTH_READ_REG_ADDR              0x88
#define YEAR_READ_REG_ADDR               0x8c

#define WRITE_PROTECT_REG                0X8e
#define WRITE_PROTECT_DISABLE            0x00
#define WRITE_PROTECT_ENABLE             0x80

#define RAM_REG													 0xc0
#define MAGIC_DATA                       0x55

#define DELAY_NOP(x) {a=x;while(a--);}

#define DELAY()   DELAY_NOP(50)

static unsigned char read_data(unsigned char reg_addr)
{
  unsigned char i;
  unsigned char temp_data=0;
 
  DATA_OUT;
  RST_LOW;
  CLK_LOW;
  //delay();
  RST_HIGH;
	
	for(i=0;i<8;i++)
	{
  	if(reg_addr & (0x01<<i))        
  	{
    	DATA_HIGH;
  	}
  	else        
  	{
    	DATA_LOW;   
   	}
  	//delay();
  	CLK_LOW;   
   	//delay();
  	CLK_HIGH;
  	//delay();
	}
     
	DATA_IN;
	for(i=0;i<8;i++)
  {
 		CLK_LOW;   
  	//delay();
  	if(DATA_FROM_CLOCK)        
  	{
    	temp_data |= (0x01<<i);
 		}
  	else        
		{
    	temp_data &= (~(0x01<<i));
  	}
  	//delay();
  	CLK_HIGH;
  	//delay();
  }
	CLK_LOW;
	//delay();
	RST_LOW;
	DATA_OUT;
  //delay();
  
  return temp_data;
}

static unsigned char write_data(unsigned char reg_addr,unsigned char data)
{
  unsigned char i; 
  
  DATA_OUT;
  RST_LOW;
  CLK_LOW;
  //delay();
  RST_HIGH;
	
  for(i=0;i<8;i++)
  {
    if(reg_addr & (0x01<<i))        
    {
      DATA_HIGH;
    }
    else        
    {
      DATA_LOW;
    }
    //delay();
    CLK_LOW;
    //delay();
    CLK_HIGH;
    //delay();
  }
    
	for(i=0;i<8;i++)
  {
    if(data & (0x01<<i))    
    {
    	DATA_HIGH;
    }
    else        
    {
      DATA_LOW;
    }
    //delay();
    CLK_LOW;
    //delay();
    CLK_HIGH;
		//delay();
  }   
  CLK_LOW;
  //delay();
  RST_LOW;
	
  return 0;
}


unsigned char ds1302_read_time(unsigned char *time)
{
	unsigned char temp;
	
	temp=read_data(YEAR_READ_REG_ADDR|0x01);
	if(temp>0x99)
	{
		return 1;
	}
	time[0] = (temp>>4)*10+(temp&0x0f);
	
	temp=read_data(MONTH_READ_REG_ADDR|0x01);
	if((temp>0x12)||(temp==0))
	{
		return 1;
	}
	time[1] = (temp>>4)*10+(temp&0x0f);
	
	temp=read_data(DATE_READ_REG_ADDR|0x01);
	if(time[1]==2)
	{
		if(((time[0]%4)==0)&&((time[0]%100) != 0))
		{
			if((temp>0x29)||(temp==0))
			{
				return 1;
			}
		}
		else
		{
			if((temp>0x28)||(temp==0))
			{
				return 1;
			}	
		}

	}
	else if((time[1] == 4)||(time[1] == 6)||(time[1] == 9)||(time[1] == 11))
	{
		if((temp>0x30)||(temp==0))
		{
			return 1;
		}		
	}
	else
	{
		if((temp>0x31)||(temp==0))
		{
			return 1;
		}		
	}
	time[2] = (temp>>4)*10+(temp&0x0f);
	
	temp=read_data(HR_READ_REG_ADDR|0x01);
	if((temp&0x7f)>0x23)
	{
		return 1;
	}
	time[3] = (temp>>4)*10+(temp&0x0f);
	
	temp=read_data(MIN_READ_REG_ADDR|0x01);
	if((temp&0x7f)>0x59)
	{
		return 1;
	}
	time[4] = (temp>>4)*10+(temp&0x0f);

	temp=read_data(SEC_READ_REG_ADDR|0x01);
	if((temp&0x7f)>0x59)
	{
		return 2;
	}
	time[5] = (temp>>4)*10+(temp&0x0f);
	
	return 0;	
}

unsigned char ds1302_write_time(unsigned char *time)
{
	unsigned char temp;
	unsigned char a;

	//去掉写保护位.
	write_data(WRITE_PROTECT_REG,WRITE_PROTECT_DISABLE);
	DELAY();
	
	temp = (time[0]/10)*16+(time[0]%10);
	write_data(YEAR_READ_REG_ADDR, temp);
	DELAY();
	
	temp = (time[1]/10)*16+(time[1]%10);
	write_data(MONTH_READ_REG_ADDR, temp);
	DELAY();
	
	temp = (time[2]/10)*16+(time[2]%10);
	write_data(DATE_READ_REG_ADDR, temp);
	DELAY();
	
	temp = (time[3]/10)*16+(time[3]%10);
	write_data(HR_READ_REG_ADDR, temp);
	DELAY();
	
	temp = (time[4]/10)*16+(time[4]%10);
	write_data(MIN_READ_REG_ADDR, temp);
	DELAY();
	
	temp = (time[5]/10)*16+(time[5]%10);
	write_data(SEC_READ_REG_ADDR, temp);
	DELAY();
	
	//加上写保护位.
	write_data(WRITE_PROTECT_REG,WRITE_PROTECT_ENABLE);
	
  return 0;
}

unsigned char ds1302_init(unsigned char *start_time)
{
//	unsigned char ret=0;
	unsigned char temp;
	unsigned int a;
	
	//去掉写保护位.
	write_data(WRITE_PROTECT_REG,WRITE_PROTECT_DISABLE);
	DELAY();
	
	write_data(RAM_REG, MAGIC_DATA);
	DELAY();
	
	temp=read_data(RAM_REG|0x01);
	if(temp != MAGIC_DATA)
	{
		return 1;
	}
	//判断时钟是否在运行.
	temp=read_data(SEC_READ_REG_ADDR|0x01);
	if(temp & 0x80)
	{
		write_data(YEAR_READ_REG_ADDR, start_time[0]);
		DELAY();
		write_data(MONTH_READ_REG_ADDR,start_time[1]);
		DELAY();
		write_data(DATE_READ_REG_ADDR, start_time[2]);
		DELAY();
		write_data(HR_READ_REG_ADDR, start_time[3]);
		DELAY();
		write_data(MIN_READ_REG_ADDR, start_time[4]);
		DELAY();
		//时钟开始运行.
		write_data(SEC_READ_REG_ADDR, 0); 
	}
	DELAY();
	//加上写保护位.
	write_data(WRITE_PROTECT_REG,WRITE_PROTECT_ENABLE);
	
   return 0;
}


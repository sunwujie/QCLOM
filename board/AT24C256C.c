#include "IO_define.h"

#define SLAVE_ADDRESS  0xA0

#define DELAY_NOP(x) {a=x;while(a--);}

#define SHORT_DELAY()  DELAY_NOP(2)
//#define iic_init()   {DAT3_HIGH;CLK3_HIGH;}
//#define NEW_DRI

static void iic_start(void)
{
	unsigned char a;

#ifdef NEW_DRI
	DAT3_OUTPUT;
  DAT3_HIGH;
  SHORT_DELAY();
  CLK3_HIGH;
  SHORT_DELAY();
  DAT3_LOW;	
  SHORT_DELAY();
#else
  DISABLE_INT;
  DAT3_HIGH;
  DAT3_OUTPUT;
  
  SHORT_DELAY();
  CLK3_HIGH;
  SHORT_DELAY();
  DAT3_LOW;
  SHORT_DELAY();
  CLK3_LOW;
  ENABLE_INT;
  SHORT_DELAY();
#endif
}

static void iic_stop(void)
{
	 unsigned char a;

#ifdef NEW_DRI
  CLK3_HIGH;
	SHORT_DELAY();
	DAT3_OUTPUT;
  DAT3_LOW;
  SHORT_DELAY();
  DAT3_HIGH;	
  SHORT_DELAY();
#else
	DISABLE_INT;

	DAT3_LOW;
	DAT3_OUTPUT;
	CLK3_HIGH;
	SHORT_DELAY();
	// DAT3_LOW;
 //  DAT3_OUTPUT;
	 
	DAT3_HIGH;
	SHORT_DELAY();
	CLK3_LOW;
	ENABLE_INT;
	SHORT_DELAY();
	 
#endif
}

static void iic_write(unsigned char data)
{
  unsigned char i;
	unsigned char a;

#ifdef NEW_DRI
	for(i=0;i<8;i++)
  {
  	CLK3_LOW;
    SHORT_DELAY();
    if(data & (1<<(7-i)))
    {
      DAT3_HIGH;
    }
    else
    {
      DAT3_LOW;
    }
    SHORT_DELAY();
    CLK3_HIGH;
    SHORT_DELAY();    
  }	
	CLK3_LOW;
	DAT3_INPUT;
	SHORT_DELAY();
	CLK3_HIGH;
	SHORT_DELAY();
	DAT3_OUTPUT;
	DAT3_HIGH;
#else
	
  DISABLE_INT;
  
  DAT3_OUTPUT;
  SHORT_DELAY();
  for(i=0;i<8;i++)
  {
    if(data & (1<<(7-i)))
    {
      DAT3_HIGH;
    }
    else
    {
      DAT3_LOW;
    }
    SHORT_DELAY();
    CLK3_HIGH;
    SHORT_DELAY();
    CLK3_LOW;
    SHORT_DELAY();
  }
  DAT3_LOW;
  SHORT_DELAY();
  CLK3_HIGH;
  SHORT_DELAY();
  CLK3_LOW;
  ENABLE_INT;
  SHORT_DELAY();
#endif
}

static unsigned char iic_read(void)
{
  unsigned char data=0;
  unsigned char i;
	unsigned char a;

#ifdef NEW_DRI
	DAT3_INPUT;
  SHORT_DELAY();
  for(i=0;i<8;i++)
  {
    CLK3_HIGH;
    SHORT_DELAY();
    if(DAT3_IS_HIGH)
    {
      data |= 1<<(7-i);
    }
    else
    {
      data &= ~(1<<(7-i));
    }
    CLK3_LOW;
    SHORT_DELAY();
  }
#else	
  DISABLE_INT;
  
  DAT3_INPUT;
  SHORT_DELAY();
  for(i=0;i<8;i++)
  {
    CLK3_HIGH;
    SHORT_DELAY();
    if(DAT3_IS_HIGH)
    {
      data |= 1<<(7-i);
    }
    else
    {
      data &= ~(1<<(7-i));
    }
    CLK3_LOW;
    SHORT_DELAY();
  }
	ENABLE_INT;
/*  DAT3_OUTPUT;
  SHORT_DELAY();
  DAT3_LOW;
  SHORT_DELAY();
  CLK3_HIGH;
  SHORT_DELAY();
  CLK3_LOW;
  ENABLE_INT;
  SHORT_DELAY();*/
#endif	
  return data;
}

void AT24CXX_hardware_init(void)
{
	unsigned char a;
	
	CLK3_LOW;
	CLK3_OUTPUT;
	DAT3_LOW;
	DAT3_OUTPUT;
	SHORT_DELAY();
	SHORT_DELAY();
	DAT3_HIGH;
	CLK3_HIGH;
}

unsigned char AT24CXX_write_byte( unsigned int reg_addr, unsigned char data)
{
	unsigned char temp1;
  unsigned char temp2;
	unsigned char a;
  
  temp2 =(unsigned char)reg_addr;
  temp1 =(unsigned char)(reg_addr >> 8);
//	DISABLE_INT;
  iic_start();
  iic_write(SLAVE_ADDRESS);
  SHORT_DELAY();
  iic_write(temp1);
  SHORT_DELAY();
  iic_write(temp2);
  SHORT_DELAY();
  iic_write(data);
  SHORT_DELAY();
  iic_stop();
//  ENABLE_INT;  
	return 0;
}

unsigned char AT24CXX_write_page(unsigned int reg_addr,
													unsigned char *data, unsigned char data_len)
{
  unsigned char temp1;
  unsigned char temp2;
	unsigned char a;
    
  temp2 =(unsigned char)reg_addr;
  temp1 =(unsigned char)(reg_addr >> 8);
//  DISABLE_INT;  
  iic_start();
  iic_write(SLAVE_ADDRESS);
  SHORT_DELAY();
  iic_write(temp1);
  SHORT_DELAY();
  iic_write(temp2);
  SHORT_DELAY();  
  while(data_len>0)
  {
    iic_write(*data);
    data++;
    data_len--;
  }
  iic_stop();
 //	ENABLE_INT; 
  return 0;
}

unsigned char AT24CXX_read_byte(unsigned int reg_addr)
{
  unsigned char data;
  unsigned char temp1,temp2;
	unsigned char a;
  
  temp2 = (unsigned char)reg_addr;
  temp1 = (unsigned char)(reg_addr>>8);
//  DISABLE_INT;
  iic_start();
  iic_write(SLAVE_ADDRESS);
  SHORT_DELAY();
  iic_write(temp1);
  SHORT_DELAY();
  iic_write(temp2);
  //SHORT_DELAY();
  NOP;
  iic_start();
  iic_write(SLAVE_ADDRESS|0x01);
  SHORT_DELAY();
  data = iic_read();
	DISABLE_INT;
	DAT3_HIGH;
	DAT3_OUTPUT;
	
  SHORT_DELAY();
	CLK3_HIGH;
	SHORT_DELAY();
	CLK3_LOW;
	SHORT_DELAY();
	ENABLE_INT; 
  iic_stop(); 
  return data;
}

unsigned char AT24CXX_read_page(unsigned int reg_addr,  
                 unsigned char *data, unsigned char data_len)
{
  unsigned char temp1,temp2;
	unsigned char a;
  
  temp2 = (unsigned char)reg_addr;
  temp1 = (unsigned char)(reg_addr>>8);
 // DISABLE_INT;
  iic_start();
  iic_write(SLAVE_ADDRESS);
  SHORT_DELAY();
  iic_write(temp1);
  SHORT_DELAY();
  iic_write(temp2);
  SHORT_DELAY();
  
  iic_start();
  iic_write(SLAVE_ADDRESS|0x01);
  SHORT_DELAY();
  while(data_len>0)
  {
    *data = iic_read();
		DISABLE_INT;
    data++;
    data_len--;
		
		if(data_len==0)
			DAT3_HIGH;
		else
			DAT3_LOW;
		DAT3_OUTPUT;
 	 	SHORT_DELAY();
		CLK3_HIGH;
		SHORT_DELAY();
		CLK3_LOW;
		SHORT_DELAY();
		DAT3_LOW;
		//ENABLE_INT;
  }
	ENABLE_INT;
  iic_stop();
 // ENABLE_INT; 
  return 0;
}





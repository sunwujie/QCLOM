#include "IO_define.h"

#define SHORT_DELAY   NOP
#define LCD_hardware_init()  {CLK2_HIGH;DAT2_HIGH;}

static void iic_start(void)
{
  DISABLE_INT;
	
  DAT2_HIGH;
  SHORT_DELAY;
  CLK2_HIGH;
  SHORT_DELAY;
  DAT2_LOW;
  SHORT_DELAY;
	CLK2_LOW;
	SHORT_DELAY;
	SHORT_DELAY;
	
	ENABLE_INT;
}

static void iic_stop(void)
{
  DISABLE_INT;
	
	DAT2_LOW;
	SHORT_DELAY;
	CLK2_HIGH;
	SHORT_DELAY;
	DAT2_OUTPUT;
	SHORT_DELAY;
	DAT2_HIGH;
	SHORT_DELAY;
	CLK2_LOW;
	SHORT_DELAY;
	SHORT_DELAY;
	
	ENABLE_INT;
}

static void iic_send_byte(unsigned char data)
{
	unsigned char i;

	DISABLE_INT;
	
	DAT2_OUTPUT;
	for(i=0;i<8;i++)
	{
		if(data & 0x80)
		{
			DAT2_HIGH;
		}
		else
		{
			DAT2_LOW;
		}
		data <<= 1;
		SHORT_DELAY;
		CLK2_HIGH;
		SHORT_DELAY;
		CLK2_LOW;
		SHORT_DELAY;
	}
	DAT2_INPUT;
	SHORT_DELAY;
	CLK2_HIGH;
	SHORT_DELAY;
	CLK2_LOW;
	SHORT_DELAY;
	SHORT_DELAY;
	DAT2_OUTPUT;

	ENABLE_INT;
}


unsigned char lcd_init(void)
{
	LCD_hardware_init();
	iic_start();
	// ���豸��ַ��д����
	iic_send_byte(0x70); 
	// ������д����
	iic_send_byte(0x80); 
	// ʹ����ʾ
	iic_send_byte(0xC8);
	// ������д����
	iic_send_byte(0x80);  
	// �豸��ַ
	iic_send_byte(0xE0);
	// ���һ��д����
	iic_send_byte(0x00); 
	// ��ֹ��˸  
	iic_send_byte(0xF4);       
	iic_stop();

	return 0;
}

unsigned char lcd_off(void)
{
  LCD_hardware_init();
	iic_start();
	// ���豸��ַ��д����
	iic_send_byte(0x70); 
	// ������д����
	iic_send_byte(0x80); 
	// �ر���ʾ
	iic_send_byte(0xC0); 
	// ������д����
	iic_send_byte(0x80);
	// �豸��ַ
	iic_send_byte(0xE0); 
	// ���һ��д����
	iic_send_byte(0x00); 
	// ��ֹ��˸  
	iic_send_byte(0xF4);       
	iic_stop();   
  return 0;
}

void lcd_update(unsigned char *ptr)
{
    unsigned char i;
  
    LCD_hardware_init();
    iic_start();
		// ���豸��ַ��д����
    iic_send_byte(0x70);  
		// ����д����
    iic_send_byte(0x80);     
    //iic_send_byte(0x0C);     // ��ʼ��S12
    // ��ʼ��S0
    iic_send_byte(0x00);  
		// д����
    iic_send_byte(0x40);     
    // ���ֽ�дRAM
    for (i = 0; i < 30; i++)
    {
    	iic_send_byte(*(ptr+i));     
    }
    iic_stop();         
}




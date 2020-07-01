#include "board.h"
#include "press_key.h"

#define key_read_value()  KEY_VALUE

key_manage key;

void key_init(void)
{
	key.raw_value =0;
	key.code_value =0;
	key.state =0;
	key.flag = RELEASE;
	key.counter =0;
	key.attr = (1<<(DOWN_KEY-1))|(1<<(UP_KEY-1));
}

static unsigned char short_key_encode(unsigned char raw_key)
{
	unsigned char i;
	static const unsigned char key_table[]={0x01,0x02,0x04,0x08,0x10,0x20};

	for(i=0;i<sizeof(key_table);i++)
	{
		if(raw_key == key_table[i])
		{
			return (i+1);
		}
	}

	return 0;
}

void key_value_affirm(void)
{	
	unsigned char temp;
	
	temp= key_read_value();
	if(key.state == 0)
	{
		if(temp != NO_KEY)
		{
			key.state = 1;
			key.counter = 0;
			key.flag = PRESSED;
			key.raw_value = temp;
		}
	}
	else if(key.state ==1)
	{
		if(key.raw_value == temp)
		{
			if(key.counter < 1000)
			{
				key.counter++;   
			}			
			if(key.counter == SHORT_KEY_COUNTER)
			{
				key.code_value = short_key_encode(key.raw_value);			
			}	
			if(key.counter == LONG_KEY_COUNTER)
			{
				key.code_value = 0x80+short_key_encode(key.raw_value);	
			}
		}
		else
		{
			key.state = 0;
			key.counter = 0;
			key.code_value =0;
			if(temp == NO_KEY)
			{
				key.raw_value = NO_KEY;
				key.flag = RELEASE;
			}
		}
	}
}

void key_clear(void)
{
	key.code_value =0;
}

unsigned char key_speed_up_attr(void)
{
	unsigned char i;

	for(i=0; i< 6; i++)
	{
		if(((key.attr & (1<< i))>0)&&(key.raw_value==(1<< i)))
		{
			if(key.counter >= SPEED_UP_KEY_COUNTER)
			{
				key.code_value = 0x80+short_key_encode(key.raw_value);
				key.counter -= SPEED_UP_INTERVAL_TIME;
				return 0;
			}						
		}
	}
	return 0;
}




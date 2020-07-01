#include "board.h"
#include "event_record_mange.h"


#define DELAY_NOP(x) {a=x;while(a--);}

#define DELAY()   DELAY_NOP(480)   //500

unsigned char event_request_table[EVENT_TYPE_NUM/8+1];

unsigned char event_table_init(void)
{
	unsigned char i;

	for(i=0;i<(EVENT_TYPE_NUM/8+1);i++)
		event_request_table[i]=0;
	
	return 0;
}

unsigned char event_request(unsigned char event_type)
{
	if(event_type >= EVENT_TYPE_NUM)
	{
		return 1;
	}
	
	event_request_table[event_type/8] |= (1<<(event_type%8));
	return 0;
}

unsigned char event_clear(unsigned char event_type)
{
	if(event_type >= EVENT_TYPE_NUM)
	{
		return 1;
	}
	
	event_request_table[event_type/8] &= ~(1<<(event_type%8));
	return 0;
}

unsigned char clear_all_record(void)
{
	unsigned int  index_addr;
	unsigned int  flag_addr;
	unsigned int  addr=RECORD_START_ADDR;
  unsigned int a;

	while(addr <= (RECORD_START_ADDR + BLOCK_SIZE*(EVENT_TYPE_NUM-1)))
	{
		index_addr = addr + INDEX_OFFSET;
		AT24CXX_write_byte(index_addr, 0xff);
		DELAY();
		flag_addr = addr + FLAG_OFFSET;	
		AT24CXX_write_byte(flag_addr, 0xff);
		addr += BLOCK_SIZE;
		DELAY();
	}
		
	return 0;
}

signed char check_event_request(void)
{
	unsigned char i;
	unsigned char j;

	for(i=0;i<(EVENT_TYPE_NUM/8+1);i++)
	{
		if(event_request_table[i]>0)
		{
			for(j=0;j<8;j++)
			{
				if(event_request_table[i] & (1 << j))
				{
						return (i*8+ j);
				}
			}
		}
	}
	return -1;
}

unsigned char write_record(unsigned char event_type,unsigned int record_data
																												,unsigned char *time)
{
	unsigned int  index_addr;
	unsigned int  flag_addr;
	unsigned int  data_addr;
	unsigned char buf[RECORD_LENGTH];
	unsigned char ret=0;
	unsigned char record_num;
  unsigned int a;
	
	data_addr = RECORD_START_ADDR+event_type *BLOCK_SIZE;
	index_addr = data_addr + INDEX_OFFSET;
	flag_addr = data_addr + FLAG_OFFSET;
	///读取上次的记录条数.
	record_num = AT24CXX_read_byte(index_addr);
	if(record_num == 0xff)
	{
		record_num =0;
	}

	buf[0]=(unsigned char)(record_data >> 8);
	buf[1]=(unsigned char)(record_data & 0xff);
	buf[2]= time[0];
	buf[3]= time[1];
	buf[4]= time[2];
	buf[5]= time[3];
	buf[6]= time[4];
	///记录条数加一
	record_num++;
	if(record_num > EVENT_RECORD_NUM)
	{
		record_num=1;
		AT24CXX_write_byte(flag_addr, RECORD_FULL_FLAG);
		DELAY();
	}
	DELAY();
	////更新记录条数.
	AT24CXX_write_byte(index_addr,record_num);
	DELAY();
	///将新记录写入data_addr地址.
	AT24CXX_write_page(data_addr+RECORD_LENGTH*(record_num-1), buf, (RECORD_LENGTH-1));
	DELAY();			
	return ret;
}

unsigned char read_record_data(unsigned char *buf,unsigned char event_type
																								  ,unsigned char record_index)
{
	unsigned char ret=0;
//	unsigned int  index_addr;
//	unsigned int  flag_addr;
	unsigned int  base_addr;
//	unsigned char index;
//	unsigned char  flag;
	unsigned int  start_addr;
//	unsigned int a;
	
	base_addr = RECORD_START_ADDR+event_type *BLOCK_SIZE;
/*	index_addr = base_addr + INDEX_OFFSET;
//	flag_addr = base_addr + FLAG_OFFSET;

//	index = AT24CXX_read_byte(index_addr);
//	DELAY();
//	flag = AT24CXX_read_byte(flag_addr);

//	//无记录,返回1.
//	if(index > EVENT_RECORD_NUM)
//	{
//		return 1;
//	}
	if(flag == RECORD_FULL_FLAG)
	{
		if((record_num+index)>EVENT_RECORD_NUM)
		{
			start_addr=base_addr+(record_num+index-EVENT_RECORD_NUM-1)*RECORD_LENGTH;
		}
		else
		{
			start_addr=base_addr+(index+record_num-1)*RECORD_LENGTH;	
		}
	}
	else 
	{
	*/
		start_addr=base_addr+(record_index-1)*RECORD_LENGTH;	
	//}
	ret=AT24CXX_read_page(start_addr, buf, (RECORD_LENGTH-1));
	
	return ret;
}


unsigned char read_record_info(unsigned char *buf,unsigned char event_type)
{
	unsigned char ret=0;
	unsigned int  index_addr;
	unsigned int  flag_addr;
	unsigned int  data_addr;
	unsigned char record_index;
	unsigned int  flag;
	unsigned int a;
	
	data_addr = RECORD_START_ADDR+event_type *BLOCK_SIZE;
	index_addr = data_addr + INDEX_OFFSET;
	flag_addr = data_addr + FLAG_OFFSET;

	record_index = AT24CXX_read_byte(index_addr);
	DELAY();
	flag = AT24CXX_read_byte(flag_addr);
	if(flag == 0xff){
		if(record_index <= EVENT_RECORD_NUM){
			buf[0] = 0;
			buf[1]= record_index;
		}
		else{
			buf[0] = 0;
			buf[1] = 0;
		}
	}
	else if(flag == RECORD_FULL_FLAG){
		buf[0] = 1;
		buf[1]= record_index;	
	}
	else{
		buf[0] = 0;
		buf[1] = 0;
		ret = 2;
	}
	
	return ret;
}


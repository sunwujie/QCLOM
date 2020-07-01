#include "board.h"
#include "display_font.h"
#include "display_component.h"

#define  ICON_TWA            0x10
#define  ICON_STEL           0x08
#define  ICON_L              0x04
#define  ICON_F              0x02
#define  ICON_H              0x01

#define  ICON_MESSAGE        0x40
#define  ICON_COMM           0x20
#define  ICON_SCALE          0x10

#define  ICON_BATTERY        0x08
#define  ICON_CLOCK          0x04
#define  ICON_FAN            0x02

#define  ICON_FLA            0x01
#define  ICON_LEL            0x01
#define  ICON_02             0x01
#define  ICON_VOL            0x01
#define  ICON_CO             0x10
#define  ICON_PPM_CO         0x10
#define  ICON_H2S            0x10
#define  ICON_PPM_H2S        0x10

#define DISPLAY_BUF_LENGTH  30

static unsigned char guc_display_buf[DISPLAY_BUF_LENGTH];


void display_init(void)
{
	unsigned char i;

	for(i=0;i<DISPLAY_BUF_LENGTH;i++)
	{
		guc_display_buf[i]=0;
	}
	lcd_init();
}


void display_update(void) 
{
	lcd_update(guc_display_buf);
}


void display_seg7(unsigned char position, unsigned char num)
{
	//////第一行的字符与数字显示. 1~7
	if(position == 1)
	{
		guc_display_buf[17] = up_seg7_decode_table[num][1];
	}
	else if((position >= 2)&&(position <= 7))
	{
		guc_display_buf[7-position]= up_seg7_decode_table[num][1];
	}
	//////第二行的字符与数字显示. 8~15
	else if((position >=8)&&(position <= 15))
	{
		guc_display_buf[position + 11] &= 0x01;
		guc_display_buf[position + 11] |= first_row_seg7_decode_table[num][1];
	}
	//////第三行的字符与数字显示.16~23
	else if((position >= 16)&&(position <= 23))
	{
		guc_display_buf[position-10] &= 0xf0;
		guc_display_buf[position-10] |= second_row_seg7_decode_table[num][1];
		guc_display_buf[position -9] &= 0x1f;
		guc_display_buf[position -9] |= second_row_seg7_decode_table[num][2];
	}
}

unsigned char display_decimal_point(unsigned char pos)
{
	unsigned char ret=0;
	
	switch(pos)
	{
		case 1:
			guc_display_buf[2] |= 0x80;
			break;
		case 2:
			guc_display_buf[20] |= 0x01;
			break;
		case 3:
			guc_display_buf[21] |= 0x01;
			break;
		case 4:
			guc_display_buf[24] |= 0x01;
			break;
		case 5:
			guc_display_buf[25] |= 0x01;
			break;
		case 6:
			guc_display_buf[8] |= 0x10;
			break;
		case 7:
			guc_display_buf[9] |= 0x10;
			break;
		case 8:
			guc_display_buf[12] |= 0x10;
			break;
		case 9:
			guc_display_buf[13] |= 0x10;
			break;
		default: ret =1;
			break;
	}
	return ret;
}

void display_icon_H(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[18] &= ~ICON_H;	
	}
	else
	{
		guc_display_buf[18] |= ICON_H;
	}
}

void display_icon_L(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[18] &= ~ICON_L;	
	}
	else
	{
		guc_display_buf[18] |= ICON_L;
	}
}

void display_icon_F(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[18] &= ~ICON_F;
	}
	else
	{
		guc_display_buf[18]|= ICON_F;
	}
}

void display_icon_TWA(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[18] &= ~ICON_TWA;	
	}
	else
	{
		guc_display_buf[18] |= ICON_TWA;
	}
}

void display_icon_STEL(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[18] &= ~ICON_STEL;	
	}
	else
	{
		guc_display_buf[18] |= ICON_STEL;
	}
}


void display_icon_COMM(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[6]&= ~ICON_COMM;	
	}
	else
	{
		guc_display_buf[6]|= ICON_COMM;
	}
}

void display_icon_record(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[6] &= ~ICON_MESSAGE;	
	}
	else
	{
		guc_display_buf[6] |= ICON_MESSAGE;
	}
}

/////////显示标定标志////////////////////
void display_icon_scale(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[6] &= ~ICON_SCALE;	
	}
	else
	{
		guc_display_buf[6] |= ICON_SCALE;
	}
}

void display_icon_CLOCK(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[14]&= ~ICON_CLOCK;	
	}
	else
	{
		guc_display_buf[14]|= ICON_CLOCK;
	}
}

void display_icon_FAN(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[14] &= ~ICON_FAN;	
	}
	else
	{
		guc_display_buf[14] |= ICON_FAN;
	}
}

void display_icon_FLA(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[19] &= ~ICON_FLA;	
	}
	else
	{
		guc_display_buf[19] |= ICON_FLA;
	}
}

void display_icon_LEL(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[22]&= ~ICON_LEL;	
	}
	else
	{
		guc_display_buf[22]|= ICON_LEL;
	}
}

void display_icon_O2(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[23] &= ~ICON_02;	
	}
	else
	{
		guc_display_buf[23] |= ICON_02;
	}
}

void display_icon_VOL(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[26] &= ~ICON_VOL;	
	}
	else
	{
		guc_display_buf[26] |= ICON_VOL;
	}
}


void display_icon_CO(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[7]&= ~ICON_CO;	
	}
	else
	{
		guc_display_buf[7]|= ICON_CO;
	}
}

void display_icon_COPPM(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[10] &= ~ICON_PPM_CO;	
	}
	else
	{
		guc_display_buf[10] |= ICON_PPM_CO;
	}
}

void display_icon_H2S(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[11] &= ~ICON_H2S;	
	}
	else
	{
		guc_display_buf[11] |= ICON_H2S;
	}
}

void display_icon_H2SPPM(unsigned char state)
{
	if(state == 0)
	{
		guc_display_buf[14] &= ~ICON_PPM_H2S;	
	}
	else
	{
		guc_display_buf[14] |= ICON_PPM_H2S;
	}
}

void display_icon_BAT(unsigned char energy)
{
	guc_display_buf[14] |= ICON_BATTERY;
	switch(energy)
	{
		case 0: guc_display_buf[15] &= ~0x08;
			guc_display_buf[16] &= ~0x80;
			guc_display_buf[16] &= ~0x08;
			guc_display_buf[15] &= ~0x80;	
			break;
		case 1: guc_display_buf[15]|= 0x08;
			guc_display_buf[16] &= ~0x80;
			guc_display_buf[16] &= ~0x08;
			guc_display_buf[15] &= ~0x80;
			break;
		case 2: guc_display_buf[15]|= 0x08;
			guc_display_buf[16] |= 0x80;
			guc_display_buf[16] &= ~0x08;
			guc_display_buf[15] &= ~0x80;
			break;
		case 3: guc_display_buf[15]|= 0x08;
			guc_display_buf[16] |= 0x80;
			guc_display_buf[16] |= 0x08;
			guc_display_buf[15] &= ~0x80;
			break;
		case 4: guc_display_buf[15]|= 0x08;
			guc_display_buf[16] |= 0x80;
			guc_display_buf[16] |= 0x08;
			guc_display_buf[15] |= 0x80;
			break;
		default:
			break;
	}
}

void display_int_value(unsigned int ivalue,unsigned char pos
																									,unsigned char length)
{
	unsigned char i;
	unsigned int temp;

	if(ivalue==0)
	{
		display_seg7(pos, 0);
		return;
	}
	
	for(i = 0; i <=length; ++i)
	{
		if(ivalue >0)
		{
			temp = ivalue%10;
			display_seg7(pos-i, temp);
			ivalue /= 10;
		}
		else 
		{
			break;
		}
	}
}

void display_num(unsigned int num,unsigned char start_pos,
													unsigned char decimal, signed char length)
{
	signed char i;
	unsigned char temp;

	for(i=length; i>=0;--i)
	{
		if(num > 0)
		{
			temp = num%10;
			display_seg7(start_pos+i, temp);
			num = num/10;
		}
		else if(i>(2-decimal))
			display_seg7(start_pos+i,0);
		else
			return;
	}
}

void display_Err(unsigned char Err_postion)
{
	display_seg7(Err_postion, char7_E);
	display_seg7(Err_postion + 1, char7_r);
	display_seg7(Err_postion + 2, char7_r);
}

void display_OFF(unsigned char OFF_postion)
{
	display_seg7(OFF_postion, char7_O);
	display_seg7(OFF_postion + 1, char7_F);
	display_seg7(OFF_postion + 2, char7_F);	
}

void display_ON(unsigned char ON_postion)
{
	display_seg7(ON_postion, char7_O);
	display_seg7(ON_postion + 1, char7_n);	
}

void display_time(unsigned char *time)
{
	volatile unsigned char temp;

	///年显示.
	temp = time[0] / 10;
	display_seg7(8, temp);
	temp = time[0] % 10;
	display_seg7(9,temp);
	display_seg7(10,char7_minus_sign);
	
	///月显示.
	temp = time[1] / 10;
	display_seg7(11, temp);
	temp = time[1] % 10;
	display_seg7(12,temp);
	display_seg7(13,char7_minus_sign);	

	////日显示.
	temp = time[2] / 10;
	display_seg7(14, temp);
	temp = time[2] % 10;
	display_seg7(15,temp);

	///时显示.
	temp = time[3] / 10;
	display_seg7(19, temp);
	temp = time[3] % 10;
	display_seg7(20,temp);
	display_seg7(21, char7_colon);

	////分显示.
	temp = time[4] / 10;
	display_seg7(22, temp);
	temp = time[4] % 10;
	display_seg7(23,temp);
}

void flash_display_time(unsigned char iterm,unsigned temp)
{
	switch(iterm)
	{
		case 0:display_seg7(8, temp);
		display_seg7(9,temp);
		break;
		case 1:display_seg7(11, temp);
		display_seg7(12,temp);
		break;	
		case 2:display_seg7(14, temp);
		display_seg7(15,temp);
		break;
		case 3:display_seg7(19, temp);
		display_seg7(20,temp);
		break;
		case 4:display_seg7(22, temp);
		display_seg7(23,temp);
		break;
		default: break;
	}
	
}

void display_state(unsigned char state)
{
	unsigned char i;

	if(state >= 2)
	{
		state=1;
	}
	for(i=0;i<DISPLAY_BUF_LENGTH;i++)
	{
		guc_display_buf[i]=0xff*state;
	}
}



#ifndef _DISPLAY_COMPONENT_H
#define _DISPLAY_COMPONENT_H

#define DISPLAY_CLEAR  0
#define DISPLAY_FULL   1

//以下宏的数值为各字母在segment7_decoded_table[]中的位置
#define char7_A  10
#define char7_B  11
#define char7_b  12
#define char7_C  13
#define char7_D  14
#define char7_d  15
#define char7_E  16
#define char7_e  17
#define char7_F  18
#define char7_G  19
#define char7_H  20
#define char7_I  21
#define char7_J  22
#define char7_L  23
#define char7_l  24
#define char7_n  25
#define char7_O  26
#define char7_o  27
#define char7_p  28
#define char7_q  29
#define char7_r  30
#define char7_S  31
#define char7_t  32

#define char7_U  33
#define char7_minus_sign 34
#define char7_no_seg    35
#define char7_underline 36
#define char7_colon     37

extern void display_init(void);
extern void display_update(void);
extern void display_seg7(unsigned char position, unsigned char num);
extern unsigned char display_decimal_point(unsigned char pos);
extern void display_icon_H(unsigned char state);
extern void display_icon_L(unsigned char state);
extern void display_icon_F(unsigned char state);
extern void display_icon_TWA(unsigned char state);
extern void display_icon_STEL(unsigned char state);
extern void display_icon_COMM(unsigned char state);
extern void display_icon_record(unsigned char state);
extern void display_icon_scale(unsigned char state);
extern void display_icon_CLOCK(unsigned char state);
extern void display_icon_FAN(unsigned char state);
extern void display_icon_FLA(unsigned char state);
extern void display_icon_LEL(unsigned char state);
extern void display_icon_O2(unsigned char state);
extern void display_icon_VOL(unsigned char state);
extern void display_icon_CO(unsigned char state);
extern void display_icon_COPPM(unsigned char state);
extern void display_icon_H2S(unsigned char state);
extern void display_icon_H2SPPM(unsigned char state);
extern void display_icon_BAT(unsigned char energy);
extern void display_int_value(unsigned int ivalue,unsigned char pos
																									,unsigned char length);
extern void display_num(unsigned int num,unsigned char start_pos,
												unsigned char decimal, signed char length);
extern void display_Err(unsigned char pos);
extern void display_OFF(unsigned char OFF_postion);
extern void display_ON(unsigned char ON_postion);
extern void display_time(unsigned char* time);
extern void flash_display_time(unsigned char iterm,unsigned temp);
extern void display_state(unsigned char state);

#endif


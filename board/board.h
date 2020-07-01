#ifndef _BOARD_H
#define _BOARD_H

#include "IO_define.h"

///////////mcu init////////////////
extern unsigned char clock_init(void);
extern unsigned char mcu_init(void);
extern void mcu_poweroff(void);
extern void extern_int_init(void);
extern unsigned char uart1_config(unsigned int band);

//////////ADS1110////////////
extern unsigned char ads1110_init(unsigned char pga);
extern void ads1110_reset(void);
extern unsigned char ads1110_read_data(signed int * value);

/////////AT24CXX///////////////////
extern void AT24CXX_hardware_init(void);
extern unsigned char AT24CXX_write_byte( unsigned int reg_addr, unsigned char data);
extern unsigned char AT24CXX_write_page(unsigned int reg_addr,
													unsigned char *data, unsigned char data_len);
extern unsigned char AT24CXX_read_byte(unsigned int reg_addr);
extern unsigned char AT24CXX_read_page(unsigned int reg_addr,  
                unsigned char *data, unsigned char data_len);

//////////DS1302///////////////////
extern unsigned char ds1302_read_time(unsigned char *time);
extern unsigned char ds1302_write_time(unsigned char *time);
extern unsigned char ds1302_init(unsigned char *start_time);

/////////////LCD/////////////////
extern unsigned char lcd_init(void);
extern unsigned char lcd_off(void);
extern void lcd_update(unsigned char *ptr);

///////////////LMP91000/////////////////
extern void lmp910x_O2_init(unsigned char O2_pag, unsigned char O2_bias);
extern void lmp910x_O2_powerOff(void);
extern void lmp910x_CO_init(unsigned char CO_pag, unsigned char CO_bias);
extern void lmp910x_CO_powerOff(void);
extern void lmp910x_H2S_init(unsigned char H2S_pag, unsigned char H2S_bias);
extern void lmp910x_H2S_powerOff(void);

extern unsigned char lmp91000_init(unsigned char O2_pag,unsigned char O2_bias,unsigned char CO_pag,
										unsigned char CO_bias,unsigned char H2S_pag,unsigned char H2S_bias);
//extern unsigned char lmp910x_poweroff(void);

#define start_get_signal_value()  START_ADC()

#define pump_current_check()			ADC_CHANNEL_STATE(0x01)
#define get_pumpcurrent_value()  ADC_CHANNEL0_VALUE	

#define BAT_one_value_check()			ADC_CHANNEL_STATE(0x04)
#define get_BAT_one_value()				ADC_CHANNEL2_VALUE

#define BAT_two_value_check()			ADC_CHANNEL_STATE(0x02)
#define get_BAT_two_value()				ADC_CHANNEL1_VALUE

//#define FLA_value_check()					
//#define get_FLA_value()

#define O2_value_check()					ADC_CHANNEL_STATE(0x10)
#define get_O2_value()						ADC_CHANNEL4_VALUE

#define H2S_value_check()					ADC_CHANNEL_STATE(0x08)
#define get_H2S_value()						ADC_CHANNEL3_VALUE

#define CO_value_check()					ADC_CHANNEL_STATE(0x20)
#define get_CO_value()						ADC_CHANNEL5_VALUE

#define Temp_value_check()				ADC_CHANNEL_STATE(0x40)
#define get_Temp_value()					ADC_CHANNEL6_VALUE

#define set_backlight_duty(x)			(TBCCR4=x)

#endif 


#ifndef _GAS_ALARM_CFG_H
#define _GAS_ALARM_CFG_H


#define HP4

//#define H4

//#define ESP210

//#define M1


//#define DEBUG  

/////��ز��Կ���.
//#define DEBUG_BAT

/////EEPROM���Կ���.
//#define DEBUG_EEP

/////�������Ե�������.
//#define DEBUG_POWERON

/////����������Կ���. 1:FLA, 2:O2,3:CO,4:H2S.
#define DEBUG_CACL_CONCE  0

///���Ե͵�ѹ�����µ�O2 ADCֵ.
//#define DEBUG_LOW_VOLTAGE

///0: ���л���Դ,1: �л���Դ.
//#define CFG_CHANGE_POWER   0  

#ifdef HP4

#define MAIN_VER 'c'
#define SUB_VER  '1'

/////�õ�״̬�Ƿ�д��EEPROM,0:��д��. 1:д��.
#define PUMP_SAVE  0
	
#endif	

#endif

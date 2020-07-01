#ifndef _GAS_ALARM_CFG_H
#define _GAS_ALARM_CFG_H


#define HP4

//#define H4

//#define ESP210

//#define M1


//#define DEBUG  

/////电池测试开关.
//#define DEBUG_BAT

/////EEPROM测试开关.
//#define DEBUG_EEP

/////开机测试电量开关.
//#define DEBUG_POWERON

/////气体测量测试开关. 1:FLA, 2:O2,3:CO,4:H2S.
#define DEBUG_CACL_CONCE  0

///测试低电压环境下的O2 ADC值.
//#define DEBUG_LOW_VOLTAGE

///0: 不切换电源,1: 切换电源.
//#define CFG_CHANGE_POWER   0  

#ifdef HP4

#define MAIN_VER 'c'
#define SUB_VER  '1'

/////泵的状态是否写入EEPROM,0:不写入. 1:写入.
#define PUMP_SAVE  0
	
#endif	

#endif

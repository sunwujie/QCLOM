#include "IO_define.h"

#define SLEEP_STATE			0

extern unsigned char sys_work_state;

extern void timerA_handle(void);
//extern void port1_handle(void);
//extern void port2_handle(void);
extern void uart1_tx_handle(void);
extern void uart1_rx_handle(unsigned char temp);

extern unsigned char uart1_config(unsigned int band);
extern void uart1_poweroff(void);


//IAR C compiler
#ifdef __IAR_SYSTEMS_ICC__              
#if __VER__ >= 200
#pragma vector=TIMERA0_VECTOR
__interrupt void msp430f2xx_timerA_interrupt (void)
#else                               
interrupt[TIMERA0_VECTOR] void msp430f2xx_timerA_interrupt(void)
#endif
#endif
{
	timerA_handle();		
}

//IAR C compiler
#ifdef __IAR_SYSTEMS_ICC__              
#if __VER__ >= 200
#pragma vector=PORT1_VECTOR
__interrupt void msp430f2xx_port1_interrupt (void)
#else                               
interrupt[PORT1_VECTOR] void msp430f2xx_port1_interrupt(void)
#endif
#endif
{
  //clear interrupt flag
	P1IFG &=~ 0xFF;        
	if(sys_work_state == SLEEP_STATE)
	{
		if(INTERRUPT_CHECK > 0)
		{
			LOW_TO_HIGH_INT;
			///按键松开.
			
		}
		else
		{
			CPU_wakeup();
			HIGH_TO_LOW_INT;
			///按键按下.
		}
	}
}

//IAR C compiler
#ifdef __IAR_SYSTEMS_ICC__              
#if __VER__ >= 200
#pragma vector=PORT2_VECTOR
__interrupt void msp430f2xx_port2_interrupt (void)
#else                               
interrupt[PORT2_VECTOR] void msp430f2xx_port2_interrupt(void)
#endif
#endif
{
  //clear interrupt flag
	P2IFG &=~ 0xFF;          
	if(EXTTERN_POWER != 0)
	{
		///插入USB电源.
		POWER_ON_INT;
		if(sys_work_state == SLEEP_STATE)
		{
			CPU_wakeup();
		}
		uart1_config(1200);
	}
	else
	{
		//拔掉USB电源.
		POWER_OFF_INT;
		uart1_poweroff();
	}      
}

//IAR C compiler
#ifdef __IAR_SYSTEMS_ICC__           
#if __VER__ >= 200
#pragma vector=USCIAB1RX_VECTOR
__interrupt void urx1_int (void)
#else                               
interrupt[USCIAB1RX_VECTOR] void urx1_int(void)
#endif
#endif
{
	if((UC1IFG&UCA1RXIFG)&&(UC1IE&UCA1RXIE))
  {
  	UC1IFG&=~UCA1RXIFG;
		uart1_rx_handle(UCA1RXBUF);
  }  
}

//IAR C compiler
#ifdef __IAR_SYSTEMS_ICC__           
#if __VER__ >= 200
#pragma vector=USCIAB1TX_VECTOR
__interrupt void utx1_int(void)     ///USART1  transmit interrupt  
#else                               
interrupt[USCIAB1TX_VECTOR] void utx1_int(void)
#endif
#endif
{
	if((UC1IFG&UCA1TXIFG)&&(UC1IE&UCA1TXIE))
	{
  	UC1IFG&=~UCA1TXIFG;
		uart1_tx_handle();	 
	}
}





#ifndef _IO_DEFINE_H
#define _IO_DEFINE_H

#include <in430.h>
#include <msp430x24x.h>


//interrupt 
#define DISABLE_INT            _DINT()
#define ENABLE_INT             _EINT()

#define DISABLE_WDT()					(WDTCTL = WDTPW + WDTHOLD)
#define ENABLE_WDT()					(WDTCTL = WDT_ARST_1000 + WDTNMI)
#define feed_WDT()						ENABLE_WDT()

#define CPU_sleep()				 		 	LPM3
#define CPU_wakeup()						LPM3_EXIT
#define CPU_RESET_FLAG()				IFG1
#define CPU_RESET_SET(X)				(IFG1=X)

#define NOP                    _NOP()

//////P1 interrupt 
#define INTERRUPT_CHECK			(P1IES & BIT4)
#define HIGH_TO_LOW_INT			(P1IES |= BIT4)
#define LOW_TO_HIGH_INT			(P1IES &= ~BIT4)

/////P2 interrupt
#define EXTTERN_POWER     (P2IN & BIT5)
#define POWER_ON_INT			(P2IES |= BIT5)
#define POWER_OFF_INT			(P2IES &= ~BIT5)

//ds1302 IO start
#define DATA_FROM_CLOCK       (P3IN & BIT0)   
#define DATA_OUT              (P3DIR |=  BIT0)
#define DATA_IN               (P3DIR &= ~BIT0)
#define DATA_HIGH             (P3OUT |=  BIT0)
#define DATA_LOW              (P3OUT &= ~BIT0)

#define RST_HIGH              (P3OUT |=  BIT3)
#define RST_LOW               (P3OUT &= ~BIT3)
#define CLK_HIGH              (P2OUT |=  BIT7)
#define CLK_LOW               (P2OUT &= ~BIT7)
///ds1302 IO end

///AT24C256C IO start
#define DAT3_INPUT						(P3DIR &= ~BIT1)
#define DAT3_OUTPUT						(P3DIR |= BIT1)
#define DAT3_HIGH							(P3OUT |= BIT1)
#define DAT3_IS_HIGH					(P3IN & BIT1)
#define DAT3_LOW							(P3OUT &= ~BIT1)

#define CLK3_OUTPUT						(P3DIR |= BIT2)
#define CLK3_HIGH							(P3OUT |= BIT2)
#define CLK3_LOW							(P3OUT &= ~BIT2)
////AT24C256C IO end

////ADS1110 IO start
#define ADS_POWER_ON					(P4OUT |= BIT1)
#define ADS_POWER_OFF					(P4OUT &= ~BIT1)

#define DAT1_INPUT						(P2DIR &= ~BIT0)
#define DAT1_OUTPUT						(P2DIR |= BIT0)
#define DAT1_HIGH							(P2OUT |= BIT0)
#define DAT1_IS_HIGH					(P2IN & BIT0)
#define DAT1_LOW							(P2OUT &= ~BIT0)

#define CLK1_HIGH							(P2OUT |= BIT1)
#define CLK1_LOW							(P2OUT &= ~BIT1)
////ADS1110 IO end

////LMP91000 IO start///
#define MPOW_POWER_ON					(P4OUT |= BIT0)
#define MPOW_POWER_OFF				(P4OUT&= ~BIT0)

#define DAT4_OUTPUT						(P5DIR |= BIT1)
#define DAT4_INPUT						(P5DIR &= ~BIT1)
#define DAT4_HIGH							(P5OUT |= BIT1)
#define DAT4_IS_HIGH					(P5IN & BIT1)
#define DAT4_LOW							(P5OUT &= ~BIT1)

#define CLK4_HIGH							(P5OUT |= BIT2)
#define CLK4_LOW							(P5OUT &= ~BIT2)

#define MENB2_HIGH						(P2OUT |= BIT2)
#define MENB2_LOW							(P2OUT &= ~BIT2)
#define MENB2_INPUT						(P2DIR &= ~BIT2)
#define MENB2_OUTPUT					(P2DIR |= BIT2)

#define MENB3_HIGH						(P2OUT |= BIT3)
#define MENB3_LOW							(P2OUT &= ~BIT3)
#define MENB3_INPUT						(P2DIR &= ~BIT3)
#define MENB3_OUTPUT					(P2DIR |= BIT3)

#define MENB4_HIGH						(P2OUT |= BIT4)
#define MENB4_LOW							(P2OUT &= ~BIT4)
#define MENB4_INPUT						(P2DIR &= ~BIT4)
#define MENB4_OUTPUT					(P2DIR |= BIT4)
/////LMP91000 IO end/////

////LCD IO  start
#define DAT2_INPUT						(P4DIR &= ~BIT6)
#define DAT2_OUTPUT						(P4DIR |= BIT6)
#define DAT2_HIGH							(P4OUT |= BIT6)
#define DAT2_IS_HIGH					(P4OUT & BIT6)
#define DAT2_LOW							(P4OUT &= ~BIT6)

#define CLK2_HIGH							(P4OUT |= BIT7)
#define CLK2_LOW							(P4OUT &= ~BIT7)
////LCD IO  end

////BEEP  IO  start
#define BEEP_POWER_ON					(P5OUT |= BIT5)
#define BEEP_POWER_OFF				(P5OUT &= ~BIT5)
#define BEEP_ON								(P5OUT |= BIT6)
#define BEEP_OFF							(P5OUT &= ~BIT6)
///BEEP  IO   end

////LED  IO  start
#define RED_LED_ON						(P5OUT |= BIT3)
#define RED_LED_OFF						(P5OUT &= ~BIT3)
#define GREEN_LED_ON					(P5OUT |= BIT0)
#define GREEN_LED_OFF					(P5OUT &= ~BIT0)
#define YELL_LED_ON						(P5OUT |= BIT4)
#define YELL_LED_OFF					(P5OUT &= ~BIT4)
////LED IO  end

////pump IO  start
#define PUMP_POWER_ON					(P4OUT |= BIT5)
#define PUMP_POWER_OFF				(P4OUT &= ~BIT5)
#define PUMP_ON								(P3OUT |= BIT4)
#define PUMP_OFF							(P3OUT &= ~BIT4)
#define PUMP_CHECK						(P2IN & BIT6)
///pump IO  end

////BATTERY IO  start
#define BAT_ONE_CHARGE		{P6OUT |= BIT6; P6OUT &= ~BIT7;}
#define BAT_TWO_CHARGE		{P6OUT &= ~BIT6; P6OUT |= BIT7;}
#define BAT_STOP_CHARGE		{P6OUT |= BIT6; P6OUT |= BIT7;}

#define BAT_ONE_DISCHARGE 	{P4OUT |= BIT2; P4OUT &= ~BIT3;}
#define BAT_TWO_DISCHARGE		{P4OUT &= ~BIT2; P4OUT &= ~BIT3;}
#define BAT_STOP_DISCHARGE	{P4OUT |= BIT2; P4OUT |= BIT3;}
////BATTERY IO  end

////MOTOR  IO  start
#define MOTOR_ON							(P3OUT |= BIT5)
#define MOTOR_OFF							(P3OUT &= ~BIT5)
////MOTOR  IO  end


/////////KEY IO start/////////////////

#define KEY_VALUE         (P1IN & 0x03f)

/////////KEY IO end///////////////////


////////UART1 start///////////////////
#define uart1_start_send(data)  {(UCA1TXBUF = data);UC1IE |= UCA1TXIE;}
#define uart1_send_data(data)		(UCA1TXBUF = data)
///////UART1 end/////////////////////

///////ADC12 start//////////////////
#define START_ADC()									(ADC12CTL0 |= ADC12SC)
#define ADC_CHANNEL_STATE(channel) 	(ADC12IFG&channel) 
#define ADC_CHANNEL0_VALUE					(ADC12MEM0)
#define ADC_CHANNEL1_VALUE					(ADC12MEM1)
#define ADC_CHANNEL2_VALUE					(ADC12MEM2)
#define ADC_CHANNEL3_VALUE					(ADC12MEM3)
#define ADC_CHANNEL4_VALUE					(ADC12MEM4)
#define ADC_CHANNEL5_VALUE					(ADC12MEM5)
#define ADC_CHANNEL6_VALUE					(ADC12MEM6)
//////ADC12 end////////////////////

#endif

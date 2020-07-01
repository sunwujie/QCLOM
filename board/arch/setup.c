#include <in430.h>
#include <msp430x24x.h>


#define MCLK_1MHZ 1

unsigned char clock_init(void)
{
#if (MCLK_1MHZ)
  // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1 = CALBC1_1MHZ;            

  //MCLK, SMCLK  DIVM--MCLK��Ƶϵ��,  DIVS--SMCLK��Ƶϵ��
  BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;          
#else    //MCLK_4MHZ
  // Set DCO to 8MHz
  DCOCTL = CALDCO_8MHZ;
  BCSCTL1 = CALBC1_8MHZ;            

  //MCLK, SMCLK DIVM--MCLK��Ƶϵ��,  DIVS--SMCLK��Ƶϵ��
  BCSCTL2 = SELM_0 + DIVM_1 + DIVS_1;          
#endif
   //ACLK
  BCSCTL1 |= XT2OFF;  
  //LFXT1ѡ���Ƶģʽ
  BCSCTL1 &= ~XTS;    
  //ACLKѡ��1��Ƶ 
  BCSCTL1 &= ~(DIVA0|DIVA1);       
  BCSCTL3 = XT2S_2+LFXT1S_0+XCAP_1+LFXT1OF;

	// osc fault flag �ⲿ����ʧЧ���
  do
  {   
    IFG1 &= ~OFIFG;             
  }while (IFG1 & OFIFG);                      

  return 0;
}


/**********************************************************
1.PxSEL : ����ѡ��Ĵ���,00:I/O�ڹ���. 
2.PxDIR : ����Ĵ���,1:�������,0:���빦��.
3.PxOUT : ���ʱ,��ʾ���ߵ͵�ƽ;����ʱ,��ʾ����������������.
4.PxIN  : ���빦��ʱ,��ʾ����Ϊ�ߵ�ƽ���ߵ͵�ƽ.
5.pxREN : ���빦��ʱ,��ʾʹ��������������.
6.pxIE  : I/O���ж�ʹ�ܼĴ���.1:ʹ��;0:��ֹ.
7.pxIFG : I/O���ж������־. 1:���ж�����; 0:���ж�����.
8.PxIES : �жϴ�����ʽ, 0:�����ش���; 1:�½��ش���.
ע: unused pin should be UNCONNECTED and configured as IO function,
OUTPUT direction, output value could be either 0 or 1
***************************************************************/
unsigned char GPIO_init(void)
{
  /********************** 
	P1.0:UP_KEY
	P1.1:DOWN_KEY
	P1.2:LEFT_KEY
	P1.3:RIGHT_KEY
	P1.4:RETURN_KEY
  P1.5:ENTER_KEY
  **********************/
	P1SEL =0;
	P1DIR =0;
	P1REN =0;

	P1IE = BIT0|BIT1|BIT2|BIT3|BIT4|BIT5;
	P1IES = 0;
	P1IFG =0;

	/********************** 
	P2.0:AD-SDA
	P2.1:AD-SCL
	P2.2:MENB2
	P2.3:MENB3
	P2.4:MENB4
	P2.5:VCC-SYS 
	P2.6:JC
	P2.7:TIME-SCLK
	**********************/
	P2SEL = 0;
	P2DIR = (unsigned char)(BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
	P2DIR &= ~(BIT5|BIT6);   
	P2OUT = 0;    
	P2REN = 0;
    
	P2IE=BIT5;   
	P2IES &= ~BIT5;
	P2IFG=0;

	/********************** 
	P3.0:TIME-I/O
	P3.1:SDA (I2C�洢)
	P3.2:SCL (I2C�洢)
 	P3.3:TIME-RST
	P3.4:PUMP_EN
	P3.5:MCU-VIB
	P3.6:UART-RX
	P3.7:UART-TX
	**********************/
	P3SEL=(unsigned char)(BIT6|BIT7);
	P3DIR =BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6;   
	P3OUT=0;    
	P3REN=0;

 	/********************** 
	P4.0:MPOW-EN
	P4.1:SPOW-EN
	P4.2:BAT2_SEL_MCU
	P4.3:BAT1_SEL_MCU
	P4.4:MCU-LOAD
	P4.5:MOTOR-EN
	P4.6:DIS-SDA
	P4.7:DIS-SCL
	**********************/
	P4SEL=0;
	P4DIR=0xFF;        
	P4OUT=0;        
	P4REN=0;	
 
	/********************** 
	P5.0:LED-G
	P5.1:S-SDA
	P5.2:S-SCL
	P5.3:LED-R
	P5.4:LED-Y
	P5.5:BUZZER_POWER_EN
	P5.6:BUZZER
	P5.7:BACKLIGHT_EN
	**********************/
	P5SEL=0;
	P5DIR=0xFF;        
	P5OUT=0;    
	P5REN=0;

	/********************** 
	P6.0:VOUT4
	P6.1:VOUT3
 	P6.2:VOUT2
 	P6.3:BAT1_RESISTOR
 	P6.4:BAT2_RESISTOR
 	P6.5:MOTOR_CURRENT
 	P6.6:MCU-BT1
 	P6.7:MCU-BT2
	**********************/
	P6SEL=BIT0|BIT1|BIT2|BIT3|BIT4|BIT5;
	P6DIR=BIT6|BIT7;          
	P6OUT=BIT6|BIT7;
 	P6REN=0;	

	return 0;
}

void GPIO_poweroff(void)
{
	P1DIR = BIT0+BIT1+BIT2+BIT3+BIT5+BIT6+BIT7;
	P1OUT =0;
	P2DIR += BIT6;
	P2OUT =0;
	P3DIR = 0XFF;
	P3OUT = BIT1+BIT2;
	P4DIR = BIT2 + BIT3 + BIT6 + BIT7;
  P4OUT = BIT2 + BIT3 + BIT6 + BIT7;
  P5DIR = 0xFF;
  P5OUT = 0;
  P6DIR = 0xFF;
  P6OUT = BIT6 + BIT7;
}


#define TIMERA_SRC_CLK  32.768  //��ʱ����ʱ��ԴΪ32768 HZ.

//timerA��ʱtimer_ms����.
unsigned char timerA_config(unsigned int num_ms)
{
  //TASSEL_1--ACLK as timerA source;  
  //TACLR--clear TAR, clk divider, count direction
  TACTL = TASSEL_1+TACLR; 
  TACCR0=(unsigned int)(num_ms*TIMERA_SRC_CLK);        
  //start  timer, up mode--0~CCR0
  TACTL |= MC0;        
  //enable Compare interrupt
  TACCTL0=CCIE;        
    
  return 0;
}

//2000
unsigned char timerB_config(unsigned int pwm_frq_hz)
{
  P4DIR |= BIT4;
  P4SEL |= BIT4;
  TBCCR0 = pwm_frq_hz;
  TBCTL = TASSEL_2+ID_3+MC_1;
  TBCCTL4 |= OUTMOD_7;
  TBCCR4 = 0;
    
  return 0;
}

unsigned char adc_config(void)
{
  // ʹ�� A/D ͨ�� ����
  P6SEL = 0x3F; 
  // ���� ADC12, �趨����Ƶ�ʣ�ʹ���ڲ�2.5V�ο���ѹ��ʹ���ڲ��ο���ѹ
  ADC12CTL0 = ADC12ON+MSC+SHT0_2+REF2_5V+REFON; 
  // ʹ�ò�����ʱ��, ת��һ��
  ADC12CTL1 = SHP+ADC12SSEL_2+CONSEQ_1; 
  // ref+=VREF+, channel = A5
  ADC12MCTL0 = SREF_1+INCH_5;  
  // ref+=VREF+, channel = A4
  ADC12MCTL1 = SREF_1+INCH_4;
  // ref+=VREF+, channel = A3
  ADC12MCTL2 = SREF_1+INCH_3;  
  // ref+=VREF+, channel = A2
  ADC12MCTL3 = SREF_1+INCH_2; 
  // ref+=VREF+, channel = A1
  ADC12MCTL4 = SREF_1+INCH_1; 
   // ref+=VREF+, channel = A0
  ADC12MCTL5 = SREF_1+INCH_0;  
  // ref+=VREF+, channel = A10 End of squence (�¶ȴ�����)
  ADC12MCTL6 = SREF_1+INCH_10+EOS; 
  // ת��ʹ��
  ADC12CTL0 |= ENC;                                       
	//��ֹת���ж�.
	ADC12IE = 0; 
	
  return 0;
}

void adc_poweroff(void)
{
	ADC12CTL0 &= ~ENC;
	ADC12CTL0 &= ~ADC12SC; 
	P6SEL = 0; 
	P6DIR = 0x3F;
	P6OUT = 0;	
}

#define UART1_SRC_CLK  32768  //uart1��ʱ��ԴΪ32768 HZ.

unsigned char uart1_config(unsigned int band)
{
  P3SEL |= (BIT6|BIT7);
  //TXD1
  P3DIR |= BIT6;   
  //RXD1
  P3DIR &= (~BIT7);  

  //UART mode, 8-bit character, odd parity,  1 bit stop
  UCA1CTL1 |= UCSWRST;  
  // CLK = ACLK
  UCA1CTL1 |= UCSSEL_1;  
  
  UCA1BR0 = (unsigned char)(UART1_SRC_CLK/band);                           
  UCA1BR1 = 0x00; 
  // Modulation UCBRSx = 2
  UCA1MCTL = UCBRS1; 
  // 8-bit character,Odd parity enabled,One stop bit
 // UCA1CTL0= UCPEN ;   
  // **Initialize USCI state machine**
  UCA1CTL1 &= ~UCSWRST; 
  // clear interruption flag
  UC1IFG&=~UCA1RXIFG; 
   // Enable USCI_A1 RX interrupt
  UC1IE |= UCA1RXIE|UCA1TXIE;                         
  //UCA1TXIE
   __bis_SR_register(GIE); 
   
  return 0;
}

void uart1_poweroff(void)
{
	P3SEL &=~ (BIT6|BIT7); 
	P3DIR |= BIT7;
	P3OUT |= BIT6 + BIT7;
	UC1IE &= ~UCA1TXIE;
}

unsigned char mcu_init(void)
{
  unsigned char ret=0;
  
//  ret=clock_init();
  GPIO_init();
  timerA_config(5);
  ret=timerB_config(2000);
  adc_config();
  
  return ret;
}

void mcu_poweroff(void)
{
	adc_poweroff();
	uart1_poweroff();
	GPIO_poweroff();
}


void extern_int_init(void)
{
	P1SEL =0;
	P1DIR =0;
	P1REN =0;

	P1IE = BIT0|BIT1|BIT2|BIT3|BIT4|BIT5;
	P1IES = 0;
	P1IFG =0;	

	P2SEL = 0;
	P2DIR &= ~(BIT5|BIT6);   
	P2OUT = 0;    
	P2REN = 0;
    
	P2IE=BIT5;   
	//0:�����ش����ж�. 
	//1:�½��ش����ж�.BIT5
	P2IES =0;
	P2IFG=0;
}

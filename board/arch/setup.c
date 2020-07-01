#include <in430.h>
#include <msp430x24x.h>


#define MCLK_1MHZ 1

unsigned char clock_init(void)
{
#if (MCLK_1MHZ)
  // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  BCSCTL1 = CALBC1_1MHZ;            

  //MCLK, SMCLK  DIVM--MCLK分频系数,  DIVS--SMCLK分频系数
  BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;          
#else    //MCLK_4MHZ
  // Set DCO to 8MHz
  DCOCTL = CALDCO_8MHZ;
  BCSCTL1 = CALBC1_8MHZ;            

  //MCLK, SMCLK DIVM--MCLK分频系数,  DIVS--SMCLK分频系数
  BCSCTL2 = SELM_0 + DIVM_1 + DIVS_1;          
#endif
   //ACLK
  BCSCTL1 |= XT2OFF;  
  //LFXT1选择低频模式
  BCSCTL1 &= ~XTS;    
  //ACLK选择1分频 
  BCSCTL1 &= ~(DIVA0|DIVA1);       
  BCSCTL3 = XT2S_2+LFXT1S_0+XCAP_1+LFXT1OF;

	// osc fault flag 外部晶振失效检测
  do
  {   
    IFG1 &= ~OFIFG;             
  }while (IFG1 & OFIFG);                      

  return 0;
}


/**********************************************************
1.PxSEL : 功能选择寄存器,00:I/O口功能. 
2.PxDIR : 方向寄存器,1:输出功能,0:输入功能.
3.PxOUT : 输出时,表示出高低电平;输入时,表示上拉或者下拉输入.
4.PxIN  : 输入功能时,表示输入为高电平或者低电平.
5.pxREN : 输入功能时,表示使能上拉或者下拉.
6.pxIE  : I/O口中断使能寄存器.1:使能;0:禁止.
7.pxIFG : I/O口中断请求标志. 1:有中断请求; 0:无中断请求.
8.PxIES : 中断触发方式, 0:上升沿触发; 1:下降沿触发.
注: unused pin should be UNCONNECTED and configured as IO function,
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
	P3.1:SDA (I2C存储)
	P3.2:SCL (I2C存储)
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


#define TIMERA_SRC_CLK  32.768  //定时器的时钟源为32768 HZ.

//timerA定时timer_ms毫秒.
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
  // 使能 A/D 通道 输入
  P6SEL = 0x3F; 
  // 开启 ADC12, 设定采样频率，使用内部2.5V参考电压，使能内部参考电压
  ADC12CTL0 = ADC12ON+MSC+SHT0_2+REF2_5V+REFON; 
  // 使用采样定时器, 转换一次
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
  // ref+=VREF+, channel = A10 End of squence (温度传感器)
  ADC12MCTL6 = SREF_1+INCH_10+EOS; 
  // 转换使能
  ADC12CTL0 |= ENC;                                       
	//禁止转换中断.
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

#define UART1_SRC_CLK  32768  //uart1的时钟源为32768 HZ.

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
	//0:上升沿触发中断. 
	//1:下降沿触发中断.BIT5
	P2IES =0;
	P2IFG=0;
}

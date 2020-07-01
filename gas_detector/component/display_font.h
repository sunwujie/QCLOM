#ifndef _DISPLAY_FONT_H
#define _DISPLAY_FONT_H

/****************** LCD七段第一行显示和小数点 *****************/
#define  SEG7_A          0x80
#define  SEG7_E          0x40
#define  SEG7_F          0x20
#define  SEG7_D          0x10
#define  SEG7_B          0x08
#define  SEG7_C          0x04
#define  SEG7_G          0x02

/****************** LCD七段第二行显示和小数点 *****************/
#define  DOWN_SEG7_A     0x08
#define  DOWN_SEG7_E     0x04
#define  DOWN_SEG7_F     0x02
#define  DOWN_SEG7_D     0x01
#define  DOWN_SEG7_B     0x80
#define  DOWN_SEG7_C     0x40
#define  DOWN_SEG7_G     0x20
/****************** LCD上方七段显示和小数点 *****************/
#define  UP_SEG7_G       0x40
#define  UP_SEG7_C       0x20
#define  UP_SEG7_B       0x10
#define  UP_SEG7_D       0x08
#define  UP_SEG7_F       0x04
#define  UP_SEG7_E       0x02
#define  UP_SEG7_A       0x01

//七段显示的二维数组，字符的ASCII码与显示段码一一对应。
const unsigned char first_row_seg7_decode_table[][2]=
{
    '0',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_F,        
    '1',    SEG7_B+SEG7_C,                                       
    '2',    SEG7_A+SEG7_B+SEG7_D+SEG7_E+SEG7_G,                     
    '3',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_G,                    
    '4',    SEG7_B+SEG7_C+SEG7_F+SEG7_G,                          
    '5',    SEG7_A+SEG7_C+SEG7_D+SEG7_F+SEG7_G,                    
    '6',    SEG7_A+SEG7_C+SEG7_D+SEG7_E+SEG7_F+SEG7_G,              
    '7',    SEG7_A+SEG7_B+SEG7_C,                              
    '8',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_F+SEG7_G,       
    '9',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_F+SEG7_G,          
    'A',    SEG7_A+SEG7_B+SEG7_C+SEG7_E+SEG7_F+SEG7_G,             
    'B',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_F+SEG7_G,      
    'b',    SEG7_C+SEG7_D+SEG7_E+SEG7_F+SEG7_G,               
    'C',    SEG7_A+SEG7_D+SEG7_E+SEG7_F,                      
    'D',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_F,            
    'd',    SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_G,               
    'E',    SEG7_A+SEG7_F+SEG7_E+SEG7_D+SEG7_G,                 
    'e',    SEG7_A+SEG7_B+SEG7_C+SEG7_E+SEG7_F+SEG7_G,            
    'F',    SEG7_A+SEG7_E+SEG7_F+SEG7_G,                       
    'G',    SEG7_A+SEG7_C+SEG7_D+SEG7_E+SEG7_F,              
    'H',    SEG7_B+SEG7_C+SEG7_E+SEG7_F+SEG7_G,                
    'I',    SEG7_B+SEG7_C,                                    
    'J',    SEG7_B+SEG7_C+SEG7_D,                             
    'L',    SEG7_D+SEG7_E+SEG7_F,                              
    'l',    SEG7_B+SEG7_C,                                      
    'n',    SEG7_A+SEG7_B+SEG7_C+SEG7_E+SEG7_F,                 
    'O',    SEG7_A+SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_F,          
    'o',    SEG7_C+SEG7_D+SEG7_E+SEG7_G,                      
    'P',    SEG7_A+SEG7_B+SEG7_G+SEG7_E+SEG7_F,               
    'q',    SEG7_A+SEG7_B+SEG7_C+SEG7_F+SEG7_G,              
    'r',    SEG7_E+SEG7_G,                                    
    'S',    SEG7_A+SEG7_C+SEG7_D+SEG7_F+SEG7_G,              
    't',    SEG7_D+SEG7_E+SEG7_F+SEG7_G,                      
    'U',    SEG7_B+SEG7_C+SEG7_D+SEG7_E+SEG7_F,               
    '-',    SEG7_G,                                           
    ' ',    0,    
    '_',    SEG7_D,
    ':',    SEG7_D+SEG7_G,
};


//七段显示的二维数组，字符的ASCII码与显示段码一一对应。
const unsigned char second_row_seg7_decode_table[][3]=
{
    '0',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_B+DOWN_SEG7_C,
    '1',    0,                                               DOWN_SEG7_B+DOWN_SEG7_C,
    '2',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E,             DOWN_SEG7_B+DOWN_SEG7_G,
    '3',    DOWN_SEG7_A+DOWN_SEG7_D,                         DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    '4',    DOWN_SEG7_F,                                     DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    '5',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_F,             DOWN_SEG7_C+DOWN_SEG7_G,
    '6',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_C+DOWN_SEG7_G,    
    '7',    DOWN_SEG7_A,                                     DOWN_SEG7_B+DOWN_SEG7_C,
    '8',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,    
    '9',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_F,             DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    'A',    DOWN_SEG7_A+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    'B',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,   
    'b',    DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_C+DOWN_SEG7_G,
    'C',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, 0,   
    'D',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_B+DOWN_SEG7_C,  
    'd',    DOWN_SEG7_D+DOWN_SEG7_E,                         DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    'E',    DOWN_SEG7_A+DOWN_SEG7_F+DOWN_SEG7_E+DOWN_SEG7_D, DOWN_SEG7_G,       
    'e',    DOWN_SEG7_A+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    'F',    DOWN_SEG7_A+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_G, 
    'G',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_C,
    'H',    DOWN_SEG7_E+DOWN_SEG7_F,                         DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    'I',    0,                                               DOWN_SEG7_B+DOWN_SEG7_C,
    'J',    DOWN_SEG7_D,                                     DOWN_SEG7_B+DOWN_SEG7_C,
    'L',    DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F,             0,
    'l',    0,                                               DOWN_SEG7_B+DOWN_SEG7_C,
    'n',    DOWN_SEG7_A+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_B+DOWN_SEG7_C,
    'O',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F, DOWN_SEG7_B+DOWN_SEG7_C,
    'o',    DOWN_SEG7_D+DOWN_SEG7_E,                         DOWN_SEG7_C+DOWN_SEG7_G,
    'P',    DOWN_SEG7_A+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_B+DOWN_SEG7_G,
    'q',    DOWN_SEG7_A+DOWN_SEG7_F,                         DOWN_SEG7_B+DOWN_SEG7_C+DOWN_SEG7_G,
    'r',    DOWN_SEG7_E,                                     DOWN_SEG7_G,
    'S',    DOWN_SEG7_A+DOWN_SEG7_D+DOWN_SEG7_F,             DOWN_SEG7_C+DOWN_SEG7_G,
    't',    DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_G,
    'U',    DOWN_SEG7_D+DOWN_SEG7_E+DOWN_SEG7_F,             DOWN_SEG7_B+DOWN_SEG7_C,
    '-',    0,                                               DOWN_SEG7_G,
    ' ',    0,                                               0,
    '_',    DOWN_SEG7_D,                                     0,
    ':',    DOWN_SEG7_D,                                     DOWN_SEG7_G,
};

//右上角七段显示的二维数组，字符的ASCII码与显示段码一一对应。
const unsigned char up_seg7_decode_table[][2]=
{
    '0',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,        
    '1',    UP_SEG7_B+UP_SEG7_C,                                       
    '2',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_D+UP_SEG7_E+UP_SEG7_G,                     
    '3',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_G,                    
    '4',    UP_SEG7_B+UP_SEG7_C+UP_SEG7_F+UP_SEG7_G,                          
    '5',    UP_SEG7_A+UP_SEG7_C+UP_SEG7_D+UP_SEG7_F+UP_SEG7_G,                    
    '6',    UP_SEG7_A+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,              
    '7',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C,                              
    '8',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,       
    '9',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_F+UP_SEG7_G,          
    'A',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,             
    'B',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,      
    'b',    UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,               
    'C',    UP_SEG7_A+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,                      
    'D',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,            
    'd',    UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_G,               
    'E',    UP_SEG7_A+UP_SEG7_F+UP_SEG7_E+UP_SEG7_D+UP_SEG7_G,                 
    'e',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,            
    'F',    UP_SEG7_A+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,                       
    'G',    UP_SEG7_A+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,              
    'H',    UP_SEG7_B+UP_SEG7_C+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,                
    'I',    UP_SEG7_B+UP_SEG7_C,                                    
    'J',    UP_SEG7_B+UP_SEG7_C+UP_SEG7_D,                             
    'L',    UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,                              
    'l',    UP_SEG7_B+UP_SEG7_C,                                      
    'n',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_E+UP_SEG7_F,                 
    'O',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,          
    'o',    UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_G,                      
    'P',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_G+UP_SEG7_E+UP_SEG7_F,               
    'q',    UP_SEG7_A+UP_SEG7_B+UP_SEG7_C+UP_SEG7_F+UP_SEG7_G,              
    'r',    UP_SEG7_E+UP_SEG7_G,                                    
    'S',    UP_SEG7_A+UP_SEG7_C+UP_SEG7_D+UP_SEG7_F+UP_SEG7_G,              
    't',    UP_SEG7_D+UP_SEG7_E+UP_SEG7_F+UP_SEG7_G,                      
    'U',    UP_SEG7_B+UP_SEG7_C+UP_SEG7_D+UP_SEG7_E+UP_SEG7_F,               
    '-',    UP_SEG7_G,                                           
    ' ',    0,  
    '_',    UP_SEG7_D,
    ':',    UP_SEG7_D+UP_SEG7_G,
};


#endif


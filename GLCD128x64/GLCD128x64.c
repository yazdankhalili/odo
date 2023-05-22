/**
******************************************************************************
* @file		GLCD128x64.c
* @author	Yazdan Khalili
* @date		6 February 2020
* @note		
******************************************************************************
*/

/** Includes ---------------------------------------------------------------- */
#include "GLCD128x64.h"
#include "delay.h"	

typedef enum
{
  Disable = 0,
  Enable
}Activation_Enum ; 


uint16_t Y_Address;
/** Private function prototypes --------------------------------------------- */
static void GLCD128x64_toggle_e(void);
static void GLCD128x64_write(uint8_t data, uint8_t rs);
void GLCD128x64_write_Data(uint8_t data);
static uint8_t GLCD128x64_read(uint8_t rs);
static uint8_t GLCD128x64_wait_busy(void);

void GLCD128x64_putc(char c_string[]);
void GLCD128x64_CS1(Activation_Enum state );
void GLCD128x64_CS2(Activation_Enum state );




void GLCD128x64_Initialize(void )
{
  GPIO_InitTypeDef GPIO_InitStruct;
  // GPIO clock for control and data lines
  RCC_APB2PeriphClockCmd(GLCD128x64_RCC_GPIO_CONTROL, ENABLE);
  RCC_APB2PeriphClockCmd(GLCD128x64_RCC_GPIO_DATA, ENABLE);
  
  // Configure I/O for control lines as output
  GPIO_InitStruct.GPIO_Pin = GLCD128x64_PIN_RS | GLCD128x64_PIN_RW |
    GLCD128x64_PIN_EN | GLCD128x64_PIN_CS1 | GLCD128x64_PIN_CS2 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GLCD128x64_GPIO_CONTROL, &GPIO_InitStruct);
  
  // Configure I/O for data lines as output
  GPIO_InitStruct.GPIO_Pin = GLCD128x64_PIN_D4 | GLCD128x64_PIN_D5 |
    GLCD128x64_PIN_D6 | GLCD128x64_PIN_D7| GLCD128x64_PIN_D3| GLCD128x64_PIN_D2|
      GLCD128x64_PIN_D1| GLCD128x64_PIN_D0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GLCD128x64_GPIO_DATA, &GPIO_InitStruct);
  
  GLCD128x64_CS1(Enable);
  GLCD128x64_CS2(Enable);
  Delay_Us(GLCD128x64_DELAY_POWER_ON);
  GLCD128x64_write_command(0x40);
  GLCD128x64_write_command(0xB8);
  GLCD128x64_write_command(0xC0);
  GLCD128x64_write_command(GLCD128x64_DISPLAY_ON);
  GLCD128x64_Clr_All();
}


void GLCD128x64_write_command(uint8_t cmd)
{
   //GLCD128x64_wait_busy();
  GLCD128x64_write(cmd, 0);
}


void GLCD128x64_write_Data(uint8_t data)
{
  //GLCD128x64_wait_busy();
  GLCD128x64_write(data, 1);
}


static void GLCD128x64_toggle_e()
{
  GPIO_SetBits(GLCD128x64_GPIO_EN, GLCD128x64_PIN_EN);// EN pin = HIGH
  Delay_Us(1);// Pulse length in us
  GPIO_ResetBits(GLCD128x64_GPIO_EN, GLCD128x64_PIN_EN); // EN pin = LOW
  Delay_Us(1); // Pulse length in us
}


static void GLCD128x64_write(uint8_t data, uint8_t rs)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_ResetBits(GLCD128x64_GPIO_RW, GLCD128x64_PIN_RW);   // Write mode (RW = 0)
  if (rs == 1)
  {
    GPIO_SetBits(GLCD128x64_GPIO_RS, GLCD128x64_PIN_RS);   // Write data (RS = 1)
  }
  else	
  {       
    GPIO_ResetBits(GLCD128x64_GPIO_RS, GLCD128x64_PIN_RS);   // Write instruction (RS = 0)
  }
  
  // Configure I/O for data lines as output
  GPIO_InitStruct.GPIO_Pin = GLCD128x64_PIN_D4 | GLCD128x64_PIN_D5 |
    GLCD128x64_PIN_D6 | GLCD128x64_PIN_D7| GLCD128x64_PIN_D3| GLCD128x64_PIN_D2|
      GLCD128x64_PIN_D1| GLCD128x64_PIN_D0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GLCD128x64_GPIO_DATA, &GPIO_InitStruct);
  
  GPIO_ResetBits(GLCD128x64_GPIO_D7, GLCD128x64_PIN_D7);// Output high nibble first
  GPIO_ResetBits(GLCD128x64_GPIO_D6, GLCD128x64_PIN_D6);
  GPIO_ResetBits(GLCD128x64_GPIO_D5, GLCD128x64_PIN_D5);
  GPIO_ResetBits(GLCD128x64_GPIO_D4, GLCD128x64_PIN_D4);
  GPIO_ResetBits(GLCD128x64_GPIO_D3, GLCD128x64_PIN_D3); // Output high nibble first
  GPIO_ResetBits(GLCD128x64_GPIO_D2, GLCD128x64_PIN_D2);
  GPIO_ResetBits(GLCD128x64_GPIO_D1, GLCD128x64_PIN_D1);
  GPIO_ResetBits(GLCD128x64_GPIO_D0, GLCD128x64_PIN_D0);
  if (data & 0x80)
  {
    GPIO_SetBits(GLCD128x64_GPIO_D7, GLCD128x64_PIN_D7);
  }
  if (data & 0x40)
  {
    GPIO_SetBits(GLCD128x64_GPIO_D6, GLCD128x64_PIN_D6);
  }
  if (data & 0x20)
  {
    GPIO_SetBits(GLCD128x64_GPIO_D5, GLCD128x64_PIN_D5);
  }
  if (data & 0x10) 
  {
    GPIO_SetBits(GLCD128x64_GPIO_D4, GLCD128x64_PIN_D4);
  }
  if (data & 0x08) 
  {
    GPIO_SetBits(GLCD128x64_GPIO_D3, GLCD128x64_PIN_D3);
  }
  
  if (data & 0x04)
  {
    GPIO_SetBits(GLCD128x64_GPIO_D2, GLCD128x64_PIN_D2);
  }
  if (data & 0x02) 
  {
    GPIO_SetBits(GLCD128x64_GPIO_D1, GLCD128x64_PIN_D1);
  }
  if (data & 0x01)
  {
    GPIO_SetBits(GLCD128x64_GPIO_D0, GLCD128x64_PIN_D0);
  }
  
  GLCD128x64_toggle_e();
  
}


static uint8_t GLCD128x64_read(uint8_t rs)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t data = 0;
  
  GPIO_SetBits(GLCD128x64_GPIO_RW, GLCD128x64_PIN_RW);   // Read mode (RW = 1)
  if (rs == 1)
  {
    GPIO_SetBits(GLCD128x64_GPIO_RS, GLCD128x64_PIN_RS);   // Read data (RS = 1)
  }
  else	
  {       
    GPIO_ResetBits(GLCD128x64_GPIO_RS, GLCD128x64_PIN_RS);   // Read busy flag and DDRAM address (RS = 0)
  }
  
  // Configure all data pins as input
  GPIO_InitStruct.GPIO_Pin = GLCD128x64_PIN_D4 | GLCD128x64_PIN_D5 |
    GLCD128x64_PIN_D6 | GLCD128x64_PIN_D7| GLCD128x64_PIN_D3| GLCD128x64_PIN_D2|
      GLCD128x64_PIN_D1| GLCD128x64_PIN_D0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GLCD128x64_GPIO_DATA, &GPIO_InitStruct);
  
  
  
  GPIO_SetBits(GLCD128x64_GPIO_EN, GLCD128x64_PIN_EN); // EN pin = HIGH
  
  Delay_Us(100); // Pulse length in us
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D7, GLCD128x64_PIN_D7) == Bit_SET)
  { 
    data |= 0x80 ;
  }
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D6, GLCD128x64_PIN_D6) == Bit_SET)
  {
    data |= 0x40;
  }
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D5, GLCD128x64_PIN_D5) == Bit_SET)
  {
    
    data |= 0x20 ;
  }
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D4, GLCD128x64_PIN_D4) == Bit_SET) 
  {
    
    data |= 0x10 ;
  }
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D3, GLCD128x64_PIN_D3) == Bit_SET) 
  {
    data |= 0x08 ;
  }
  
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D2, GLCD128x64_PIN_D2) == Bit_SET)
  {
    data |= 0x04 ;
  }
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D1, GLCD128x64_PIN_D1) == Bit_SET) 
  {
    data |= 0x02 ;
  }
  if (GPIO_ReadInputDataBit(GLCD128x64_GPIO_D0, GLCD128x64_PIN_D0) == Bit_SET)
  {
    data |= 0x01 ;
  }
  GPIO_ResetBits(GLCD128x64_GPIO_EN, GLCD128x64_PIN_EN); // EN pin = Low
  Delay_Us(10);
  return data;
  
}


static uint8_t GLCD128x64_wait_busy()
{
  
  while (GLCD128x64_read(0) & (0x80));  // Wait until busy flag is cleared
  {
    Delay_Us(8); // Delay needed for address counter is updated after busy flag is cleared
  }
  
  // Read and return address counter
  return GLCD128x64_read(0);
}


void GLCD128x64_GOTO_Row(uint8_t y )
{
    GLCD128x64_CS1(Enable);
    GLCD128x64_CS2(Enable);
  GLCD128x64_write_command(0xB8 | y ) ; 
  
}

void GLCD128x64_GOTO_Column(uint8_t x )
{
  if ( x < 64 )
  {
    GLCD128x64_CS1(Enable);
    GLCD128x64_CS2(Disable);
    GLCD128x64_write_command(0x40 | x ) ; 
  }
  else
  {
    GLCD128x64_CS2(Enable);
    GLCD128x64_CS1(Disable);
    GLCD128x64_write_command(0x40 | (x-64) ) ; 
  }
  
}
void GLCD128x64_GOTO_XY(uint8_t x , uint8_t y )
{ 
  GLCD128x64_GOTO_Row(y);
  GLCD128x64_GOTO_Column(x);
  Y_Address =x;
}


void GLCD128x64_Clr_All(void )
{
  GLCD128x64_CS1(Enable);
  GLCD128x64_CS2(Enable);
  for ( uint8_t i = 0 ; i < 8 ; i++ )
  {
    GLCD128x64_write_command(0xB8 | i ) ; 	/* Increment Line */ 
    for ( uint8_t j = 0 ; j < 64 ; j++ )
    {
      GLCD128x64_write_Data(0); /* after writing instruction ,Y adderess is increased by 1 */
    } 
  } 
  GLCD128x64_write_command(0x40);
  GLCD128x64_write_command(0xB8);
}

void GLCD128x64_Clr_Line( uint16_t end_column , uint16_t strat_column , uint8_t line )
{
    uint16_t y_address;
    GLCD128x64_GOTO_XY(strat_column ,line );
  y_address = strat_column ;
  for ( uint8_t i = 0 ; i <= end_column - strat_column  ; i++ )
  {
    if ( Y_Address >=64 )
    {
        
       GLCD128x64_CS1(Disable);
       GLCD128x64_CS2(Enable);
       if ( Y_Address == 64 )
        {
            GLCD128x64_write_command(0x40);
        }
    }
    else 
    {
      GLCD128x64_CS2(Disable);
      GLCD128x64_CS1(Enable);
       
    }
    GLCD128x64_write_Data(0); 
    Y_Address++; 
  }
}


void GLCD128x64_CS1(Activation_Enum state )
{
  if ( state == Enable )
  {
    GPIO_SetBits(GLCD128x64_GPIO_CS, GLCD128x64_PIN_CS1);
    
  } 
  else 
  {
    GPIO_ResetBits(GLCD128x64_GPIO_CS, GLCD128x64_PIN_CS1);
  }
}
void GLCD128x64_CS2(Activation_Enum state )
{
  if ( state == Enable )
  {
    GPIO_SetBits(GLCD128x64_GPIO_CS, GLCD128x64_PIN_CS2);
    
  }
  else 
  {
    GPIO_ResetBits(GLCD128x64_GPIO_CS, GLCD128x64_PIN_CS2);
  }
  
}



void Costum_Char( uint8_t* data_byte )
{
 uint8_t len = strlen(data_byte)+1;
 
  for (uint8_t i = 0 ; i < len ; i++ )
  {
      if ( Y_Address ==128 )
      {
          break;
      }
       if ( Y_Address >=64 )
    {
        
       GLCD128x64_CS1(Disable);
       GLCD128x64_CS2(Enable);
       if ( Y_Address == 64 )
        {
            GLCD128x64_write_command(0x40);
        }
    }
    else 
    {
      GLCD128x64_CS2(Disable);
       GLCD128x64_CS1(Enable);
       
    }
    GLCD128x64_write_Data(data_byte[i]); 
    Y_Address++; 
  }
}




void GLCD128x64_putc(char c_string[])
{
  uint8_t i_count = 0 ; 
  char Space[]={0x00,0x00,0x00};  
  char Exclamation[]={0x4F,0x00,0x00};
  char Nunnation[]={0x07,0x00,0x07};
  char Hashtag[]={0x14,0x7F,0x14,0x7F,0x14};
  char Dolares[]={0x24,0x2A,0x7F,0x2A,0x12};
  char Percentage[]={0x23,0x13,0x08,0x64,0x62};
  char And[]={0x35,0x49,0x55,0x22,0x1450};
  char Single_Quotation[]={0x05,0x03,0x00};
  char Open_Parantheses[]={0x1C,0x22,0x41};
  char Close_Parantheses[]={0x41,0x22,0x1C};
  char Star[]={0x14,0x08,0x3E,0x08,0x14};
  char Plus[]={0x08,0x08,0x3E,0x08,0x08};
  char Kama[]={0x50,0x30,0x00};
  char Minus[]={0x08,0x08,0x08,0x08,0x08};
  char Dot[]={0x60,0x60,0x00};
  char Slash[]={0x20,0x10,0x08,0x04,0x02};
  char Zero[]={0x3E,0x61,0x49,0x45,0x3E};
  char One[]={0x42,0x7F,0x40};
  char Two[]={0x42,0x61,0x51,0x49,0x46};
  char Three[]={0x21,0x41,0x45,0x4B,0x31};
  char Four[]={0x18,0x14,0x12,0x7F,0x10};
  char Five[]={0x27,0x45,0x45,0x45,0x39};
  char Six[]={0x3C,0x4A,0x49,0x49,0x30};
  char Seven[]={0x01,0x01,0x79,0x05,0x03};
  char Eight[]={0x36,0x49,0x49,0x49,0x36};
  char Nine[]={0x06,0x49,0x49,0x29,0x1E};
  char Colon[]={0x66,0x66,0x00};
  char Semi_Colon[]={0x56,0x36,0x00};
  char Less[]={0x08,0x14,0x22,0x41};
  char Equal[]={0x14,0x14,0x14,0x14,0x014};
  char More[]={0x41,0x22,0x14,0x08};
  char Quistion[]={0x02,0x01,0x51,0x09,0x06};
  char A[]={0x7e, 0x11, 0x11, 0x11, 0x7e}; // 41 A
  char B[]={0x7f, 0x49, 0x49, 0x49, 0x36}; // 42 B
  char C[]={0x3e, 0x41, 0x41, 0x41, 0x22} ;// 43 C
  char D[]={0x7f, 0x41, 0x41, 0x22, 0x1c}; // 44 D
  char E[]={0x7f, 0x49, 0x49, 0x49, 0x41} ;// 45 E
  char F[]={0x7f, 0x09, 0x09, 0x09, 0x01} ;// 46 F
  char G[]={0x3e, 0x41, 0x49, 0x49, 0x7a} ;// 47 G
  char H[]={0x7f, 0x08, 0x08, 0x08, 0x7f} ;// 48 H
  char I[]={0x41, 0x7f, 0x41};             // 49 I
  char J[]={0x20, 0x40, 0x41, 0x3f, 0x01}; // 4a J
  char K[]={0x7f, 0x08, 0x14, 0x22, 0x41}; // 4b K
  char L[]={0x7f, 0x40, 0x40, 0x40, 0x40}; // 4c L
  char M[]={0x7f, 0x02, 0x0c, 0x02, 0x7f}; // 4d M
  char N[]={0x7f, 0x04, 0x08, 0x10, 0x7f} ;// 4e N
  char O[]={0x3e, 0x41, 0x41, 0x41, 0x3e} ;// 4f O
  char P[]={0x7f, 0x09, 0x09, 0x09, 0x06} ;// 50 P
  char Q[]={0x3e, 0x41, 0x51, 0x21, 0x5e} ;// 51 Q
  char R[]={0x7f, 0x09, 0x19, 0x29, 0x46} ;// 52 R
  char S[]={0x46, 0x49, 0x49, 0x49, 0x31} ;// 53 S
  char T[]={0x01, 0x01, 0x7f, 0x01, 0x01} ;// 54 T
  char U[]={0x3f, 0x40, 0x40, 0x40, 0x3f} ;// 55 U
  char V[]={0x1f, 0x20, 0x40, 0x20, 0x1f} ;// 56 V
  char W[]={0x3f, 0x40, 0x38, 0x40, 0x3f} ;// 57 W
  char X[]={0x63, 0x14, 0x08, 0x14, 0x63} ;// 58 X
  char Y[]={0x07, 0x08, 0x70, 0x08, 0x07} ;// 59 Y
  char Z[]={0x61, 0x51, 0x49, 0x45, 0x43} ;// 5a Z
  char Open_Braket[]={0x7f, 0x41, 0x41} ;// 5b [
  char Left_Quistion[]={0x02, 0x04, 0x08, 0x10, 0x20}; // 5c ?
  char Close_Braket[]={0x41, 0x41, 0x7f} ;// 5d ]
  char Power[]={0x04, 0x02, 0x01, 0x02, 0x04}; // 5e ^
  char Underline[]={0x40, 0x40, 0x40, 0x40, 0x40}; // 5f _
  char Abro[]={0x01, 0x02, 0x04} ;// 60 `
  char a[]={0x20, 0x54, 0x54, 0x54, 0x78}; // 61 a
  char b[]={0x7f, 0x48, 0x44, 0x44, 0x38}; // 62 b
  char c[]={0x38, 0x44, 0x44, 0x44, 0x20} ;// 63 c
  char d[]={0x38, 0x44, 0x44, 0x48, 0x7f} ;// 64 d
  char e[]={0x38, 0x54, 0x54, 0x54, 0x18} ;// 65 e
  char f[]={0x08, 0x7e, 0x09, 0x01, 0x02} ;// 66 f
  char g[]={0x0c, 0x52, 0x52, 0x52, 0x3e} ;// 67 g
  char h[]={0x7f, 0x08, 0x04, 0x04, 0x78} ;// 68 h
  char i[]={0x44, 0x7d, 0x40, 0x00} ;// 69 i
  char j[]={0x20, 0x40, 0x44, 0x3d, 0x00}; // 6a j 
  char k[]={0x7f, 0x10, 0x28, 0x44,0x00} ;// 6b k
  char l[]={0x41, 0x7f, 0x40, 0x00}; // 6c l
  char m[]={0x7c, 0x04, 0x18, 0x04, 0x78}; // 6d m
  char n[]={0x7c, 0x08, 0x04, 0x04, 0x78} ;// 6e n
  char o[]={0x38, 0x44, 0x44, 0x44, 0x38}; // 6f o
  char p[]={0x7c, 0x14, 0x14, 0x14, 0x08} ;// 70 p
  char q[]={0x08, 0x14, 0x14, 0x18, 0x7c}; // 71 q
  char r[]={0x7c, 0x08, 0x04, 0x04, 0x08} ;// 72 r
  char s[]={0x48, 0x54, 0x54, 0x54, 0x20}; // 73 s
  char t[]={0x04, 0x3f, 0x44, 0x40, 0x20}; // 74 t
  char u[]={0x3c, 0x40, 0x40, 0x20, 0x7c}; // 75 u
  char v[]={0x1c, 0x20, 0x40, 0x20, 0x1c}; // 76 v
  char w[]={0x3c, 0x40, 0x30, 0x40, 0x3c}; // 77 w
  char x[]={0x44, 0x28, 0x10, 0x28, 0x44}; // 78 x
  char y[]={0x0c, 0x50, 0x50, 0x50, 0x3c}; // 79 y
  char z[]={0x44, 0x64, 0x54, 0x4c, 0x44}; // 7a z
  char Open_Brace[]={0x08, 0x36, 0x41}; // 7b {
  char Or[]={0x7f,0x00,0x00}; // 7c |
  char Close_Brace[]={0x41, 0x36, 0x08} ;// 7d }
  char Arrow_Right[]={0x10, 0x08, 0x08, 0x10, 0x08}; // 7e ->
  char Arrow_Left[]={0x78, 0x46, 0x41, 0x46, 0x78} ;// 7f <-
  while( c_string[i_count] != 0x00 )
  {
    if ( c_string[i_count] == 0x20 )
    {
      Costum_Char(Space);
    }
    else if ( c_string[i_count] == 0x21 )
    {
      Costum_Char(Exclamation);
    }
    else if ( c_string[i_count] == 0x22 )
    {
      Costum_Char(Nunnation);
    }
    else if ( c_string[i_count] == 0x23 )
    {
      Costum_Char(Hashtag);
    }
    else if ( c_string[i_count] == 0x24 )
    {
      Costum_Char(Dolares);
    }
    else if ( c_string[i_count] == 0x25 )
    {
      Costum_Char(Percentage);
    }
    else if ( c_string[i_count] == 0x26 )
    {
      Costum_Char(And);
    }
    else if ( c_string[i_count] == 0x27 )
    {
      Costum_Char(Single_Quotation);
    }
    else if ( c_string[i_count] == 0x28 )
    {
      Costum_Char(Open_Parantheses);
    }
    else if ( c_string[i_count] == 0x29 )
    {
      Costum_Char(Close_Parantheses);
    }
    else if ( c_string[i_count] == 0x2A )
    {
      Costum_Char(Star);
    }
    else if ( c_string[i_count] == 0x2B )
    {
      Costum_Char(Plus);
    }
    else if ( c_string[i_count] == 0x2C )
    {
      Costum_Char(Kama);
    }
    else if ( c_string[i_count] == 0x2D )
    {
      Costum_Char(Minus);
    }
    else if ( c_string[i_count] == 0x2E )
    {
      Costum_Char(Dot);
    }
    else if ( c_string[i_count] == 0x2F )
    {
      Costum_Char(Slash);
    }
    else if ( c_string[i_count] == 0x30 )
    {
      Costum_Char(Zero);
    }
    else if ( c_string[i_count] == 0x31 )
    {
      Costum_Char(One);
    }
    else if ( c_string[i_count] == 0x32 )
    {
      Costum_Char(Two);
    }
    else if ( c_string[i_count] == 0x33 )
    {
      Costum_Char(Three);
    }
    else if ( c_string[i_count] == 0x34 )
    {
      Costum_Char(Four);
    }
    else if ( c_string[i_count] == 0x35 )
    {
      Costum_Char(Five);
    }
    
    else if ( c_string[i_count] == 0x36 )
    {
      Costum_Char(Six);
    }
    else if ( c_string[i_count] == 0x37 )
    {
      Costum_Char(Seven);
    }
    
    else if ( c_string[i_count] == 0x38 )
    {
      Costum_Char(Eight);
    }
    
    else if ( c_string[i_count] == 0x39 )
    {
      Costum_Char(Nine);
    }
    
    else if ( c_string[i_count] == 0x3A )
    {
      Costum_Char(Colon);
    }
    
    else if ( c_string[i_count] == 0x3B )
    {
      Costum_Char(Semi_Colon);
    }
    
    else if ( c_string[i_count] == 0x3C )
    {
      Costum_Char(Less);
    }
    
    else if ( c_string[i_count] == 0x3D )
    {
      Costum_Char(Equal);
    }
    else if ( c_string[i_count] == 0x3E )
    {
      Costum_Char(More);
    }
    else if ( c_string[i_count] == 0x3F )
    {
      Costum_Char(Quistion);
    }
    
    else if ( c_string[i_count] == 0x41 )
    {
      Costum_Char(A);
    }
    else if ( c_string[i_count] == 0x42 )
    {
      Costum_Char(B);
    }
    else if ( c_string[i_count] == 0x43 )
    {
      Costum_Char(C);
    }
    else if ( c_string[i_count] == 0x44 )
    {
      Costum_Char(D);
    }
    else if ( c_string[i_count] == 0x45 )
    {
      Costum_Char(E);
    }
    else if ( c_string[i_count] == 0x46 )
    {
      Costum_Char(F);
    }
    else if ( c_string[i_count] == 0x47 )
    {
      Costum_Char(G);
    }
    
    else if ( c_string[i_count] == 0x48 )
    {
      Costum_Char(H);
    }
    else if ( c_string[i_count] == 0x49 )
    {
      Costum_Char(I);
    }
    else if ( c_string[i_count] == 0x4A )
    {
      Costum_Char(J);
    }
    else if ( c_string[i_count] == 0x4B )
    {
      Costum_Char(K);
    }
    else if ( c_string[i_count] == 0x4C )
    {
      Costum_Char(L);
    }

    else if ( c_string[i_count] == 0x4D )
    {
      Costum_Char(M);
    }
    else if ( c_string[i_count] == 0x4E )
    {
      Costum_Char(N);
    }
    else if ( c_string[i_count] == 0x4F )
    {
      Costum_Char(O);
    }
    else if ( c_string[i_count] == 0x50 )
    {
      Costum_Char(P);
    }
    else if ( c_string[i_count] == 0x51 )
    {
      Costum_Char(Q);
    }
    else if ( c_string[i_count] == 0x52 )
    {
      Costum_Char(R);
    }
    else if ( c_string[i_count] == 0x53 )
    {
      Costum_Char(S);
    }
    else if ( c_string[i_count] == 0x54 )
    {
      Costum_Char(T);
    }
    else if ( c_string[i_count] == 0x48 )
    {
      Costum_Char(H);
    }
    else if ( c_string[i_count] == 0x55 )
    {
      Costum_Char(U);
    }
    else if ( c_string[i_count] == 0x56 )
    {
      Costum_Char(V);
    }
    else if ( c_string[i_count] == 0x57 )
    {
      Costum_Char(W);
    } 
    else if ( c_string[i_count] == 0x58 )
    {
      Costum_Char(X);
    }
    else if ( c_string[i_count] == 0x59 )
    {
      Costum_Char(Y);
    }
    else if ( c_string[i_count] == 0x5A )
    {
      Costum_Char(Z);
    }
    else if ( c_string[i_count] == 0x5B )
    {
      Costum_Char(Open_Braket);
    }
    else if ( c_string[i_count] == 0x5C )
    {
      Costum_Char(Left_Quistion);
    }
    else if ( c_string[i_count] == 0x5D )
    {
      Costum_Char(Close_Braket);
    }
    else if ( c_string[i_count] == 0x5E )
    {
      Costum_Char(Power);
    }
    else if ( c_string[i_count] == 0x5F )
    {
      Costum_Char(Underline);
    }
    else if ( c_string[i_count] == 0x60 )
    {
      Costum_Char(Abro);
    }
    else if ( c_string[i_count] == 0x61 )
    {
      Costum_Char(a);
    }
    else if ( c_string[i_count] == 0x62 )
    {
      Costum_Char(b);
    }
    else if ( c_string[i_count] == 0x63 )
    {
      Costum_Char(c);
    }
    else if ( c_string[i_count] == 0x64 )
    {
      Costum_Char(d);
    }
    else if ( c_string[i_count] == 0x65 )
    {
      Costum_Char(e);
    }
    else if ( c_string[i_count] == 0x66 )
    {
      Costum_Char(f);
    } 
    else if ( c_string[i_count] == 0x67 )
    {
      Costum_Char(g);
    }
    else if ( c_string[i_count] == 0x68 )
    {
      Costum_Char(h);
    }
    else if ( c_string[i_count] == 0x69 )
    {
      Costum_Char(i);
    }
    else if ( c_string[i_count] == 0x6A )
    {
      Costum_Char(j);
    }
    else if ( c_string[i_count] == 0x6B )
    {
      Costum_Char(k);
    } 
    else if ( c_string[i_count] == 0x6C )
    {
      Costum_Char(l);
    } 
    else if ( c_string[i_count] == 0x6D )
    {
      Costum_Char(m);
    } 
    else if ( c_string[i_count] == 0x6E )
    {
      Costum_Char(n);
    }
    else if ( c_string[i_count] == 0x6F )
    {
      Costum_Char(o);
    } 
    else if ( c_string[i_count] == 0x70 )
    {
      Costum_Char(p);
    }
    else if ( c_string[i_count] == 0x71 )
    {
      Costum_Char(q);
    }
    else if ( c_string[i_count] == 0x72 )
    {
      Costum_Char(r);
    }
    else if ( c_string[i_count] == 0x73 )
    {
      Costum_Char(s);
    }
    else if ( c_string[i_count] == 0x74 )
    {
      Costum_Char(t);
    }
    else if ( c_string[i_count] == 0x75 )
    {
      Costum_Char(u);
    }
    else if ( c_string[i_count] == 0x76 )
    {
      Costum_Char(v);
    }
    else if ( c_string[i_count] == 0x77 )
    {
      Costum_Char(w);
    }
    else if ( c_string[i_count] == 0x78 )
    {
      Costum_Char(x);
    }
    else if ( c_string[i_count] == 0x79 )
    {
      Costum_Char(y);
    }
    else if ( c_string[i_count] == 0x7A )
    {
      Costum_Char(z);
    }
    else if ( c_string[i_count] == 0x7B )
    {
      Costum_Char(Open_Brace);
    }
    else if ( c_string[i_count] == 0x7C )
    {
      Costum_Char(Or);
    }
    else if ( c_string[i_count] == 0x7D )
    {
      Costum_Char(Close_Brace);
    }
    else if ( c_string[i_count] == 0x7E )
    {
      Costum_Char(Arrow_Right);
    }
    else if ( c_string[i_count] == 0x7F )
    {
      Costum_Char(Arrow_Left);
    }
    i_count++;
  }
}


//
//void GLCD_String(char *str)		/* GLCD string write function */
//{
//  int column,page,page_add=0xB8,k=0;
//  int page_inc=0.5;
//  //CS1 = 1; CS2 = 0;
//  GLCD128x64_CS1(Enable);
//  GLCD128x64_CS2(Disable);
//  for(page=0;page<8;page++) 
//  {
//    for(column=0;column<64;column++)
//    {
//      GLCD128x64_write_Data(str[column+k]);
//    }
//    //CS1 = ~CS1; CS2 = ~CS2;
//    GLCD128x64_CS2(Enable);
//    GLCD128x64_CS1(Disable);
//    GLCD128x64_write_command((page_add+page_inc));
//    page_inc=page_inc+1;
//    k=k+64;
//  }
//  GLCD128x64_write_command(0x40);
//  GLCD128x64_write_command(0xB8);
//}



//void GLCD_String(char page_no, char *str)	/* GLCD string write function */
//{
//  unsigned int i,column,Page=((0xB8)+page_no),Y_address=0;	
//  float Page_inc=0.5;														
//  //CS1 = 1; CS2 = 0;		/* Select first Left half of display */
//  GLCD128x64_CS1(Enable);
//  GLCD128x64_CS2(Disable);
//  GLCD128x64_write_command(Page);
//  for(i=0;str[i]!=0;i++)	/* Print each char in string till null */
//  {
//    if (Y_address>(1024-(((page_no)*128)+5)))   /* Check Whether Total Display get overflowed */
//      break;                                    /* If yes then break writing */
//    if (str[i]!=32)				/* Check whether character is not a SPACE */
//    {
//      for (column=1;column<=5;column++)
//      {
//        if ((Y_address+column)==(128*((int)(Page_inc+0.5))))	/* If yes then check whether it overflow from right side of display */
//        {
//          if (column==5)					/* Also check and break if it overflow after 5th column */
//            break;
//          GLCD128x64_write_command(0x40);					/* If not 5th and get overflowed then change Y address to START column */
//          Y_address = Y_address+column;				/* Increment Y address count by column no. */
//         // CS1 = ~CS1; CS2 = ~CS2;				/* If yes then change segment controller to display on other half of display */
//          GLCD128x64_CS2(Enable);
//         GLCD128x64_CS1(Disable);
//          GLCD128x64_write_command((Page+Page_inc));			/* Execute command for page change */
//          Page_inc=Page_inc+0.5;				/* Increment Page No. by half */
//        }
//      }
//    }
//    if (Y_address>(1024-(((page_no)*128)+5)))   /* Check Whether Total Display get overflowed */
//      break;                                    /* If yes then break writing */
//    if((font[((str[i]-32)*5)+4])==0 || str[i]==32)/* Check whether character is SPACE or character last column is zero */
//    {
//      for(column=0;column<5;column++)
//      {
//        GLCD128x64_write_Data(font[str[i]-32][column]);			/* If yes then then print character */
//        if((Y_address+1)%64==0)					/* check whether it gets overflowed  from either half of side */
//        {
//         // CS1 = ~CS1; CS2 = ~CS2;
//          GLCD128x64_CS2(Enable);
//         GLCD128x64_CS1(Disable);/* If yes then change segment controller to display on other half of display */ 
//          GLCD128x64_write_command((Page+Page_inc));			/* Execute command for page change */
//          Page_inc=Page_inc+0.5;				/* Increment Page No. by half */
//        }
//        Y_address++;						/* Increment Y_address count per column */
//      }
//    }
//    else																				/* If character is not SPACE or character last column is not zero */
//    {
//      for(column=0;column<5;column++)
//      {
//        GLCD128x64_write_Data(font[str[i]-32][column]); 		/* Then continue to print hat char */
//        if((Y_address+1)%64==0)				/* check whether it gets overflowed  from either half of side */
//        {
//          //CS1 = ~CS1; CS2 = ~CS2;			/* If yes then change segment controller to display on other half of display */ 
//          GLCD128x64_CS2(Enable);
//         GLCD128x64_CS1(Disable);
//          GLCD128x64_write_command((Page+Page_inc));		/* Execute command for page change */
//          Page_inc=Page_inc+0.5;			/* Increment Page No. by half */
//        }
//        Y_address++;					/* Increment Y_address count per column */
//      }
//      GLCD128x64_write_Data(0);					/* Add one column of zero to print next character next of zero */
//      Y_address++;					/* Increment Y_address count for last added zero */
//      if((Y_address)%64==0)				/* check whether it gets overflowed  from either half of side */
//      {
//        //CS1 = ~CS1; CS2 = ~CS2;				/* If yes then change segment controller to display on other half of display */ 
//        GLCD128x64_CS2(Enable);
//         GLCD128x64_CS1(Disable);
//        GLCD128x64_write_command((Page+Page_inc));			/* Execute command for page change */
//        Page_inc=Page_inc+0.5;				/* Increment Page No. by half */
//      }
//    }
//  }
//  GLCD128x64_write_command(0x40);					/* Set Y address (column=0) */
//}
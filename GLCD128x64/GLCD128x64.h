/**
******************************************************************************
* @file		lcd16x2.h
* @author	Yazdan Khalili
* @date		6 February 2020
* @note		
******************************************************************************
*/

#ifndef __GLCD128x64_H
#define __GLCD128x64_H

#ifdef __cplusplus
extern "C" {
#endif
  
  /** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
  
  
  
  
  /** Port and pin definition for 4-bit mode ---------------------------------- */
  // LCD control lines (must be on the same port)
#define GLCD128x64_RCC_GPIO_CONTROL	        RCC_APB2Periph_GPIOC
#define GLCD128x64_GPIO_CONTROL	                GPIOC
#define GLCD128x64_GPIO_RS			GPIOC
#define GLCD128x64_GPIO_RW			GPIOC
#define GLCD128x64_GPIO_EN			GPIOC
#define GLCD128x64_GPIO_CS			GPIOC
  // LCD data lines (must be on the same port)
#define GLCD128x64_RCC_GPIO_DATA		RCC_APB2Periph_GPIOC
#define GLCD128x64_GPIO_DATA	                GPIOC
#define GLCD128x64_GPIO_D4			GPIOC
#define GLCD128x64_GPIO_D5			GPIOC
#define GLCD128x64_GPIO_D6			GPIOC
#define GLCD128x64_GPIO_D7			GPIOC
#define GLCD128x64_GPIO_D3			GPIOC
#define GLCD128x64_GPIO_D2			GPIOC
#define GLCD128x64_GPIO_D1			GPIOC
#define GLCD128x64_GPIO_D0			GPIOC
  // Pin definition
#define GLCD128x64_PIN_RS	                GPIO_Pin_4
#define GLCD128x64_PIN_RW	                GPIO_Pin_5
#define GLCD128x64_PIN_EN	                GPIO_Pin_6
#define GLCD128x64_PIN_CS1			GPIO_Pin_11
#define GLCD128x64_PIN_CS2			GPIO_Pin_12
#define GLCD128x64_PIN_D4	                GPIO_Pin_3		// 4-bit mode LSB
#define GLCD128x64_PIN_D5	                GPIO_Pin_2
#define GLCD128x64_PIN_D6	                GPIO_Pin_1
#define GLCD128x64_PIN_D7	                GPIO_Pin_0		// 4-bit mode MSB
#define GLCD128x64_PIN_D3	                GPIO_Pin_7		// 4-bit mode LSB
#define GLCD128x64_PIN_D2	                GPIO_Pin_8
#define GLCD128x64_PIN_D1	                GPIO_Pin_9
#define GLCD128x64_PIN_D0	                GPIO_Pin_10
  
  
  
  /** Delay value ------------------------------------------------------------- */
  // Delay power on
#define GLCD128x64_DELAY_POWER_ON   	100000
  // Delay after initialize 8-bit
#define GLCD128x64_DELAY_INIT      	5000
  // Delay after initialize 8-bit repeated
#define GLCD128x64_DELAY_INIT_REP    	64
  // Delay after set 4-bit mode
#define GLCD128x64_DELAY_INIT_4BIT   	64
  // Delay until address counter updated after busy flag is cleared
#define GLCD128x64_DELAY_BUSY_FLAG    	4
  // Enable pulse width high level
#define GLCD128x64_DELAY_ENABLE_PULSE_High	6
#define GLCD128x64_DELAY_ENABLE_PULSE_Low	2
  
  
  /* Display cursor on off instructions */
#define GLCD128x64_DISPLAY_ON	 	0x3F
#define GLCD128x64_DISPLAY_OFF	 	0x3E	// Initialization setting
  
  
  
  
  void GLCD128x64_Initialize(void );
  void GLCD128x64_Clr_All(void);
  void GLCD128x64_write_command(uint8_t cmd);
  void GLCD128x64_GOTO_XY(uint8_t x , uint8_t y );
  void GLCD_String(char page_no, char *str);
#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/
char font[][5] =														/* Define char fonts array in column by row in flash memory */ 
{
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20 (Space) 
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ?
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ->
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f <-
};
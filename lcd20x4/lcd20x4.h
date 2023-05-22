/**
******************************************************************************
* @file		lcd20x4.h
* @author	Yazdan Khalili
* @date		6 February 2020
* @note		
******************************************************************************
*/

#ifndef __LCD20X4_H
#define __LCD20X4_H

#ifdef __cplusplus
extern "C" {
#endif

    /** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

    /** Port and pin definition for 4-bit mode ---------------------------------- */
    // LCD control lines (must be on the same port)
#define LCD20X4_RCC_GPIO_CONTROL	RCC_APB2Periph_GPIOC
#define LCD20X4_GPIO_CONTROL	        GPIOC
#define LCD20X4_GPIO_RS			GPIOC
#define LCD20X4_GPIO_RW			GPIOC
#define LCD20X4_GPIO_EN			GPIOC
    // LCD data lines (must be on the same port)
#define LCD20X4_RCC_GPIO_DATA		RCC_APB2Periph_GPIOC
#define LCD20X4_GPIO_DATA	        GPIOC
#define LCD20X4_GPIO_D4			GPIOC
#define LCD20X4_GPIO_D5			GPIOC
#define LCD20X4_GPIO_D6			GPIOC
#define LCD20X4_GPIO_D7			GPIOC
    // Pin definition
#define LCD20X4_PIN_RS	                GPIO_Pin_4
#define LCD20X4_PIN_RW	                GPIO_Pin_5
#define LCD20X4_PIN_EN	                GPIO_Pin_6
#define LCD20X4_PIN_D4	                GPIO_Pin_3		// 4-bit mode LSB
#define LCD20X4_PIN_D5	                GPIO_Pin_2
#define LCD20X4_PIN_D6	                GPIO_Pin_1
#define LCD20X4_PIN_D7	                GPIO_Pin_0		// 4-bit mode MSB

    /** Display size ------------------------------------------------------------ */
    // Number of visible lines of the display (1 or 2 or 3 or 4)
#define LCD20X4_LINES		         4
    // Visible characters per line of the display
#define LCD20X4_DISP_LENGTH		20
    // DDRAM address of first char of line 1
#define LCD20X4_START_LINE_1	        0x00
    // DDRAM address of first char of line 2
#define LCD20X4_START_LINE_2	        0x40
    // DDRAM address of first char of line 3
#define LCD20X4_START_LINE_3	        0x14
    // DDRAM address of first char of line 4
#define LCD20X4_START_LINE_4	        0x54

    /** Delay value ------------------------------------------------------------- */
    // Delay power on
#define LCD20X4_DELAY_POWER_ON   	16000
    // Delay after initialize 8-bit
#define LCD20X4_DELAY_INIT      	5000
    // Delay after initialize 8-bit repeated
#define LCD20X4_DELAY_INIT_REP    	64
    // Delay after set 4-bit mode
#define LCD20X4_DELAY_INIT_4BIT   	64
    // Delay until address counter updated after busy flag is cleared
#define LCD20X4_DELAY_BUSY_FLAG    	4
    // Enable pulse width high level
#define LCD20X4_DELAY_ENABLE_PULSE	2

    /** Instructions bit location ----------------------------------------------- */
#define LCD20X4_CLEAR_DISPLAY		0x01
#define LCD20X4_CURSOR_HOME		0x02
#define LCD20X4_CHARACTER_ENTRY_MODE	0x04
#define LCD20X4_DISPLAY_CURSOR_ON_OFF	0x08
#define LCD20X4_DISPLAY_CURSOR_SHIFT 	0x10
#define LCD20X4_FUNCTION_SET		0x20
#define LCD20X4_SET_CGRAM_ADDRESS	0x40
#define LCD20X4_SET_DDRAM_ADDRESS	0x80
    /* Character entry mode instructions */
#define LCD20X4_INCREMENT		0x02	// Initialization setting
#define LCD20X4_DECREMENT		0x00
#define LCD20X4_DISPLAY_SHIFT_ON	0x01
#define LCD20X4_DISPLAY_SHIFT_OFF	0x00	// Initialization setting
    /* Display cursor on off instructions */
#define LCD20X4_DISPLAY_ON	 	0x04
#define LCD20X4_DISPLAY_OFF	 	0x00	// Initialization setting
#define LCD20X4_CURSOR_UNDERLINE_ON	0x02
#define LCD20X4_CURSOR_UNDERLINE_OFF	0x00	// Initialization setting
#define LCD20X4_CURSOR_BLINK_ON	        0x01
#define LCD20X4_CURSOR_BLINK_OFF	0x00	// Initialization setting
    /* Display cursor shift instructions */
#define LCD20X4_DISPLAY_SHIFT		0x08
#define LCD20X4_CURSOR_MOVE		0x00
#define LCD20X4_RIGHT_SHIFT		0x04
#define LCD20X4_LEFT_SHIFT		0x00
    /* Function set instructions */
#define LCD20X4_8BIT_INTERFACE	        0x10	// Initialization setting
#define LCD20X4_4BIT_INTERFACE	        0x00
#define LCD20X4_2LINE_MODE		0x08
#define LCD20X4_1LINE_MODE		0x00	// Initialization setting
#define LCD20X4_5X10DOT_FORMAT	        0x04
#define LCD20X4_5X7DOT_FORMAT		0x00	// Initialization setting
    /* Busy flag bit location */
#define LCD20X4_BUSY_FLAG		0x80

    /** LCD display and cursor attributes --------------------------------------- */
#define LCD20X4_DISPLAY_OFF_CURSOR_OFF_BLINK_OFF	(LCD20X4_DISPLAY_OFF | \
    LCD20X4_CURSOR_UNDERLINE_OFF | LCD20X4_CURSOR_BLINK_OFF)
#define LCD20X4_DISPLAY_ON_CURSOR_OFF_BLINK_OFF		(LCD20X4_DISPLAY_ON | \
    LCD20X4_CURSOR_UNDERLINE_OFF | LCD20X4_CURSOR_BLINK_OFF)					
#define LCD20X4_DISPLAY_ON_CURSOR_OFF_BLINK_ON		(LCD20X4_DISPLAY_ON | \
    LCD20X4_CURSOR_UNDERLINE_OFF | LCD20X4_CURSOR_BLINK_ON)
#define LCD20X4_DISPLAY_ON_CURSOR_ON_BLINK_OFF		(LCD20X4_DISPLAY_ON | \
    LCD20X4_CURSOR_UNDERLINE_ON | LCD20X4_CURSOR_BLINK_OFF)
#define LCD20X4_DISPLAY_ON_CURSOR_ON_BLINK_ON		(LCD20X4_DISPLAY_ON | \
    LCD20X4_CURSOR_UNDERLINE_ON | LCD20X4_CURSOR_BLINK_ON)

    /** Public function prototypes ---------------------------------------------- */
    void lcd20x4_init(uint8_t disp_attr);
    void lcd20x4_write_command(uint8_t cmd);
    void lcd20x4_write_data(uint8_t data);
    void lcd20x4_clrscr(void);
    void lcd20x4_home(void);
    void lcd20x4_gotoxy(uint8_t x, uint8_t y);
    uint8_t lcd20x4_getxy(void);
    void lcd20x4_entry_inc(void);
    void lcd20x4_entry_dec(void);
    void lcd20x4_entry_inc_shift(void);
    void lcd20x4_entry_dec_shift(void);
    void lcd20x4_display_on(void);
    void lcd20x4_display_off(void);
    void lcd20x4_cursor_on(void);
    void lcd20x4_cursor_off(void);
    void lcd20x4_blink_on(void);
    void lcd20x4_blink_off(void);
    void lcd20x4_display_shift_left(void);
    void lcd20x4_display_shift_right(void);
    void lcd20x4_cursor_shift_left(void);
    void lcd20x4_cursor_shift_right(void);
    void lcd20x4_putc(const char c);
    void lcd20x4_puts(const char* s);
    void lcd20x4_create_custom_char(uint8_t location, uint8_t* data_bytes);
    void lcd20x4_put_custom_char(uint8_t x, uint8_t y, uint8_t location);

#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/

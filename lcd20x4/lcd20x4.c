/**
******************************************************************************
* @file		lcd20x4.c
* @author	Yazdan Khalili
* @date		6 February 2020
* @note		
******************************************************************************
*/

/** Includes ---------------------------------------------------------------- */
#include "lcd20x4.h"
#include "delay.h"	

/** Private function prototypes --------------------------------------------- */
static void lcd20x4_toggle_e(void);
static void lcd20x4_write(uint8_t data, uint8_t rs);
static uint8_t lcd20x4_read(uint8_t rs);
static uint8_t lcd20x4_wait_busy(void);
static void lcd20x4_new_line(uint8_t pos);

static uint8_t display_cursor_on_off_control;


/** Public functions -------------------------------------------------------- */
/**
******************************************************************************
* @brief	Initialize the LCD 16x2 with 4-bit I/O mode.
* @param	Display, cursor underline, and cursor blink settings. See
* 				LCD display and cursor attributes define in lcd20x4.h file.
* @retval	None
******************************************************************************
*/
void lcd20x4_init(uint8_t disp_attr)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    // GPIO clock for control and data lines
    RCC_APB2PeriphClockCmd(LCD20X4_RCC_GPIO_CONTROL, ENABLE);
    RCC_APB2PeriphClockCmd(LCD20X4_RCC_GPIO_DATA, ENABLE);
    
    // Configure I/O for control lines as output
    GPIO_InitStruct.GPIO_Pin = LCD20X4_PIN_RS | LCD20X4_PIN_RW |
        LCD20X4_PIN_EN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LCD20X4_GPIO_CONTROL, &GPIO_InitStruct);
    
    // Configure I/O for data lines as output
    GPIO_InitStruct.GPIO_Pin = LCD20X4_PIN_D4 | LCD20X4_PIN_D5 |
        LCD20X4_PIN_D6 | LCD20X4_PIN_D7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LCD20X4_GPIO_DATA, &GPIO_InitStruct);
    
    // Delay power on
    Delay_Us(LCD20X4_DELAY_POWER_ON);
    
    // Initialize 8-bit mode first
    LCD20X4_GPIO_D5->BSRR = LCD20X4_PIN_D5;	// Function set
    LCD20X4_GPIO_D4->BSRR = LCD20X4_PIN_D4;	// 8-bit mode
    lcd20x4_toggle_e();
    // Delay, busy flag can't be checked here
    Delay_Us(LCD20X4_DELAY_INIT);
    
    // Repeat last command
    lcd20x4_toggle_e();
    // Delay, busy flag can't be checked here
    Delay_Us(LCD20X4_DELAY_INIT_REP);
    
    // Repeat last command for third time
    lcd20x4_toggle_e();
    // Delay, busy flag can't be checked here
    Delay_Us(LCD20X4_DELAY_INIT_REP);
    
    // Initialize 4-bit mode
    LCD20X4_GPIO_D5->BSRR = LCD20X4_PIN_D5;	// Function set
    LCD20X4_GPIO_D4->BRR = LCD20X4_PIN_D4;	// 4-bit mode
    lcd20x4_toggle_e();
    Delay_Us(LCD20X4_DELAY_INIT_4BIT);
    
    /* From now the LCD only accepts 4 bit I/O */
    
    // 4-bit interface, 2 lines, 5x7 dot format font
    lcd20x4_write_command(LCD20X4_FUNCTION_SET | LCD20X4_4BIT_INTERFACE |
                          LCD20X4_2LINE_MODE | LCD20X4_5X7DOT_FORMAT);
    // Display off
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF | LCD20X4_DISPLAY_OFF);
    // Clear screen
    lcd20x4_clrscr();
    // Entry mode
    lcd20x4_entry_inc();
    // Display cursor on off
    display_cursor_on_off_control = disp_attr;
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);
}

/**
******************************************************************************
* @brief	Write a command to the LCD.
* @param	The LCD instructions set.
* @retval	None
******************************************************************************
*/
void lcd20x4_write_command(uint8_t cmd)
{
    lcd20x4_wait_busy();
    lcd20x4_write(cmd, 0);
}

/**
******************************************************************************
* @brief	Write a data byte to the LCD.
* @param	Data which want to written to the LCD.
* @retval	None
******************************************************************************
*/
void lcd20x4_write_data(uint8_t data)
{
    lcd20x4_wait_busy();
    lcd20x4_write(data, 1);
}

/**
******************************************************************************
* @brief	Clear the LCD display and return cursor to home position.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_clrscr()
{
    lcd20x4_write_command(LCD20X4_CLEAR_DISPLAY);
}

/**
******************************************************************************
* @brief	Return cursor to home position.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_home()
{
    lcd20x4_write_command(LCD20X4_CURSOR_HOME);	
}

/**
******************************************************************************
* @brief	Set LCD cursor to specific position.
* @param	LCD column (x)
* @param	LCD row (y)
* @retval	None
******************************************************************************
*/
void lcd20x4_gotoxy(uint8_t x, uint8_t y)
{
#if LCD20X4_LINES == 1
    lcd16X2_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                          (LCD20X4_START_LINE_1 + x));
#elif LCD20X4_LINES == 2
    if (y == 0)
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_1 + x));
    else
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_2 + x));
    
#elif LCD20X4_LINES == 3
    if (y == 0)
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_1 + x));
    else if (y == 1)
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_2 + x));
    else
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_3 + x));
    
#elif LCD20X4_LINES == 4
    if (y == 0)
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_1 + x));
    else if (y == 1)
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_2 + x));
    else if (y == 2)
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_3 + x));
    else
        lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS |
                              (LCD20X4_START_LINE_4 + x));
#endif
}

/**
******************************************************************************
* @brief	Get LCD cursor/ DDRAM address.
* @param	None
* @retval	LCD cursor/ DDRAM address.
******************************************************************************
*/
uint8_t lcd20x4_getxy()
{
    return lcd20x4_wait_busy();
}

/**
******************************************************************************
* @brief	Set LCD entry mode: increment cursor.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_entry_inc()
{
    lcd20x4_write_command(LCD20X4_CHARACTER_ENTRY_MODE | LCD20X4_INCREMENT |
                          LCD20X4_DISPLAY_SHIFT_OFF);
}

/**
******************************************************************************
* @brief	Set LCD entry mode: decrement cursor.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_entry_dec()
{
    lcd20x4_write_command(LCD20X4_CHARACTER_ENTRY_MODE | LCD20X4_DECREMENT |
                          LCD20X4_DISPLAY_SHIFT_OFF);
}

/**
******************************************************************************
* @brief	Set LCD entry mode: increment cursor and shift character to left.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_entry_inc_shift()
{
    lcd20x4_write_command(LCD20X4_CHARACTER_ENTRY_MODE | LCD20X4_INCREMENT |
                          LCD20X4_DISPLAY_SHIFT_ON);
}

/**
******************************************************************************
* @brief	Set LCD entry mode: decrement cursor and shift character to right.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_entry_dec_shift()
{
    lcd20x4_write_command(LCD20X4_CHARACTER_ENTRY_MODE | LCD20X4_DECREMENT |
                          LCD20X4_DISPLAY_SHIFT_ON);
}

/**
******************************************************************************
* @brief	Turn on display (can see character(s) on display).
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_display_on()
{
    display_cursor_on_off_control |= LCD20X4_DISPLAY_ON;
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);	
}

/**
******************************************************************************
* @brief	Turn off display (blank/ can't see character(s) on display).
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_display_off()
{
    display_cursor_on_off_control &= ~LCD20X4_DISPLAY_ON;
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);	
}

/**
******************************************************************************
* @brief	Turn on underline cursor.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_cursor_on()
{
    display_cursor_on_off_control |= LCD20X4_CURSOR_UNDERLINE_ON; 	
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);
}

/**
******************************************************************************
* @brief	Turn off underline cursor.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_cursor_off()
{
    display_cursor_on_off_control &= ~LCD20X4_CURSOR_UNDERLINE_ON;
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);
}

/**
******************************************************************************
* @brief	Turn on blinking cursor.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_blink_on()
{
    display_cursor_on_off_control |= LCD20X4_CURSOR_BLINK_ON;
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);
}

/**
******************************************************************************
* @brief	Turn off blinking cursor.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_blink_off()
{
    display_cursor_on_off_control &= ~LCD20X4_CURSOR_BLINK_ON;
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_ON_OFF |
                          display_cursor_on_off_control);
}

/**
******************************************************************************
* @brief	Shift the LCD display to the left.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_display_shift_left()
{
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_SHIFT |
                          LCD20X4_DISPLAY_SHIFT | LCD20X4_LEFT_SHIFT);
}

/**
******************************************************************************
* @brief	Shift the LCD display to the right.
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_display_shift_right()
{
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_SHIFT |
                          LCD20X4_DISPLAY_SHIFT | LCD20X4_RIGHT_SHIFT);
}

/**
******************************************************************************
* @brief	Shift the LCD cursor to the left (DDRAM address incremented).
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_cursor_shift_left()
{
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_SHIFT |
                          LCD20X4_DISPLAY_CURSOR_SHIFT | LCD20X4_LEFT_SHIFT);
}

/**
******************************************************************************
* @brief	Shift the LCD cursor to the right (DDRAM address decremented).
* @param	None
* @retval	None
******************************************************************************
*/
void lcd20x4_cursor_shift_right()
{
    lcd20x4_write_command(LCD20X4_DISPLAY_CURSOR_SHIFT |
                          LCD20X4_DISPLAY_CURSOR_SHIFT | LCD20X4_RIGHT_SHIFT);
}

/**
******************************************************************************
* @brief	Put a character on the LCD display.
* @param	Character that want to be displayed.
* @retval	None
******************************************************************************
*/
void lcd20x4_putc(const char c)
{
    uint8_t pos = lcd20x4_getxy();
    
    if (c == '\n')
    {
        lcd20x4_new_line(pos);
    }
    else
    {
#if LCD20X4_LINES == 1
        if (pos == (LCD20X4_START_LINE_1 + LCD20X4_DISP_LENGTH))             //In a row, if you reach the end of the row, you will go to next row
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_1, 0);
#elif LCD20X4_LINES == 2
        if (pos == (LCD20X4_START_LINE_1 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_2, 0);
        else if (pos == (LCD20X4_START_LINE_2 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_1, 0);
        
#elif LCD20X4_LINES == 3
        if (pos == (LCD20X4_START_LINE_1 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_2, 0);
        else if (pos == (LCD20X4_START_LINE_2 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_3, 0);
        else if (pos == (LCD20X4_START_LINE_3 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_1, 0);

#elif LCD20X4_LINES == 4
        if (pos == (LCD20X4_START_LINE_1 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_2, 0);
        else if (pos == (LCD20X4_START_LINE_2 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_3, 0); 
        else if (pos == (LCD20X4_START_LINE_3 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_4, 0);
        else if (pos == (LCD20X4_START_LINE_4 + LCD20X4_DISP_LENGTH))
            lcd20x4_write(LCD20X4_SET_DDRAM_ADDRESS |
                          LCD20X4_START_LINE_1, 0);
#endif
        
        lcd20x4_write_data(c);
    }
}

/**
******************************************************************************
* @brief	Put string on the LCD display.
* @param	String that want to be displayed.
* @retval	None
******************************************************************************
*/
void lcd20x4_puts(const char* s)
{
    while (*s) 
    {
        lcd20x4_putc(*s++);
    }
}

/**
******************************************************************************
* @brief	Create a custom character on CGRAM location.
* @param	CGRAM location (0-7).
* @param	Custom character pattern (8 bytes).
* @retval	None
******************************************************************************
*/
void lcd20x4_create_custom_char(uint8_t location, uint8_t* data_bytes)
{
    int i;
    
    // We only have 8 locations 0-7 for custom chars
    location &= 0x07;
    
    // Set CGRAM address
    lcd20x4_write_command(LCD20X4_SET_CGRAM_ADDRESS | (location << 3));
    
    // Write 8 bytes custom char pattern
    for (i = 0; i < 8; i++)
    {
        lcd20x4_write_data(data_bytes[i]);
    }
}

/**
******************************************************************************
* @brief	Put a custom character on specific LCD display location.
* @param	LCD column
* @param	LCD row
* @param	Custom character location on CGRAM (0-7).
* @retval	None
******************************************************************************
*/
void lcd20x4_put_custom_char(uint8_t x, uint8_t y, uint8_t location)
{
    lcd20x4_gotoxy(x, y);
    lcd20x4_write_data(location);
}

/** Private functions ------------------------------------------------------- */
/**
******************************************************************************
* @brief	Give enable pulse to LCD EN pin.
* @param	None
* @retval	None
******************************************************************************
*/
static void lcd20x4_toggle_e()
{
    // EN pin = HIGH
    LCD20X4_GPIO_EN->BSRR = LCD20X4_PIN_EN;
    // Pulse length in us
    Delay_Us(LCD20X4_DELAY_ENABLE_PULSE);
    // EN pin = LOW
    LCD20X4_GPIO_EN->BRR = LCD20X4_PIN_EN;
}

/**
******************************************************************************
* @brief	Write instruction or data to LCD.
* @param	Instruction/ data that want to sent to LCD.
* @param	Instruction or data register select. If write instruction, then
*					RS = 0. Otherwise, RS = 1.
* @retval	None
******************************************************************************
*/
static void lcd20x4_write(uint8_t data, uint8_t rs)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    // Write mode (RW = 0)
    LCD20X4_GPIO_RS->BRR = LCD20X4_PIN_RW;
    
    if (rs)
        // Write data (RS = 1)
        LCD20X4_GPIO_RS->BSRR = LCD20X4_PIN_RS;
    else		
        // Write instruction (RS = 0)
        LCD20X4_GPIO_RS->BRR = LCD20X4_PIN_RS;
    
    // Configure all data pins as output
    GPIO_InitStruct.GPIO_Pin = LCD20X4_PIN_D4 | LCD20X4_PIN_D5 |
        LCD20X4_PIN_D6 | LCD20X4_PIN_D7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LCD20X4_GPIO_DATA, &GPIO_InitStruct);
    
    // Output high nibble first
    LCD20X4_GPIO_D7->BRR = LCD20X4_PIN_D7;
    LCD20X4_GPIO_D6->BRR = LCD20X4_PIN_D6;
    LCD20X4_GPIO_D5->BRR = LCD20X4_PIN_D5;
    LCD20X4_GPIO_D4->BRR = LCD20X4_PIN_D4;
    if (data & 0x80) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D7;
    if (data & 0x40) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D6;
    if (data & 0x20) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D5;
    if (data & 0x10) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D4;
    lcd20x4_toggle_e();
    
    // Output low nibble
    LCD20X4_GPIO_D7->BRR = LCD20X4_PIN_D7;
    LCD20X4_GPIO_D6->BRR = LCD20X4_PIN_D6;
    LCD20X4_GPIO_D5->BRR = LCD20X4_PIN_D5;
    LCD20X4_GPIO_D4->BRR = LCD20X4_PIN_D4;
    if (data & 0x08) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D7;
    if (data & 0x04) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D6;
    if (data & 0x02) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D5;
    if (data & 0x01) LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D4;
    lcd20x4_toggle_e();
    
    // All data pins high (inactive)
    LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D7;
    LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D6;
    LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D5;
    LCD20X4_GPIO_D7->BSRR = LCD20X4_PIN_D4;
}

/**
******************************************************************************
* @brief	Read DDRAM address + busy flag or data from LCD.
* @param	DDRAM address + busy flag or data register select.
*					If read DDRAM address + busy flag, then RS = 0. Otherwise, RS = 1.
* @retval	DDRAM address + busy flag or data value.
******************************************************************************
*/
static uint8_t lcd20x4_read(uint8_t rs)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint8_t data = 0;
    
    // Read mode (RW = 1)
    LCD20X4_GPIO_RS->BSRR = LCD20X4_PIN_RW;
    
    if (rs)
        // Read data (RS = 1)
        LCD20X4_GPIO_RS->BSRR = LCD20X4_PIN_RS;
    else
        // Read busy flag and DDRAM address (RS = 0)
        LCD20X4_GPIO_RS->BRR = LCD20X4_PIN_RS;
    
    // Configure all data pins as input
    GPIO_InitStruct.GPIO_Pin = LCD20X4_PIN_D4 | LCD20X4_PIN_D5 |
        LCD20X4_PIN_D6 | LCD20X4_PIN_D7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LCD20X4_GPIO_DATA, &GPIO_InitStruct);
    
    // EN pin = HIGH
    LCD20X4_GPIO_EN->BSRR = LCD20X4_PIN_EN;
    // Pulse length in us
    Delay_Us(LCD20X4_DELAY_ENABLE_PULSE);
    /* Read high nibble first */
    if (LCD20X4_GPIO_D4->IDR & LCD20X4_PIN_D4) data |= 0x10;	
    if (LCD20X4_GPIO_D5->IDR & LCD20X4_PIN_D5) data |= 0x20;
    if (LCD20X4_GPIO_D6->IDR & LCD20X4_PIN_D6) data |= 0x40;
    if (LCD20X4_GPIO_D7->IDR & LCD20X4_PIN_D7) data |= 0x80;
    // EN pin = LOW
    LCD20X4_GPIO_EN->BRR = LCD20X4_PIN_EN;
    
    // EN pin LOW delay
    Delay_Us(LCD20X4_DELAY_ENABLE_PULSE);
    
    // EN pin = HIGH
    LCD20X4_GPIO_EN->BSRR = LCD20X4_PIN_EN;
    // Pulse length in us
    Delay_Us(LCD20X4_DELAY_ENABLE_PULSE);
    /* Read low nibble */
    if (LCD20X4_GPIO_D4->IDR & LCD20X4_PIN_D4) data |= 0x01;
    if (LCD20X4_GPIO_D5->IDR & LCD20X4_PIN_D5) data |= 0x02;
    if (LCD20X4_GPIO_D6->IDR & LCD20X4_PIN_D6) data |= 0x04;
    if (LCD20X4_GPIO_D7->IDR & LCD20X4_PIN_D7) data |= 0x08;
    // EN pin = LOW
    LCD20X4_GPIO_EN->BRR = LCD20X4_PIN_EN;
    
    return data;
}

/**
******************************************************************************
* @brief	Wait for LCD until finish it's job.
* @param	None
* @retval	DDRAM address + busy flag value.
******************************************************************************
*/
static uint8_t lcd20x4_wait_busy()
{
    // Wait until busy flag is cleared
    while (lcd20x4_read(0) & (LCD20X4_BUSY_FLAG));
    
    // Delay needed for address counter is updated after busy flag is cleared
    Delay_Us(LCD20X4_DELAY_BUSY_FLAG);
    
    // Read and return address counter
    return lcd20x4_read(0);
}

/**
******************************************************************************
* @brief	Give new line character
* @param	Current cursor/ DDRAM address position.
* @retval	None
******************************************************************************
*/
static void lcd20x4_new_line(uint8_t pos)
{
    uint8_t address_counter;
    
#if LCD20X4_LINES == 1
    address_counter = 0;
#elif LCD20X4_LINES == 2
    if (pos < LCD20X4_START_LINE_2)
        address_counter = LCD20X4_START_LINE_2;
    else
        address_counter = LCD20X4_START_LINE_1;
#elif LCD20X4_LINES == 3
    if (pos >= LCD20X4_START_LINE_2)
        address_counter = LCD20X4_START_LINE_3;
    else if (pos >= LCD20X4_START_LINE_3)
        address_counter = LCD20X4_START_LINE_1;
    else
        address_counter = LCD20X4_START_LINE_2;
#elif LCD20X4_LINES == 4
    if (pos >= LCD20X4_START_LINE_4)
        address_counter = LCD20X4_START_LINE_1;
    else if (pos >= LCD20X4_START_LINE_2)
        address_counter = LCD20X4_START_LINE_3;
    else if (pos >= LCD20X4_START_LINE_3)
        address_counter = LCD20X4_START_LINE_4;
    else
        address_counter = LCD20X4_START_LINE_2;
#endif
    
    lcd20x4_write_command(LCD20X4_SET_DDRAM_ADDRESS | address_counter);
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
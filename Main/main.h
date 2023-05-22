
typedef enum system_clock
{
    MAIN_CLOCK_ERROR = 0,
    MAIN_CLOCK_HSI = 1,
    MAIN_CLOCK_HSE = 2,
    MAIN_CLOCK_PLL = 3
}MAIN_SYSTEM_CLOCK;

extern volatile uint32_t HCLK_Freq;
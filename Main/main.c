#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "main.h"
#include <stdio.h>
#include "IO.h"
#include "CAN_Receive.h"
#include "CAN_Transmit.h"   
#include "lcd16x2.h"


volatile uint32_t HCLK_Freq;
MAIN_SYSTEM_CLOCK system_clock_initialize(void);

void main(void)
{   
    system_clock_initialize();
    Timers_Initialize();
    Keypad_Initalize();
    GLCD128x64_Initialize();
    CAN_Initialize();
     
     
    
    while (1)
    {  
        
         Diagnostic_LCD();
     
    }
}


MAIN_SYSTEM_CLOCK system_clock_initialize(void)
{
    FlagStatus Status;
    uint16_t counter;
    MAIN_SYSTEM_CLOCK system_clock;
    
    RCC_DeInit();                                           /* RCC system reset */
    RCC_HSEConfig(RCC_HSE_ON);                              /* Enable HSE */
    counter = 1000;
    Status = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
    while((Status == RESET) && (counter != 0))              /* Wait till HSE is ready */
    {
        Status = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
        counter--;
    }
    
    RCC_PCLK2Config(RCC_HCLK_Div1);                         /* APB2CLK = HCLK=8MHz*2 = 16 MHz @bug Is it a correct comment? */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   /* Enable Prefetch Buffer */
    
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                        /* AHBCLK = SYSCLK= 16MHz @bug Is it a correct comment? */
    RCC_PCLK1Config(RCC_HCLK_Div1);
    FLASH_SetLatency(FLASH_Latency_2);                      /* Flash 2 wait state */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_2);     /* PLLCLK = 8MHz*2 =16 MHz */
    RCC_PLLCmd(ENABLE);                                     /* Enable PLL */
    
    counter = 1000;
    Status = RCC_GetFlagStatus(RCC_FLAG_PLLRDY);
    while((Status == RESET) && (counter != 0))              /* Wait till PLL is ready */
    {
        Status = RCC_GetFlagStatus(RCC_FLAG_PLLRDY);
        counter--;
    }
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);              /* Select PLL as system clock source */
    
    if(RCC_GetSYSCLKSource() == 0x00)                       /* Wait till PLL is used as system clock source */
    {
        system_clock = MAIN_CLOCK_HSI;
    }
    else if(RCC_GetSYSCLKSource() == 0x04)
    {
        system_clock = MAIN_CLOCK_HSE;
    }
    else if(RCC_GetSYSCLKSource() == 0x08)
    {
        system_clock = MAIN_CLOCK_PLL;
    }
    else
    {
        system_clock = MAIN_CLOCK_ERROR;
    }
    
    /* refer to " UM0427 User manual " page 282 and 283 September 2008 Rev 6 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    /* TIM1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    //    /* TIM2 clock enable */
    //    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    /* Enable USART1 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    /* Enable USART2 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    return system_clock;
}
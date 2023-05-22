
#include "stm32f10x.h"




typedef enum
{
    False   =   0,
    True    =   1
}BOOLEAN_T;
 typedef enum
 {
   Push = 0 ,
   Releas = 1
 }Push_State;

  typedef enum
 {Debouns_True = 0 ,
 Debouns_False = 1
 }Debunce_T;



void        system_tick_configure(void);
void        Timers_Initialize(void);
uint64_t    Time_Now(void);
void        SysTick_Handler(void);
BOOLEAN_T        Time_Delay_Occure(uint64_t start_time,uint64_t delay);
void System_Tick_Handler(void);
void delay_ms (uint32_t ms_delay);
void DelayMs(uint32_t us);
void Delay_Us(uint32_t us_delay);

void led(void);


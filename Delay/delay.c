/*!
@file timers.c
@brief
@par

@author
- Yazdan Khalili
@todo
- to be completed
@version
- 0.00
@date
-
*/


/*
--------------------------------------------------------------------------------
Include files
--------------------------------------------------------------------------------
*/
#include    "stm32f10x.h"
#include    "delay.h"



/*
--------------------------------------------------------------------------------
Local defines
--------------------------------------------------------------------------------
*/
/*!
@addtogroup TIMERS_MODULE
@{
*/
#define SYSTEM_TICK                     1000        /*!< microSeconds : set tick value for systick timer */

#define USEC_DELAY_NUMERATOR            4           /*!< Numerator for calculating microseconds delay, estimated by try and error in STM32F101VBT6 Y microcontroller. */
#define USEC_DELAY_DENUMERATOR          3           /*!< Denumerator for calculating microseconds delay, estimated by try and error in STM32F101VBT6 Y microcontroller. */
/*! @} @} */


/*
--------------------------------------------------------------------------------
Local constants
--------------------------------------------------------------------------------
*/
const   uint32_t   System_Clock_Frequency  =   16000000;   /*! system clock frequency set to 16MHz @bug Why it is not a defined? */
//uint32_t ledStep  ;
//uint64_t Master_LED_Start ;

/*
--------------------------------------------------------------------------------
Local data types
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
Local tables
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
Local global Variables (including public, protected and private variables)
--------------------------------------------------------------------------------
*/
uint64_t General_Time_Counter;     /*!< This variable shall indicate 1ms. If system clock changes, its resolution shall change to remain its unit constant (1msec) */


/*
--------------------------------------------------------------------------------
Local function prototypes
--------------------------------------------------------------------------------
*/
/*!
@addtogroup TIMERS_MODULE
@{
*/
void system_tick_configure(void);
/*! @} @} */


/*
--------------------------------------------------------------------------------
Local configuration errors
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
Global functions
--------------------------------------------------------------------------------
*/
/*!
@fn void Timers_Initialize(void)
@brief This function configures system tick as well as initializes general timer variables
@return void
*/
void Timers_Initialize(void)
{
    system_tick_configure();
    General_Time_Counter    =   0;
}

/*!
@fn void System_Tick_Handler(void)
@brief System Tick Clock Interrupt Request Handler.
@note Every point of General_Time_Counter is as valuable as SYSTEM_TICK. So It shall be acccessed only by Time_Now and Time_Delay_Occure functions to garantee its meaningful usage.
@return void
*/
void System_Tick_Handler(void)
{
    if(General_Time_Counter == 0xFFFFFFFF)
    {
        General_Time_Counter    =   0;
    }
    else
    {
        General_Time_Counter++ ;
    }
    
}

/*!
@fn uint32_t Time_Now(void)
@brief  This function returns local time value in mico second
@return uint32_t
*/
uint64_t Time_Now(void)
{
    uint64_t time_temp;
    
    time_temp   =   General_Time_Counter;
    time_temp   =   time_temp * SYSTEM_TICK;
    
    return time_temp;
}

/*!
@fn uint32_t Time_Now_In_Interrupt(void)
@brief  This function returns local time value in mico second. It is similar with Time_Now function; but it is used in interrupt service routines to prevent resource conflict.
@return uint32_t
*/
uint32_t Time_Now_In_Interrupt(void)
{
    uint32_t   time_temp;
    
    time_temp   =   General_Time_Counter;
    time_temp   =   time_temp * SYSTEM_TICK;
    
    return time_temp;
}

/*!
@fn BOOLEAN_T Time_Delay_Occure(uint32_t start_time,uint32_t delay)
@brief This function compares start time of given event with given time.
@param start_time Specifies the event whose start time must be compared.
@param delay Specifies the time that must be compared whit the start time of given event. Its unit is micro second.
@return A boolean variable That is True if delay has been occured and FALSE if we shall wait again.
*/
BOOLEAN_T Time_Delay_Occure(uint64_t start_time,uint64_t delay)
{
    uint64_t end_time;
    uint64_t current_time;
    
    end_time        =   start_time + ((uint64_t)delay);
    current_time    = ((uint64_t)General_Time_Counter) * ((uint64_t)SYSTEM_TICK);
    if(( current_time > end_time                             ) ||
       ((current_time < start_time) && (current_time > delay))   )
    {
        return True;
    }
    else
    {
        return FALSE;
    }
}

/*!
@fn BOOLEAN_T Time_Delay_Occure_In_Interrupt(uint32_t start_time,uint32_t delay)
@brief This function compares start time of given event with given time. It is similar with Time_Now function; but it is used in interrupt service routines to prevent resource conflict.
@param start_time Specifies the event whose start time must be compared.
@param delay Specifies the time that must be compared whit the start time of given event. Its unit is micro second.
@return A boolean variable That is True if delay has been occured and FALSE if we shall wait again.
*/
BOOLEAN_T Time_Delay_Occure_In_Interrupt(uint32_t start_time,uint32_t delay)
{
    uint32_t   end_time;
    uint32_t   current_time;
    end_time        =   start_time + delay;
    current_time    =   General_Time_Counter * SYSTEM_TICK;
    if((current_time > end_time) ||
       ((current_time < start_time) && (current_time > delay)))
    {
        return True;
    }
    else
    {
        return FALSE;
    }
}

/*!
@fn void Delay_Us(uint32_t us_delay)
@brief producing a micro second delay by using a while loop
@exception This function uses 2 defines estimated by try and error in STM32F101VBT6 Y to have a micro second delay. They might be different in other versions of microcontroller.
@param us_delay number of micro seconds desired for delay
@return void
*/
void Delay_Us(uint32_t us_delay)
{
    volatile uint32_t temp_delay;
    
    //    if(us_delay > 1000)
    //    {
    //        temp_delay  =   Time_Now();
    //        while(Time_Delay_Occure(temp_delay,us_delay) != True);
    //    }
    //    else
    {
        temp_delay = us_delay * USEC_DELAY_NUMERATOR;
        while (temp_delay > USEC_DELAY_DENUMERATOR)
        {
            temp_delay -=   USEC_DELAY_DENUMERATOR;
        }
    }
}

/*!
@fn void Delay_Second(uint8_t second_delay)
@brief producing a second delay by using a while loop
@param second_delay number of seconds desired for delay
@return void
*/
void Delay_Second(uint8_t second_delay)
{
    volatile uint32_t  temp_delay;
    temp_delay  =   Time_Now();
    while(Time_Delay_Occure(temp_delay, (second_delay*1000000)) != True);
}

/*
--------------------------------------------------------------------------------
Local functions
--------------------------------------------------------------------------------
*/

/*!
@fn void system_tick_configure(void)
@brief Configures the System tick clock based on System_Clock_Frequency and desired SYSTEM_TICK
@return void
*/
void system_tick_configure(void)
{
    uint32_t tick_temp;
    tick_temp   =   SYSTEM_TICK * (System_Clock_Frequency / 1000000);
    SysTick_Config(tick_temp);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    
    
}

//void led(void)
//{
//  
//  switch(ledStep)
//  {
//  case 0 :
//    GPIO_ResetBits  (GPIOA,GPIO_Pin_11);
//    Master_LED_Start = Time_Now() ;
//    ledStep = 1 ;
//    break ;
//  case 1 :
//    if ( Time_Delay_Occure(Master_LED_Start , 100000) == True )
//    {
//      GPIO_SetBits  (GPIOA,GPIO_Pin_11);
//      Master_LED_Start =  Time_Now() ;
//      ledStep = 2 ;
//    }
//    break ;
//  case 2 :
//    
//    if ( Time_Delay_Occure(Master_LED_Start , 200000) == True )
//    {
//      ledStep = 0 ;
//    }
//    break ;
//    
//  }
//}

////////////////////////////////////////////////////////////////////////////



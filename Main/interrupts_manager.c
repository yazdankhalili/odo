/*!
@file interrupts_manager.c
@brief

--------------------------------------------------------------------------------
Include files
--------------------------------------------------------------------------------
*/
#include    "stm32f10x.h"
#include    "interrupts_manager.h"
#include    "delay.h"
#include    "SMT172.h"
#include    "CAN_Receive.h"
#include    "K_Line.h"
#include    "SPI_L9826.h"


/*
--------------------------------------------------------------------------------
Local defines
--------------------------------------------------------------------------------
*/




/*
--------------------------------------------------------------------------------
Local constants
--------------------------------------------------------------------------------
*/


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




/*
--------------------------------------------------------------------------------
Local function prototypes
--------------------------------------------------------------------------------
*/
void nested_vector_interrupts_configure(void);


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
void Interrupts_Initialize(void)
{
    nested_vector_interrupts_configure();
}


/*
--------------------------------------------------------------------------------
Local functions
--------------------------------------------------------------------------------
*/
/*!
@fn void nested_vector_interrupts_configure(void)
@brief The STM32F103xx performance line embeds a nested vectored interrupt controller able to handle up to 43 maskable interrupt channels (not including the 16 interrupt lines of Cortex.-M3) and 16 priority levels. @n This hardware block provides flexible interrupt management features with minimal interrupt latency.
@return void
*/
void nested_vector_interrupts_configure(void)
{
    
}
/*
--------------------------------------------------------------------------------
Cortex-M3 Processor Exceptions Handlers
--------------------------------------------------------------------------------
*/

/*!
@fn void NMI_Handler(void)
@brief  Non maskable interrupt. The RCC Clock Security System (CSS) is linked to the NMI vector. @n Its priority is fixed to -2.
@return void
*/
void NMI_Handler(void)
{
    
}

/*!
@fn void HardFault_Handler(void)
@brief  This function handles Hard Fault (All class of fault) exception. @n Its priority is fixed to -1.
@return void
*/
void HardFault_Handler(void)
{
    /*! @note Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
@fn void MemManage_Handler(void)
@brief  This function handles Memory Manage exception. @n Its priority is settable with default value 0.
@return void
*/
void MemManage_Handler(void)
{
    /*! @note Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
@fn void BusFault_Handler(void)
@brief  This function handles Bus Fault (Pre-fetch fault AND memory access fault) exception. @n Its priority is settable with default value 1.
@return void
*/
void BusFault_Handler(void)
{
    /*! @note Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
@fn void UsageFault_Handler(void)
@brief  This function handles Usage Fault (Undefined instruction or illegal state) exception. @n Its priority is settable with default value 2.
@return void
*/
void UsageFault_Handler(void)
{
    /*! @note Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
@fn void SVC_Handler(void)
@brief  This function handles SVCall (System service call via SWI instruction) exception. @n Its priority is settable with default value 3.
@return void
*/
void SVC_Handler(void)
{
    
}

/*!
@fn void DebugMon_Handler(void)
@brief  This function handles Debug Monitor exception. @n Its priority is settable with default value 4.
@return void
*/
void DebugMon_Handler(void)
{
    
}

/*!
@fn void PendSV_Handler(void)
@brief  This function handles PendSV_Handler (Pendable request for system service) exception. @n Its priority is settable with default value 5.
@return void
*/
void PendSV_Handler(void)
{
    
}

/*!
@fn void SysTick_Handler(void)
@brief  SysTick timer has a maskable system interrupt generation when the counter reaches 0. @n Its priority is settable with default value 6.
@return void
*/
void SysTick_Handler(void)
{
    System_Tick_Handler(); 
}


/*
--------------------------------------------------------------------------------
STM32F10x Peripherals Interrupt Handlers
Add here the Interrupt Handler for the used peripheral(s) (PPP), for the available peripheral interrupt handler's name please refer to the startup file (startup_stm32f10x_xx.s).
--------------------------------------------------------------------------------
*/

/*!
@fn void WWDG_IRQHandler(void)
@brief Window Watchdog interrupt handler @n Its priority is settable with default value 7.
@return void
*/
void WWDG_IRQHandler(void)
{
    
}

/*!
@fn void PVD_IRQHandler(void)
@brief The device features an embedded programmable voltage detector (PVD) that monitors the VDD/VDDA power supply and compares it to the VPVD threshold. An interrupt can be generated when VDD/VDDA drops below the VPVD threshold and/or when VDD/VDDA is higher than the VPVD threshold. The interrupt service routine can then generate a warning message and/or put the MCU into a safe state. @n Its priority is settable with default value 8.
@return void
*/
void PVD_IRQHandler(void)
{
    
}

/*!
@fn void TAMPER_IRQHandler(void)
@brief Tamper interrupt handler @n Its priority is settable with default value 9.
@return void
*/
void TAMPER_IRQHandler(void)
{
    
}

/*!
@fn void RTC_IRQHandler(void)
@brief RTC global interrupt handler. @n Its priority is settable with default value 10.
@return void
*/
void RTC_IRQHandler(void)
{
    
}
/*!
@fn void FLASH_IRQHandler(void)
@brief In programming and erasing the internal Flash memory, the end of write operation (programming or erasing) can trigger an interrupt. @n Its priority is settable with default value 11.
@return void
*/
void FLASH_IRQHandler(void)
{
    
}

/*!
@fn void RCC_IRQHandler(void)
@brief RCC global interrupt handler @n Its priority is settable with default value 12.
@return void
*/
void RCC_IRQHandler(void)
{
    
}

/*!
@fn void EXTI0_IRQHandler(void)
@brief EXTI Line0 interrupt handler @n Its priority is settable with default value 13.
@return void
*/
void EXTI0_IRQHandler(void)
{
    
    
}

/*!
@fn void EXTI1_IRQHandler(void)
@brief EXTI Line1 interrupt handler @n Its priority is settable with default value 14.
@return void
*/
void EXTI1_IRQHandler(void)
{
    
    
}

/*!
@fn void EXTI2_IRQHandler(void)
@brief EXTI Line2 interrupt handler @n Its priority is settable with default value 15.
@return void
*/
void EXTI2_IRQHandler(void)
{
    
}

/*!
@fn void EXTI3_IRQHandler(void)
@brief EXTI Line3 interrupt handler @n Its priority is settable with default value 16.
@return void
*/
void EXTI3_IRQHandler(void)
{
    
}

/*!
@fn void EXTI4_IRQHandler(void)
@brief EXTI Line4 interrupt handler @n Its priority is settable with default value 17.
@return void
*/
void EXTI4_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel1_IRQHandler(void)
@brief DMA1 Channel1 global interrupt handler @n Its priority is settable with default value 18.
@return void
*/
void DMA1_Channel1_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel2_IRQHandler(void)
@brief DMA1 Channel2 global interrupt handler @n Its priority is settable with default value 19.
@return void
*/
void DMA1_Channel2_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel3_IRQHandler(void)
@brief DMA1 Channel3 global interrupt handler @n Its priority is settable with default value 20.
@return void
*/
void DMA1_Channel3_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel4_IRQHandler(void)
@brief DMA1 Channel4 global interrupt handler @n Its priority is settable with default value 21.
@return void
*/
void DMA1_Channel4_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel5_IRQHandler(void)
@brief DMA1 Channel5 global interrupt handler @n Its priority is settable with default value 22.
@return void
*/
void DMA1_Channel5_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel6_IRQHandler(void)
@brief DMA1 Channel6 global interrupt handler @n Its priority is settable with default value 23.
@return void
*/
void DMA1_Channel6_IRQHandler(void)
{
    
}

/*!
@fn void DMA1_Channel7_IRQHandler(void)
@brief DMA1 Channel7 global interrupt handler @n Its priority is settable with default value 24.
@return void
*/
void DMA1_Channel7_IRQHandler(void)
{
    
}

/*!
@fn void ADC1_2_IRQHandler(void)
@brief ADC1 and ADC2 global interrupt handler @n An analog watchdog feature allows very precise monitoring of the converted voltage of one, some or all selected channels. An interrupt is generated when the converted voltage is outside the programmed thresholds. @n Its priority is settable with default value 25.
@return void
*/
void ADC1_2_IRQHandler(void)
{
    
}

/*!
@fn void USB_HP_CAN1_TX_IRQHandler(void)
@brief CAN1 TX interrupts handler @n Its priority is settable with default value 26.
@return void
*/
void USB_HP_CAN1_TX_IRQHandler(void)
{
    
    
}

/*!
@fn void USB_LP_CAN1_RX0_IRQHandler(void)
@brief CAN1 RX0 interrupts handler @n Its priority is settable with default value 27.
@return void
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_LP_CAN1_RX0_Interrupt();
}

/*!
@fn void CAN1_RX1_IRQHandler(void)
@brief CAN1 RX1 interrupt handler @n Its priority is settable with default value 28.
@return void
*/
void CAN1_RX1_IRQHandler(void)
{
    
    
    
}

/*!
@fn void CAN1_SCE_IRQHandler(void)
@brief CAN1 SCE interrupt handler @n Its priority is settable with default value 29.
@return void
*/
void CAN1_SCE_IRQHandler(void)
{
    
}

/*!
@fn void EXTI9_5_IRQHandler(void)
@brief EXTI Line[9:5] interrupts handler @n Its priority is settable with default value 30.
@return void
*/
void EXTI9_5_IRQHandler(void)
{
    
}

/*!
@fn void TIM1_BRK_IRQHandler(void)
@brief TIM1 Break interrupt handler @n Its priority is settable with default value 31.
@return void
*/
void TIM1_BRK_IRQHandler(void)
{
    
}
/*!
@fn void TIM1_UP_IRQHandler(void)
@brief TIM1 Update interrupt handler @n Its priority is settable with default value 32.
@return void
*/
void TIM1_UP_IRQHandler(void)
{
}

/*!
@fn void TIM1_TRG_COM_IRQHandler(void)
@brief TIM1 Trigger and Commutation interrupts handler @n Its priority is settable with default value 33.
@return void
*/
void TIM1_TRG_COM_IRQHandler(void)
{
    
}

/*!
@fn void TIM1_CC_IRQHandler(void)
@brief TIM1 Capture Compare interrupt handler @n Its priority is settable with default value 34.
@return void
*/
void TIM1_CC_IRQHandler(void)
{
    
}
/*!
@fn void TIM2_IRQHandler(void)
@brief TIM2 global interrupt handler @n Its priority is settable with default value 35.
@return void
*/
void TIM2_IRQHandler(void)
{
    
}
/*!
@fn void TIM3_IRQHandler(void)
@brief TIM3 global interrupt handler @n Its priority is settable with default value 36.
@return void
*/
void TIM3_IRQHandler(void)
{
   
}
/*!
@fn void TIM4_IRQHandler(void)
@brief TIM4 global interrupt handler @n Its priority is settable with default value 37.
@return void
*/
void TIM4_IRQHandler(void)
{
    
}

/*!
@fn void I2C1_EV_IRQHandler(void)
@brief I2C1 event interrupt handler @n Its priority is settable with default value 38.
@return void
*/
void I2C1_EV_IRQHandler(void)
{
    
}

/*!
@fn void I2C1_ER_IRQHandler(void)
@brief I2C1 error interrupt handler @n Its priority is settable with default value 39.
@return void
*/
void I2C1_ER_IRQHandler(void)
{
    
}

/*!
@fn void I2C2_EV_IRQHandler(void)
@brief I2C2 event interrupt handler @n Its priority is settable with default value 40.
@return void
*/
void I2C2_EV_IRQHandler(void)
{
    
}
/*!
@fn void I2C2_ER_IRQHandler(void)
@brief I2C2 error interrupt handler @n Its priority is settable with default value 41.
@return void
*/
void I2C2_ER_IRQHandler(void)
{
    
}
/*!
@fn void SPI1_IRQHandler(void)
@brief SPI1 global interrupt handler @n Its priority is settable with default value 42.
@return void
*/
void SPI1_IRQHandler(void)
{

}

/*!
@fn void SPI2_IRQHandler(void)
@brief SPI2 global interrupt handler @n Its priority is settable with default value 43.
@return void
*/
void SPI2_IRQHandler(void)
{
 
}

/*!
@fn void USART1_IRQHandler(void)
@brief  This function handles USART1 global interrupt request. @n Its priority is settable with default value 44.
@note USART1 is HHU Port.
@return void
*/
void USART1_IRQHandler(void)
{
}

/*!
@fn void USART2_IRQHandler(void)
@brief  This function handles USART2 global interrupt request. @n Its priority is settable with default value 45.
@return void
*/
void USART2_IRQHandler(void)
{
    
}

/*!
@fn void USART3_IRQHandler(void)
@brief  This function handles USART3 global interrupt request. @n Its priority is settable with default value 46.
@return void
*/
void USART3_IRQHandler(void)
{
    
}

/*!
@fn void EXTI15_10_IRQHandler(void)
@brief EXTI Line[15:10] interrupts handler @n Its priority is settable with default value 47.
@return void
*/
void EXTI15_10_IRQHandler(void)
{
    
}

/*!
@fn void RTCAlarm_IRQHandler(void)
@brief RTC alarm through EXTI line interrupt handler @n Its priority is settable with default value 48.
@return void
*/
void RTCAlarm_IRQHandler(void)
{
    
}

/*!
@fn void USBWakeUp_IRQHandler(void)
@brief USB On-The-Go FS Wakeup through EXTI line interrupt handler @n Its priority is settable with default value 49.
@return void
*/
void USBWakeUp_IRQHandler(void)
{
    
}

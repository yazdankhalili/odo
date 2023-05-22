#include "delay.h"
#include "interrupts_manager.h"
#include "stm32f10x.h"
#include "CAN_Transmit.h"


CanTxMsg            TxMessage;

uint64_t            Transmit_Data_Frame;


void CAN_Transmit_Message(void)
{
    uint64_t Temp_Reg ;
    TxMessage.ExtId = 0x18DA00FA ;
    TxMessage.RTR = CAN_RTR_DATA ;
    TxMessage.IDE = CAN_ID_EXT ;
    TxMessage.DLC = 8 ;
    Temp_Reg = Transmit_Data_Frame ;
    for ( uint8_t i = 0 ; i < 8 ; i++ )
    {
        Temp_Reg <<= 56 ;
        Temp_Reg >>= 56 ;
        TxMessage.Data[i] = (uint8_t)Temp_Reg ;
        Temp_Reg = Transmit_Data_Frame >> (i+1)*8 ;   
    }
    CAN_Transmit(CAN1, &TxMessage);
} 





void  SignalDataArangment(const uint8_t data[] ) 
{
  Transmit_Data_Frame = 0x0000000000000000 ;
    uint64_t datatemp  ;
    for (uint8_t i =0 ; i <8 ; i++)
    {
        datatemp = data[i] ;
        Transmit_Data_Frame |= ( datatemp << (i*8)) ; 
    }
}




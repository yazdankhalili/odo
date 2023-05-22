
#include "stm32f10x.h"





#define     ECU_Recieve0             UDS_Message.ECU[0]
#define     ECU_Recieve1             UDS_Message.ECU[1]
#define     ECU_Recieve2             UDS_Message.ECU[2]
#define     ECU_Recieve3             UDS_Message.ECU[3]
#define     ECU_Recieve4             UDS_Message.ECU[4]
#define     ECU_Recieve5             UDS_Message.ECU[5]
#define     ECU_Recieve6             UDS_Message.ECU[6]
#define     ECU_Recieve7             UDS_Message.ECU[7]




typedef struct
{
    uint64_t        ECU[7];
}UDS_Message_Struct;

extern UDS_Message_Struct  UDS_Message;



void Diagnostic_LCD(void);
void    CAN_Initialize(void);

void    CAN_Receive_Message_Decode(void);
uint16_t Hex2Decimal(uint64_t hex_value) ;
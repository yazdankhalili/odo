#include "delay.h"
#include "interrupts_manager.h"
#include "stm32f10x.h"
#include "CAN_Receive.h"
#include "IO.h"


#define GPIO_Pin_CAN_TX           GPIO_Pin_12 
#define GPIO_Pin_CAN_RX           GPIO_Pin_11
#define GPIO_Port_CAN_RX          GPIOA

const uint8_t       ECU_StartBit[9]              = {0,8,16,24,32,40,48,56,64}; 


uint8_t   StartCommunicationRequestData[8]       = {0x01,0x81,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
uint8_t   ReadEcuIdentificationData[8]           = {0x02,0x1A,0x94,0xAA,0xAA,0xAA,0xAA,0xAA};
uint8_t   FlowControlFrameData[8]                = {0x30,0xFF,0x10,0xAA,0xAA,0xAA,0xAA,0xAA};
uint8_t   SequrityAccessRequestData[8]           = {0x02,0x27,0x09,0xAA,0xAA,0xAA,0xAA,0xAA};
uint8_t   SequrityAccessFlowControlData[8]       = {0x30,0xFF,0x10,0xAA,0xAA,0xAA,0xAA,0xAA};
uint8_t   DiagnosticSessionControlData[8]        = {0x03,0x10,0x81,0x0A,0xAA,0xAA,0xAA,0xAA};
uint8_t   TesterPresentData[8]                   = {0x02,0x3E,0x01,0x00,0x00,0x00,0x00,0x00};
uint8_t   Frame[5][8]                             = {{0x10,0x1F,0x2C,0xF0,0x80,0x01,0x00,0x0C},
                                                   {0x21,0x02,0x01,0x00,0x0D,0x02,0x01,0x00},
                                                   {0x22,0x18,0x02,0x01,0x00,0x1C,0x02,0x01},
                                                   {0x23,0x00,0x0D,0x02,0x01,0x00,0x1F,0x02},
                                                   {0x24,0x01,0x00,0x25,0x02,0xAA,0xAA,0xAA}};
uint8_t    SF_Read_Data[8]                       = {0x02,0x21,0xF0,0xAA,0xAA,0xAA,0xAA,0xAA};
uint8_t    FCF_Read_Data[8]                      = {0x30,0xFF,0x10,0xAA,0xAA,0xAA,0xAA,0xAA};

uint64_t     TP_Time_Tag;
uint64_t     TP_Transmit_Time_Tag ;
uint64_t     End_Of_TP_Time_Tag;
uint16_t     Parameters_Hex_Value ;
uint16_t     Parameters_Ph_Value[6] ;
UDS_Message_Struct  UDS_Message;
char Fill_Arrow[]                               ={0x08, 0x1C, 0x3E, 0x7F, 0x7F};
char Empty_Arrow[]                              ={0x00, 0x00, 0x00, 0x00, 0x00};
CanRxMsg            RxMessage;



void USB_LP_CAN1_RX0_Interrupt(void);
void data_transmit_receive(void);
void FF_CF(void);
void USB_LP_CAN1_RX0_Interrupt(void)
{
    CAN_Receive( CAN1, CAN_FIFO0 , &RxMessage); 
}

void CAN_Receive_Message_Decode(void)
{
    uint8_t   i = 0;
    uint64_t  Receive_Data_Frame;
    uint8_t   ECU_Data_Legth[( sizeof(ECU_StartBit)/sizeof(ECU_StartBit[0])-1 )];
    
    
    uint64_t  Temp_Reg; 
    Receive_Data_Frame = (uint16_t) RxMessage.Data[7];     
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[6];
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[5]; 
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[4]; 
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[3];
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[2]; 
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[1];
    Receive_Data_Frame <<= 8;
    Receive_Data_Frame |= (uint16_t) RxMessage.Data[0];
    
    Temp_Reg = Receive_Data_Frame ;
    
    switch( RxMessage.ExtId )
    {
    case 0x18DAFA00 :
        for( i=0 ; i < ( sizeof(ECU_StartBit)/sizeof(ECU_StartBit[0])-1 ) ; i++ )
        {   
            ECU_Data_Legth[i] = ECU_StartBit[i+1]- ECU_StartBit[i] ; 
            Temp_Reg <<= ( 64 - ECU_Data_Legth[i] );
            Temp_Reg >>= ( 64 - ECU_Data_Legth[i] );
            UDS_Message.ECU[i] = Temp_Reg ;
            Temp_Reg = Receive_Data_Frame >> ( ECU_StartBit[i+1] );  
        }
        break ; 
        
    }   
}






/////////////////////////////////////////////////////////////////////////////////////////


typedef enum
{
    Mode_Select,
    Diagnostic_Mode,
    Programing_Mode,
    Parameters,
    IO,
    Fault,
    Back_Step
}LCD_Step_ENUM;

typedef enum
{
    No,
    Data_Transmit_Step,
    Data_Posetive_Response_Step,
    Read_ECU_Identification,
    Diagnostic_Mode_Select,
}Data_LCD_Step_ENUM;
typedef enum
{
    TP_First_Transmit ,
    TP_Posetive_Response ,
    TP_Periodic_Transmit
}TP_Transmit_Step_ENUM;

typedef enum
{
    None,
    Diagnostic_Mode_Data_Transmit,
    Diagnostic_Mode_Data_Posetive_Response,
    Read_ECU_Identification_Transmit,
    Read_ECU_Identification_Posetive_Response,
    Read_ECU_Identification_Flow_Control,
    Read_ECU_Identification_Flow_Control_Response,
    Read_ECU_Identification_Flow_Control_Response1,
    Sequrity_Access,
    Sequrity_Access_Response,
    Sequrity_Access_Flow_Control,
    Sequrity_Access_Flow_Control_Response,
    Diagnostic_Mode_Strat,
    Diagnostic_Mode_Response,
    TP,
    Parameters_Data,
    Fault_Data,
    IO_Data
}Data_Transmit_Receive_Step_ENUM;

typedef enum
{
    Parameters_Groupe1,
    Parameters_Groupe2,
    Parameters_Groupe3,
    Parameters_Groupe4
}Parameters_ENUM;
typedef enum
{
    Parameters_Data_Groupe1,
    Parameters_Data_Groupe2,
    Parameters_Data_Groupe3,
    Parameters_Data_Groupe4
}Parameters_Data_Groupe_ENUM;
typedef enum
{
    FF,
    FCF,
    CF1,
    CF2,
    CF3,
    CF4,
    SF,
    SF_Read_Data_Local_ID,
    FF_Read_Data_Local_ID,
    FCF_Read_Data_Local_ID,
    CF1_Read_Data_Local_ID,
    CF2_Read_Data_Local_ID,
    Repetitive_Transmit_Time_Delay
}Parameters_Multi_Frame_ENUM;

Parameters_Multi_Frame_ENUM     Parameters_Multi_Frame ;

Parameters_Data_Groupe_ENUM     Parameters_Data_Groupe;
Parameters_ENUM                 Parameters_Groupe;


Data_Transmit_Receive_Step_ENUM      Data_Transmit_Receive_Step;
LCD_Step_ENUM                        LCD_Step ;
Data_LCD_Step_ENUM                   Data_LCD_Step;
TP_Transmit_Step_ENUM                TP_Transmit_Step ;

uint8_t                              Back_Step_Number ;
uint64_t                             Recieve_Time_Tag;
uint8_t                              Direction_Number ; 
uint8_t                              Frame_To_Send[8];
char                                 Parameters_Buffer[5];
uint64_t                             Multi_Frame_Time_Tag;
uint64_t                             Parameter_Show_Time_Tag;
void Diagnostic_LCD(void)
{
    KeyPad_Number();
    switch(LCD_Step)
    {
    case Mode_Select :   //Back_Step_Number = 0
        GLCD128x64_GOTO_XY(0,2);
        GLCD128x64_putc("Diagnostic Mode");
        GLCD128x64_GOTO_XY(0,4);
        GLCD128x64_putc("Progrming Mode");
        data_transmit_receive(); //3E 7E 
        
        if ( Key_Number == 0 )
        {
            Direction_Number++; 
            if ( Direction_Number >= 1 )
            {
                Direction_Number = 1;
            }
        }
        else if ( Key_Number == 5 )
        {
            Direction_Number--;
        }
        
        if ( Direction_Number == 0 )
        {
            GLCD128x64_GOTO_XY(120,4);
            GLCD128x64_Clr_Line(127,120,4) ;
            GLCD128x64_GOTO_XY(120,2);
            Costum_Char(Fill_Arrow); 
        }
        if ( Direction_Number == 1 )
        {
            GLCD128x64_GOTO_XY(120,2);
            GLCD128x64_Clr_Line(127,120,2);
            GLCD128x64_GOTO_XY(120,4);
            Costum_Char(Fill_Arrow);
            Direction_Number = 1 ;
        }
        if(( Direction_Number == 0 ) && (Key_Number == 8 ) )
        {
            LCD_Step = Diagnostic_Mode ;
            Back_Step_Number++ ;
            GLCD128x64_Clr_All();
            Direction_Number == 0;
        }
        else if( ( Direction_Number == 1 ) && Key_Number == 8  )
        {
            LCD_Step = Programing_Mode;
            Back_Step_Number++ ;
            GLCD128x64_Clr_All();
            Direction_Number == 0;
        }
        break;
    case Diagnostic_Mode :
        GLCD128x64_GOTO_XY(0,2);
        GLCD128x64_putc("Parameter");
        GLCD128x64_GOTO_XY(0,4);
        GLCD128x64_putc("IO Control");
        GLCD128x64_GOTO_XY(0,6);
        GLCD128x64_putc("Faults");
        data_transmit_receive(); //3E 7E   from Back Step
        //Back_Step_Number = 1
        switch(Data_LCD_Step)
        {
        case No :
            data_transmit_receive();
            Data_Transmit_Receive_Step = Diagnostic_Mode_Data_Transmit ;
            break;
        case Data_Transmit_Step :
            data_transmit_receive(); 
            break;
        case Diagnostic_Mode_Select :
            
            if ( Key_Number == 0 )
            {
                Direction_Number++; 
                if ( Direction_Number >= 2 )
                {
                    Direction_Number = 2;
                }
            }
            else if ( Key_Number == 5 )
            {
                Direction_Number--;
            }
            
            if ( Direction_Number == 0 )
            {
                GLCD128x64_GOTO_XY(120,4);
                GLCD128x64_Clr_Line(127,120,4);
                GLCD128x64_GOTO_XY(120,6);
                GLCD128x64_Clr_Line(127,120,6);
                GLCD128x64_GOTO_XY(120,2);
                Costum_Char(Fill_Arrow); 
            }
            if ( Direction_Number == 1 )
            {
                GLCD128x64_GOTO_XY(120,2);
                GLCD128x64_Clr_Line(127,120,2); 
                GLCD128x64_GOTO_XY(120,6);
                GLCD128x64_Clr_Line(127,120,6);
                GLCD128x64_GOTO_XY(120,4);
                Costum_Char(Fill_Arrow);
            }
            if ( Direction_Number == 2 )
            {
                GLCD128x64_GOTO_XY(120,4);
                GLCD128x64_Clr_Line(127,120,4) ;
                GLCD128x64_GOTO_XY(120,2);
                GLCD128x64_Clr_Line(127,120,2);
                GLCD128x64_GOTO_XY(120,6);
                Costum_Char(Fill_Arrow);
                Direction_Number = 2 ;
            }
            if(( Direction_Number == 0 ) && (Key_Number == 8 ) )
            {
                LCD_Step = Parameters;
                Back_Step_Number++ ;
                GLCD128x64_Clr_All();
            }
            else if(( Direction_Number == 1 ) && (Key_Number == 8 ) )
            {
                LCD_Step = IO;
                Back_Step_Number++ ;
                GLCD128x64_Clr_All();
            }
            else if(( Direction_Number == 2 ) && (Key_Number == 8 ) )
            {
                LCD_Step = Fault;
                Back_Step_Number++ ;
                GLCD128x64_Clr_All();
            }
            else if( Key_Number == 10 )
            {
                LCD_Step = Back_Step ;
                
            }
            
            else
            {
                Data_Transmit_Receive_Step = TP; 
                
                data_transmit_receive();// for first time run ,if T > 2355 ms , transmit the 3E 
            }
            break ;
        }
        break;
    case Parameters :
        switch(Parameters_Groupe)
        {
        case Parameters_Groupe1:
            GLCD128x64_GOTO_XY(0 , 0 );
            GLCD128x64_putc ("Parameters             Unit         (1-4)");
            //Frame[0][7]= 0x0C;Frame[2][4]= 0x0D;Frame[3][1]= 0x18;Frame[3][5]= 0x1C;Frame[4][2]= 0x1D;Frame[4][6]= 0x1F;Frame[5][2]= 0x25;
            Data_Transmit_Receive_Step = Parameters_Data ;
             Parameters_Data_Groupe =Parameters_Data_Groupe1 ;
             data_transmit_receive();
            if (Time_Delay_Occure(Parameter_Show_Time_Tag, 700000) == True)
            {
            GLCD128x64_GOTO_XY(0 , 1 );
            GLCD128x64_putc("Eng.Speed");
            GLCD128x64_GOTO_XY(80 , 1 );
            GLCD128x64_putc("rpm");
            sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[0]*0.1);
            GLCD128x64_GOTO_XY(100 , 1 );
            GLCD128x64_putc(Parameters_Buffer);
            
            GLCD128x64_GOTO_XY(0 , 2 );
            GLCD128x64_putc("Ignition");
            GLCD128x64_GOTO_XY(80 , 2 );
            GLCD128x64_putc("ms");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[1]);
            GLCD128x64_GOTO_XY(100 , 2 );
            GLCD128x64_putc(Parameters_Buffer);

            
            GLCD128x64_GOTO_XY(0 , 3 );
            GLCD128x64_putc("Purge valve pos");
            GLCD128x64_GOTO_XY(80 , 3 );
            GLCD128x64_putc("%");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[2]);
            GLCD128x64_GOTO_XY(100 , 3 );
            GLCD128x64_putc(Parameters_Buffer);
            
             GLCD128x64_GOTO_XY(0 , 4 );
            GLCD128x64_putc("Actefftorque");
            GLCD128x64_GOTO_XY(80 , 4 );
            GLCD128x64_putc("N*M");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[3]);
            GLCD128x64_GOTO_XY(100 , 4 );
            GLCD128x64_putc(Parameters_Buffer);
            
             GLCD128x64_GOTO_XY(0 , 5 );
            GLCD128x64_putc("T15 Voltge");
            GLCD128x64_GOTO_XY(80 , 5 );
            GLCD128x64_putc("V");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[4]);
            GLCD128x64_GOTO_XY(100 , 5 );
            GLCD128x64_putc(Parameters_Buffer);
            
             GLCD128x64_GOTO_XY(0 , 6 );
            GLCD128x64_putc("ECU temp");
            GLCD128x64_GOTO_XY(80 , 6 );
            GLCD128x64_putc("c");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[5]);
            GLCD128x64_GOTO_XY(100 , 6 );
            
            GLCD128x64_GOTO_XY(0 , 7 );
            GLCD128x64_putc("Vehicle Speed");
            GLCD128x64_GOTO_XY(80 , 7 );
            GLCD128x64_putc("km/h");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[6]);
            GLCD128x64_GOTO_XY(100 , 7 );
           
            Parameter_Show_Time_Tag = Time_Now();
             }
            
            if ( Key_Number == 9 )
            {
                Parameters_Groupe = Parameters_Groupe2 ;
                Parameters_Multi_Frame = FF ;
                Parameter_Show_Time_Tag = 0 ;
                GLCD128x64_Clr_All();
            }
            else if( Key_Number == 10 )
            {
                LCD_Step = Back_Step ;
                 Parameters_Groupe = Parameters_Groupe1 ;
                
            }
            
            break;
        case Parameters_Groupe2:
            GLCD128x64_GOTO_XY(0 , 0 );
            GLCD128x64_putc ("Parameters             Unit        (2-4)");   
           // Frame[0][7]= 0x27;Frame[2][4]= 0x028;Frame[3][1]= 0x2B;Frame[3][5]= 0x32;Frame[4][2]= 0x33;Frame[4][6]= 0x34;Frame[5][2]= 0x53;
            Data_Transmit_Receive_Step = Parameters_Data ;
            Parameters_Data_Groupe =Parameters_Data_Groupe2 ;
            data_transmit_receive();
            if (Time_Delay_Occure(Parameter_Show_Time_Tag, 700000) == True)
            {
                
            GLCD128x64_GOTO_XY(0 , 1 );
            GLCD128x64_putc("trqEffdes");
            GLCD128x64_GOTO_XY(80 , 1 );
            GLCD128x64_putc("N*M");
            sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[0]*0.1);
            GLCD128x64_GOTO_XY(100 , 1 );
            GLCD128x64_putc(Parameters_Buffer);
            
            GLCD128x64_GOTO_XY(0 , 2 );
            GLCD128x64_putc("trqFrc");
            GLCD128x64_GOTO_XY(80 , 2 );
            GLCD128x64_putc("N*M");
            sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[1]*0.1);
            GLCD128x64_GOTO_XY(100 , 2 );
            GLCD128x64_putc(Parameters_Buffer);

            
            GLCD128x64_GOTO_XY(0 , 3 );
            GLCD128x64_putc("trqEffcc");
            GLCD128x64_GOTO_XY(80 , 3 );
            GLCD128x64_putc("N*M");
            sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[2]*0.1);
            GLCD128x64_GOTO_XY(100 , 3 );
            GLCD128x64_putc(Parameters_Buffer);
            
             GLCD128x64_GOTO_XY(0 , 4 );
            GLCD128x64_putc("posPedDC");
            GLCD128x64_GOTO_XY(80 , 4 );
            GLCD128x64_putc("%");
            sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[3]*0.1);
            GLCD128x64_GOTO_XY(100 , 4 );
            GLCD128x64_putc(Parameters_Buffer);
            
             GLCD128x64_GOTO_XY(0 , 5 );
            GLCD128x64_putc("rpmIdleNVm");
            GLCD128x64_GOTO_XY(80 , 5 );
            GLCD128x64_putc("rpm");
            sprintf(Parameters_Buffer,"%d",Parameters_Ph_Value[4]);
            GLCD128x64_GOTO_XY(100 , 5 );
            GLCD128x64_putc(Parameters_Buffer);
            
             GLCD128x64_GOTO_XY(0 , 6 );
            GLCD128x64_putc("posPedSens");
            GLCD128x64_GOTO_XY(80 , 6 );
            GLCD128x64_putc("%");
            sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[5]*0.1);
            GLCD128x64_GOTO_XY(100 , 6 );
            
            GLCD128x64_GOTO_XY(0 , 7 );
            GLCD128x64_putc("trqEffpTO");
            GLCD128x64_GOTO_XY(80 , 7 );
            GLCD128x64_putc("N*M");
             sprintf(Parameters_Buffer,"%1.1f",Parameters_Ph_Value[6]*0.1);   
                
            
                Parameter_Show_Time_Tag = Time_Now();
            }
            
            if ( Key_Number == 9 )
            {
                Parameters_Groupe = Parameters_Groupe3 ;
                Parameters_Multi_Frame = FF ;
                Parameter_Show_Time_Tag = 0 ;
                GLCD128x64_Clr_All();
            }
            else if ( Key_Number == 7 )
            {
                Parameters_Groupe = Parameters_Groupe1 ;
                Parameters_Multi_Frame = FF ;
                Parameter_Show_Time_Tag = 0 ;
                GLCD128x64_Clr_All();
            }
            else if( Key_Number == 10 )
            {
                LCD_Step = Back_Step ;
                Parameters_Groupe = Parameters_Groupe1 ;
                
            }
            break;
        case Parameters_Groupe3:
            GLCD128x64_GOTO_XY(0 , 0 );
            GLCD128x64_putc ("Parameters             Unit       (3-4)");
            Data_Transmit_Receive_Step = Parameters_Data ;
            Parameters_Data_Groupe =Parameters_Data_Groupe3 ;
            data_transmit_receive();
            
            if ( Key_Number == 9 )
            {
                Parameters_Groupe = Parameters_Groupe4 ;
                Parameters_Multi_Frame = FF ;
                Parameter_Show_Time_Tag = 0 ;
                GLCD128x64_Clr_All();
            }
            else if ( Key_Number == 7 )
            {
                Parameters_Groupe = Parameters_Groupe2 ;
                Parameters_Multi_Frame = FF ;
                Parameter_Show_Time_Tag = 0 ;
                GLCD128x64_Clr_All();
            }
            else if( Key_Number == 10 )
            {
                LCD_Step = Back_Step ;
                Parameters_Groupe = Parameters_Groupe1 ;
                
            }
            break;
        case Parameters_Groupe4:
            GLCD128x64_GOTO_XY(0 , 0 );
            GLCD128x64_putc ("Parameters             Unit       (4-4)");
            Data_Transmit_Receive_Step = Parameters_Data ;
            Parameters_Data_Groupe =Parameters_Data_Groupe4 ;
            data_transmit_receive();
            if ( Key_Number == 7 )
            {
                Parameters_Groupe = Parameters_Groupe3 ;
                Parameters_Multi_Frame = FF ;
                Parameter_Show_Time_Tag = 0 ;
                GLCD128x64_Clr_All();
            }
            else if( Key_Number == 10 )
            {
                LCD_Step = Back_Step ;
                Parameters_Groupe = Parameters_Groupe1 ;
                
            }
            break; 
        }
        break;
        
        if( Key_Number == 10 )
        {
            LCD_Step = Back_Step ;
        }
        break;
    case Fault :
        
        GLCD128x64_GOTO_XY(0 , 4 );
        GLCD128x64_putc ("This Section In Progress");
        //Back_Step_Number = 2
        if( Key_Number == 10 )
        {
            LCD_Step = Back_Step ;
        }
        break;
    case IO : 
        GLCD128x64_GOTO_XY(0 , 4 );
        GLCD128x64_putc ("This Section In Progress");
        //Back_Step_Number = 2
        if(  Key_Number == 10)
        {
            LCD_Step = Back_Step ;
        }
        
        break;
        
    case Programing_Mode :
        GLCD128x64_GOTO_XY(0 , 4 );
        GLCD128x64_putc ("This Section In Progress");
        if(  Key_Number == 10 )
        {
            LCD_Step = Back_Step ;
        }
        break;
    case Back_Step :
        if ( Back_Step_Number == 1 )
        {
            LCD_Step = Mode_Select ;
            Back_Step_Number-- ;
            TP_Time_Tag = Time_Now(); 
            TP_Transmit_Step = TP_First_Transmit ;
            Direction_Number = 0;
            GLCD128x64_Clr_All();
        }
        else if ( Back_Step_Number == 2 )
        {
            LCD_Step = Diagnostic_Mode ;
            Data_LCD_Step = Diagnostic_Mode_Select ;
            Back_Step_Number-- ;
            TP_Time_Tag = Time_Now(); //time tag for delay for 02 3E transmit
            TP_Transmit_Step = TP_First_Transmit ;
            Direction_Number = 0;
            GLCD128x64_Clr_All();
        }
        break; 
    }
}



void data_transmit_receive(void)
{
    switch(Data_Transmit_Receive_Step)
    {
    case None :
        
        break;
    case Diagnostic_Mode_Data_Transmit:
        SignalDataArangment( StartCommunicationRequestData ) ; //01 81 AA AA AA AA AA AA
        CAN_Transmit_Message();
        Data_Transmit_Receive_Step = Diagnostic_Mode_Data_Posetive_Response ;
        Data_LCD_Step = Data_Transmit_Step ;
        Recieve_Time_Tag = Time_Now();
        break;
    case Diagnostic_Mode_Data_Posetive_Response :
        if (Time_Delay_Occure(Recieve_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode();
            
            Data_Transmit_Receive_Step = Read_ECU_Identification_Transmit ;
            
            Recieve_Time_Tag = Time_Now();
        }
        break;
        
    case Read_ECU_Identification_Transmit :
        if (Time_Delay_Occure(Recieve_Time_Tag, 100000) == True)
        {
            SignalDataArangment( ReadEcuIdentificationData ) ; //02 1A 94 AA AA AA AA AA
            CAN_Transmit_Message();
            Data_Transmit_Receive_Step = Read_ECU_Identification_Posetive_Response;
            Recieve_Time_Tag = Time_Now();
        }
        break;
        
    case Read_ECU_Identification_Posetive_Response :
        if (Time_Delay_Occure(Recieve_Time_Tag, 1000) == True)
        {
            
            CAN_Receive_Message_Decode(); //10 11 5A 94 41 45 43 20
            Data_Transmit_Receive_Step = Read_ECU_Identification_Flow_Control ;
            Recieve_Time_Tag = Time_Now();
        }
    case Read_ECU_Identification_Flow_Control :
        if (Time_Delay_Occure(Recieve_Time_Tag, 100) == True)
        {
            SignalDataArangment( FlowControlFrameData ) ;  //30 FF 10 AA AA AA AA AA
            CAN_Transmit_Message();
            Data_Transmit_Receive_Step = Read_ECU_Identification_Flow_Control_Response ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
        
    case Read_ECU_Identification_Flow_Control_Response :
        if (Time_Delay_Occure(Recieve_Time_Tag, 2000) == True)
        {
            
            CAN_Receive_Message_Decode(); //21 30 33 2E 31 37 2E 30                              
            Data_Transmit_Receive_Step = Read_ECU_Identification_Flow_Control_Response1 ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
    case Read_ECU_Identification_Flow_Control_Response1 :
        if (Time_Delay_Occure(Recieve_Time_Tag, 100) == True)
        {
            
            CAN_Receive_Message_Decode(); // 22 34 31 20 20 37 2E 30    
            Data_Transmit_Receive_Step = Sequrity_Access ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
    case Sequrity_Access :
        if (Time_Delay_Occure(Recieve_Time_Tag, 7000) == True)
        {
            
            SignalDataArangment( SequrityAccessRequestData ) ;  //02 27 09 AA AA AA AA AA
            CAN_Transmit_Message();                             
            Data_Transmit_Receive_Step = Sequrity_Access_Response ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
    case  Sequrity_Access_Response :
        if (Time_Delay_Occure(Recieve_Time_Tag, 1000) == True)
        {
            
            CAN_Receive_Message_Decode(); // 10 0A 67 09 00 00 00 00  
            Data_Transmit_Receive_Step = Sequrity_Access_Flow_Control ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
    case Sequrity_Access_Flow_Control :
        if (Time_Delay_Occure(Recieve_Time_Tag, 100) == True)
        {
            SignalDataArangment( SequrityAccessFlowControlData ) ;  //30 FF 10 AA AA AA AA AA
            CAN_Transmit_Message();                             
            Data_Transmit_Receive_Step = Sequrity_Access_Flow_Control_Response  ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
    case Sequrity_Access_Flow_Control_Response :
        if (Time_Delay_Occure(Recieve_Time_Tag, 100) == True)
        {
            
            CAN_Receive_Message_Decode(); // 21 00 00 00 00 00 00 00 
            Data_Transmit_Receive_Step = Diagnostic_Mode_Strat ;
            Recieve_Time_Tag = Time_Now();
            
        }
        break;
        
        
        
    case Diagnostic_Mode_Strat :
        if (Time_Delay_Occure(Recieve_Time_Tag, 3000) == True)
        {
            SignalDataArangment( DiagnosticSessionControlData ) ;  //03 10 81 0A AA AA AA AA
            CAN_Transmit_Message();                             
            Data_Transmit_Receive_Step = Diagnostic_Mode_Response  ;
            Recieve_Time_Tag = Time_Now();
        }
        break;
        
    case Diagnostic_Mode_Response :
        if (Time_Delay_Occure(Recieve_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode(); // 03 50 81 0A 00 00 00 00
            Data_Transmit_Receive_Step = None ;         
            Data_LCD_Step = Diagnostic_Mode_Select ;    
            TP_Time_Tag = Time_Now(); // for 3E Transmit for first time after SA 
            
        }
        break;
        
    case TP:
        switch (TP_Transmit_Step )
        {
        case TP_First_Transmit :
            if (Time_Delay_Occure(TP_Time_Tag, 2355000) == True) // delay 2355 ms and then transmit 3E
            {
                SignalDataArangment( TesterPresentData ) ; //02 3E 01 00 00 00 00 00
                CAN_Transmit_Message();
                //Data_Transmit_Receive_Step = None ;
                TP_Transmit_Step = TP_Posetive_Response ;
                
                TP_Time_Tag = Time_Now();
            }
            break ;
        case TP_Posetive_Response :
            TP_Transmit_Step = TP_Periodic_Transmit ;
            CAN_Receive_Message_Decode();
            
            break;
        case TP_Periodic_Transmit :
            if (Time_Delay_Occure(TP_Time_Tag, 2255000) == True) //trnsmit the message every 2255 ms 
            {
                SignalDataArangment( TesterPresentData ) ; //02 3E 01 00 00 00 00 00
                CAN_Transmit_Message(); 
                TP_Time_Tag = Time_Now();
                TP_Transmit_Step = TP_Posetive_Response ; /* it is necessary or not?   */
                
            }
            break;
        }
        break;
        
    case Parameters_Data:
        switch( Parameters_Data_Groupe)
        {
        case Parameters_Data_Groupe1 :
            FF_CF();
            break;
        case Parameters_Data_Groupe2 :
            FF_CF();
            break;
        case Parameters_Data_Groupe3 :
            FF_CF();
            break;
        case Parameters_Data_Groupe4 :
            FF_CF();
            break;  
            
            
        }
    case Fault_Data:
        
        break;
        
    case IO_Data:
        
        break;
    }
}


void FF_CF(void)
{
    switch ( Parameters_Multi_Frame )
    {
    case FF :
        for ( uint8_t i = 0 ; i < 8 ; i ++ )
        {
            Frame_To_Send[i] = Frame[0][i];
        }
        SignalDataArangment( Frame_To_Send ) ; //10 1F 2C F0 80 01 00 0C
        CAN_Transmit_Message();
        Parameters_Multi_Frame = FCF;
        Multi_Frame_Time_Tag = Time_Now();
        break;
    case FCF :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode(); //30 FF 00 00 00 00 00 00
            Parameters_Multi_Frame = CF1 ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case CF1 :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 5000) == True)
        {
            for ( uint8_t i = 0 ; i < 8 ; i ++ )
            {
                Frame_To_Send[i] = Frame[1][i];
            }
            SignalDataArangment( Frame_To_Send ) ; //21 02 01 00 0D 02 01 00
            CAN_Transmit_Message();
            Parameters_Multi_Frame = CF2 ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case CF2 :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 5000) == True)
        {
            for ( uint8_t i = 0 ; i < 8 ; i ++ )
            {
                Frame_To_Send[i] = Frame[2][i];
            }
            SignalDataArangment( Frame_To_Send ) ; //22 18 02 01 00 1C 02 01
            CAN_Transmit_Message();
            Parameters_Multi_Frame = CF3 ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case CF3 :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 5000) == True)
        {
            for ( uint8_t i = 0 ; i < 8 ; i ++ )
            {
                Frame_To_Send[i] = Frame[3][i];
            }
            SignalDataArangment( Frame_To_Send ) ; //23 00 1D 02 01 00 1F 02
            CAN_Transmit_Message();
            Parameters_Multi_Frame = CF4 ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case CF4 :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 5000) == True)
        {
            for ( uint8_t i = 0 ; i < 8 ; i ++ )
            {
                Frame_To_Send[i] = Frame[4][i];
            }
            SignalDataArangment( Frame_To_Send ) ; //24 01 00 25 02 AA AA AA
            CAN_Transmit_Message();
            Parameters_Multi_Frame = SF ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case SF :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode(); //02 6C F0 00 00 00 00 00   posetive response 0x6C
            Parameters_Multi_Frame = FF ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break; 
    case SF_Read_Data_Local_ID :
        
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 19000) == True)
        {
            
            SignalDataArangment( SF_Read_Data ) ; //02 21 F0 AA AA AA AA AA
            CAN_Transmit_Message();
            Parameters_Multi_Frame = FF_Read_Data_Local_ID ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case FF_Read_Data_Local_ID   :
        for (uint8_t i = 0 ; i < 6 ; i++)
        {
            Parameters_Ph_Value[i] = 0x0000;
        }
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode(); //10 10 61 F0 00 00 01 F3   posetive response 0x61
            Parameters_Multi_Frame = FCF_Read_Data_Local_ID ;
            Multi_Frame_Time_Tag = Time_Now(); 
            Parameters_Hex_Value |= ECU_Recieve4 ;
            Parameters_Hex_Value <<= 8 ;
            Parameters_Hex_Value |= ECU_Recieve5 ;
            Parameters_Ph_Value[0] = Hex2Decimal(Parameters_Hex_Value);    
            
            Parameters_Hex_Value |= ECU_Recieve6 ;
            Parameters_Hex_Value <<= 8 ;
            Parameters_Hex_Value |= ECU_Recieve7 ;
            Parameters_Ph_Value[1] = Hex2Decimal(Parameters_Hex_Value);
        }
        break;
    case FCF_Read_Data_Local_ID   :
        
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 1000) == True)
        {
            
            SignalDataArangment( FCF_Read_Data ) ; //30 FF 10 AA AA AA AA AA
            CAN_Transmit_Message();
            Parameters_Multi_Frame = CF1_Read_Data_Local_ID ;
            Multi_Frame_Time_Tag = Time_Now(); 
        }
        break;
    case CF1_Read_Data_Local_ID   : 
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode(); //21 00 00 FE AB 00 7C 22
            Parameters_Multi_Frame = CF2_Read_Data_Local_ID ;
            Multi_Frame_Time_Tag = Time_Now();
            Parameters_Hex_Value |= ECU_Recieve1 ;
            Parameters_Hex_Value <<= 8 ;
            Parameters_Hex_Value |= ECU_Recieve2 ;
            Parameters_Ph_Value[2] = Hex2Decimal(Parameters_Hex_Value);
            
            Parameters_Hex_Value |= ECU_Recieve3 ;
            Parameters_Hex_Value <<= 8 ;
            Parameters_Hex_Value |= ECU_Recieve4 ;
            Parameters_Ph_Value[3] = Hex2Decimal(Parameters_Hex_Value);
            
            Parameters_Hex_Value |= ECU_Recieve5 ;
            Parameters_Hex_Value <<= 8 ;
            Parameters_Hex_Value |= ECU_Recieve6 ;
            Parameters_Ph_Value[4] = Hex2Decimal(Parameters_Hex_Value);
            
            Parameters_Hex_Value |= ECU_Recieve5 ;
            Parameters_Hex_Value <<= 8 ;
        }
        break;
    case CF2_Read_Data_Local_ID   :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 1000) == True)
        {
            CAN_Receive_Message_Decode(); //22 20 00 00 AB 00 7C 22
            Parameters_Multi_Frame = Repetitive_Transmit_Time_Delay ;
            Multi_Frame_Time_Tag   = Time_Now(); 
            Parameters_Hex_Value |= ECU_Recieve1 ;
            Parameters_Ph_Value[5] = Hex2Decimal(Parameters_Hex_Value);
            
            Parameters_Hex_Value |= ECU_Recieve2 ;
            Parameters_Hex_Value <<= 8 ;
            Parameters_Hex_Value |= ECU_Recieve3 ;
            Parameters_Ph_Value[6] = Hex2Decimal(Parameters_Hex_Value);
            
        }
        break;
    case Repetitive_Transmit_Time_Delay :
        if (Time_Delay_Occure(Multi_Frame_Time_Tag, 232000) == True)   // 19ms+232ms = 251ms 
        {
            Parameters_Multi_Frame = SF_Read_Data_Local_ID ;
            Multi_Frame_Time_Tag   = Time_Now();
        }
        break;
    }
}


// Function to convert Hexadecimal_Char to decimal 
int hexadecimalToDecimal(char hexVal[]) 
{    
    int len = strlen(hexVal); 
    
    // Initializing base value to 1, i.e 16^0 
    int base = 1; 
    
    int dec_val = 0; 
    
    // Extracting characters as digits from last character 
    for (int i=len-1; i>=0; i--) 
    {    
        // if character lies in '0'-'9', converting  
        // it to integral 0-9 by subtracting 48 from 
        // ASCII value. 
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
            
            // incrementing base by power 
            base = base * 16; 
        } 
        
        // if character lies in 'A'-'F' , converting  
        // it to integral 10 - 15 by subtracting 55  
        // from ASCII value 
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
            
            // incrementing base by power 
            base = base*16; 
        } 
    } 
    
    return dec_val; 
} 



uint16_t Hex2Decimal(uint64_t hex_value)
{
    uint64_t temp = hex_value  ;
    uint64_t decimal = 0 ;
    uint64_t base = 1 ;
    for ( int i = 0 ; i < 64 ;i++ )
    {
        hex_value = temp ;
        hex_value>>= i ;
        hex_value <<= 63 ;
        hex_value >>= 63 ;
        decimal += hex_value * base ;
        base *= 2 ;
    }
    return decimal;     
}

void diag_single_frame(void)
{
    
    
    
}


void CAN_Initialize(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure;
    NVIC_InitTypeDef       NVIC_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    
    /* Configure CAN pin: RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIO_Port_CAN_RX , &GPIO_InitStructure);
    /* Configure CAN pin: TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_Port_CAN_RX, &GPIO_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* CAN register init */
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);
    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1  = CAN_BS1_4tq;
    CAN_InitStructure.CAN_BS2  = CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler = 4;
    CAN_Init(CAN1, &CAN_InitStructure); 
    /* CAN filter init */
    CAN_FilterInitStructure.CAN_FilterNumber=0 ;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow=0x741;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure); 
    
   
    CAN_FilterInitStructure.CAN_FilterNumber=1 ;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow=0x641;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    
  
    
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}




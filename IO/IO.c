#include "delay.h"
#include "interrupts_manager.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "IO.h"

typedef enum 
{
    PIN_SET     =   1,
    PIN_RESET   =   0
}PIN_STATE_ENUM;
typedef enum
{
    None_Column,
    Column1,
    Column2,
    Column3
}Column_Number_ENUM;


typedef enum
{
    Column_Grounding1,
    Column_Grounding2,
    Column_Grounding3,
    None,
}Column_Grounding_ENUM;


uint64_t                        Delta[NUMBER_OF_PIN] ;
uint64_t                        Continue_Time_Tag[NUMBER_OF_PIN] ;

Input_Chang_State_ENUM          Key_Change_State[NUMBER_OF_PIN] ;
TypeDefEnum_State               Key_Change_State_Occur[NUMBER_OF_PIN]; 
Column_Grounding_ENUM           Column_Grounding ;
Column_Number_ENUM              Column_Number ;

uint64_t                        Key_Change_State_Time_Tag[NUMBER_OF_PIN] ;

uint8_t Key_Number ;
const uint64_t                 Debounce_Time[NUMBER_OF_PIN]    = {100000,100000,100000,100000};




const GPIO_TypeDef*               Key_Input_Port[NUMBER_OF_PIN]           =
{
    Key1_Input_Port,
    Key2_Input_Port,
    Key3_Input_Port,
    Key4_Input_Port,
};
const uint16_t                    Key_Input_Pin[NUMBER_OF_PIN]            =
{
    Key1_Input_Pin,
    Key2_Input_Pin,
    Key3_Input_Pin,
    Key4_Input_Pin,
};

const GPIOMode_TypeDef            Key_Input_Mode[NUMBER_OF_PIN]           =
{
    GPIO_Mode_IPU,
    GPIO_Mode_IPU,
    GPIO_Mode_IPU,
    GPIO_Mode_IPU,
};
const GPIO_TypeDef*               Key_Output_Port[3]                      =
{
    Key5_Output_Port,
    Key6_Output_Port,
    Key7_Output_Port,
};
const uint16_t                    Key_Output_Pin[3]                       =
{
    Key5_Output_Pin,
    Key6_Output_Pin,
    Key7_Output_Pin,
};

const GPIOMode_TypeDef            Key_Output_Mode[3]                     =
{
    GPIO_Mode_Out_PP,
    GPIO_Mode_Out_PP,
    GPIO_Mode_Out_PP,
};


void Key_Input_Initialize(void);


//Input Monitoring

void Input_Monitoring(void)
{
    uint8_t k ;
    PIN_STATE_ENUM  pin_status;
    for ( k=0 ; k < NUMBER_OF_PIN ; k++ )  /* Loop for All Pins */
    {
        if ( Key_Input_Mode[k] == GPIO_Mode_IPU )   /* Input Pull Up  */
        {
            if ( Key_Change_State_Occur[k] == Not_Occur )
            {
                if ( Time_Delay_Occure(Key_Change_State_Time_Tag[k] ,Debounce_Time[k] ) == True )
                {
                    Key_Change_State_Occur[k] = Occur ;
                }
            }
            else 
            {
                if(GPIO_ReadInputDataBit((GPIO_TypeDef*)Key_Input_Port[k],Key_Input_Pin[k]) != 0x00)
                {
                    pin_status = PIN_SET;
                }
                else
                {
                    pin_status = PIN_RESET;
                }
                switch ( Key_Change_State[k] )
                {
                case  Input_Low:
                    if ( pin_status == PIN_RESET )
                    {
                        Key_Change_State[k] = Input_Low_Continue ;
                    }
                    else
                    {
                        Key_Change_State[k]    = Input_High ;
                        Key_Change_State_Occur[k]      = Not_Occur ;
                        Key_Change_State_Time_Tag[k]     = Time_Now();
                    }
                    break ;
                case Input_High:
                    if ( pin_status == PIN_RESET )
                    {
                        Key_Change_State[k]       = Input_Low ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k] = Time_Now(); 
                    }
                    else
                    {
                        Key_Change_State[k] = Input_High_Continue ; 
                    }
                    break ;
                case  Input_Low_Continue:
                    if ( pin_status == PIN_RESET )
                    {
                        Key_Change_State[k] = Input_Low_Continue ; 
                        Continue_Time_Tag[k] = Time_Now(); 
                        Delta[k] = Continue_Time_Tag[k] - Key_Change_State_Time_Tag[k]-Debounce_Time[k] ;
                    }
                    else
                    {
                        Key_Change_State[k]       = Input_High ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k] = Time_Now();
                    }
                    break ;
                case  Input_High_Continue:
                    if ( pin_status == PIN_RESET )
                    {
                        Key_Change_State[k]       = Input_Low ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k] = Time_Now();
                    }
                    else
                    {
                        Key_Change_State[k] = Input_High_Continue ; 
                    }
                    break ;
                    
                default :
                    if ( pin_status == PIN_RESET )
                    {
                        Key_Change_State[k]          = Input_Low ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k] = Time_Now();
                    }
                    else
                    {
                        Key_Change_State[k] = Input_High_Continue ; 
                    }
                    
                    break ;
                }
            } 
        }
        
        else if ( Key_Input_Mode[k] == GPIO_Mode_IPD )   /* Input Pull Down  */
        {
            if ( Key_Change_State_Occur[k] == Not_Occur )
            {
                if ( Time_Delay_Occure(Key_Change_State_Time_Tag[k] ,Debounce_Time[k] ) == True )
                {
                    Key_Change_State_Occur[k] = Occur ;
                }
            }
            else 
            {
                if(GPIO_ReadInputDataBit((GPIO_TypeDef*)Key_Input_Port[k],Key_Input_Pin[k]) != 0x00)
                {
                    pin_status = PIN_SET;
                }
                else
                {
                    pin_status = PIN_RESET;
                }
                switch ( Key_Change_State[k] )
                {
                case  Input_Low:
                    if ( pin_status == PIN_SET )
                    {
                        Key_Change_State[k]       = Input_High ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k] = Time_Now();
                    }
                    else
                    {
                        Key_Change_State[k] = Input_Low_Continue ;
                    }
                    break ;
                case Input_High:
                    if ( pin_status == PIN_SET )
                    {
                        Key_Change_State[k] = Input_High_Continue ; 
                    }
                    else
                    {
                        Key_Change_State[k]           = Input_Low ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k]  = Time_Now(); 
                    }
                    break ;
                case  Input_Low_Continue:
                    if ( pin_status == PIN_SET )
                    {
                        Key_Change_State[k]       = Input_High ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k] = Time_Now();
                    }
                    else
                    {
                        Key_Change_State[k] = Input_Low_Continue ;
                        
                    }
                    break ;
                case  Input_High_Continue:
                    if ( pin_status == PIN_SET )
                    {
                        Key_Change_State[k] = Input_High_Continue ; 
                        Continue_Time_Tag[k] = Time_Now(); 
                        Delta[k] = Continue_Time_Tag[k] - Key_Change_State_Time_Tag[k]-Debounce_Time[k] ;
                    }
                    else
                    {
                        Key_Change_State[k]           = Input_Low ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k]  = Time_Now();
                    }
                    break ;
                    
                default :
                    if ( pin_status == PIN_SET )
                    {
                        Key_Change_State[k]           = Input_High ;
                        Key_Change_State_Occur[k]     = Not_Occur ;
                        Key_Change_State_Time_Tag[k]  = Time_Now();
                    }
                    else
                    {
                        Key_Change_State[k] = Input_Low_Continue ; 
                    }
                    
                    break ;
                }
            } 
        }
    }
}




//void Key_Operation(void)
//{
//    Input_Monitoring();
//    uint8_t k = 0 ;
//    for ( k = 0 ; k < NUMBER_OF_PIN ; k++ )
//    {
//        if(  ( ( Key_Change_State_Occur[1] == Occur ) && ( Key_Change_State[1] == Input_Low )   ) )
//        {
//            
//        }
//        
//        
//    }                 
//}



//Output Initialize




//Input Initialize
void GPIO_Input_Initialize(void)
{
    uint8_t k = 0 ;
    GPIO_InitTypeDef GPIO_InitStructure;
    for(k =0 ;k < NUMBER_OF_PIN ; k++ )
    {
        GPIO_InitStructure.GPIO_Pin   = Key_Input_Pin[k] ;
        GPIO_InitStructure.GPIO_Mode  = Key_Input_Mode[k];
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init((GPIO_TypeDef*)Key_Input_Port[k], &GPIO_InitStructure);
        // GPIO_SetBits((GPIO_TypeDef*)Key_Input_Port[k],Key_Input_Pin[k]);
        
    }
}
void GPIO_Output_Initialize(void)
{
    uint8_t k = 0 ;
    GPIO_InitTypeDef GPIO_InitStructure;
    for(k =0 ;k < 3 ; k++ )
    {
        GPIO_InitStructure.GPIO_Pin   = Key_Output_Pin[k] ;
        GPIO_InitStructure.GPIO_Mode  = Key_Output_Mode[k];
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init((GPIO_TypeDef*)Key_Output_Port[k], &GPIO_InitStructure);  
    }
}


void KeyPad_Number(void)
{
    switch ( Column_Grounding)
    {
    case Column_Grounding1:
        GPIO_ResetBits((GPIO_TypeDef*)Key_Output_Port[0],Key_Output_Pin[0]);
        Input_Monitoring();
        
        if ( ( ( Key_Change_State_Occur[0] == Not_Occur ) && ( Key_Change_State[0] == Input_Low ) ) )
        {
            Column_Number = Column1 ; 
            Column_Grounding = None ;
            GPIO_ResetBits((GPIO_TypeDef*)Key_Output_Port[0],Key_Output_Pin[0]);
        }
        else if ( ( ( Key_Change_State_Occur[1] == Not_Occur ) && ( Key_Change_State[1] == Input_Low ) ) )
        {
            Column_Number = Column1 ; 
            Column_Grounding = None ;
        }
        else if ( ( ( Key_Change_State_Occur[2] == Not_Occur ) && ( Key_Change_State[2] == Input_Low ) ) )
        {
            Column_Number = Column1 ; 
            Column_Grounding = None ;
        }
        else if ( ( ( Key_Change_State_Occur[3] == Not_Occur ) && ( Key_Change_State[3] == Input_Low ) ) )
        {
            Column_Number = Column1 ; 
            Column_Grounding = None ;
        }
        else
        {
            GPIO_SetBits((GPIO_TypeDef*)Key_Output_Port[0],Key_Output_Pin[0]);
            Column_Grounding = Column_Grounding2;
            GPIO_ResetBits((GPIO_TypeDef*)Key_Output_Port[1],Key_Output_Pin[1]);
        }
        break;
    case Column_Grounding2:
        Input_Monitoring();
        if ( ( ( Key_Change_State_Occur[0] == Not_Occur ) && ( Key_Change_State[0] == Input_Low ) ) )
        {
            Column_Number = Column2 ; 
            Column_Grounding = None ;
            GPIO_ResetBits((GPIO_TypeDef*)Key_Output_Port[0],Key_Output_Pin[0]);
        }
        else if ( ( ( Key_Change_State_Occur[1] == Not_Occur ) && ( Key_Change_State[1] == Input_Low ) ) )
        {
            Column_Number = Column2 ; 
            Column_Grounding = None ;
        }
        else if ( ( ( Key_Change_State_Occur[2] == Not_Occur ) && ( Key_Change_State[2] == Input_Low ) ) )
        {
            Column_Number = Column2 ; 
            Column_Grounding = None ;
        }
        else if ( ( ( Key_Change_State_Occur[3] == Not_Occur ) && ( Key_Change_State[3] == Input_Low ) ) )
        {
            Column_Number = Column2 ; 
            Column_Grounding = None ;
        }
        else
        {
            GPIO_SetBits((GPIO_TypeDef*)Key_Output_Port[1],Key_Output_Pin[1]);
            Column_Grounding = Column_Grounding3;
            GPIO_ResetBits((GPIO_TypeDef*)Key_Output_Port[2],Key_Output_Pin[2]);
        }
        break;
    case Column_Grounding3:
        
        Input_Monitoring();
        if ( ( ( Key_Change_State_Occur[0] == Not_Occur ) && ( Key_Change_State[0] == Input_Low ) ) )
        {
            Column_Number = Column3 ; 
            Column_Grounding = None ;
            GPIO_ResetBits((GPIO_TypeDef*)Key_Output_Port[0],Key_Output_Pin[0]);
        }
        else if ( ( ( Key_Change_State_Occur[1] == Not_Occur ) && ( Key_Change_State[1] == Input_Low ) ) )
        {
            Column_Number = Column3 ; 
            Column_Grounding = None ;
        }
        else if ( ( ( Key_Change_State_Occur[2] == Not_Occur ) && ( Key_Change_State[2] == Input_Low ) ) )
        {
            Column_Number = Column3 ; 
            Column_Grounding = None ;
        }
        else if ( ( ( Key_Change_State_Occur[3] == Not_Occur ) && ( Key_Change_State[3] == Input_Low ) ) )
        {
            Column_Number = Column3 ; 
            Column_Grounding = None ;
        }
        else
        {
            GPIO_SetBits((GPIO_TypeDef*)Key_Output_Port[2],Key_Output_Pin[2]);
            Column_Grounding = Column_Grounding1;   
        }
        break;
    case None :
        break;    
    }
    Key_Number = 30 ;
    Input_Monitoring();
    switch(Column_Number)
    {   
    case Column1 :
        if ( ( ( Key_Change_State_Occur[0] == Occur ) && ( Key_Change_State[0] == Input_Low ) ) )
        {
            Key_Number =  1 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[1] == Occur ) && ( Key_Change_State[1] == Input_Low ) ) )
        {
            Key_Number =  4 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[2] == Occur ) && ( Key_Change_State[2] == Input_Low ) ) )
        {
            Key_Number =   7 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[3] == Occur ) && ( Key_Change_State[3] == Input_Low ) ) )
        {
            Key_Number =  10 ;
            Column_Grounding = Column_Grounding1;
        }
        else
        {
            
        }
        break;
    case Column2 :
        if ( ( ( Key_Change_State_Occur[0] == Occur ) && ( Key_Change_State[0] == Input_Low )  ) )
        {
            Key_Number =   2 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[1] == Occur ) && ( Key_Change_State[1] == Input_Low ) ) )
        {
            Key_Number =   5 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[2] == Occur ) && ( Key_Change_State[2] == Input_Low ) ) )
        {
            Key_Number =   8 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[3] == Occur ) && ( Key_Change_State[3] == Input_Low ) ) )
        {
            Key_Number =   0 ;
            Column_Grounding = Column_Grounding1;
        }
        else
        {  
        }
        break;
    case Column3 :
        if ( ( ( Key_Change_State_Occur[0] == Occur ) && ( Key_Change_State[0] == Input_Low ) ) )
        {
            Key_Number =   3 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[1] == Occur ) && ( Key_Change_State[1] == Input_Low ) ) )
        {
            Key_Number =   6 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[2] == Occur ) && ( Key_Change_State[2] == Input_Low ) ) )
        {
            Key_Number =   9 ;
            Column_Grounding = Column_Grounding1;
        }
        else if ( ( ( Key_Change_State_Occur[3] == Occur ) && ( Key_Change_State[3] == Input_Low ) ) )
        {
            Key_Number =   12 ;
            Column_Grounding = Column_Grounding1;
        }
        else
        {   
        }
        break;
    case None_Column:
        {
        }
        break;   
    }
}

void Keypad_Initalize(void)
{
GPIO_Input_Initialize();
  GPIO_Output_Initialize();
}
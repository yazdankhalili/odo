
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define NUMBER_OF_PORT   4
#define NUMBER_OF_PIN    4

 typedef enum
 {
   Input_None          = 0,
   Input_High          = 1,
   Input_High_Continue = 2 ,
   Input_Low           = 3,
   Input_Low_Continue  = 4
 }Input_Chang_State_ENUM ;

typedef enum
{
  Occur,
  Not_Occur
}TypeDefEnum_State;

extern TypeDefEnum_State           Key_Change_State_Occur[NUMBER_OF_PIN]; 
extern Input_Chang_State_ENUM      Key_Change_State[NUMBER_OF_PIN] ;
extern uint8_t                     Key_Number ;

void Input_Monitoring(void) ;
void GPIO_Input_Initialize(void);
void GPIO_Output_Initialize(void);



#define Key1_Input_Port                 GPIOA 
#define Key1_Input_Pin                  GPIO_Pin_1
#define Key2_Input_Port                 GPIOA 
#define Key2_Input_Pin                  GPIO_Pin_2
#define Key3_Input_Port                 GPIOA 
#define Key3_Input_Pin                  GPIO_Pin_3
#define Key4_Input_Port                 GPIOA 
#define Key4_Input_Pin                  GPIO_Pin_4
#define Key5_Output_Port                GPIOA 
#define Key5_Output_Pin                 GPIO_Pin_5
#define Key6_Output_Port                GPIOA 
#define Key6_Output_Pin                 GPIO_Pin_6
#define Key7_Output_Port                GPIOA 
#define Key7_Output_Pin                 GPIO_Pin_7



void Keypad_Initalize(void) ;
void Key_Operation(void);
void KeyPad_Number(void);



#ifndef __MAIN_H__
#define __MAIN_H__


#include "stm32f0xx_ll_iwdg.h"
#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"
//#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"
#include "math.h"
#include "stdio.h"
#include "stdbool.h"
#include "stm32f0xx_it.h"


#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define limitabs(a,b) ((a>0)?(min(a,b)):(max(a,-b)))
#define sign(a) ((a>0)?1:((a<0)?-1:0))
#define fw_version "1.51"
//#define minabs(a,b) ((a>0)?(max(a,b)):(min(a,-b)))

//#define MISO_Pin LL_GPIO_PIN_6
//#define MISO_GPIO_Port GPIOA
//#define MOSI_Pin LL_GPIO_PIN_7
//#define MOSI_GPIO_Port GPIOA
#define CAL_Pin LL_GPIO_PIN_0
#define CAL_GPIO_Port GPIOA
#define CLOSE_Pin LL_GPIO_PIN_1
#define CLOSE_GPIO_Port GPIOA
#define SET2_Pin LL_GPIO_PIN_2
#define SET2_GPIO_Port GPIOA
#define SET1_Pin LL_GPIO_PIN_3
#define SET1_GPIO_Port GPIOA
#define CLKIN_Pin LL_GPIO_PIN_0
#define CLKIN_GPIO_Port GPIOB
#define CLKIN_EXTI_IRQn EXTI0_1_IRQn
#define DIRIN_Pin LL_GPIO_PIN_1
#define DIRIN_GPIO_Port GPIOB
#define DIRIN_EXTI_IRQn EXTI0_1_IRQn
#define ENIN_Pin LL_GPIO_PIN_2
#define ENIN_GPIO_Port GPIOB
#define ENIN_EXTI_IRQn EXTI2_3_IRQn
#define LED_Pin LL_GPIO_PIN_11  
#define LED_GPIO_Port GPIOB
#define PWM1_Pin LL_GPIO_PIN_4
#define PWM1_GPIO_Port GPIOB
#define PWM2_Pin LL_GPIO_PIN_5
#define PWM2_GPIO_Port GPIOB
#define IN1_Pin LL_GPIO_PIN_6
#define IN1_GPIO_Port GPIOB
#define IN2_Pin LL_GPIO_PIN_7
#define IN2_GPIO_Port GPIOB
#define IN3_Pin LL_GPIO_PIN_8
#define IN3_GPIO_Port GPIOB
#define IN4_Pin LL_GPIO_PIN_9
#define IN4_GPIO_Port GPIOB
//#define NSS_Pin LL_GPIO_PIN_4
//#define NSS_GPIO_Port GPIOA



//------------------------------			   
// #define OLED_CS_Pin             LL_GPIO_PIN_12		//
// #define OLED_CS_GPIO_Port       GPIOB
// #define OLED_RST_Pin            LL_GPIO_PIN_13 		//
// #define OLED_RST_GPIO_Port      GPIOC
// #define OLED_RS_Pin             LL_GPIO_PIN_13		//
// #define OLED_RS_GPIO_Port       GPIOB

// #define OLED_SCLK_Pin           LL_GPIO_PIN_15		//
// #define OLED_SCLK_GPIO_Port     GPIOB
// #define OLED_SDIN_Pin           LL_GPIO_PIN_14	    //
// #define OLED_SDIN_GPIO_Port     GPIOB


#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif



#define CAL     LL_GPIO_IsInputPinSet(CAL_GPIO_Port,  CAL_Pin) 
#define CLOSE   LL_GPIO_IsInputPinSet(CLOSE_GPIO_Port, CLOSE_Pin) 
#define SET1    LL_GPIO_IsInputPinSet(SET1_GPIO_Port, SET1_Pin) 
#define SET2    LL_GPIO_IsInputPinSet(SET2_GPIO_Port, SET2_Pin)
#define ENIN    LL_GPIO_IsInputPinSet(ENIN_GPIO_Port, ENIN_Pin)
#define DIRIN   LL_GPIO_IsInputPinSet(DIRIN_GPIO_Port,DIRIN_Pin)

#define LED_H     LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin)  
#define LED_L     LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin) 
#define LED_F     LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)
#define IN1_HIGH  LL_GPIO_SetOutputPin(IN1_GPIO_Port, IN1_Pin) 
#define IN1_LOW   LL_GPIO_ResetOutputPin(IN1_GPIO_Port, IN1_Pin)
#define IN2_HIGH  LL_GPIO_SetOutputPin(IN2_GPIO_Port, IN2_Pin) 
#define IN2_LOW   LL_GPIO_ResetOutputPin(IN2_GPIO_Port, IN2_Pin)
#define IN3_HIGH  LL_GPIO_SetOutputPin(IN3_GPIO_Port, IN3_Pin) 
#define IN3_LOW   LL_GPIO_ResetOutputPin(IN3_GPIO_Port, IN3_Pin)
#define IN4_HIGH  LL_GPIO_SetOutputPin(IN4_GPIO_Port, IN4_Pin) 
#define IN4_LOW   LL_GPIO_ResetOutputPin(IN4_GPIO_Port, IN4_Pin)

//#define NSS_H   LL_GPIO_SetOutputPin(NSS_GPIO_Port, NSS_Pin)  
//#define NSS_L   LL_GPIO_ResetOutputPin(NSS_GPIO_Port, NSS_Pin) 

//#define SPI_TX_OD  LL_GPIO_SetPinOutputType(MOSI_GPIO_Port, MOSI_Pin, LL_GPIO_OUTPUT_OPENDRAIN)
//#define SPI_TX_PP  LL_GPIO_SetPinOutputType(MOSI_GPIO_Port, MOSI_Pin, LL_GPIO_OUTPUT_PUSHPULL)

/*******************OLED output Defined****************************/
// #define OLED_CS_H       LL_GPIO_SetOutputPin(OLED_CS_GPIO_Port, OLED_CS_Pin)  		//
// #define OLED_CS_L       LL_GPIO_ResetOutputPin(OLED_CS_GPIO_Port, OLED_CS_Pin)

// #define OLED_RST_H      LL_GPIO_SetOutputPin(OLED_RST_GPIO_Port,OLED_RST_Pin) 		//
// #define OLED_RST_L      LL_GPIO_ResetOutputPin(OLED_RST_GPIO_Port,OLED_RST_Pin) 	//

// #define OLED_RS_H       LL_GPIO_SetOutputPin(OLED_RS_GPIO_Port,OLED_RS_Pin)		//
// #define OLED_RS_L       LL_GPIO_ResetOutputPin(OLED_RS_GPIO_Port,OLED_RS_Pin)		//

// #define OLED_SCLK_H     LL_GPIO_SetOutputPin(OLED_SCLK_GPIO_Port,OLED_SCLK_Pin)		//
// #define OLED_SCLK_L     LL_GPIO_ResetOutputPin(OLED_SCLK_GPIO_Port,OLED_SCLK_Pin)		//

// #define OLED_SDIN_H     LL_GPIO_SetOutputPin(OLED_SDIN_GPIO_Port,OLED_SDIN_Pin)	    //
// #define OLED_SDIN_L     LL_GPIO_ResetOutputPin(OLED_SDIN_GPIO_Port,OLED_SDIN_Pin)		//



/* Functionality mode */
//#define REFERESH_ANGLE		0

#define UMAXCL   62         //
#define UMAXOP   160        // Open loop maximum current, this translate to (3.3V / 256) * 160 = 2V on the Vref pin
#define UMAXSUM  25600      //

#define angle_conversion 360.0f/32768.0f

extern int16_t kp;     
extern int16_t ki;
extern int16_t kd;

extern int32_t s;
extern int32_t s_1;
extern int32_t s_sum;
extern int32_t r;   
extern int32_t r_1;   
extern uint8_t dir; 
extern int16_t y;   
extern int16_t y_1;
extern int32_t yw;  
extern int32_t yw_1;
extern int16_t advance;
extern int32_t wrap_count; 
extern int32_t pid_e;  
extern int32_t iterm;
extern int32_t dterm;
extern int16_t u;     
extern int32_t stepnumber;              // Used in OneStep() function
extern uint8_t stepangle;
extern int32_t angle_prev;
extern int32_t cpu_idle;

extern volatile uint8_t menuActive;           // JaSw: In-menu = 1 else 0
extern volatile uint32_t tickCount;     // JaSw: Counts ticks
extern volatile uint32_t tim6Counter;   // JaSw: Counts timer 6 interrupts
extern bool tuningMode;
extern uint8_t oledClock;               // JaSw: Holds OLED Display Clock Divide Ratio/Oscillator Frequency

extern uint16_t hccount;
extern uint8_t closemode;
extern uint8_t enmode;

extern uint8_t Currents;          //800ma
//extern uint8_t Microstep;            //4->4096
extern uint8_t Motor_Dir;                  //

//extern volatile uint8_t Menu_update_flag;      //
extern volatile uint8_t Data_update_flag;  
extern volatile uint16_t Data_update_Count;
//extern uint8_t Menu_Num ;             //
//extern uint8_t Menu_Num_item;         //
//extern uint8_t Menu_move_num;         //

//extern uint8_t CalibrateEncoder_finish_flag; //
//extern uint8_t Second_Calibrate_flag; //
// extern uint8_t Second_Menu;           //
// extern uint8_t Menu_Num2_item;        //  
// extern uint8_t Menu_Num3_item;        //
// extern uint8_t Menu_Num4_item;        //
// extern uint8_t Menu_Num5_item;        // 
// extern uint8_t Menu_Num6_item;        // 
extern volatile uint8_t Calibration_flag;      //
extern int16_t Motor_speed ;
extern int16_t Motor_speed_count;
extern uint8_t start_measu_V_flag;
extern uint8_t measure_once_flag;

//extern volatile uint8_t flash_store_flag;
extern volatile uint16_t table1[15];                    //

//extern uint8_t Rx1_buff[8];
//extern uint8_t Receive_Count;
//extern uint8_t Rx1_temp_num;                       
//extern uint8_t Receive_finish_flag;          //
//extern volatile uint8_t  Communications_Process_flag;     //
//extern volatile uint8_t  Uart_CRC_flag;                   //  
//extern volatile uint8_t  frame_Error_flag;                //
//extern volatile uint8_t  Urat_CRC_Correct_flag;           //
//extern int16_t value_Temp;
//extern uint8_t Currents_Set;
//extern uint8_t Microstep_Set;            //4->4096

//extern uint8_t Dir_Enable;           //
extern uint8_t Motor_Dir;                  //
extern uint8_t Motor_Enable;               //
//extern uint8_t Motor_mode;              //
extern  uint8_t Motor_ENmode_flag; //
//extern volatile uint8_t enter1_once_flag ;    //
//extern volatile uint8_t enter2_once_flag ;
//extern volatile uint8_t dir1_once_flag ;      //
//extern volatile uint8_t dir2_once_flag ;
//extern volatile uint8_t close_loop1_flag ;      //
//extern volatile uint8_t close_loop2_flag ;

void CalibrateEncoder(void);
void StoreCurrentParameters();
void ChangeOLEDClock();
extern void Output(int32_t theta,uint8_t effort);
extern uint16_t ReadValue(uint16_t RegValue);
extern uint16_t ReadAngle(void);
extern void PID_Cal_value_init(void);

//To minimize latency - it is inline
static inline void restart_init(void)
{
  enmode = 1;
  // Allow to init while control loop is disabled
  LL_TIM_DisableCounter(TIM1);
  LL_TIM_DisableCounter(TIM6); // Control loop
  PID_Cal_value_init();
  LL_TIM_EnableCounter(TIM6);
  LL_TIM_EnableCounter(TIM3);
  LL_TIM_EnableCounter(TIM1);
}

// Verify if we are enabled/disabled in the beginning, as we only check that in interrupts during normal work
static inline void handle_en(void)
{
  if (!enmode && ((ENIN == Motor_ENmode_flag))) // || (SoftEnable)))
  {
    rampup_current = 0;
    restart_init();
  };

  if (enmode && ((ENIN != Motor_ENmode_flag))) // && (!SoftEnable)))
  {
    enmode = 0;
    pid_e = 0;//Do not display error when motor is disabled

    // Disable motor current output
    WRITE_REG(TIM3->CCR1, 0);
    WRITE_REG(TIM3->CCR2, 0);

    //Disable bridge
    IN1_LOW;  
    IN2_LOW;  
    IN3_LOW;  
    IN4_LOW;  
  };
}

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif 

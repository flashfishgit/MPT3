/**
  ******************************************************************************
  * @file    main.c 
  * @author  Marco Soellinger
  * @version V1.0
  * @date    14.10.2025
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h"
#include "sysdelay.h"
#include "board_led.h"
#include "board_button.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the sys tick timer (M0 core) which is used for delay.
  * @param  None
  * @retval None
  */
void SysTick_Init(void) 
{
    /* init the sys tick timer to be called every 1ms */
    SysTick_Config(SystemCoreClock / 1000);
} 

/**
  * @brief  This is the SysTick interrupt handler which is called every 1ms. 
            We have to increment the HAL tick counter which is used for SysDelay
  * @param  None
  * @retval None
  */
void SysTick_Handler()
{
    SysDelay_IncTicks();
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	SysTick_Init();
	
	LED_Initialize();
	Button_Initialize();
	
	uint8_t counter = 0;
	
while (1){
		uint32_t state = Button_GetState();
		
		if(state & BUTTON_USER0_MASK){
			counter++;
		}
		
		if(state & BUTTON_USER1_MASK){
			counter--;
		}
		
		if(counter
	
		LED_SetOut(counter);
    SysDelay_Delay(200);

  }
    
}

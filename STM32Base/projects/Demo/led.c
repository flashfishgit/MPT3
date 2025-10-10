/**
  ******************************************************************************
  * @file    led.c 
  * @author  Marco Söllinger
  * @version V1.0
  * @date    10.10.2025
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h"
#include "sysdelay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
	#define LED3_ON GPIO_BSRR_BR_0
	#define LED3_OFF GPIO_BSRR_BS_0
  #define LED4_ON GPIO_BSRR_BR_1
	#define LED4_OFF GPIO_BSRR_BS_1
	#define LED5_ON GPIO_BSRR_BR_2
	#define LED5_OFF GPIO_BSRR_BS_2
	
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
  * @brief  This function advances the Led cycle by one step
	*					The operation in this functions are atomic.
  * @param  None
  * @retval None
  */
void cycleLed(){
	static uint8_t pos = 1;
	
	switch(pos)  {
		case 0:
			GPIOC->BSRR = (LED3_ON | LED4_OFF |  LED5_OFF);
			break;
		case 1:
			GPIOC->BSRR = (LED3_OFF | LED4_ON |  LED5_OFF);
			break;
		case 2:
			GPIOC->BSRR = (LED3_OFF | LED4_OFF |  LED5_ON);
			break;
		case 3:
			GPIOC->BSRR = (LED3_OFF | LED4_ON |  LED5_OFF);
			break;

		default:
			GPIOC->BSRR = (LED3_ON | LED4_ON |  LED5_ON);
			pos = 0xFF;
		break;

	}
	
	pos = (pos+1)%4;
}

/**
  * @brief  Setup the inital state for the Led cycle
	*					Doesnt setup the clock of the Port
  * @param  None
  * @retval None
  */
void initCycleLed(){
	GPIOC->BSRR = (LED3_ON | LED4_OFF |  LED5_OFF);
	GPIOC->MODER |=  GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	SysTick_Init();
	
   // enable clock for GPIO Port C
   RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	initCycleLed();
	
while (1)
    {
        SysDelay_Delay(200);
				cycleLed();
    }
    
}

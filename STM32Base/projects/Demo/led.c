/**
  ******************************************************************************
  * @file    led.c 
  * @author  Andreas Oyrer
  * @version V1.0
  * @date    30-September-2022
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h"
#include "sysdelay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
	#define LED3 GPIO_ODR_0
	#define LED4 GPIO_ODR_1
	#define LED5 GPIO_ODR_2
	
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
	/*
	
		GPIO_InitTypeDef initStruct;
	
	GPIO_StructInit(&initStruct);
	initStruct.GPIO_Pin = GPIO_Pin_0;
	initStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &initStruct);
	
	
	*/

	
	
	
    // enable clock for GPIO C
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // set pins 0-2 on port C to output
		GPIOC->MODER |=  GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0;
	
		char Led_Mask = 0x01;
		GPIOC->ODR = (GPIO_ODR_0 & ~0x07)| ~Led_Mask;
	
while (1)
    {
				Led_Mask = (Led_Mask << 1);
        GPIOC->ODR = (GPIOC->ODR & ~0x07)| ~Led_Mask;
				
        SysDelay_Delay(200);
			
				Led_Mask = (Led_Mask << 1);
				GPIOC->ODR = (GPIOC->ODR & ~0x07)| ~Led_Mask;
				
        SysDelay_Delay(200);
			
				Led_Mask = (Led_Mask >> 1);
				GPIOC->ODR = (GPIOC->ODR & ~0x07)| ~Led_Mask;
			
				SysDelay_Delay(200);
			
				Led_Mask = (Led_Mask >> 1);
				GPIOC->ODR = (GPIOC->ODR & ~0x07)| ~Led_Mask;
			
				SysDelay_Delay(200);
    }
    
}

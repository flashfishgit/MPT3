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
    GPIO_InitTypeDef initStruct;
 
    /* init the sys tick counter (needed for SysDelay) */
    SysTick_Init();
    
    /* enable clock for GPIO C */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    /* Configure GPIO pin for LED3 as output (connected to PC0) */
    GPIO_StructInit(&initStruct);
    initStruct.GPIO_Pin = GPIO_Pin_0;
    initStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &initStruct);

    while (1)
    {
        /* disable LED3 (set pin to high because of negative logic) */
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
        SysDelay_Delay(200);
        /* enable LED3 */
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
        SysDelay_Delay(200);
    }

    /* alternative using direct register access */

    /*
    // enable clock for GPIO C
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // set pins 0-2 on port C to output
    GPIOC->MODER = (1 << (2*0));

    while (1)
    {
        // disable LED3 (set pin to high because of negative logic)
        GPIOC->ODR = GPIO_Pin_0;
        SysDelay_Delay(200);
        // enable LED3
        GPIOC->ODR = 0;
        SysDelay_Delay(200);
    }
    */
}

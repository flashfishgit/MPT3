/**
  ******************************************************************************
  * @file    board_button.c
  * @author  Marco Soellinger
  * @version V1.0
  * @date    14-10-2025
  * @brief   APUS Board button control Source File.
  ******************************************************************************
  */

#include "stm32f0xx_gpio.h"
#include "board_button.h"

#define BUTTON_USER0_PIN       GPIO_Pin_0
#define BUTTON_USER1_PIN       GPIO_Pin_1
#define BUTTON_WAKEUP_PIN      GPIO_Pin_0

/* GPIO Pin identifier */
typedef struct _GPIO_PIN 
{
    GPIO_TypeDef *port;
    uint16_t      pin;
} GPIO_PIN;

/* LED GPIO Pins */
/* Must be in the same order as the return mask */
/* first in array is on first position in return*/
static const GPIO_PIN BUTTON_PIN[] = 
{
    { GPIOD, BUTTON_USER0_PIN },
    { GPIOD, BUTTON_USER1_PIN },
    { GPIOA, BUTTON_WAKEUP_PIN }
};


/**
  * @brief  Initialize buttons including GPIO ports.
  * @param  None
  * @retval None
*/
void Button_Initialize(void){
	  GPIO_InitTypeDef initStruct;

    /* GPIO Ports Clock Enable */
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    /* initialize the GPIO struct */
    GPIO_StructInit(&initStruct);
    
    for (uint32_t idx = 0; idx < sizeof(BUTTON_PIN) / sizeof(GPIO_PIN); idx++)
    {
	    /* Configure GPIO pin */
    	initStruct.GPIO_Pin = BUTTON_PIN[idx].pin;
	    initStruct.GPIO_Mode = GPIO_Mode_IN;
			initStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
			
    	GPIO_Init(BUTTON_PIN[idx].port, &initStruct);
    }
}

/**
  * @brief  Get buttons state.
  * @param  None
  * @retval The state of pressed buttons.
*/
uint32_t Button_GetState(void){
	uint32_t retVal = 0;
	
	for (uint32_t idx = 0; idx < sizeof(BUTTON_PIN) / sizeof(GPIO_PIN); idx++){
		if(GPIO_ReadInputDataBit(BUTTON_PIN[idx].port, BUTTON_PIN[idx].pin)){
			retVal |= (1 << idx);
		}
		
  }
	
	return retVal;
}
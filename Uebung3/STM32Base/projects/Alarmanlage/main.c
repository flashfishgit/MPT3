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
static void SysTick_Init(void) 
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
static void SysTick_Handler()
{
    SysDelay_IncTicks();
}

static void SwitchClockTo24Mhz(){
	//Switch to clock HSI
	RCC->CFGR &= ~RCC_CFGR_SW; 
	
	//switch to HSE 
	RCC->CFGR |= RCC_CFGR_SW_HSE;
	
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE){
	}
	
	// reconfigure PLL
	RCC->CR &= ~RCC_CR_PLLON;
	
	//TODO:Wait until PLLRDY is cleared. The PLL is now fully stopped.
	
	//Change to desired parameter.
	RCC->CFGR &= ~RCC_CFGR_PLLMUL;
	RCC->CFGR |= RCC_CFGR_PLLMUL3;
	
	//TODO: Enable the PLL again by setting PLLON to 1.
	//TODO: Wait until PLLRDY is set.
	
	//TODO: Switch back to PLL as sysclock.
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
		
	// counter for 3 bit Adder
	uint8_t counter = 0;

while (1){
		uint32_t state = Button_GetState();
		
		if(state & BUTTON_USER0_MASK){
			// increments adder and removes overflows
			counter = (counter+1) &0x7;
		}
		
		if(state & BUTTON_USER1_MASK){
			// dercrement adder and removes underflows
			counter = (counter-1) & (uint32_t)0x7;
		}
	
		LED_SetOut(counter);
		
		// 2 Hz freq
    SysDelay_Delay(500);

  }
    
}

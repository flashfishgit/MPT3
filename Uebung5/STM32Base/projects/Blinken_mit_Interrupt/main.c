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
#include <stdbool.h>

#include "board_button.h"
#include "board_led.h"
#include "stm32f0xx_conf.h"
#include "stm32f0xx_flash.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"
#include "sysdelay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint32_t g_ms = 0;
static bool zone1_active = false;
static bool zone2_active = false;

/* Private function prototypes -----------------------------------------------*/
static void SysTick_Init(void);
void SysTick_Handler(void); // <<< muss global & exakt so heißen
static void SwitchClockTo24Mhz(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initialize the sys tick timer (M0 core) which is used for delay.
 * @param  None
 * @retval None
 */
static void SysTick_Init(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);
}

/**
  * @brief  This is the SysTick interrupt handler which is called every 1ms.
            We have to increment the HAL tick counter which is used for SysDelay
  * @param  None
  * @retval None
  */
void SysTick_Handler() {
  g_ms++;
  SysDelay_IncTicks();
}

void EXTI_Initialize(){
	// enable the clock for the syscfg register to configure the external interrupt
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	//configure EXTI registers to enable PD0 and PD1 as source for interrupt
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1);
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PD| SYSCFG_EXTICR1_EXTI1_PD; // für PortD
	
	//configure mask bit to enable IRQ for line 0 and line 1
	EXTI->IMR |= GPIO_Pin_0 | GPIO_Pin_1;
	
	//configure EXTI0 and EXTI1 for rising edge interrupts
	EXTI->RTSR |= GPIO_Pin_0 | GPIO_Pin_1;
	
	//configure NVIC and enable EXT0_1 IRQ channel
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	
}

// EXTI0_1 Interrupt Handler
void EXTI0_1_IRQHandler(void){
	if(EXTI->PR & GPIO_Pin_0){
		//reset pending bit
		//No read modify write  here bacause we read a 1 if an other IRQ is pendig
		EXTI->PR = GPIO_Pin_0; 
		
		LED_Toggle(LED4);
		
	} 
	if(EXTI->PR & GPIO_Pin_1){
		//reset pending bit
		//No read modify write  here bacause we read a 1 if an other IRQ is pendig
		EXTI->PR = GPIO_Pin_0; 
		
		LED_Toggle(LED5);
	}
}




int main(void) {
  SysTick_Init();
  LED_Initialize();
  Button_Initialize();

  uint32_t lastToggle = 0;
  bool switchedHSI = false;

	EXTI_Initialize();
	
  while (1) {
   

    SysDelay_Delay(1);
  }
}

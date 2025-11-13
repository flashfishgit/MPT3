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
// 2*5Hz
#define TIM_PSC (1000-1)
#define TIM_ARR (4800-1)

#define BUTTON_USER0_PIN       GPIO_Pin_0
#define BUTTON_USER1_PIN       GPIO_Pin_1

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

//should be initalized before Interrupt for correct order
void TIM3_Init(){
	// Activate timer 3 clock
	 if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN)) {
     RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  }
	 
	// Stop timer if already running
	TIM3->CR1 &= ~TIM_CR1_CEN;
	
	// Sets the timer freq and DutyCylce
	TIM3->ARR = TIM_ARR;
	TIM3->PSC = TIM_PSC;
	
	// Enable upddate interrupt enable so ISR gets called
	TIM3->DIER |= TIM_DIER_UIE;
	                     
	// Make sure Status Flag is reset
	TIM3->SR &= ~TIM_SR_UIF;
	
	// Enable interrupt
	NVIC_EnableIRQ(TIM3_IRQn);
}

static void inline TIM3_StartBlinken(){
	// Clear Flag
	TIM3->SR &= ~TIM_SR_UIF;
	// make sure timer start at beginning
	TIM3->CNT = 0; 
	//start the compare match timer
	TIM3->CR1 |= TIM_CR1_CEN;
};

static void inline TIM3_StopBlinken(){
	//stop timer
	TIM3->CR1 &= ~TIM_CR1_CEN;
	
	LED_Off(LED3);
};

// Interrupt Routine
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;    // Flag löschen
        LED_Toggle(LED3);           // Blink-LED
    }
}

void EXTI_Initialize(){
	// enable the clock for the syscfg register to configure the external interrupt
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	//configure EXTI registers to enable PD0 and PD1 as source for interrupt
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1);
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PD| SYSCFG_EXTICR1_EXTI1_PD; // für PortD
	
	//configure mask bit to enable IRQ for line 0 and line 1
	EXTI->IMR |= BUTTON_USER0_PIN | BUTTON_USER1_PIN;
	
	//configure EXTI0 and EXTI1 for rising edge interrupts
	EXTI->RTSR |= BUTTON_USER0_PIN | BUTTON_USER1_PIN;
	
	//configure NVIC and enable EXT0_1 IRQ channel
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

// EXTI0_1 Interrupt Handler
void EXTI0_1_IRQHandler(void){
	if(EXTI->PR & BUTTON_USER0_PIN){
		//reset pending bit
		EXTI->PR = BUTTON_USER0_PIN; 
		
		LED_Toggle(LED4);
		TIM3_StartBlinken();
		
	} 
	if(EXTI->PR & BUTTON_USER1_PIN){
		//reset pending bit
		EXTI->PR = BUTTON_USER1_PIN; 
		
		LED_Toggle(LED5);
		TIM3_StopBlinken();
	}
}



int main(void) {
  SysTick_Init();
  LED_Initialize(); 
  Button_Initialize();
	TIM3_Init();
	EXTI_Initialize();
	
  while (1) {
    SysDelay_Delay(1);
  }
}

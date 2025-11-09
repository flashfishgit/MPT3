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
// 2Hz  1:5
#define BTN_USER0_PSC (1000-1)
#define BTN_USER0_ARR (24000-1)
#define BTN_USER0_CCR (4000)

// 4Hz 1:3
#define BTN_USER1_PSC (1000-1)
#define BTN_USER1_ARR (12000-1)
#define BTN_USER1_CCR (3000)

// 9 Hz 3:2
#define BTN_WAKEUP_PSC (1000-1)
#define BTN_WAKEUP_ARR (5333-1)
#define BTN_WAKEUP_CCR (3200)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint32_t g_ms = 0;

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


void TIM3_InitForCompareOutput(uint16_t psc, uint16_t arr, uint16_t ccr3){
	// Activate GPIO clock when not activated
	if (!(RCC->AHBENR & RCC_AHBENR_GPIOCEN)) {
     RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  }
	// Activate timer 3 clock
	 if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN)) {
     RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  }
	 
	// Stop timer if already running
	TIM3->CR1 &= ~TIM_CR1_CEN;
	// Disable before configruation
	TIM3->CCER &= ~TIM_CCER_CC3E;
	
	// Initalise Pin
	// configure PC8 to AF mode
	GPIO_InitTypeDef GPIO_initStruct;
	GPIO_StructInit(&GPIO_initStruct);
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF; // configur alternate function
	GPIO_Init(GPIOC, &GPIO_initStruct);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_0); 
	
		// configure output compare mode to PWM mode 1
	TIM3->CCMR2 = (TIM3->CCMR2 & ~(7u<<4))|(6u << 4);
	//TIM3->CR1   |= TIM_CR1_ARPE; 
	
	// Sets the timer freq and DutyCylce
	TIM3->ARR = arr;
	TIM3->PSC = psc;
	TIM3->CCR3 = ccr3;
	
                               
	
	// enable output control of pin
	TIM3->CCER |= TIM_CCER_CC3E;
	//start the compare match timer
	TIM3->CR1 |= TIM_CR1_CEN;
}

int main(void) {
  SysTick_Init();
  LED_Initialize();
  Button_Initialize();
	
	// Falls mehrere Knöpfe gedrückt werden gewinnte das erste in der if
  while (1) {
   uint32_t button = Button_GetState();
		
		if(button & BUTTON_USER0_MASK){
			TIM3_InitForCompareOutput(BTN_USER0_PSC, BTN_USER0_ARR, BTN_USER0_CCR);
		} else if(button & BUTTON_USER1_MASK){
			TIM3_InitForCompareOutput(BTN_USER1_PSC, BTN_USER1_ARR, BTN_USER1_CCR);
		} else if(button & BUTTON_WAKEUP_MASK){
			TIM3_InitForCompareOutput(BTN_WAKEUP_PSC, BTN_WAKEUP_ARR, BTN_WAKEUP_CCR);
		}

    SysDelay_Delay(10);
  }
}

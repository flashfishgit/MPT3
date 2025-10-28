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


void TIM3_InitForCompareOutput(){
	// configure PC8 to AF mode
	GPIO_InitTypeDef GPIO_initStruct;
	
	// Activate GPIO clock
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	GPIO_StructInit(&GPIO_initStruct);
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOC, &GPIO_initStruct);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_0); 
	
	//configure timer 3 to toggle PC8 with 1 Hz
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// 1000L = Prescaler=1000, 2 = 50% high/50% low
	TIM3->ARR = (SystemCoreClock / 1000L / 2UL) -1 ;
	TIM3->PSC = (1000 -1);
	
	TIM3->CCR3 = (SystemCoreClock / 1000L / 2UL / 2UL) -1;
	
	// configure output compare mode to PWM mode 2
	TIM3->CCMR2 |= (7 << 4);
	
	// enable output control of pin
	TIM3->CCER |= TIM_CCER_CC3E;
	
	//start the compare match timer
	TIM3->CR1 |= TIM_CR1_CEN;
}

int main(void) {
  SysTick_Init();
  LED_Initialize();
  Button_Initialize();

  uint32_t lastToggle = 0;
  bool switchedHSI = false;

	TIM3_InitForCompareOutput();
	
  while (1) {
   

    SysDelay_Delay(1);
  }
}

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

#include "stm32f0xx_conf.h"
#include "stm32f0xx_tim.h"
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

#include "stm32f0xx_conf.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_rcc.h"     // <<< für RCC_GetClocksFreq & Co.
#include "stm32f0xx_flash.h"   // <<< für FLASH_SetLatency
#include "sysdelay.h"
#include "board_led.h"
#include "board_button.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ALARMZONE1_BUTTON BUTTON_USER0_MASK
#define ALARMZONE2_BUTTON BUTTON_USER1_MASK
#define ALARM_OFF_BUTTON  BUTTON_WAKEUP_MASK

#define ALARMZONE1_LED    LED4
#define ALARMZONE2_LED    LED5
#define ALARM_SIREN       LED3

#define SIREN_TOGGLE_MS   100u  // ~5 Hz: toggle alle 100 ms

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint32_t g_ms = 0;          // <<< eigene 1-ms Zeitbasis
static bool zone1_active = false;           // <<< Zustände merken
static bool zone2_active = false;

/* Private function prototypes -----------------------------------------------*/
static void SysTick_Init(void);
void SysTick_Handler(void);                 // <<< muss global & exakt so heißen
static void SwitchClockTo24Mhz(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the sys tick timer (M0 core) which is used for delay.
  * @param  None
  * @retval None
  */
static void SysTick_Init(void) 
{
    /* init the sys tick timer to be called every 1ms */
		SystemCoreClockUpdate();
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
		g_ms++;
    SysDelay_IncTicks();
}

/**
  * @brief  Umschalten auf SYSCLK = 24 MHz (HSI/2 * 6 über PLL)
  *         Modfifyed from sytem configuration
  */
static void SwitchClockTo24Mhz(void){ // Copied from System_stm32f and modified
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	
	// Make sure HSI is on
	RCC->CR |= RCC_CR_HSION;
  while ((RCC->CR & RCC_CR_HSIRDY) == 0) {}
		
	// Switch SYSCLK to HSI 
  RCC->CFGR &= ~RCC_CFGR_SW;          
  RCC->CFGR |=  RCC_CFGR_SW_HSI;       
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}
	
	// Turn off PLL
	RCC->CR &= ~RCC_CR_PLLON;
  while (RCC->CR & RCC_CR_PLLRDY) {}   

	//From SystemInit()
		
  /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_Latency_0; // Modified
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

    /* PLL configuration */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL3);
            
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }  
	
	// If this is not done the system clock is not correct
	// and the blinking of the led gets slower
	SysTick_Init();
}


int main(void)
{
    SysTick_Init();
    LED_Initialize();
    Button_Initialize();

    uint32_t lastToggle = 0;
		bool switchedHSI = false;
	
    while (1)
    {
        uint32_t state = Button_GetState();

        if (state & ALARMZONE1_BUTTON) {
            zone1_active = true;        
            LED_On(ALARMZONE1_LED);
        }

        if (state & ALARMZONE2_BUTTON) {
            zone2_active = true;             
            LED_On(ALARMZONE2_LED);          
        }

        
        if (state & ALARM_OFF_BUTTON) {
            zone1_active = false;
            zone2_active = false;

            LED_Off(ALARMZONE1_LED);
            LED_Off(ALARMZONE2_LED);
            LED_Off(ALARM_SIREN);
					
						if(!switchedHSI){
							SwitchClockTo24Mhz();
							switchedHSI = true;
						}

					
        }

        /* Sirenen-LED blinken NUR wenn irgendeine Zone aktiv ist */
        bool alarmActive = (zone1_active || zone2_active);
        if (alarmActive) {
            uint32_t now = g_ms;          
            if ((uint32_t)(now - lastToggle) >= SIREN_TOGGLE_MS) {
                LED_Toggle(ALARM_SIREN);
                lastToggle = now;
            }
        } else {
            LED_Off(ALARM_SIREN);
            // lastToggle könnte zurückgesetzt werden, ist aber nicht nötig
        }

     
				SysDelay_Delay(1);
    }
}


